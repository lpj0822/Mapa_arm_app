#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <unistd.h>

#include "SE_Interface.h"
#include "PublicDefine.h"
#include "utils.h"
#include "WSErrCode.h"

#define CONTRL_SWITCH_FUN       0
#define SPEED_TO_RUN            0
#define CONTRL_FPS              1
#define PRINT_FRAME_RANGE       (100)

#define READ_YUV_DATA       0
#define DAY_OR_NIGHT        0
#define SHOW_TIME	    1
#define LDWS		    1
#define FCWSD		    1
#define FCWST		    1
#define OBJRLCT             1
#define OBJVERF             1
#define OBJVERF_CNN         0
#define FCWSD_CNN           1
#define TDMAP               0

#define TIP_LINE_NUM	9
#define LD_RESULT_MAX	18
#define FC_RESULT_MAX	10

typedef struct FRAME_DATA
{
    unsigned int bufNo;
    unsigned char* yBuf;
} FRAME_DATA_s;

static int gBufIn = 0;
static float gSpeed;
static unsigned char* gMemStartAddr = NULL;
static FRAME_DATA_s gBuf[CACHE_BUF_NUM];
static SE_UI_RESULT_s* gpUIResult;
static pthread_cond_t gBufCond;
static pthread_mutex_t gBufMutex;
static int InitMem();
static int startThread(const char* name, void *(*thread_func)(void*),
        pthread_t *tId);
static int writeTipFile(TIP_LINE_s* tipLine);

#if LDWS || FCWST
static double gTTC = 4.0;
#endif

#if DAY_OR_NIGHT
#include "day_or_night.h"
#endif
//0-day, 1-night
static unsigned char gDayOrNight = 0;

#if LDWS
#include "LDWS_Interface.h"
#define LDWS_GAP                    (33)
#define LDWS_THREAD_PRIORITY		(99) /* thread priority,value : 0~99 */
#define LDWS_THREAD_CPU_ID			(8) /* CPU3*/
volatile int gIsLDWExit = 0;
static pthread_mutex_t gLDMutex;
static pthread_t gLDWTid;
static int drawLDWidth[FRAME_HIG] = {0};
static TIP_LINE_s gTipLine;
static LDWS_Output* gpLDOutput = NULL;
static LDWS_InitGuid* gpLDGuid = NULL;
static int initLDW(const char* path1, const char* path2);
static void* runLDW(void* arg);
static int freeLDW();
static int InitTipLine();
#endif //LDWS

#if FCWSD
#if OBJRLCT == 1
#define OBJRLCT_DAY_HANDLE_ID		        (2)
#define OBJRLCT_NIGHT_HANDLE_ID		        (6)
#endif
#if FCWSD_CNN == 1
#include "cnn_verf.h"
#include "OBJVERF_Interface.h"
#define CNN_IMAGE_WIDTH             (25)
#define CNN_IMAGE_HEIGHT            (25)
static float* FCWSDObjectResize = NULL;
#endif
#include "FCWSD_Interface.h"
#define FCWSD_GAP                   (100)
#define FCWSD_THREAD_PRIORITY		(99) /* thread priority,value : 0~99 */
#define FCWSD_THREAD_CPU_ID			(6) /* CPU1-2*/
#define FCWSD_THREAD_MAX_RT	        (200)
#define FCWSD_DAY_HANDLE_ID		        (0)
#define FCWSD_NIGHT_HANDLE_ID		        (4)
volatile int gIsFCWSDExit = 0;
static pthread_mutex_t gFCMutex;
static pthread_t gFCDTid;
static FCWSDParam gFCWSDParam;
static FCWSD_Output* gpFCWSDOutput = NULL;
#if LDWS
static pthread_mutex_t gLD4FCMutex;
static LDWS_Point* gpLDInputPoints = NULL;
static LDWS_Output gLDInput;
#endif //LDWS
static int initFCWSD(const char* dayPath, const char* nightPath);
static void* runFCWSD(void* arg);
static int freeFCWSD(void);
#endif //FCWSD

#if FCWST
#include "vehicle_shadow.h"
#include "Fcw_Tracker.h"
#define FCWST_GAP                   (33)
#define FCWST_THREAD_PRIORITY		(99) /* thread priority,value : 0~99 */
#define FCWST_THREAD_CPU_ID			(8) /* CPU3*/
#define FCWST_DAY_HANDLE_ID		        (3)
#define FCWST_NIGHT_HANDLE_ID		        (7)
volatile int gIsFCWSTExit = 0;
static pthread_mutex_t gFCTMutex;
static pthread_t gFCTTid;
static MuliTracker* gpFCWSTOutput = NULL;
static int initFCWST(const char* dayPath, const char* nightPath);
static void* runFCWST(void* arg);
static int freeFCWST(void);
#if READ_YUV_DATA
static void readUVImage(const unsigned char* yuvImage, const int width, const int height, unsigned char* vImage);
#endif
#endif //FCWST

#if OBJVERF
#include "OBJVERF_Interface.h"
#if OBJVERF_CNN == 1
#include "cnn_verf.h"
#define CNN_OBJVERF_ID  2
#endif
#define OBJVERF_GAP                 (33)
#define OBJVERF_THREAD_PRIORITY	    (99) /* thread priority,value : 0~99 */
#define OBJVERF_THREAD_CPU_ID	    (8) /* CPU3*/
#define OBJVERF_DAY_HANDLE_ID           (1)
#define OBJVERF_NIGHT_HANDLE_ID         (5)
#define CNN_IMAGE_WIDTH             (25)
#define CNN_IMAGE_HEIGHT            (25)
volatile int gIsObjVerfWExit = 0;
static pthread_t gObjVerfTid;
static pthread_mutex_t gObjVerfInputMutex;
static pthread_mutex_t gObjVerfOutputMutex;
static MuliTracker* gpFCWSTInput = NULL;
static verfobj objVerf[MAX_TRACK_MUM];
static int initObjVerf(const char* dayPath, const char* nightPath);
static void* runObjVerf(void* arg);
static int freeObjVerf(void);
#endif //OBJVERF

#if TDMAP
#include "DISP_Interface.h"
#include "3Dmap_Interface.h"
#define TDMAP_THREAD_PRIORITY		(99) /* thread priority,value : 0~99 */
#define TDMAP_THREAD_CPU_ID			(6) /* CPU3-1*/
#define TDMAP_GAP                   (100)
volatile int gIsTDMAPExit = 0;
static pthread_mutex_t gTDMAPMutex;
static pthread_t gTDMAPTid;
static FENCE_LIST gDrawFrences;
static int initTDMAP(void);
static void* runTDMAP(void* arg);
static int freeTDMAP(void);
#endif //TDMAP

int saveFile(char* name, void* buf, int size);
int SE_GetResult(SE_UI_RESULT_s** result);

/*
 * I/O:	  Name		        Type	            Size    Content
 * [in]	  ldParamFile       const char*		    /	    LDW config file path.
 * [in]	  ldCalibFile	    const char*		   	/       LDW config file path.
 * [in]	  fcCarInfoFile	    const char*		   	/       FCW config file path.
 * [out]  returned value    int                 4-Byte	record function status.
 * Realized function:
 * + Declaration of function
 * + Free the memory space of variables.
 *
 */
