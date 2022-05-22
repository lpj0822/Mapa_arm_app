#ifndef __UI_INTERFACE_H__
#define __UI_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WHITE   0xFFFFFF
#define YELLOW  0xFFFF00
#define CYAN    0x00FFFF
#define GREEN   0x00FF00
#define MAGENTA 0xFF00FF
#define RED     0xFF0000
#define BLUE    0x0000FF
#define BLACK   0x000000

    /*
enum COLOR {
	WHITE = 0,
	YELLOW,
	CYAN,
	GREEN,
	MAGENTA,
	RED,
	BLUE,
	BLACK,
	COLORBAR,
};
*/

int draw_init(void);
int draw_bmp(int status);
int draw_line(unsigned int color, int x1, int y1, int x2, int y2);
int draw_line_no_scale(unsigned int color, int x1, int y1, int x2, int y2);
int draw_deinit(void);
int draw_clean(void);
int draw_show(void);
int draw_buf(unsigned char* buf, int wid, int hig);
int draw_clean_rect(int startx,int starty, int endx, int endy);
int draw_color_rect(int startx, int starty, int endx, int endy, int color);

#ifdef __cplusplus
}
#endif

#endif
