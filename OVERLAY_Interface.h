#ifndef __OVERLAY_INTERFACE_H__
#define __OVERLAY_INTERFACE_H__

#define WHITE   0
#define YELLOW  5
#define CYAN    6
#define GREEN   4
#define MAGENTA 7
#define RED     2
#define BLUE    3
#define BLACK   1


int OVERLAY_init(void);
int OVERLAY_clean(void);
int OVERLAY_line(unsigned int color, int x1, int y1, int x2, int y2);
int OVERLAY_rect(unsigned int color, int x, int y, int wid, int hig);
int OVERLAY_show(void);
int OVERLAY_deinit(void);

#endif