int SE_Init(const char* ldParamFile, const char* ldCalibFile,
        const char* fcCarInfoFile)
{
    const char* carDayModelFile2 = "/wissen/car_day_2.txt";
    const char* carNightModelFile2 = "/wissen/car_night_2.txt";

    if (NULL == ldParamFile || NULL == ldCalibFile || NULL == fcCarInfoFile)
    {
        printf("param file load fail\n");
        return WS_ERR_PARAM_INVALID;
    }

    pthread_mutex_init(&gBufMutex, NULL);
    pthread_cond_init(&gBufCond, NULL);

    if(WS_OK != InitMem())
    {
        printf("malloc mem fail\n");
        return WS_ERR_MEM_ALLOC;
    }
    printf("malloc mem over\n");

#if FCWSD_CNN || OBJVERF_CNN
    cnn_before();
    printf("CNN init over!\n");
#endif

#if LDWS
    pthread_mutex_init(&gLDMutex, NULL);
    initLDW(ldParamFile, ldCalibFile);
    InitTipLine();
    gTTC = LDWS_GetWarningTTC();
    writeTipFile(&gTipLine);
    printf("LDWS init over!\n");
#endif //LDWS
#if FCWSD
#if LDWS
    pthread_mutex_init(&gLD4FCMutex, NULL);
    gLDInput.NB_INTERVALLES = gpLDOutput->NB_INTERVALLES;
    gLDInput.NB_BANDES = gpLDOutput->NB_BANDES;
    gLDInput.pCaPoint = gpLDInputPoints;
#endif //LDWS
    pthread_mutex_init(&gFCMutex, NULL);
    initFCWSD(carDayModelFile2, carNightModelFile2);
    printf("FCWSD init over!\n");
#endif //FCWSD
#if FCWST
    pthread_mutex_init(&gFCTMutex, NULL);
    initFCWST(carDayModelFile2, carNightModelFile2);
    printf("FCWST init over!\n");
#endif //FCWST

#if OBJVERF
    pthread_mutex_init(&gObjVerfInputMutex, NULL);
    pthread_mutex_init(&gObjVerfOutputMutex, NULL);
    initObjVerf(carDayModelFile2, carNightModelFile2);
    printf("OBJVERF init over!\n");
#endif //OBJVERF

#if TDMAP
    pthread_mutex_init(&gTDMAPMutex, NULL);
    initTDMAP();
    printf("TDMAP init over!\n");
#endif //TDMAP

    return WS_OK;
}

static int InitMem()
{
    int ReVal = 0;
    //int i = 0;
    unsigned char* tmpPtr = NULL;
    int memSize = 0;
    memSize = /*CAHCE_BUF_NUM * FRAME_WID * FRAME_HIG << 1 + */sizeof(SE_UI_RESULT_s)
        + sizeof(LD_UI_RESULT_s) * LD_RESULT_MAX
        + sizeof(FC_UI_RESULT_s) * FC_RESULT_MAX
        + sizeof(FC_UI_RESULT_s) * FC_RESULT_MAX;
#if LDWS
    memSize += sizeof(POINT_2D_s) * TIP_LINE_NUM;
#if FCWSD
    memSize += sizeof(LDWS_Point) * 64;
#endif
#endif
    gMemStartAddr = (unsigned char*)my_malloc(memSize);
    memset(gMemStartAddr, 0, memSize);
    tmpPtr = gMemStartAddr;
    /*
       for (i = 0; i < CAHCE_BUF_NUM; i++)
       {
       gBuf[i].yBuf = tmpPtr;
       tmpPtr += (FRAME_WID * FRAME_HIG << 1);
       }
       */
    gpUIResult = (SE_UI_RESULT_s*) tmpPtr;
    tmpPtr += sizeof(SE_UI_RESULT_s);
    gpUIResult->ldResult = (LD_UI_RESULT_s*) tmpPtr;
    tmpPtr += sizeof(LD_UI_RESULT_s) * LD_RESULT_MAX;
    gpUIResult->fcResult = (FC_UI_RESULT_s*) tmpPtr;
    tmpPtr += sizeof(FC_UI_RESULT_s) * FC_RESULT_MAX;
    gpUIResult->detResult = (FC_UI_RESULT_s*) tmpPtr;
    tmpPtr += sizeof(FC_UI_RESULT_s) * FC_RESULT_MAX;
#if LDWS
    gTipLine.points = (POINT_2D_s*) tmpPtr;
    tmpPtr += sizeof(POINT_2D_s) * TIP_LINE_NUM;
#if FCWSD
    gpLDInputPoints = (LDWS_Point*) tmpPtr;
    tmpPtr += sizeof(LDWS_Point) * 64;
#endif
#endif

    return ReVal;
}

int SE_Start()
{
    int ReVal = 0;

#if LDWS
    gIsLDWExit = 0;
    startThread("LDWS", runLDW, &gLDWTid);
#endif
#if FCWSD
    gIsFCWSDExit = 0;
    startThread("FCWSD", runFCWSD, &gFCDTid);
#endif
#if FCWST
    gIsFCWSTExit = 0;
    startThread("FCWST", runFCWST, &gFCTTid);
#endif
#if OBJVERF
    gIsObjVerfWExit = 0;
    startThread("OBJVERF", runObjVerf, &gObjVerfTid);
#endif //OBJVERF
#if TDMAP
    gIsTDMAPExit = 0;
    startThread("TDMAP", runTDMAP, &gTDMAPTid);
#endif

    return ReVal;
}

int SE_Main(char* yBuf, int width, int height, int frameNo, float speed)
{
    int ReVal = 0;
#if DAY_OR_NIGHT
    int resultDayOrNight = 0;
#endif
    static int frameIndex = -1;
    frameIndex++;
    if (frameIndex >= CACHE_BUF_NUM)
    {
        frameIndex = 0;
    }

    //memcpy(gBuf[frameIndex].yBuf, yBuf, width * height);
    gBuf[frameIndex].yBuf = (unsigned char*) yBuf;
    gBuf[frameIndex].bufNo = frameNo;

#if DAY_OR_NIGHT
    if(frameNo < 1000 || frameNo % 10 == 0)
    {
        resultDayOrNight = mvDayOrNight(gBuf[frameIndex].yBuf, FRAME_WID, FRAME_HIG);   
    }
#endif

    pthread_mutex_lock(&gBufMutex);
    gSpeed = speed;
#if DAY_OR_NIGHT
    if(resultDayOrNight >= 6)
    {
        gDayOrNight = 0;
    }
    else if(resultDayOrNight <= -6)
    {
        gDayOrNight = 1;
    }
#endif
    gBufIn = frameIndex;
    //pthread_cond_signal(&gBufCond);
    pthread_mutex_unlock(&gBufMutex);
#if DAY_OR_NIGHT
    if(frameNo % 600 == 0)
    {
        printf("day or night value: %d\n", resultDayOrNight);
        printf("day or night: %d\n", gDayOrNight);
    }
#endif
    //usleep(100000);

    return ReVal;
}

