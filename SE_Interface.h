#ifndef __SE_INTERFACE_H__
#define __SE_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <pthread.h>

#define LD_SAFE_LEFT    (1)
#define LD_SAFE_RIGHT   (2)
#define LD_WARN_LEFT    (4)
#define LD_WARN_RIGHT	(8)
#define LD_UNKNOW   	(16)
#define FC_WARN     	(32)
#define FC_LEFT     	(64)
#define FC_MID      	(128)
#define FC_RIGHT    	(256)
#define FC_UNKNOW    	(512)
#define FC_HMW     	    (1024)

typedef struct POINT_2D
{
	int x;
	int y;
} POINT_2D_s;

typedef struct TIP_LINE
{
	POINT_2D_s *points;
	int num;
} TIP_LINE_s;

typedef struct LD_UI_RESULT
{
	POINT_2D_s point;
    int width;
	int position;
	int status;
} LD_UI_RESULT_s;

typedef struct FC_UI_RESULT
{
	POINT_2D_s point;
	int wid;
	int hig;
	int status;
} FC_UI_RESULT_s;

typedef struct TDMAP_RECT{
    short up;
    short down;
    short left;
    short right;
	float fttc;
}TDMAP_RECT_t;

typedef struct TDMAP_UI_RESULT
{
    TDMAP_RECT_t rects;
    int color;
}TDMAP_UI_RESULT_t;

typedef struct SE_UI_RESULT
{
    int tdmapWarnNum;
	int ldCount;
	int fcCount;
	int detCount;
    int tdmapCount;
	float ttc;
	double leftRange;
	double rightRange;
	LD_UI_RESULT_s* ldResult;
	FC_UI_RESULT_s* fcResult;
	FC_UI_RESULT_s* detResult;
    TDMAP_UI_RESULT_t *tdmap;
} SE_UI_RESULT_s;

int SE_Init(const char* ldParamFile, const char* ldCalibFile,
		const char* fcCarInfoFile);
//int SE_Start(pthread_t *ldTId, pthread_t *fcwsdTId, pthread_t *fcwstTId);
int SE_Start();
int SE_Main(char* yBuf, int width, int height, int frameNo, float speed);
int SE_GetResult(SE_UI_RESULT_s** result);
int SE_GetTipLine(TIP_LINE_s** line);
int SE_Stop();
int SE_Release();

#ifdef __cplusplus
}
#endif
#endif
