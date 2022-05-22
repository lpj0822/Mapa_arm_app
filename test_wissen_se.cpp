#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include <pthread.h>

#define LOG         0
#define DRAW        0
#define DRAW_HDMI   0
#define BEEP        1
#define DSM         0
#define SMART_EYE   1

#include "utils.h"
#include "PublicDefine.h"
#include "YUV_Interface.h"
#include "LOG_Interface.h"
#include "CAN_Interface.h"

#if DRAW
#if DRAW_HDMI
#include "UI_Interface.h"
#else
#include "OVERLAY_Interface.h"
#endif //DRAW_HDMI
#endif//DRAW

#if BEEP
#include "Audio_Interface.h"
#endif

#if DSM
static bool gLoginSuccess = false;
#include "DSM_Interface.h"
#endif

#include "SE_Interface.h"

#define WS_DEBUG 0
#define TIME_GAP 35

#define SE_MAX(x, x1) (x>x1?x:x1)
#define SE_MIN(x, x1) (x>x1?x1:x)

#if DSM
extern bool g_login;
extern bool g_register;
extern bool g_calibration;
#endif

#if DRAW
int showTipLine(void);
#endif
void sig_stop(int sig);
void release_resource();

static bool gSmartEyePause = false;
#if SMART_EYE
#if DRAW
static int getStatusColor(int status);
#endif
int getBeepType(int status);
int write_result(SE_UI_RESULT_s* se);
#endif

int draw_result(void* se, void* dsm);
int beep_result(void* se, void* dsm, CAN_INFO_t info);

static int gExit = 0;

void sig_stop(int sig)
{
    printf("get signal: %d\n", sig);
    gExit = 1;
    //release_resource();
}

void release_resource()
{

    printf("start release resource!!!\n");

#if DSM
    DSM_deinit();
#endif
   
#if SMART_EYE
    SE_Stop();
    SE_Release();
    printf("free smart eye over!\n");
#endif

#if DRAW
#if DRAW_HDMI
    draw_deinit();
    printf("free draw over!\n");
#else
    OVERLAY_deinit();
#endif
#endif
#if BEEP
    play_audio_deinit();
    printf("free audio over!\n");
#endif

#if SMART_EYE
    CAN_deinit();
    YUV_deinit();
#endif //SMART_EYE

    LOG_deinit();

}

int saveFile(void* buf, int size, char* name)
{
    //char name[128] = {0};
    //sprintf(name,"/sdcard/test/%s", name);
    int fp = open(name, O_RDWR | O_CREAT, 0777);
    if(fp > 0)
    {
        write(fp, buf, size);
        //printf("write file success\n");
        close(fp);
    }else{
        printf("open %s fail\n",name);
    }
    return 0;
}

#if BEEP
#if SMART_EYE
int getBeepType(int status)
{
    switch(status)
    {
        case LD_SAFE_LEFT:
        case LD_SAFE_RIGHT:
        case FC_MID:
        case FC_LEFT:
        case FC_RIGHT:
        case LD_UNKNOW:
        case FC_UNKNOW:
            return 0;
        case LD_WARN_LEFT:
        case LD_WARN_RIGHT:
            return AUDIO_LD;
        case FC_HMW:
            return AUDIO_HMW;
        case FC_WARN:
            return AUDIO_TTC;
        default:
            return 0;
    }
}
#endif //SMART_EYE