int SE_GetResult(SE_UI_RESULT_s** result)
{
    int ReVal = 0;
    //if (gpLDOutput->alarm_result > 0)
    //    printf("gpLDOutput->alarm_result: %d\n",gpLDOutput->alarm_result);

    *result = gpUIResult;

    gpUIResult->tdmapWarnNum = 0;
    gpUIResult->ldCount = 0;
    gpUIResult->fcCount = 0;
    gpUIResult->detCount = 0;
    gpUIResult->tdmapCount = 0;
    gpUIResult->ttc = -1.0f;
    gpUIResult->leftRange = 0.0;
    gpUIResult->rightRange = 0.0;
    memset(gpUIResult->ldResult, 0, sizeof(LD_UI_RESULT_s) * LD_RESULT_MAX);
    memset(gpUIResult->fcResult, 0, sizeof(FC_UI_RESULT_s) * FC_RESULT_MAX);

#if LDWS
    pthread_mutex_lock(&gLDMutex);
    if (NULL != gpLDOutput && gpLDOutput->Route != 0)
    {
        int i = 0, z = 0;
        int startNum = 0, zStart = 0, zEnd = 0;
        int confiIndex = 0, status = 0;

        switch (gpLDOutput->Route)
        {
            case 1:
                zStart = 0;
                zEnd = 2;
                if (gpLDOutput->Route_half == 0)
                {
                    confiIndex = 0;
                }
                else
                {
                    confiIndex = 1;
                }
                break;
            case 2:
                zStart = 0;
                zEnd = 1;
                confiIndex = 2;
                break;
            case 3:
                zStart = 1;
                zEnd = 2;
                confiIndex = 3;
                break;
            default:
                zStart = 0;
                zEnd = 2;
                break;
        }

        //left and right two line
        for (z = zStart; z < zEnd; z++)
        {
            if (gpLDOutput->Confidence_detection[confiIndex]
                    > gpLDOutput->Confidence - 3)
            {
                //is warnning?
                if (gpLDOutput->alarm_result - 1 == z)
                {
                    status = (z ? LD_WARN_RIGHT : LD_WARN_LEFT);
                } else {
                    status = (z ? LD_SAFE_RIGHT : LD_SAFE_LEFT);
                }
                if (z == 0)
                {
                    gpUIResult->leftRange = gpLDOutput->Param[1];
                } else {
                    gpUIResult->rightRange = gpLDOutput->Param[0]
                        - gpLDOutput->Param[1];
                }
            } else {
                status = LD_UNKNOW;
                continue;
            }

            if (gpLDOutput->Route == 1 && gpLDOutput->Route_half)
            {
                startNum = 3;
            } else {
                startNum = 0;
            }

            for (i = startNum; i < gpLDOutput->NB_INTERVALLES; i++)
            {
                gpUIResult->ldResult[gpUIResult->ldCount].point.x =
                    gpLDOutput->pCaPoint[z * gpLDOutput->NB_INTERVALLES + i].x;
                gpUIResult->ldResult[gpUIResult->ldCount].point.y =
                    gpLDOutput->pCaPoint[z * gpLDOutput->NB_INTERVALLES + i].y;
                gpUIResult->ldResult[gpUIResult->ldCount].position = z;
                gpUIResult->ldResult[gpUIResult->ldCount].width = drawLDWidth[gpUIResult->ldResult[gpUIResult->ldCount].point.y];
                gpUIResult->ldResult[gpUIResult->ldCount].status = status;
                gpUIResult->ldCount++;
            }
        }
    }
    pthread_mutex_unlock(&gLDMutex);
#endif


#if FCWSD
    pthread_mutex_lock(&gFCMutex);
    if (NULL != gpFCWSDOutput && 0 != gpFCWSDOutput->nObjectNum)
    {
        int i = 0;
        for (i = 0; i < gpFCWSDOutput->nObjectNum; i++)
        {
            objectCar* car = gpFCWSDOutput->objects + i;
            gpUIResult->detResult[i].point.x = car->objectRect.point.x;
            gpUIResult->detResult[i].point.y = car->objectRect.point.y;
            gpUIResult->detResult[i].wid = car->objectRect.size.width;
            gpUIResult->detResult[i].hig = car->objectRect.size.height;
            gpUIResult->detCount++;
        }
    }
    pthread_mutex_unlock(&gFCMutex);
#endif

#if FCWST
    pthread_mutex_lock(&gFCTMutex);
    if (NULL != gpFCWSTOutput && 0 != gpFCWSTOutput->nTrackeNum)
    {
        int i = 0;

        for (i = 0; i < gpFCWSTOutput->nTrackeNum; i++)
        {
            trakobj *pGroup = gpFCWSTOutput->pTrackerset + i;

            if (!pGroup->bTrue){
                continue;
                //gpUIResult->fcResult[gpUIResult->fcCount].status = FC_UNKNOW;
            }else{
                if (0 < pGroup->dTTC && 0.8 > pGroup->dTTC
                        && (1
                            == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath))
                {
                    gpUIResult->fcResult[gpUIResult->fcCount].status = FC_HMW;
                    gpUIResult->ttc = pGroup->dTTC;
                } else if (0 < pGroup->fTTC && gTTC > pGroup->fTTC
                        && (1
                            == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath))
                {
                    gpUIResult->fcResult[gpUIResult->fcCount].status = FC_WARN;
                    gpUIResult->ttc = pGroup->fTTC;
                } else {
                    if (2
                            == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath)
                    {
                        gpUIResult->fcResult[gpUIResult->fcCount].status = FC_LEFT;
                    } else if (1
                            == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath)
                    {
                        gpUIResult->fcResult[gpUIResult->fcCount].status = FC_MID;
                        gpUIResult->ttc = pGroup->fTTC;
                    } else if (3
                            == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath)
                    {
                        gpUIResult->fcResult[gpUIResult->fcCount].status = FC_RIGHT;
                    } else {
                        gpUIResult->fcResult[gpUIResult->fcCount].status = FC_UNKNOW;
                    }
                }
            }

            gpUIResult->fcResult[gpUIResult->fcCount].point.x = pGroup->objRec.x
                * 2;
            gpUIResult->fcResult[gpUIResult->fcCount].point.y = pGroup->objRec.y
                * 2;
            gpUIResult->fcResult[gpUIResult->fcCount].wid = pGroup->objRec.width
                * 2;
            gpUIResult->fcResult[gpUIResult->fcCount].hig =
                pGroup->objRec.height * 2;
            gpUIResult->fcCount++;

        }
    }
    pthread_mutex_unlock(&gFCTMutex);
#endif //FCWST

#if TDMAP
    pthread_mutex_lock(&gTDMAPMutex);
    if (0 != gDrawFrences.nFenceNum)
    {
        gpUIResult->tdmap = (TDMAP_UI_RESULT_t*)gDrawFrences.frences;
        gpUIResult->tdmapCount = gDrawFrences.nFenceNum;
        gpUIResult->tdmapWarnNum = gDrawFrences.nWarningNum; 
        //printf("fenceNum: %d\n", gDrawFrences.nFenceNum);
    }
    pthread_mutex_unlock(&gTDMAPMutex);

#endif //TDMAP

    return ReVal;
}

int SE_GetTipLine(TIP_LINE_s** line)
{
    int ReVal = 0;

#if LDWS
    *line = &gTipLine;
#endif

    return ReVal;
}

int SE_Stop()
{
    int ReVal = 0;

#if LDWS
    gIsLDWExit = 1;
#endif
#if FCWSD
    gIsFCWSDExit = 1;
#endif
#if FCWST
    gIsFCWSTExit = 1;
#endif
#if OBJVERF
    gIsObjVerfWExit = 1;
#endif //OBJVERF
#if TDMAP
    gIsTDMAPExit = 1;
#endif

    return ReVal;
}

int SE_Release()
{
    int ReVal = 0;
    void* re = NULL;

#if TDMAP
    if(0 < gTDMAPTid)
    {
        pthread_join(gTDMAPTid, &re);
    }            
    freeTDMAP();
    pthread_mutex_destroy(&gTDMAPMutex);
#endif
#if OBJVERF
    if(0 < gObjVerfTid)
    {
        pthread_join(gObjVerfTid, &re);
    }
#endif
#if FCWST
    if(0 < gFCTTid)
    {
        pthread_join(gFCTTid, &re);
    }
#endif

#if OBJVERF
    freeObjVerf();
    pthread_mutex_destroy(&gObjVerfInputMutex);
    pthread_mutex_destroy(&gObjVerfOutputMutex);
#endif
#if FCWST
    freeFCWST();
    pthread_mutex_destroy(&gFCTMutex);
#endif
#if FCWSD
    if(0 < gFCDTid)
    {
        pthread_join(gFCDTid, &re);
    }
    pthread_mutex_destroy(&gFCMutex);
    freeFCWSD();
#endif
#if LDWS
    if(0 < gLDWTid)
    {
        pthread_join(gLDWTid, &re);
    }        
    pthread_mutex_destroy(&gLDMutex);
    freeLDW();
#endif

    my_free(gMemStartAddr);
    gMemStartAddr = NULL;
    pthread_cond_destroy(&gBufCond);
    pthread_mutex_destroy(&gBufMutex);
#if LDWS&&FCWSD
    pthread_mutex_destroy(&gLD4FCMutex);
#endif

#if FCWSD_CNN || OBJVERF_CNN
    cnn_release();
    printf("cnn release\n");
    cnn_after();
#endif

    return ReVal;
}

int startThread(const char* name, void *(*thread_func)(void*), pthread_t *tId)
{
    int ReVal = 0;

    if (pthread_create(tId, NULL, thread_func, NULL) != 0)
        return -1;
    //printf("create %s thread success! id: %u\n", name, *tId);
    if (pthread_setname_np(*tId, name) != 0)
        printf("set thread name fail, name : %s\n", name);

    return ReVal;
}

