#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include "sm64.h"
#include "game/area.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "port/compat.h"
#include "port/renderer.h"
extern u32 gGlobalTimer;
static u16 prev=0;
static void read_pad(void){s8 x=0,y=0;if(keydown(KEY_LEFT))x-=64;if(keydown(KEY_RIGHT))x+=64;if(keydown(KEY_UP))y+=64;if(keydown(KEY_DOWN))y-=64;u16 b=0;if(keydown(KEY_F1))b|=A_BUTTON;if(keydown(KEY_F2))b|=B_BUTTON;if(keydown(KEY_F3))b|=Z_TRIG;if(keydown(KEY_F5))b|=L_CBUTTONS;if(keydown(KEY_F6))b|=R_CBUTTONS;gPlayer1Controller->stickX=x;gPlayer1Controller->stickY=y;gPlayer1Controller->stickMag=(x||y)?64.0f:0.0f;gPlayer1Controller->buttonDown=b;gPlayer1Controller->buttonPressed=b&~prev;prev=b;}
int main(void){sm64_port_init();clearevents();while(1){key_event_t e;do{e=pollevent();if(e.type==KEYEV_DOWN){if(e.key==KEY_EXIT)goto quit;if(e.key==KEY_F4){sm64_port_init();prev=0;}}}while(e.type!=KEYEV_NONE);read_pad();execute_mario_action(NULL);sm64_animation_tick();update_camera(gCurrentArea->camera);graph_renderer_draw();gGlobalTimer++;sleep_ms(33);}quit:dclear(C_WHITE);dupdate();return 1;}
