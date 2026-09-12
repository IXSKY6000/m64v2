/*
 * Seul ce fichier inclut gint.
 * Il n'inclut volontairement AUCUN header de SM64 afin d'eviter les conflits
 * u32/ssize_t entre gint et les typedefs Ultra64.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include "platform_graph35.h"

static unsigned int s_current=0;
static unsigned int s_previous=0;

static int physical_key(enum GraphKey k)
{
    switch(k){
        case GKEY_EXIT:return KEY_EXIT;
        case GKEY_F1:return KEY_F1;
        case GKEY_F2:return KEY_F2;
        case GKEY_F3:return KEY_F3;
        case GKEY_F4:return KEY_F4;
        case GKEY_F5:return KEY_F5;
        case GKEY_F6:return KEY_F6;
        case GKEY_UP:return KEY_UP;
        case GKEY_DOWN:return KEY_DOWN;
        case GKEY_LEFT:return KEY_LEFT;
        case GKEY_RIGHT:return KEY_RIGHT;
        default:return 0;
    }
}

void graph_platform_init(void)
{
    clearevents();
    s_current=0;
    s_previous=0;
}

void graph_input_update(void)
{
    s_previous=s_current;
    s_current=0;
    for(int i=0;i<GKEY_COUNT;i++){
        int key=physical_key((enum GraphKey)i);
        if(key && keydown(key)) s_current|=(1u<<i);
    }
}

int graph_key_down(enum GraphKey key)
{
    return (s_current&(1u<<key))!=0;
}

int graph_key_pressed(enum GraphKey key)
{
    unsigned int b=1u<<key;
    return (s_current&b)!=0 && (s_previous&b)==0;
}

void graph_sleep_ms(int ms){ sleep_ms(ms); }

void graph_clear_white(void){ dclear(C_WHITE); }
void graph_line_black(int x1,int y1,int x2,int y2){ dline(x1,y1,x2,y2,C_BLACK); }
void graph_rect_white(int x1,int y1,int x2,int y2){ drect(x1,y1,x2,y2,C_WHITE); }
void graph_pixel_black(int x,int y){ dpixel(x,y,C_BLACK); }
void graph_text_black(int x,int y,const char *text){ dtext(x,y,C_BLACK,text); }
void graph_present(void){ dupdate(); }