#if LDWS
static int initLDW(const char* path1, const char* path2)
{
    int ReVal = 0;

    gIsLDWExit = 0;
    LDWS_AllocModel();
    LDWS_Init(path1, path2);
    LDWS_GetResult(&gpLDOutput);
    //left
    for (int index = 0; index < gpLDOutput->NB_INTERVALLES; index++)
    {
        drawLDWidth[gpLDOutput->pCaPoint[index].y] = (int)(LDWS_GetXLengthofImage(0.15, gpLDOutput->pCaPoint[index].y) * 0.5 + 0.5);
    }
    return ReVal;
}

static void* runLDW(void* arg)
{

    int proCount = 0;

    unsigned char frameIn = 0;
    unsigned char tmpFrameIn = 0;
#if CONTRL_FPS
    unsigned int curTime = 0;
    unsigned int proTime = 0;
#endif

#if SHOW_TIME
    unsigned int timeCount = 0;
    unsigned int algTime = 0;
#endif
    //float speed = -1.0f;
    //	pthread_mutex_t mutex;
    //	pthread_mutex_init(&mutex, NULL);
    unsigned long mask = LDWS_THREAD_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("LDW set cpu core fail!\n");
    }

    while (!gIsLDWExit)
    {
        pthread_mutex_lock(&gBufMutex);
        //speed = gSpeed; 
        tmpFrameIn = gBufIn;
        pthread_mutex_unlock(&gBufMutex);

        /*
        if(speed > 0 && speed < LD_START_SPEED){
            gpLDOutput->Route = 0;
            sleep(1);
            continue;
        }
        */

        if (tmpFrameIn != frameIn)
        {
            proCount++;
#if SHOW_TIME
            timeCount = my_gettime();
#endif
            LDWS_Tracker(gBuf[tmpFrameIn].yBuf);
#if FCWSD
            pthread_mutex_lock(&gLD4FCMutex);
#endif
            pthread_mutex_lock(&gLDMutex);
            LDWS_GetResult(&gpLDOutput);
            pthread_mutex_unlock(&gLDMutex);
#if FCWSD
            pthread_mutex_unlock(&gLD4FCMutex);
#endif
#if SHOW_TIME
            algTime += (my_gettime() - timeCount);
#endif
            if (proCount >= PRINT_FRAME_RANGE)
            {
#if SHOW_TIME
                printf("LDWS process %d frame -- cost time : %d ms route: %d\n",
                        proCount, algTime,gpLDOutput->Route);
                algTime = 0;
#endif
                proCount = 0;
            }
            //SE_GetResult(gBuf[tmpFrameIn].bufNo);

            frameIn = tmpFrameIn;
#if CONTRL_FPS
            curTime = my_gettime();
            if (curTime - proTime < LDWS_GAP)
                usleep((LDWS_GAP - (curTime - proTime)) * 1000);
            proTime = my_gettime();
#endif


        }
        else
        {
            usleep(1000);
            //			pthread_mutex_lock(&mutex);
            //			pthread_cond_wait(&gBufCond, &mutex);
            //			pthread_mutex_unlock(&mutex);
        }
    }
    //	pthread_mutex_destroy(&mutex);

    return NULL;
}

static int freeLDW(void)
{
    int ReVal = 0;

    LDWS_Finalization();
    if(gpLDOutput != NULL)
        LDWS_FreeResult(&gpLDOutput);
    if(gpLDGuid != NULL)
        LDWS_Freeinit(&gpLDGuid);

    printf("LDW release\n");
    return ReVal;
}

static int InitTipLine()
{
    int ReVal = 0;

    LDWS_Getinit(&gpLDGuid);
    memcpy(gTipLine.points, gpLDGuid->pBoundPoint,
            sizeof(LDWS_Point) * 7);
    int vanishY = LDWS_GetVanishY();
    gTipLine.points[7].x = 0;
    gTipLine.points[7].y = vanishY;
    gTipLine.points[8].x = FRAME_WID;
    gTipLine.points[8].y = vanishY;
    gTipLine.num = TIP_LINE_NUM;

    return ReVal;
}
#endif //LDWS

#if FCWSD
static int initFCWSD(const char* dayPath, const char* nightPath)
{
    const char* carDayModelFile1 = "/wissen/car_day_1.txt";
    const char* carNightModelFile1 = "/wissen/car_night_1.txt";
    int ReVal = 0;
    gIsFCWSDExit = 0;
    gFCWSDParam.srcROIYFactor = 0.3;
    gFCWSDParam.scalingFactor = 1.2;
    gFCWSDParam.eps = 0.2;
    gFCWSDParam.srcWidth = FRAME_WID;
    gFCWSDParam.srcHeight = FRAME_HIG;
    gFCWSDParam.fixedTaskNum = 5000;
    gFCWSDParam.startMFactor = 1;
    gFCWSDParam.useFixedTaskNum = 1.0;
    gFCWSDParam.roi.point.x = 0;
    gFCWSDParam.roi.point.y = gFCWSDParam.srcROIYFactor * FRAME_HIG;
    gFCWSDParam.roi.size.width = FRAME_WID;
    gFCWSDParam.roi.size.height = FRAME_HIG - gFCWSDParam.roi.point.y;

#if LDWS
    FCWSD_Init(FCWSD_DAY_HANDLE_ID, &gFCWSDParam, gpLDOutput, 0, carDayModelFile1);
    FCWSD_Init(FCWSD_NIGHT_HANDLE_ID, &gFCWSDParam, gpLDOutput, 0, carNightModelFile1);
#else
    FCWSD_Init(FCWSD_DAY_HANDLE_ID, &gFCWSDParam, NULL, NULL, carDayModelFile1);
    FCWSD_Init(FCWSD_NIGHT_HANDLE_ID, &gFCWSDParam, NULL, NULL, carNightModelFile1);
#endif

#if FCWSD_CNN == 1
    MuliObjVerfInit(CNN_IMAGE_WIDTH, CNN_IMAGE_HEIGHT, 1);
    FCWSDObjectResize = (float*)(malloc(CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float)));
    memset(FCWSDObjectResize, 0, CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float));
    cnn_init(0);
    cnn_init(1);
#endif

#if OBJRLCT == 1
    gFCWSDParam.startMFactor = 1.0;
    FCWSD_Init(OBJRLCT_DAY_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, dayPath);
    FCWSD_Init(OBJRLCT_NIGHT_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, nightPath);
#endif
    return ReVal;
}

static void* runFCWSD(void* arg)
{

    int proCount = 0;
#if CONTRL_FPS
    unsigned int curTime = 0;
    unsigned int proTime = 0;
#endif
#if SHOW_TIME
    unsigned int timeCount = 0;
    unsigned int algTime = 0;
#endif

    unsigned int locationFrameNum = 0;
    unsigned char frameIn = 0;
    unsigned char tmpFrameIn = 0;
    unsigned char dayOrNight = 0;

    //const int th = LDWS_GetFCWSD_th();
    FCWSDImage fcwsdORIImg = { 0 };
    FCWSDSize fcwsdMinSize = { 0 };
    FCWSDSize fcwsdMaxSize = { 0 };

    fcwsdMinSize.width = 30;
    fcwsdMinSize.height = 30;
    fcwsdMaxSize.width = 800;
    fcwsdMaxSize.height = 800;

    fcwsdORIImg.nWid = FRAME_WID;
    fcwsdORIImg.nHig = FRAME_HIG;

#if OBJRLCT == 1
    FCWSDImage locationImage = {0};
    locationImage.nWid = FRAME_WID;
    locationImage.nHig = FRAME_HIG;
#endif

#if LDWS
#else
    FCWSDRect fcwsdRect = {0}; 
    fcwsdRect.point.x = 0;
    fcwsdRect.point.y = 300;
    fcwsdRect.size.width = FRAME_WID;
    fcwsdRect.size.height = 300;
#endif

    //	pthread_mutex_t mutex;
    //	pthread_mutex_init(&mutex, NULL);
    unsigned long mask = FCWSD_THREAD_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("LDW set cpu core fail!\n");
    }
#if CONTRL_SWITCH_FUN
    float speed = 0.0f;
