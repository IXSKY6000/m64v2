#include "sm64.h"
#include "game/area.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "port/compat.h"
#include "port/renderer.h"
#include "port/platform_graph35.h"

extern u32 gGlobalTimer;
static u16 prev=0;

static void read_pad(void)
{
    s8 x=0,y=0;
    if(graph_key_down(GKEY_LEFT))x-=64;
    if(graph_key_down(GKEY_RIGHT))x+=64;
    if(graph_key_down(GKEY_UP))y+=64;
    if(graph_key_down(GKEY_DOWN))y-=64;

    u16 b=0;
    if(graph_key_down(GKEY_F1))b|=A_BUTTON;
    if(graph_key_down(GKEY_F2))b|=B_BUTTON;
    if(graph_key_down(GKEY_F3))b|=Z_TRIG;
    if(graph_key_down(GKEY_F5))b|=L_CBUTTONS;
    if(graph_key_down(GKEY_F6))b|=R_CBUTTONS;

    gPlayer1Controller->stickX=x;
    gPlayer1Controller->stickY=y;
    gPlayer1Controller->stickMag=(x||y)?64.0f:0.0f;
    gPlayer1Controller->buttonDown=b;
    gPlayer1Controller->buttonPressed=b&~prev;
    prev=b;
}

int main(void)
{
    sm64_port_init();
    graph_platform_init();

    while(1){
        graph_input_update();

        if(graph_key_pressed(GKEY_EXIT))break;
        if(graph_key_pressed(GKEY_F4)){
            sm64_port_init();
            prev=0;
        }

        read_pad();
        execute_mario_action(NULL);
        sm64_animation_tick();
        update_camera(gCurrentArea->camera);
        graph_renderer_draw();
        gGlobalTimer++;
        graph_sleep_ms(33);
    }

    graph_clear_white();
    graph_present();
    return 1;
}