int beep_result(void* se, void* dsm, CAN_INFO_t info)
{
#if SMART_EYE
    SE_UI_RESULT_s* result = (SE_UI_RESULT_s*) se;
    if (result) {
        int i = 0;
        int beeptype = 0;
        for(i = 0; i < result->fcCount; i++)
        {
            FC_UI_RESULT_s* fc = result->fcResult + i;
            beeptype = getBeepType(fc->status);
            if(beeptype)
            {
                //play_audio_send(beeptype,1);
                //write_result(result);
                if(beeptype == AUDIO_HMW) {
                    printf("BEEP --------------  HMW\n");
                } else {
                    play_audio_send(beeptype,1);
                    printf("BEEP --------------  TTC\n");
                }
                //return 0;
            }
        }

        if(result->tdmapWarnNum > 1)
        {
            //play_audio_send(AUDIO_TDMAP,1);
            //write_result(result);
            printf("BEEP --------------  TD\n");
            //return 0;
        }
    }
#endif

#if DSM
    //if(((DSM_RESULT_t*)dsm)->sleep == 1 && (info.gear == 10 || info.speed > 0.0f))
    if(((DSM_RESULT_t*)dsm)->sleep == 1) {
        //play_audio_send(AUDIO_DSM_DROW,1);
        printf("BEEP --------------  DSM SLEEP\n");
        //return 0;
    }
    //if(((DSM_RESULT_t*)dsm)->distraction == 1 && (info.gear == 10 || info.speed > 0.0f))
    if(((DSM_RESULT_t*)dsm)->distraction == 1) {
        //play_audio_send(AUDIO_DSM_DIST,1);
        printf("BEEP --------------  DSM DIST\n");
        //return 0;
    }
#endif

#if SMART_EYE
    if (result && info.speed > LD_START_SPEED) {
        if(0 < result->ldCount) {
            LD_UI_RESULT_s* ld = result->ldResult;
            // It's just depends on the speed when we use gps information.(corneringLamp = 0,steel = 0.0)
            if(((ld->status&LD_WARN_LEFT)&&(info.corneringLamp != 2) && (info.steel > -4.0f)) || ((ld->status&LD_WARN_RIGHT)&&(info.corneringLamp != 1)&&(info.steel < 4.0f))) {
                //play_audio_send(AUDIO_LD,1);
                //write_result(result);
                printf("BEEP --------------  LD\n");
            } else {
                ld = result->ldResult +(result->ldCount - 1);
                // It's just depends on the speed when we use gps information.(corneringLamp = 0,steel = 0.0)
                if(((ld->status&LD_WARN_LEFT)&&(info.corneringLamp != 2) && (info.steel > -4.0f)) || ((ld->status&LD_WARN_RIGHT)&&(info.corneringLamp != 1)&&(info.steel < 4.0f))) {
                    //play_audio_send(AUDIO_LD,1);
                    //write_result(result);
                    printf("BEEP --------------  LD\n");
                }
            }
        }
    }
#endif //SMART_EYE

    return 0;
}
#endif //BEEP

#if SMART_EYE
int write_result(SE_UI_RESULT_s* se)
{
#if LOG
    static int logCount = 0;
    if(logCount <= 0)
        LOG_init();

    LOG_write(0, se);

    logCount ++;
    if(logCount >= 1000)
    {
        LOG_deinit();
        logCount = 0;
    }
#endif //LOG

    return 0;
}
#endif

#if DRAW
int showTipLine(void)
{
#if DRAW_HDMI
    int i = 0; 
    TIP_LINE_s* line = NULL;
    SE_GetTipLine(&line);
    if(line)
    {
        draw_clean();
        for(i = 0; i < 7; i++)
        {
	        POINT_2D_s *point = line->points+i;
	        POINT_2D_s *point2 = line->points+i+1;
            //printf("tip[%d]: %dx%d--%dx%d\n",i,point->x,point->y,point2->x,point2->y);
            draw_line(YELLOW,point->x,point->y,point2->x,point2->y);
        }
        draw_line(YELLOW,line->points[7].x,line->points[7].y, line->points[8].x,line->points[8].y);
        draw_show();
    }else{
        printf("get tip line error \n");
    }
#endif//DRAW_HDMI
    return 0;
}

#if SMART_EYE
#if DRAW
static int getStatusColor(int status)
{
    switch(status)
    {
        case LD_SAFE_LEFT:
        case LD_SAFE_RIGHT:
        case FC_MID:
            return GREEN;
        case LD_WARN_LEFT:
        case LD_WARN_RIGHT:
        case FC_WARN:
        case FC_HMW:
            return RED;
        case LD_UNKNOW:
        case FC_UNKNOW:
            return WHITE;
        case FC_LEFT:
            return CYAN;
        case FC_RIGHT:
            return MAGENTA;
        default:
            return YELLOW;
    }
}
#endif //DRAW
#endif //SMART_EYE