#endif
    while (!gIsFCWSDExit)
    {
        pthread_mutex_lock(&gBufMutex);
        tmpFrameIn = gBufIn;
#if CONTRL_SWITCH_FUN
        speed = gSpeed;
#endif
	dayOrNight = gDayOrNight;
        pthread_mutex_unlock(&gBufMutex);

#if CONTRL_SWITCH_FUN
        if(speed < 0.0f ||  speed < SPEED_TO_RUN)
        {
            usleep(200000);
            continue;
        }
#endif

        if (frameIn != tmpFrameIn)
        {
            proCount++;
#if SHOW_TIME
            timeCount = my_gettime();
#endif
            fcwsdORIImg.ptr = gBuf[tmpFrameIn].yBuf;
	    locationFrameNum = gBuf[tmpFrameIn].bufNo;
#if LDWS
            pthread_mutex_lock(&gLD4FCMutex);
            //if(gpLDOutput->Route  == 0)
            //{
            //    memcpy(gpLDInputPoints, gpLDGuid->pCaPoint,
            //            sizeof(LDWS_Point) * gpLDGuid->NB_INTERVALLES
            //            * gpLDGuid->NB_BANDES);
            //}else{
            memcpy(gpLDInputPoints, gpLDOutput->pCaPoint,
                        sizeof(LDWS_Point) * gpLDOutput->NB_INTERVALLES
                        * gpLDOutput->NB_BANDES);
            //}
            pthread_mutex_unlock(&gLD4FCMutex);

	    if(dayOrNight == 0)
	    {
		FCWSD_Processor(FCWSD_DAY_HANDLE_ID, &fcwsdORIImg, &gLDInput, NULL,
                    NULL, &fcwsdMinSize, &fcwsdMaxSize, 6,
                    FCWSD_THREAD_MAX_RT);
	    }
	    else if(dayOrNight == 1)
	    {
		FCWSD_Processor(FCWSD_NIGHT_HANDLE_ID, &fcwsdORIImg, &gLDInput, NULL,
                    NULL, &fcwsdMinSize, &fcwsdMaxSize, 21,
                    FCWSD_THREAD_MAX_RT);
	    }

#else

	    if(dayOrNight == 0)
	    {
		FCWSD_Processor(FCWSD_DAY_HANDLE_ID, &fcwsdORIImg, NULL, NULL, &fcwsdRect, &fcwsdMinSize, &fcwsdMaxSize, 6, FCWSD_THREAD_MAX_RT);
	    }
	    else if(dayOrNight == 1)
	    {
		FCWSD_Processor(FCWSD_NIGHT_HANDLE_ID, &fcwsdORIImg, NULL, NULL, &fcwsdRect, &fcwsdMinSize, &fcwsdMaxSize, 21, FCWSD_THREAD_MAX_RT);
	    }
#endif

#if FCWSD_CNN == 1
            int objectCount = 0;
            int objectIndex = 0;
            unsigned char CNNResult = 0;
            objectSetsCar* FCWSDResult = NULL;
            if(dayOrNight == 0)
            {
                if(dayOrNight == 0)
                {
                    getDetectObject(FCWSD_DAY_HANDLE_ID, &FCWSDResult);
                }
                else if(dayOrNight == 1)
                {
                    getDetectObject(FCWSD_NIGHT_HANDLE_ID, &FCWSDResult);
                }       
                for(objectIndex = 0; objectIndex < FCWSDResult->nObjectNum; objectIndex++)
                {
                    objectCar object = FCWSDResult->objects[objectIndex];
                    resizeFCWSDObject(CNN_IMAGE_WIDTH, CNN_IMAGE_HEIGHT, &fcwsdORIImg, object, FCWSDObjectResize);
                    cnn_main(dayOrNight, FCWSDObjectResize, &CNNResult);
                    if(CNNResult > 0)
                    {
                        FCWSDResult->objects[objectCount] = object;
                        objectCount++;
                    }
                }
                FCWSDResult->nObjectNum = objectCount;
            }
#endif

#if OBJRLCT
            pthread_mutex_lock(&gBufMutex);
            locationImage.ptr = gBuf[gBufIn].yBuf;
	    locationFrameNum = gBuf[gBufIn].bufNo;
            pthread_mutex_unlock(&gBufMutex);
            detcorByRec(locationImage, dayOrNight);
#endif

            pthread_mutex_lock(&gFCMutex);
	    if(dayOrNight == 0)
	    {
	        FCWSD_GetResult(FCWSD_DAY_HANDLE_ID, &gpFCWSDOutput);
	    }
	    else if(dayOrNight == 1)
	    {
		FCWSD_GetResult(FCWSD_NIGHT_HANDLE_ID, &gpFCWSDOutput);
	    }
            gpFCWSDOutput->frameNumber = locationFrameNum;
            pthread_mutex_unlock(&gFCMutex);

#if SHOW_TIME
            algTime += my_gettime() - timeCount;
#endif
            if (proCount >= PRINT_FRAME_RANGE)
            {
#if SHOW_TIME
                printf("FCWSD process %d frame -- cost time : %d ms\n",
                        proCount, algTime);
                algTime = 0;
#endif
                proCount = 0;
            }
            frameIn = tmpFrameIn;
#if CONTRL_FPS
            curTime = my_gettime();
            if (curTime - proTime < FCWSD_GAP)
                usleep((FCWSD_GAP - (curTime - proTime)) * 1000);
            proTime = my_gettime();
#endif

        }
        else
        {
            usleep(1000);
            //			pthread_mutex_lock(&mutex);
            //			pthread_cond_wait(&gBufCond, &mutex);
            //			pthread_mutex_unlock(&mutex);
        }
    }
    //	pthread_mutex_destroy(&mutex);

    return NULL;
}

static int freeFCWSD(void)
{
    int ReVal = 0;

    FCWSD_Free(FCWSD_DAY_HANDLE_ID);
    FCWSD_Free(FCWSD_NIGHT_HANDLE_ID);
    if(gpFCWSDOutput != NULL)
        FCWSD_FreeResult(&gpFCWSDOutput);

#if FCWSD_CNN == 1
    if(FCWSDObjectResize != NULL)
    {
        free(FCWSDObjectResize);
        FCWSDObjectResize = NULL;
    }
    MuliObjVerfRelease(1);
#endif

#if OBJRLCT == 1
    FCWSD_Free(OBJRLCT_DAY_HANDLE_ID);
    FCWSD_Free(OBJRLCT_NIGHT_HANDLE_ID);
#endif

    printf("FCWSD release\n");
    return ReVal;
}
#endif //FCWSD

#if FCWST
static int initFCWST(const char* dayPath, const char* nightPath)
{
    int ReVal = 0;

    gIsFCWSTExit = 0;
    AdaSize sizeImg;
    sizeImg.width = FRAME_WID >> 1;
    sizeImg.height = FRAME_HIG >> 1;

    //initShadowDetection(FRAME_WID, FRAME_HIG, 0.5f);

    gFCWSDParam.startMFactor = 1.0;

    FCWSD_Init(FCWST_DAY_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, dayPath);
    FCWSD_Init(FCWST_NIGHT_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, nightPath);
	
    /*
       gFCWSDParam.srcROIYFactor = 0;

#if LDWS
FCWSD_Init(FCWST_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, path);
#else
FCWSD_Init(FCWST_HANDLE_ID, &gFCWSDParam, NULL, NULL, path);
#endif
*/
    FCW_TRACK_Init_adas_global_param(sizeImg);

    return ReVal;
}

