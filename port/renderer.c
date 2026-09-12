#include <gint/display.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "sm64.h"
#include "types.h"
#include "game/area.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "engine/math_util.h"
#include "mario_mesh.h"

static int anim_index(s32 frame,const u16 **attr){int r;if(frame<(*attr)[0])r=(*attr)[1]+frame;else r=(*attr)[1]+(*attr)[0]-1;*attr+=2;return r;}
typedef enum {AT_TRANSLATION,AT_LATERAL,AT_VERTICAL,AT_NONE,AT_ROTATION} AnimType;
typedef struct {int parent;s16 tx,ty,tz;const Sm64Edge *edges;int n;} Part;
static const Part parts[20]={
{-1,0,0,0,NULL,0},{0,0,0,0,mesh_butt,MESH_BUTT_COUNT},{1,68,0,0,mesh_torso,MESH_TORSO_COUNT},{2,87,0,0,mesh_head,MESH_HEAD_COUNT},
{2,67,-10,79,NULL,0},{4,0,0,0,mesh_l_arm,MESH_L_ARM_COUNT},{5,65,0,0,mesh_l_forearm,MESH_L_FOREARM_COUNT},{6,60,0,0,mesh_l_hand,MESH_L_HAND_COUNT},
{2,68,-10,-79,NULL,0},{8,0,0,0,mesh_r_arm,MESH_R_ARM_COUNT},{9,65,0,0,mesh_r_forearm,MESH_R_FOREARM_COUNT},{10,60,0,0,mesh_r_hand,MESH_R_HAND_COUNT},
{1,13,-8,42,NULL,0},{12,0,0,0,mesh_l_thigh,MESH_L_THIGH_COUNT},{13,89,0,0,mesh_l_leg,MESH_L_LEG_COUNT},{14,67,0,0,mesh_l_foot,MESH_L_FOOT_COUNT},
{1,13,-8,-42,NULL,0},{16,0,0,0,mesh_r_thigh,MESH_R_THIGH_COUNT},{17,89,0,0,mesh_r_leg,MESH_R_LEG_COUNT},{18,67,0,0,mesh_r_foot,MESH_R_FOOT_COUNT}};

static Mat4 sView;
static void point_xform(const Mat4 m,f32 x,f32 y,f32 z,Vec3f o){o[0]=x*m[0][0]+y*m[1][0]+z*m[2][0]+m[3][0];o[1]=x*m[0][1]+y*m[1][1]+z*m[2][1]+m[3][1];o[2]=x*m[0][2]+y*m[1][2]+z*m[2][2]+m[3][2];}
static bool project(Vec3f p,int *x,int *y){Vec3f c;point_xform(sView,p[0],p[1],p[2],c);f32 d=-c[2];if(d<20.0f)return false;*x=64+(int)(c[0]*77.0f/d);*y=32-(int)(c[1]*77.0f/d);return *x>-100&&*x<228&&*y>-100&&*y<164;}
static void world_line(Vec3f a,Vec3f b){int ax,ay,bx,by;if(project(a,&ax,&ay)&&project(b,&bx,&by))dline(ax,ay,bx,by,C_BLACK);}

static void extra_torso_rotation(Mat4 dst,Mat4 butt){Vec3s r={0,0,0};struct MarioBodyState *b=gMarioState->marioBodyState;s32 act=b->action;if(act!=ACT_BUTT_SLIDE&&act!=ACT_HOLD_BUTT_SLIDE&&act!=ACT_WALKING&&act!=ACT_RIDING_SHELL_GROUND){b->torsoAngle[0]=b->torsoAngle[1]=b->torsoAngle[2]=0;}r[0]=b->torsoAngle[1];r[1]=b->torsoAngle[2];r[2]=b->torsoAngle[0];Vec3f t={0,0,0};Mat4 q;mtxf_rotate_xyz_and_translate(q,t,r);mtxf_mul(dst,q,butt);}
static void extra_head_rotation(Mat4 dst,Mat4 head){Vec3s r={0,0,0};struct MarioBodyState *b=gMarioState->marioBodyState;if(gCurrentArea->camera->mode==CAMERA_MODE_C_UP){r[0]=gPlayerCameraState->headRotation[1];r[2]=gPlayerCameraState->headRotation[0];}else if(b->action&ACT_FLAG_WATER_OR_TEXT){r[0]=b->headAngle[1];r[1]=b->headAngle[2];r[2]=b->headAngle[0];}Vec3f t={0,0,0};Mat4 q;mtxf_rotate_xyz_and_translate(q,t,r);mtxf_mul(dst,q,head);}