int draw_result(void* se, void* dsm_result)
{
    int ReVal = 0;

#if DRAW_HDMI
    draw_clean();
#else
    static int drawCount = 0;
    if(drawCount < 3) {
        drawCount++;
        return 1;
    }
    OVERLAY_clean();
#endif
#if SMART_EYE
    if(se){
        SE_UI_RESULT_s* result = (SE_UI_RESULT_s*)se;
        int color = 0;
        int i = 0;
#if DRAW_HDMI
        for(i = 0; i < result->ldCount; i++)
        {
            LD_UI_RESULT_s* ld = result->ldResult + i;
            color = getStatusColor(ld->status);
            int x1 = ld->point.x-ld->width;
            int x2 = ld->point.x+ld->width;
            int y = ld->point.y;
            x1 = SE_MAX(0, x1);
            x1 = SE_MIN(x1, FRAME_WID);
            x2 = SE_MAX(0, x2);
            x2 = SE_MIN(x2, FRAME_WID);
            draw_line(color, x1, y, x2, y);
        }
#else
        if(result->ldCount > 0)
        {
            color = getStatusColor(result->ldResult[0].status);
            int x1 = result->ldResult[0].point.x;
            int x2 = result->ldResult[result->ldCount/2-1].point.x;
            x1 = SE_MAX(0, x1);
            x1 = SE_MIN(x1, FRAME_WID);
            x2 = SE_MAX(0, x2);
            x2 = SE_MIN(x2, FRAME_WID);
            OVERLAY_line(color, x1, result->ldResult[0].point.y, x2, result->ldResult[result->ldCount/2-1].point.y);
        
            color = getStatusColor(result->ldResult[result->ldCount-1].status);
            x1 = result->ldResult[result->ldCount/2].point.x;
            x2 = result->ldResult[result->ldCount-1].point.x;
            x1 = SE_MAX(0, x1);
            x1 = SE_MIN(x1, FRAME_WID);
            x2 = SE_MAX(0, x2);
            x2 = SE_MIN(x2, FRAME_WID);
            OVERLAY_line(color, x1, result->ldResult[result->ldCount/2].point.y, x2, result->ldResult[result->ldCount-1].point.y);
        }
#endif

        for(i = 0; i < result->fcCount; i++)
        {
            FC_UI_RESULT_s* fc = result->fcResult + i;
            color = getStatusColor(fc->status);
#if DRAW_HDMI
            draw_line(color, fc->point.x, fc->point.y, fc->point.x + fc->wid, fc->point.y);
            draw_line(color, fc->point.x, fc->point.y, fc->point.x, fc->point.y + fc->hig);
            draw_line(color, fc->point.x, fc->point.y + fc->hig, fc->point.x + fc->wid, fc->point.y + fc->hig);
            draw_line(color, fc->point.x + fc->wid, fc->point.y, fc->point.x + fc->wid, fc->point.y + fc->hig);
#else
            OVERLAY_rect(color,fc->point.x,fc->point.y,fc->wid,fc->hig);
#endif
        }

        for(i = 0; i < result->tdmapCount; i ++){
            color = (result->tdmap+i)->color;
            TDMAP_RECT_t rect = (result->tdmap+i)->rects;
#if DRAW_HDMI
            draw_line(color, rect.left, rect.up, rect.right, rect.up);
            draw_line(color, rect.left, rect.up, rect.left, rect.down);
            draw_line(color, rect.left, rect.down, rect.right, rect.down);
            draw_line(color, rect.right, rect.up, rect.right, rect.down);
#else
            OVERLAY_rect(color<<8|0xFF,rect.left,rect.up,rect.right-rect.left,rect.down-rect.up);
#endif
        }
    }
#endif //SMART_EYE

#if DSM
    DSM_RESULT_t* dsm = (DSM_RESULT_t*)dsm_result;
    if(dsm->faceX > 0)
    {
#if DRAW_HDMI
        draw_line(YELLOW, 0, 0, 0, 100);
        draw_line(YELLOW, 1, 0, 1, 100);
        draw_line(YELLOW, 2, 0, 2, 100);
        int y = (int)(dsm->sleep*100.0);
        draw_line(RED, 0, 0, 0, y);
        draw_line(RED, 1, 0, 1, y);
        draw_line(RED, 2, 0, 2, y);

        draw_line(BLACK, dsm->faceX, dsm->faceY, dsm->faceX+dsm->faceWid, dsm->faceY);
        draw_line(BLACK, dsm->faceX, dsm->faceY, dsm->faceX, dsm->faceY+dsm->faceHig);
        draw_line(BLACK, dsm->faceX, dsm->faceY+dsm->faceHig, dsm->faceX+dsm->faceWid, dsm->faceY+dsm->faceHig);
        draw_line(BLACK, dsm->faceX+dsm->faceWid, dsm->faceY, dsm->faceX+dsm->faceWid, dsm->faceY+dsm->faceHig);
#endif
    }
#endif
#if DRAW_HDMI
    draw_show();
#else
    OVERLAY_show();
#endif

    return ReVal;
}
#endif //DRAW

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>