static void* runFCWST(void* arg)
{

    int proCount = 0;
#if CONTRL_FPS
    unsigned int curTime = 0;
    unsigned int proTime = 0;
#endif
#if SHOW_TIME
    unsigned int timeCount = 0;
    unsigned int algTime = 0;
#endif
    int fcwsdIndex = -1;
    unsigned char frameIn = 0;
    unsigned char tmpFrameIn = 0;
    float speed = -1.0f;
    unsigned char dayOrNight = 0;

    int trackNum = 0;
    AdasRect objRec[32];
    float fzoomRate = 2.0;
    unsigned char* resize_img = NULL;
#if READ_YUV_DATA
    unsigned char* vImageData = NULL;
#endif
    int i = 0, j = 0;
    PortInput InPutParam =
    { 0 };
    imgage fcwst_ori_img =
    { 0 };

    fcwst_ori_img.nWid = FRAME_WID;
    fcwst_ori_img.nHig = FRAME_HIG;
    resize_img = (unsigned char*) my_malloc(FRAME_WID * FRAME_HIG >> 2);
#if READ_YUV_DATA
    vImageData = (unsigned char*) my_malloc(FRAME_WID * FRAME_HIG);
#endif

    //	pthread_mutex_t mutex;
    //	pthread_mutex_init(&mutex, NULL);
    unsigned long mask = FCWST_THREAD_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("LDW set cpu core fail!\n");
    }
    while (!gIsFCWSTExit)
    {

        pthread_mutex_lock(&gBufMutex);
        tmpFrameIn = gBufIn;
        speed = gSpeed;
	dayOrNight = gDayOrNight;
        pthread_mutex_unlock(&gBufMutex);

#if CONTRL_SWITCH_FUN
        if(speed < 0.0f || speed < SPEED_TO_RUN)
        {

            pthread_mutex_lock(&gFCTMutex);
            if(gpFCWSTOutput)
                gpFCWSTOutput->nTrackeNum = 0;
            pthread_mutex_unlock(&gFCTMutex);

            usleep(200000);
            continue;
        }
#endif

        if (tmpFrameIn != frameIn
                && ((NULL != gpFCWSDOutput && 0 != gpFCWSDOutput->nObjectNum)
                    || (NULL != gpFCWSTOutput
                        && 0 != gpFCWSTOutput->nTrackeNum)))
        {
            int startIndexSrc = 0;
            int startIndexDes = 0;
            proCount++;

#if SHOW_TIME
            timeCount = my_gettime();
#endif

#if OBJVERF
            pthread_mutex_lock(&gObjVerfOutputMutex);
            getMuliObjVerf();
            pthread_mutex_unlock(&gObjVerfOutputMutex);
            //printf("getMuliObjVerf  cost time: %d\n",(my_gettime()-timeCount));
#endif //OBJVERF

            fcwst_ori_img.ptr = gBuf[tmpFrameIn].yBuf;

            if (NULL != gpFCWSDOutput && 0 != gpFCWSDOutput->nObjectNum && fcwsdIndex != gpFCWSDOutput->frameNumber)
            {
                int m = 0;
                trackNum = 0;
                for (m = 0; m < gpFCWSDOutput->nObjectNum; m++)
                {
                    if (gpFCWSDOutput->objects)
                    {
                        if (gpFCWSDOutput->objects[m].confidence < 0)
                            continue;

                        objRec[trackNum].x =
                            (int) (gpFCWSDOutput->objects[m].objectRect.point.x
                                    / fzoomRate);
                        objRec[trackNum].y =
                            (int) (gpFCWSDOutput->objects[m].objectRect.point.y
                                    / fzoomRate);
                        objRec[trackNum].width =
                            (int) (gpFCWSDOutput->objects[m].objectRect.size.width
                                    / fzoomRate);
                        objRec[trackNum].height =
                            (int) (gpFCWSDOutput->objects[m].objectRect.size.height
                                    / fzoomRate);
                        objRec[trackNum].confidence =
                            gpFCWSDOutput->objects[m].confidence;
                        objRec[trackNum].nType = Car;
                        trackNum++;
                    }
                    else
                    {
                        printf("gpFCWSDOutput->objects is NULL \n");
                    }
                }
                InPutParam.nRecNum = trackNum;
                fcwsdIndex = gpFCWSDOutput->frameNumber;
            }
            else
            {
                InPutParam.nRecNum = 0;
            }

            for (i = 0; i < (FRAME_HIG >> 1); i++)
            {
                startIndexSrc = (i << 1) * FRAME_WID;
                startIndexDes = i * (FRAME_WID >> 1);
                for (j = 0; j < (FRAME_WID >> 1); j++)
                {
                    resize_img[startIndexDes + j] =
                    fcwst_ori_img.ptr[startIndexSrc + (j << 1)];
                }
            }

#if READ_YUV_DATA
            readUVImage(gBuf[tmpFrameIn].yBuf, FRAME_WID, FRAME_HIG, vImageData);
	    InPutParam.pOriGrayfram.pvtr = vImageData;
#endif
            //printf("resize  cost time: %d\n",(my_gettime()-timeCount));
            InPutParam.pGrayfram = resize_img;
            InPutParam.nFramSeq = gBuf[tmpFrameIn].bufNo;
            InPutParam.pOriGrayfram.ptr = fcwst_ori_img.ptr;
            InPutParam.pOriGrayfram.nWid = fcwst_ori_img.nWid;
            InPutParam.pOriGrayfram.nHig = fcwst_ori_img.nHig;
	    InPutParam.pOriGrayfram.CANData.fSpeed = speed;
	    InPutParam.pOriGrayfram.dayOrNight = dayOrNight;
            //printf("FCWST frame number:%d\n", gBuf[tmpFrameIn].bufNo);
            //InPutParam.pOriGrayfram.groundValue = computeShadow(&InPutParam.pOriGrayfram);
            //printf("computeShadow  cost time: %d\n",(my_gettime()-timeCount));

            curTime = my_gettime();
            InPutParam.objTime.wHour = 0;
            InPutParam.objTime.wMin = 0;
            InPutParam.objTime.wSec = 0;
            InPutParam.objTime.wMilSec = curTime;

            InPutParam.objRec = objRec;

            FCW_TRACK_MultieTrack(&InPutParam);
            //printf("FCW_TRACK_MultieTrack  cost time: %d\n",(my_gettime()-timeCount));

            pthread_mutex_lock(&gFCTMutex);
            FCW_TRACK_GetResult(&gpFCWSTOutput);
            pthread_mutex_unlock(&gFCTMutex);

#if OBJVERF
            pthread_mutex_lock(&gObjVerfInputMutex);
            FCW_TRACK_GetResult(&gpFCWSTInput);
            pthread_mutex_unlock(&gObjVerfInputMutex);
#endif //OBJVERF

            if (NULL != gpFCWSTOutput && 0 != gpFCWSTOutput->nTrackeNum)
            {
                int i = 0;

                for (i = 0; i < gpFCWSTOutput->nTrackeNum; i++)
                {
                    trakobj *pGroup = gpFCWSTOutput->pTrackerset + i;

                    /*if (!pGroup->bTrue){
                        continue;
                    }else*/ if (1 == pGroup->pMotion[(pGroup->nMotionLeng - 1) & 0x3F].bInCollishionPath)
                    {
                        printf("FCWST TTC: %d,%.2f,%ld,%.2f\n", gBuf[tmpFrameIn].bufNo, speed, pGroup->nId, pGroup->fTTC);
                    }
                }
            }


#if SHOW_TIME
            algTime += my_gettime() - timeCount;
#endif
            if (proCount >= PRINT_FRAME_RANGE)
            {
                
#if SHOW_TIME
                printf("FCWST process %d frame -- cost time : %d ms\n",
                        proCount, algTime);
                algTime = 0;
#endif
                proCount = 0;
            }
            frameIn = tmpFrameIn;
#if CONTRL_FPS
            curTime = my_gettime();
            if (curTime - proTime < FCWST_GAP)
                usleep((FCWST_GAP - (curTime - proTime)) * 1000);
            proTime = my_gettime();
#endif
        }
        else
        {
            usleep(1000);
            //			pthread_mutex_lock(&mutex);
            //			pthread_cond_wait(&gBufCond, &mutex);
            //			pthread_mutex_unlock(&mutex);
        }
    }
    free(resize_img);
    resize_img = NULL;
#if READ_YUV_DATA
    free(vImageData);
    vImageData = NULL;
#endif
    //	pthread_mutex_destroy(&mutex);

    return NULL;
}

static int freeFCWST(void)
{
    int ReVal = 0;

    //freeShadowDetection();
    FCWSD_Free(FCWST_DAY_HANDLE_ID);
    FCWSD_Free(FCWST_NIGHT_HANDLE_ID);

    FCW_TRACK_mvClearWholeGroup();
    FCW_TRACK_Unit_adas_global_param();

    printf("FCWST release\n");
    return ReVal;
}

