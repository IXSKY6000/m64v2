#include <string.h>
#include "sm64.h"
#include "game/area.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/interaction.h"
#include "game/level_update.h"
#include "game/main.h"
#include "game/mario.h"
#include "game/mario_actions_automatic.h"
#include "game/mario_actions_cutscene.h"
#include "game/mario_actions_submerged.h"
#include "game/mario_misc.h"
#include "game/memory.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "game/object_list_processor.h"
#include "game/paintings.h"
#include "game/ingame_menu.h"
#include "engine/surface_collision.h"
#include "engine/behavior_script.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "animation_data.h"

struct MarioState gMarioStates[1];
struct MarioState *gMarioState=&gMarioStates[0];
struct Controller gControllers[3];
struct Controller *gPlayer1Controller=&gControllers[0];
struct Controller *gPlayer2Controller=&gControllers[1];
struct Controller *gPlayer3Controller=&gControllers[2];
struct Object gMarioObjectStorage;
struct Object *gMarioObject=&gMarioObjectStorage;
struct MarioBodyState gBodyStates[2];
/* gPlayerCameraState and gCameraMovementFlags are from original camera.c */
struct Area gAreaData[1];
struct Area *gAreas=gAreaData;
struct Area *gCurrentArea=&gAreaData[0];
struct Camera gCameraStorage;
struct DmaHandlerList gMarioAnimsBuf;
static struct Animation sAnimBuffer;

s16 gCurrLevelNum=1;
s16 gCurrSaveFileNum=1;
s16 gCurrCourseNum=0;
s16 gCurrActNum=1;
s8 gDebugLevelSelect=0;
s8 gShowDebugText=0;
u32 gGlobalTimer=0;
u32 gAudioRandom=1;
f32 gGlobalSoundSource[3]={0};
u8 gSpecialTripleJump=0;
u16 gAreaUpdateCounter=0;
s16 gCheckingSurfaceCollisionsForCamera=0;
struct DemoInput *gCurrDemoInput=NULL;
u8 gLastCompletedStarNum=0;
struct Painting *gRipplingPainting=NULL;
s32 gDialogResponse=0;

const BehaviorScript bhvJumpingBox[]={0};

static struct Surface sFlatFloor={
 .type=SURFACE_DEFAULT,.force=0,.flags=0,.room=0,.lowerY=-100,.upperY=100,
 .normal={0.0f,1.0f,0.0f},.originOffset=0.0f,.object=NULL,
};
static struct Surface sDeepFloor={
 .type=SURFACE_DEFAULT,.force=0,.flags=0,.room=0,.lowerY=-12000,.upperY=-9000,
 .normal={0.0f,1.0f,0.0f},.originOffset=-10000.0f,.object=NULL,
};

s32 f32_find_wall_collision(f32*x,f32*y,f32*z,f32 oy,f32 r){(void)x;(void)y;(void)z;(void)oy;(void)r;return 0;}
s32 find_wall_collisions(struct WallCollisionData*d){d->numWalls=0;return 0;}
f32 find_ceil(f32 x,f32 y,f32 z,struct Surface **p){(void)x;(void)y;(void)z;*p=NULL;return 20000.0f;}
f32 find_floor(f32 x,f32 y,f32 z,struct Surface **p){(void)y;if(x>=-4096&&x<=4096&&z>=-4096&&z<=4096){*p=&sFlatFloor;return 0.0f;}*p=&sDeepFloor;return -10000.0f;}
f32 find_water_level(f32 x,f32 z){(void)x;(void)z;return -11000.0f;}
f32 find_poison_gas_level(f32 x,f32 z){(void)x;(void)z;return -11000.0f;}

void *segmented_to_virtual(const void *addr){return (void*)addr;}
s32 load_patchable_table(struct DmaHandlerList *list,s32 index){const struct Animation *src=sm64_get_static_anim(index);*(struct Animation*)list->bufTarget=*src;list->currentAddr=(void*)src;return 0;}

void play_sound(s32 b,f32*p){(void)b;(void)p;} void stop_sound(u32 b,f32*p){(void)b;(void)p;}
void set_sound_moving_speed(u8 b,u8 s){(void)b;(void)s;} void raise_background_noise(s32 a){(void)a;} void lower_background_noise(s32 a){(void)a;}
void play_infinite_stairs_music(void){} void fadeout_cap_music(void){} void stop_cap_music(void){}
void spawn_wind_particles(s16 p,s16 y){(void)p;(void)y;} void print_text_fmt_int(s32 x,s32 y,const char*s,s32 n){(void)x;(void)y;(void)s;(void)n;}