#define MYPORT  8888
#define QUEUE   20
#define BUFFER_SIZE 512


void *handlePreKey(void* arg) {

    prctl(PR_SET_NAME, "PRES_KEY");
#if 0
    char a = '$';
    while(1) {
        a = getchar();
        if(a == 'r' || a == 'R') {
            extern bool g_register;
            g_register = true;
        }

        if(a == 'l' || a == 'L') {
            extern bool g_login;
            g_login = true;
        }
        
        if(a == 'q' || a == 'Q') {
            sig_stop(1);
            break;
        }
        printf("-----------------: %c\n", a);
    }
#endif
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    printf("监听%d端口\n",MYPORT);
    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    while(1)
    {

        ///成功返回非负描述字，出错返回-1
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
        printf("客户端成功连接\n");

        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);
        if(len <= 0) {
            close(conn);
            continue;
        }

        printf("来自客户端数据：%s\n",buffer);

        if(strncmp("quit", buffer, 4) == 0){
            sig_stop(1);
        }

#if DSM
        if(strncmp("login", buffer, 5) == 0){
            gSmartEyePause = true;
            g_login = true;
        }

        if(strncmp("calib", buffer, 5) == 0){
             gSmartEyePause = true;
            g_calibration = true;
        }

        if(strncmp("regist", buffer, 6) == 0){
            gSmartEyePause = true;
            g_register = true;
        }
#endif

        close(conn);
    }
    close(server_sockfd);
 
    return NULL;
}

#if DSM
void dsm_event_handle(int event) {
    switch(event) {
#if BEEP
        case DSM_EVENT_REGISTER_FAIL:
            play_audio_send(AUDIO_REGISTER_FAIL,1);
            break;
        case DSM_EVENT_REGISTER_SUCCESS:
            g_login = true;
            play_audio_send(AUDIO_REGISTER_SUCCESS,1);
            break;
        case DSM_EVENT_LOGIN_FAIL:
            play_audio_send(AUDIO_LOGIN_FAIL,1);
            break;
        case DSM_EVENT_LOGIN_SUCCESS:
            g_calibration = true;
            play_audio_send(AUDIO_LOGIN_SUCCESS,1);
            break;
        case DSM_EVENT_CALIB_FAIL:
            printf("CALIBRATION FAIL\n");
            break;
        case DSM_EVENT_CALIB_SUCCESS:
            gLoginSuccess = true;
            gSmartEyePause = false;
            play_audio_send(AUDIO_CALIBRATED,1);
            printf("CALIBRATION SUCCESS\n");
            break;
#endif
        case DSM_EVENT_HAND_OK:
            printf("DETECT A OK HAND\n");
            break;
        default:
            break;
    }
}
#endif