#if READ_YUV_DATA
static void readUVImage(const unsigned char* yuvImage, const int width, const int height, unsigned char* vImage)
{
    int row = 0;
    int col = 0;
    const int rowWidth = 2 * width;
    const unsigned char *uvData = yuvImage + 2 * width * height;
    unsigned char *vRow = vImage;
    unsigned char *vNextRow = vImage + width;
    for (row = 0; row < height; row += 2)
    {
        for (col = 0; col < width; col += 2)
        {
            uvData++;

            vRow[col] = *uvData;
            vRow[col+1] = *uvData;
            vNextRow[col] = *uvData;
            vNextRow[col+1] = *uvData;

            uvData++;
        }
        vRow += rowWidth;
        vNextRow += rowWidth;
    }
}
#endif
#endif //FCWST

#if OBJVERF
static int initObjVerf(const char* dayPath, const char* nightPath)
{
#if 1
    int objverfIndex = 0;
    for(objverfIndex = 0; objverfIndex < MAX_TRACK_MUM; objverfIndex++)
    {
        objVerf[objverfIndex].objData = (float*)(malloc(CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float)));
        memset(objVerf[objverfIndex].objData, 0, CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float));
    }
#if LDWS
    FCWSD_Init(OBJVERF_DAY_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, dayPath);
#else
    FCWSD_Init(OBJVERF_DAY_HANDLE_ID, &gFCWSDParam, NULL, NULL, dayPath);
#endif
#else
#if OBJVERF_CNN == 1
    cnn_init(CNN_OBJVERF_ID);
    int objverfIndex = 0;
    for(objverfIndex = 0; objverfIndex < MAX_TRACK_MUM; objverfIndex++)
    {
        objVerf[objverfIndex].objData = (float*)(malloc(CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float)));
        memset(objVerf[objverfIndex].objData, 0, CNN_IMAGE_WIDTH * CNN_IMAGE_HEIGHT * sizeof(float));
    }
#else
#if LDWS
    FCWSD_Init(OBJVERF_DAY_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, dayPath);
    FCWSD_Init(OBJVERF_NIGHT_HANDLE_ID, &gFCWSDParam, gpLDOutput, NULL, nightPath);
#else
    FCWSD_Init(OBJVERF_DAY_HANDLE_ID, &gFCWSDParam, NULL, NULL, dayPath);
    FCWSD_Init(OBJVERF_NIGHT_HANDLE_ID, &gFCWSDParam, NULL, NULL, nightPath);
#endif
#endif
#endif

    MuliObjVerfInit(CNN_IMAGE_WIDTH, CNN_IMAGE_HEIGHT, 0);
    gIsObjVerfWExit = 0;
    gpFCWSTInput = NULL;

    gFCWSDParam.startMFactor = 1.0;

    return 0;
}

static void* runObjVerf(void* arg)
{
    int proCount = 0;
#if CONTRL_FPS
    unsigned int curTime = 0;
    unsigned int proTime = 0;
#endif
#if SHOW_TIME
    unsigned int timeCount = 0;
    unsigned int algTime = 0;
#endif
    unsigned int frameIn = 0;
    unsigned int tmpFrameIn = 0;
	unsigned char dayOrNight = 0;

    int objverfIndex = 0;
    int objverfBufIndex = 0;
    float fzoomRate = 2.0;
    int objVerfNum = 0;
    MuliVerfobj pVerfobjInput;

#if CONTRL_SWITCH_FUN
    float speed = 0.0f;
#endif

    unsigned long mask = OBJVERF_THREAD_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("OBJVERF set cpu core fail!\n");
    }
    while (!gIsObjVerfWExit)
    {
        

        pthread_mutex_lock(&gBufMutex);
#if CONTRL_SWITCH_FUN
        speed = gSpeed;
#endif
		dayOrNight = gDayOrNight;
        pthread_mutex_unlock(&gBufMutex);
#if CONTRL_SWITCH_FUN
        if(speed < 0.0f || speed < SPEED_TO_RUN)
        {
            usleep(200000);
            continue;
        }
#endif
        objVerfNum = 0;
        pthread_mutex_lock(&gObjVerfInputMutex);
        if (gpFCWSTInput != NULL && gpFCWSTInput->nTrackeNum > 0)
        {
            tmpFrameIn = gpFCWSTInput->pTrackerset[0].nFramSeq;
            if(tmpFrameIn != frameIn)
            {
                for (objverfIndex = 0; objverfIndex < gpFCWSTInput->nTrackeNum; objverfIndex++)
                {
                    if (gpFCWSTInput->pTrackerset[objverfIndex].bTrue == 1)
                    {
                        continue;
                    }
                    objVerf[objVerfNum].objRec.x = gpFCWSTInput->pTrackerset[objverfIndex].objRec.x * fzoomRate;
                    objVerf[objVerfNum].objRec.y = gpFCWSTInput->pTrackerset[objverfIndex].objRec.y * fzoomRate;
                    objVerf[objVerfNum].objRec.width = gpFCWSTInput->pTrackerset[objverfIndex].objRec.width * fzoomRate;
                    objVerf[objVerfNum].objRec.height = gpFCWSTInput->pTrackerset[objverfIndex].objRec.height * fzoomRate;
                    objVerf[objVerfNum].nGroupID = gpFCWSTInput->pTrackerset[objverfIndex].nId;
                    objVerfNum++;
                }
            }
        }
        pthread_mutex_unlock(&gObjVerfInputMutex);

        if(tmpFrameIn != frameIn)
        {
            //printf("OBJVERF frame:%d\n", tmpFrameIn);
            for(objverfBufIndex = 0; objverfBufIndex < CACHE_BUF_NUM; objverfBufIndex++)
            {
                if(gBuf[objverfBufIndex].bufNo == tmpFrameIn)
                {
                    break;
                }
            }

            if (objverfBufIndex < CACHE_BUF_NUM && tmpFrameIn != frameIn && objVerfNum > 0)
            {
                proCount++;
#if SHOW_TIME
                timeCount = my_gettime();
#endif

                //printf("objverf process frame:%d, count:%d\n", objverfBufIndex, objVerfNum);
                pVerfobjInput.pOriGrayfram.ptr = gBuf[objverfBufIndex].yBuf; //\BB\F1ȡ\B8\FA\D7ٽ\E1\B9\FB\B5\C4\C4\C7һ֡
                pVerfobjInput.pOriGrayfram.nWid = FRAME_WID;
                pVerfobjInput.pOriGrayfram.nHig = FRAME_HIG;
                pVerfobjInput.pOriGrayfram.dayOrNight = dayOrNight;
                pVerfobjInput.objInput = objVerf;
                pVerfobjInput.nObjNum = objVerfNum;
#if 1
                if(dayOrNight == 0)
                {
                    verfObjByDetcor(&pVerfobjInput);
                }
                else if(dayOrNight == 1)
                {
                    resizeVeryObject(CNN_IMAGE_WIDTH, CNN_IMAGE_HEIGHT, &pVerfobjInput);
                    for(int k = 0;k < objVerfNum; k++){
                        cnn_main(dayOrNight, pVerfobjInput.objInput[k].objData, &pVerfobjInput.objInput[k].nObjState);
                    }
                }
#else
#if OBJVERF_CNN
                resizeVeryObject(CNN_IMAGE_WIDTH, CNN_IMAGE_HEIGHT, &pVerfobjInput);
                for(int k = 0;k < objVerfNum; k++){
                    cnn_main(CNN_OBJVERF_ID,pVerfobjInput.objInput[k].objData, &pVerfobjInput.objInput[k].nObjState);
                }
#else
                verfObjByDetcor(&pVerfobjInput);
#endif
#endif

                pthread_mutex_lock(&gObjVerfOutputMutex);
                updataMuliObjVerf(&pVerfobjInput);
                pthread_mutex_unlock(&gObjVerfOutputMutex);

#if SHOW_TIME
                algTime += (my_gettime() - timeCount);
#endif
                if (proCount >= PRINT_FRAME_RANGE)
                {
#if SHOW_TIME
                    printf("OBJVERF process %d frame -- cost time : %d ms\n", proCount, algTime);
                    algTime = 0;
#endif
                    proCount = 0;
                }
                frameIn = tmpFrameIn;
#if CONTRL_FPS
                curTime = my_gettime();
                if (curTime - proTime < OBJVERF_GAP)
                    usleep((OBJVERF_GAP - (curTime - proTime)) * 1000);
                proTime = my_gettime();
#endif
            }else{
                usleep(1000);
            }
        }
        else
        {
            usleep(1000);
        }
    }

    return NULL;
}
static int freeObjVerf(void)
{
#if 1
    int objverfIndex = 0;
    for(objverfIndex = 0; objverfIndex < MAX_TRACK_MUM; objverfIndex++)
    {
        free(objVerf[objverfIndex].objData);
        objVerf[objverfIndex].objData = NULL;
    }
    FCWSD_Free(OBJVERF_DAY_HANDLE_ID);
#else
#if OBJVERF_CNN == 1
    int objverfIndex = 0;
    for(objverfIndex = 0; objverfIndex < MAX_TRACK_MUM; objverfIndex++)
    {
        free(objVerf[objverfIndex].objData);
        objVerf[objverfIndex].objData = NULL;
    }
#else
    FCWSD_Free(OBJVERF_DAY_HANDLE_ID);
    FCWSD_Free(OBJVERF_NIGHT_HANDLE_ID);
#endif
#endif
    MuliObjVerfRelease(0);

    printf("OBJVERF release\n");
    return 0;
}
#endif //OBJVERF