void mario_handle_special_floors(struct MarioState*m){(void)m;} void mario_process_interactions(struct MarioState*m){(void)m;}
void mario_drop_held_object(struct MarioState*m){m->heldObj=NULL;} void mario_grab_used_object(struct MarioState*m){m->heldObj=m->usedObj;}
void mario_throw_held_object(struct MarioState*m){m->heldObj=NULL;} void mario_stop_riding_object(struct MarioState*m){m->riddenObj=NULL;}
void mario_stop_riding_and_holding(struct MarioState*m){m->riddenObj=NULL;m->heldObj=NULL;} u32 mario_check_object_grab(struct MarioState*m){(void)m;return 0;}
void mario_blow_off_cap(struct MarioState*m,f32 s){(void)m;(void)s;}
s16 level_trigger_warp(struct MarioState*m,s32 op){(void)m;(void)op;return 0;} void load_level_init_text(u32 a){(void)a;}
s32 save_file_get_total_star_count(s32 a,s32 b,s32 c){(void)a;(void)b;(void)c;return 0;}

/* camera dependencies irrelevant on flat sandbox */
void clear_time_stop_flags(s32 f){(void)f;} void set_time_stop_flags(s32 f){(void)f;}
s16 get_dialog_id(void){return -1;} void create_dialog_box(s16 d){(void)d;} void create_dialog_box_with_response(s16 d){(void)d;}
void set_hud_camera_status(s16 s){(void)s;} f32 random_float(void){return 0.5f;} u32 save_file_get_star_flags(s32 a,s32 b){(void)a;(void)b;return 0;}
void save_file_do_save(s32 a){(void)a;} void seq_player_lower_volume(u8 a,u16 b,u8 c){(void)a;(void)b;(void)c;} void seq_player_unlower_volume(u8 a,u16 b){(void)a;(void)b;}
void play_music(u8 a,u16 b,u16 c){(void)a;(void)b;(void)c;}

void sm64_port_init(void){
 memset(gMarioState,0,sizeof(*gMarioState)); memset(gMarioObject,0,sizeof(*gMarioObject)); memset(gCurrentArea,0,sizeof(*gCurrentArea)); memset(&gCameraStorage,0,sizeof(gCameraStorage)); memset(gPlayer1Controller,0,sizeof(*gPlayer1Controller)); memset(&gBodyStates[0],0,sizeof(gBodyStates[0])); memset(&gPlayerCameraState[0],0,sizeof(gPlayerCameraState[0]));
 gCurrentArea->terrainType=TERRAIN_GRASS; gCurrentArea->camera=&gCameraStorage; gCurrentArea->index=1;
 gCameraStorage.mode=CAMERA_MODE_BEHIND_MARIO; gCameraStorage.defMode=CAMERA_MODE_BEHIND_MARIO;
 gMarioState->marioObj=gMarioObject; gMarioState->controller=gPlayer1Controller; gMarioState->area=gCurrentArea; gMarioState->marioBodyState=&gBodyStates[0]; gMarioState->statusForCamera=&gPlayerCameraState[0]; gMarioState->animList=&gMarioAnimsBuf;
 gMarioState->health=0x880;gMarioState->numLives=4;gMarioState->waterLevel=-11000;gMarioState->flags=MARIO_NORMAL_CAP|MARIO_CAP_ON_HEAD;
 gMarioObject->header.gfx.animInfo.animID=-1; gMarioObject->header.gfx.scale[0]=1;gMarioObject->header.gfx.scale[1]=1;gMarioObject->header.gfx.scale[2]=1;
 gMarioAnimsBuf.bufTarget=&sAnimBuffer; set_mario_action(gMarioState,ACT_IDLE,0);
 reset_camera(&gCameraStorage);
}

static s16 sm64_geo_update_animation_frame(struct AnimInfo *obj,s32 *accelAssist){s32 result;struct Animation*anim=obj->curAnim;if(anim==NULL)return 0;if(obj->animTimer==gAreaUpdateCounter||anim->flags&ANIM_FLAG_2){if(accelAssist)accelAssist[0]=obj->animFrameAccelAssist;return obj->animFrame;}if(anim->flags&ANIM_FLAG_BACKWARD){if(obj->animAccel!=0)result=obj->animFrameAccelAssist-obj->animAccel;else result=(obj->animFrame-1)<<16;if(GET_HIGH_S16_OF_32(result)<anim->loopStart){if(anim->flags&ANIM_FLAG_NOLOOP)SET_HIGH_S16_OF_32(result,anim->loopStart);else SET_HIGH_S16_OF_32(result,anim->loopEnd-1);}}else{if(obj->animAccel!=0)result=obj->animFrameAccelAssist+obj->animAccel;else result=(obj->animFrame+1)<<16;if(GET_HIGH_S16_OF_32(result)>=anim->loopEnd){if(anim->flags&ANIM_FLAG_NOLOOP)SET_HIGH_S16_OF_32(result,anim->loopEnd-1);else SET_HIGH_S16_OF_32(result,anim->loopStart);}}if(accelAssist)accelAssist[0]=result;return GET_HIGH_S16_OF_32(result);}
void sm64_animation_tick(void){struct AnimInfo*a=&gMarioState->marioObj->header.gfx.animInfo;if(a->curAnim){a->animFrame=sm64_geo_update_animation_frame(a,&a->animFrameAccelAssist);a->animTimer=gAreaUpdateCounter;}gAreaUpdateCounter++;}