static void build_pose(Mat4 out[20],Mat4 *headDraw){struct AnimInfo *ai=&gMarioState->marioObj->header.gfx.animInfo;struct Animation *an=ai->curAnim;Vec3f op={gMarioState->marioObj->header.gfx.pos[0],gMarioState->marioObj->header.gfx.pos[1],gMarioState->marioObj->header.gfx.pos[2]};Vec3s oa={gMarioState->marioObj->header.gfx.angle[0],gMarioState->marioObj->header.gfx.angle[1],gMarioState->marioObj->header.gfx.angle[2]};Mat4 obj,torsoBase;mtxf_rotate_zxy_and_translate(obj,op,oa);
 const u16 *at=an?an->index:NULL;const s16 *dat=an?an->values:NULL;AnimType typ=AT_ROTATION;f32 mul=1.0f;if(an){if(an->flags&ANIM_FLAG_HOR_TRANS)typ=AT_VERTICAL;else if(an->flags&ANIM_FLAG_VERT_TRANS)typ=AT_LATERAL;else if(an->flags&ANIM_FLAG_6)typ=AT_NONE;else typ=AT_TRANSLATION;if(an->animYTransDivisor)mul=(f32)ai->animYTrans/(f32)an->animYTransDivisor;}
 for(int i=0;i<20;i++){Vec3f t={(f32)parts[i].tx,(f32)parts[i].ty,(f32)parts[i].tz};Vec3s r={0,0,0};if(an){if(typ==AT_TRANSLATION){t[0]+=dat[anim_index(ai->animFrame,&at)]*mul;t[1]+=dat[anim_index(ai->animFrame,&at)]*mul;t[2]+=dat[anim_index(ai->animFrame,&at)]*mul;typ=AT_ROTATION;}else if(typ==AT_LATERAL){t[0]+=dat[anim_index(ai->animFrame,&at)]*mul;at+=2;t[2]+=dat[anim_index(ai->animFrame,&at)]*mul;typ=AT_ROTATION;}else if(typ==AT_VERTICAL){at+=2;t[1]+=dat[anim_index(ai->animFrame,&at)]*mul;at+=2;typ=AT_ROTATION;}else if(typ==AT_NONE){at+=6;typ=AT_ROTATION;}r[0]=dat[anim_index(ai->animFrame,&at)];r[1]=dat[anim_index(ai->animFrame,&at)];r[2]=dat[anim_index(ai->animFrame,&at)];}
  Mat4 local;mtxf_rotate_xyz_and_translate(local,t,r);if(i==0)mtxf_mul(out[i],local,obj);else if(i==2){extra_torso_rotation(torsoBase,out[1]);mtxf_mul(out[i],local,torsoBase);}else mtxf_mul(out[i],local,out[parts[i].parent]);}
 extra_head_rotation(*headDraw,out[3]);}

static void draw_model(void){Mat4 pose[20],head;build_pose(pose,&head);for(int i=0;i<20;i++){if(!parts[i].edges)continue;const Mat4 *m=(i==3)?&head:&pose[i];for(int j=0;j<parts[i].n;j++){const Sm64Edge*e=&parts[i].edges[j];Vec3f a,b;point_xform(*m,e->ax,e->ay,e->az,a);point_xform(*m,e->bx,e->by,e->bz,b);world_line(a,b);}}}
static void draw_floor(void){int gx=((int)gMarioState->pos[0]/256)*256,gz=((int)gMarioState->pos[2]/256)*256;for(int i=-6;i<=6;i++){Vec3f a={(f32)(gx+i*256),0,(f32)(gz-6*256)},b={(f32)(gx+i*256),0,(f32)(gz+6*256)},c={(f32)(gx-6*256),0,(f32)(gz+i*256)},d={(f32)(gx+6*256),0,(f32)(gz+i*256)};world_line(a,b);world_line(c,d);}}
void graph_renderer_draw(void){mtxf_lookat(sView,gLakituState.pos,gLakituState.focus,gLakituState.roll);dclear(C_WHITE);draw_floor();draw_model();char b[30];snprintf(b,sizeof(b),"%08lx A%02x F%d",(unsigned long)gMarioState->action,(unsigned)gMarioState->marioObj->header.gfx.animInfo.animID,gMarioState->marioObj->header.gfx.animInfo.animFrame);drect(0,0,127,7,C_WHITE);dtext(0,0,C_BLACK,b);dupdate();}