#if TDMAP
static int initTDMAP(void)
{
    int ReVal = 0;

    setDispParams();
    Stereo_detect_init();

    return ReVal;
}

static void* runTDMAP(void * arg)
{
#if CONTRL_FPS
    unsigned int curTime = 0;
    unsigned int proTime = 0;
#endif
#if SHOW_TIME
    int proCount = 0;
    unsigned int timeCount = 0;
    unsigned int algTime = 0;
#endif
    unsigned char frameIn = 0;
    unsigned char tmpFrameIn = 0;
    float speed = -1.0f;
    RES_DISP resParams;
    TDMAP_TRACK_DISP res3DMap;
    memset(&resParams, 0, sizeof(RES_DISP));
    memset(&res3DMap, 0, sizeof(TDMAP_TRACK_DISP));
    memset(&gDrawFrences, 0, sizeof(FENCE_LIST));

    unsigned long mask = TDMAP_THREAD_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("TDMAP set cpu core fail!\n");
    }
    //	pthread_mutex_t mutex;
    //	pthread_mutex_init(&mutex, NULL);

    while (!gIsTDMAPExit)
    {

        pthread_mutex_lock(&gBufMutex);
        tmpFrameIn = gBufIn;
	    speed = gSpeed;
        pthread_mutex_unlock(&gBufMutex);

        //printf("TDMAP ---------- frameIn: %d\n", tmpFrameIn);

#if CONTRL_SWITCH_FUN
        if(speed <= 0.0f || speed >= SPEED_TO_RUN)
        {
            gDrawFrences.nFenceNum = 0;
            gDrawFrences.nWarningNum = 0;
            usleep(200000);
            continue;
        }
#endif

        if (tmpFrameIn != frameIn)
        {
#if SHOW_TIME
            proCount ++;
            timeCount = my_gettime();
#endif
            if(gBuf[tmpFrameIn].bufNo % 100 == 0 && false)
            {
                char name[128];
                sprintf(name, "/sdcard/tdmap_%d.yuv", gBuf[tmpFrameIn].bufNo);
                saveFile(name,gBuf[tmpFrameIn].yBuf,FRAME_WID*FRAME_HIG*3>>2);
            }
            setereoMatch(gBuf[tmpFrameIn].yBuf, gBuf[tmpFrameIn].yBuf+(FRAME_WID*FRAME_HIG));
            getDispResult(&resParams);
            Stereo_detect_show(resParams, SUBBITS_3);
            Stereo_detect_getresult(&res3DMap, &gDrawFrences, speed);
            //g_tdmap_ui.trackNum = res3DMap.nTrackeNum;
            //g_tdmap_ui.pResult = (TDMAP_UI_RESULT*)&res3DMap.Trackerset[0];
            //g_tdmap_ui.fenceNum = drawFrences.nFenceNum;
            //g_tdmap_ui.pFrenceRect = (FRENCE_RECT*)&drawFrences.frences[0];
            frameIn = tmpFrameIn;

#if SHOW_TIME
            algTime += my_gettime() - timeCount;
            if (proCount >= PRINT_FRAME_RANGE)
            {
                printf("TDMAP process %d frame -- cost time : %d ms\n",
                        proCount, algTime);
                algTime = 0;
                proCount = 0;
            }
#endif //SHOW_TIME

#if CONTRL_FPS
            curTime = my_gettime();
            if (curTime - proTime < TDMAP_GAP)
                usleep((TDMAP_GAP - (curTime - proTime)) * 1000);
            proTime = my_gettime();
#endif //CONTRL_FPS

        }else{
            usleep(1000);
            //			pthread_mutex_lock(&mutex);
            //			pthread_cond_wait(&gBufCond, &mutex);
            //			pthread_mutex_unlock(&mutex);
        }

    }


    return NULL;
}

static int freeTDMAP(void)
{
    int ReVal = 0;

    Stereo_detect_release();
    releaseDispBuff();

    printf("TDMAP release\n");
    return ReVal;
}
#endif //TDMAP

static int writeTipFile(TIP_LINE_s* tipLine)
{

    if(!tipLine)
        return -1;

    FILE *fp = NULL;
    //LDWS_Point ptVanish, ptOrg, ptTel;

    fp = fopen("/wissen/tipline.txt", "w");
    if(!fp)
    {
        printf("open tipline.txt error\n");
        return -1;
    }

    for(int i = 0; i < 6; i++)
    {
        fprintf(fp, "tipline%d={x1:%d,y1:%d,x2:%d,y2:%d}\n",i,tipLine->points[i].x,tipLine->points[i].y,tipLine->points[i+1].x,tipLine->points[i+1].y);
    }
        fprintf(fp, "tipline6={x1:%d,y1:%d,x2:%d,y2:%d}\n",tipLine->points[7].x,tipLine->points[7].y,tipLine->points[8].x,tipLine->points[8].y);
/*		
	int CarWidth = (int)(LDWS_GetCarWidth() * 100);// 185;
	int CameraPosx = (int)(gpLDGuid->Param[1] * 100);// 210;
	int LeftDeviation = (int)(LDWS_GetLeftDeviation() * 100);
	int RoadWidth = (int)(gpLDGuid->Param[0] * 100);// 370;

	float32_t coeff = (CameraPosx - (CarWidth / 2 + LeftDeviation)) * 1.0f / RoadWidth;
	ptOrg.x = gpLDGuid->pBoundPoint[3].x + (gpLDGuid->pBoundPoint[2].x - gpLDGuid->pBoundPoint[3].x) * coeff;
	ptOrg.y = 720;
	ptTel.x = ptOrg.x + (gpLDGuid->pBoundPoint[2].x - gpLDGuid->pBoundPoint[3].x) * (CarWidth * 1.0f / RoadWidth);
	ptTel.y = 720;
	LDWS_GetVanishPointSet(&ptVanish);
	
	fprintf(fp, "tipline7={x1:%d,y1:%d,x2:%d,y2:%d}\n", ptVanish.x, ptVanish.y, ptOrg.x, ptOrg.y);
	fprintf(fp, "tipline8={x1:%d,y1:%d,x2:%d,y2:%d}\n", ptOrg.x, ptOrg.y, ptTel.x, ptTel.y);
	fprintf(fp, "tipline9={x1:%d,y1:%d,x2:%d,y2:%d}\n", ptTel.x, ptTel.y, ptVanish.x, ptVanish.y);
*/
    if(fp)
        fclose(fp);

    return 0;
}

int saveFile(char* name, void* buf, int size)
{
    FILE* fp = fopen(name, "w");
    if(fp)
    {
        fwrite(buf, size, 1, fp);
        printf("write file success\n");
        fclose(fp);
    }else{
        printf("open %s fail\n", name);
    }
    return 0;
}

