#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <drm/drm_fourcc.h>
#include <basetypes.h>

#include "UI_Interface.h"
#include "PublicDefine.h"

static int fd;
static void *fb_mem;
static u32 width, height;
static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo var;
static char fb_dev[128];
//static char filename[128];
static int thickness = 2;
//static int color = COLORBAR;
static float xScale = 1.0f;
static float yScale = 1.0f;

static int draw_rgb_to_fb_with_argb1555(void *fb_mem, int xIndex, int yIndex, unsigned char Sr,unsigned char Sg, unsigned char Sb)
{
    unsigned char a, r, g, b;
    u16 value = 0;
    u16 *argb1555 = NULL;

    if(!fb_mem)
        return -1;

    argb1555 = (u16 *)((u8 *)fb_mem);
    r = Sr >> 3;
    g = Sg >> 3;
    b = Sb >> 3;
    a = 1;
    value = ((a << 15) + (r << 10) + (g << 5) + (b << 0));

    argb1555[yIndex*width+xIndex] = value;

    return 0;
}

int draw_buf(unsigned char* buf, int wid, int hig)
{
    int i = 0,j = 0;
    for(i = 0; i < hig; i++)
    {
        for(j = 0; j < wid; j ++){
            int index = (i*wid+j)*3;
            draw_rgb_to_fb_with_argb1555(fb_mem, j,i,buf[index], buf[index+1],buf[index+2]);
        }
    }
    return 0;
}

static int draw_paint_to_fb_with_argb1555(void *fb_mem, int xIndex, int yIndex, int color)
{
    unsigned char a, r, g, b;
    u16 value = 0;
    u16 *argb1555 = NULL;

    if(!fb_mem)
        return -1;

    argb1555 = (u16 *)((u8 *)fb_mem);
    r = color >> 19 & 0x1F;
    g = color >> 11 & 0x1F;
    b = color >> 3 & 0x1F;
    a = 1;
    value = ((a << 15) + (r << 10) + (g << 5) + (b << 0));

    argb1555[yIndex*width+xIndex] = value;

    return 0;
}

static int draw_line_to_fb(void* mem, int x0, int y0,
        int x1, int y1, int color, int thickness)
{
    int i;
    float slope;

    if(!mem)
        return -1;

    if (x0 == x1) {

        int y_temp;
        if (y0 > y1) {
            y_temp = y0;
            y0 = y1;
            y1 = y_temp;
        }
        for (i = y0; i < y1; i++)
        {
            draw_paint_to_fb_with_argb1555(mem, x0, i, color);
            draw_paint_to_fb_with_argb1555(mem, x0+1, i, color);
        }
    } else if (y0 == y1) {

        int x_temp;
        if (x0 > x1) {
            x_temp = x0;
            x0 = x1;
            x1 = x_temp;
        }
        for (i = x0; i < x1; i++)
        {
            draw_paint_to_fb_with_argb1555(mem, i, y0, color);
            draw_paint_to_fb_with_argb1555(mem, i, y0+1, color);
        }
    } else {

        int x_temp;
        int y_temp;
        if (abs(x0 - x1) >= abs(y0 - y1)) {
            if (x0 > x1) {
                x_temp = x0;
                x0 = x1;
                x1 = x_temp;
                y_temp = y0;
                y0 = y1;
                y1 = y_temp;
            }
            slope = ((float) (y1 - y0)) / (x1 - x0);
            for (i = x0; i < x1; i++) {

                int temp = y0 + slope * (i - x0);
                draw_paint_to_fb_with_argb1555(mem, i, temp, color);
                draw_paint_to_fb_with_argb1555(mem, i+1, temp, color);
            }
        } else {
            if (y0 > y1) {
                x_temp = x0;
                x0 = x1;
                x1 = x_temp;
                y_temp = y0;
                y0 = y1;
                y1 = y_temp;
            }
            slope = ((float) (x1 - x0)) / (y1 - y0);
            for (i = y0; i < y1; i++) {
                int temp = x0 + slope * (i - y0);
                draw_paint_to_fb_with_argb1555(mem, temp, i, color);
                draw_paint_to_fb_with_argb1555(mem, temp, i+1, color);
            }
        }
    }
    return 0;
}
int draw_init(void)
{
    int ReVal = 0;
	int rval;

    xScale = 1.0*WIN_WID/FRAME_WID;
    yScale = 1.0*WIN_HIG/FRAME_HIG;

	/* Open the file for reading and writing */
	strcpy(fb_dev, "/dev/fb1");
	fd = open(fb_dev, O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	/* Get fixed screen information */
	rval = ioctl(fd, FBIOGET_FSCREENINFO, &fix);
	if (rval < 0) {
		perror("FBIOGET_FSCREENINFO");
		return -1;
	}

	/* Get variable screen information */
	rval = ioctl(fd, FBIOGET_VSCREENINFO, &var);
	if (rval < 0) {
		perror("FBIOGET_VSCREENINFO");
		return -1;
	}

	width = var.xres;
	height = var.yres;

    printf("width:%d,hight:%d\n",width, height);

	/* Map the device to memory */
	fb_mem = mmap(0, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (fb_mem == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return -1;
	}
    return ReVal;
}

int draw_clean(void)
{
    int ReVal = 0;

	memset((u8 *)fb_mem, 0, (width * height) << 2);

    return ReVal;
}

int draw_clean_rect(int startx,int starty, int endx, int endy){
    int ReVal = 0;

    for(int i = starty; i < endy; i++)
    {
        memset(((u8*)fb_mem)+((i*width+startx)*2),0,(endx-startx)*2);
    }

    return ReVal;
}

int draw_color_rect(int startx, int starty, int endx, int endy, int color) {
    int ReVal = 0;

    for(int j = starty; j < endy; j++){
        for(int i = startx; i < endx; i++){
            draw_paint_to_fb_with_argb1555(fb_mem, i, j, color);
        }
    }

    return ReVal;
}

int draw_show(void)
{
    int ReVal = 0;
	var.yoffset = 0;
	ioctl(fd, FBIOPAN_DISPLAY, &var);
    return ReVal;
}

int draw_line(unsigned int color, int x1, int y1, int x2, int y2)
{
    int ReVal = 0;

    int startX = (int)x1*xScale;
    int startY = (int)y1*yScale;
    int endX = (int)x2*xScale;
    int endY = (int)y2*yScale;
	draw_line_to_fb(fb_mem, startX, startY, endX, endY, color, thickness);

    return ReVal;
}

int draw_line_no_scale(unsigned int color, int x1, int y1, int x2, int y2)
{
    int ReVal = 0;

	draw_line_to_fb(fb_mem, x1, y1, x2, y1, color, thickness);

    return ReVal;
}
int draw_bmp(int status)
{
    int ReVal = 0;

    return ReVal;
}

int draw_deinit()
{
    int ReVal = 0;

	munmap(fb_mem, fix.smem_len);
	close(fd);

    return ReVal;
}