int main(int argc, char* argv[])
{
    int ret = 0;
    unsigned int time;
    int costTime = 0;
    my_gettime();
    signal(SIGINT, sig_stop);

    prctl(PR_SET_NAME, "MAIN");
    // start thread to handle press key
    pthread_t thread_id;
    pthread_create(&thread_id,NULL, handlePreKey, NULL);

#if BEEP
    ret = play_audio_init();
    if(ret != 0) {
        printf("init beep fail\n");
        LOG_recordErr(ret, "init beep fail");
        return -1;
    }
    printf("init beep over!\n");
#endif

#if DSM
    DSM_register_event_handle(dsm_event_handle);
    ret = DSM_init();
    if(ret != 0) {
        printf("init dsm fail\n");
        LOG_recordErr(ret, "init dsm fail");
        return -1;
    }
    printf("init dsm over!\n");
    while(!gLoginSuccess && !gExit) {
        usleep(100);
    }
#endif

#if DRAW
#if DRAW_HDMI
    ret = draw_init();
    if(ret != 0) {
        printf("init draw fail\n");
        LOG_recordErr(ret, "init draw fail");
        return -1;
    }
#else
    ret = OVERLAY_init();
    if(ret != 0) {
        printf("init ovleray fail\n");
        LOG_recordErr(ret, "init overlay fail");
        return -1;
    }
#endif//DRAW_HDMI
    printf("init draw over!\n");
#endif //DRAW

    ret = CAN_init();
    if(ret != 0) {
        printf("init can fail\n");
        LOG_recordErr(ret, "init can fail");
        return -1;
    }
    printf("init can over!\n");

    unsigned char isReadFile = 0;
#if SMART_EYE
    int frameNo;
    char* yuvbuf = NULL;
    ret = SE_Init("/wissen/param_camera_MaPa.dat","/wissen/CarCalibration_MaPa.txt", "/wissen/car_day.txt");
    if(ret != 0) {
        printf("init se fail\n");
        LOG_recordErr(ret, "init se fail");
        return -1;
    }
    printf("init se over!\n");

    if(argc > 1 && 1 ==  atoi(argv[1]))
    {
#if DRAW
        showTipLine();
#endif
        while(!gExit)
            sleep(10);
        release_resource();
        return 0;
    }

    bool isReadCam = true;
    char defaultDir[8] = "test";
    char *fileDir = defaultDir;
    if(argc > 1 && 2 ==  atoi(argv[1]))
    {
        isReadFile = 1;
        isReadCam = false;
        ret = LOG_init();
        if(ret != 0) {
            printf("init log fail\n");
            return -1;
        }

        if(argc > 2) {
            fileDir = argv[2];
        }
    }
    ret = YUV_init(isReadCam, fileDir);
    if(ret != 0) {
        printf("init yuv fail\n");
        LOG_recordErr(ret, "init yuv fail");
        return -1;
    }
    printf("init yuv over!\n");

    ret = SE_Start();
    if(ret != 0) {
        printf("Smart alg start fail\n");
        LOG_recordErr(ret, "start se fail");
        return -1;
    }
    printf("start se over!\n");

#endif //SMART_EYE

#if SMART_EYE || DSM
    CAN_INFO_t canInfo;
    canInfo.speed = -1.0f;
    canInfo.steel = 0.0f;
    canInfo.gear = 0;
    canInfo.corneringLamp = 0;
#endif

    int saveTime = 0;

    while(!gExit)
    {
        time = my_gettime();

#if SMART_EYE || DSM
        canInfo = CAN_getInfo();
#endif //DSM || SMART_EYE

#if SMART_EYE
        SE_UI_RESULT_s* result = NULL;
        if(!gSmartEyePause) {
            ret = YUV_getBuffer(&yuvbuf, &frameNo);
            if(ret != 0) {
                printf("get buf fail\n");
                LOG_recordErr(ret, "get yuv buf fail");
                break;
            }
            if(frameNo == -1) {
                printf("frame is empty!\n");
                break;
            }
            if(yuvbuf == NULL) {
                usleep(1000);
                continue;
            }
            saveTime = my_gettime();
            SE_Main(yuvbuf, FRAME_WID, FRAME_HIG, frameNo, canInfo.speed);
            SE_GetResult(&result);
        }
#endif //SMART_EYE

#if DSM
        DSM_RESULT_t dsm;
        memset(&dsm, 0, sizeof(DSM_RESULT_t));
        DSM_getresult(&dsm);
#endif
    
        if(isReadFile)
        {
#if SMART_EYE
            LOG_write(frameNo, result);    
#endif
        }else{
#if DRAW
#if DSM && SMART_EYE
            draw_result(result,&dsm);
#elif DSM
            draw_result(NULL,&dsm);
#elif SMART_EYE
            draw_result(result,NULL);
#endif
#endif

#if BEEP
#if DSM && SMART_EYE
            beep_result(result, &dsm, canInfo);
#elif DSM
            beep_result(NULL, &dsm, canInfo);
#elif SMART_EYE
            beep_result(result, NULL, canInfo);
#endif
#endif

        }
        char fileName[128];
        sprintf(fileName, "/sdcard/test/input%d_%f_%d.yuv", frameNo,canInfo.speed,saveTime);
        //sprintf(fileName, "input%d_%d.yuv", frameNo ,saveTime);
        saveFile(yuvbuf,FRAME_WID*FRAME_HIG, fileName);
        costTime = my_gettime() - time;
        //printf("run frame: %d over!!! cost time:%d\n", frameNo, costTime);
        if(costTime < TIME_GAP)
            usleep((TIME_GAP-costTime)*1000);
    }

    release_resource();

    return 0;
}

