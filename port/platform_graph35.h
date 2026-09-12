#ifndef PORT_PLATFORM_GRAPH35_H
#define PORT_PLATFORM_GRAPH35_H

enum GraphKey {
    GKEY_EXIT = 0,
    GKEY_F1,
    GKEY_F2,
    GKEY_F3,
    GKEY_F4,
    GKEY_F5,
    GKEY_F6,
    GKEY_UP,
    GKEY_DOWN,
    GKEY_LEFT,
    GKEY_RIGHT,
    GKEY_COUNT
};

void graph_platform_init(void);
void graph_input_update(void);
int graph_key_down(enum GraphKey key);
int graph_key_pressed(enum GraphKey key);
void graph_sleep_ms(int ms);

void graph_clear_white(void);
void graph_line_black(int x1,int y1,int x2,int y2);
void graph_rect_white(int x1,int y1,int x2,int y2);
void graph_pixel_black(int x,int y);
void graph_text_black(int x,int y,const char *text);
void graph_present(void);

#endif
