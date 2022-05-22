#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "utils.h"
#include "Audio_Interface.h"

#define USE_ALSA    1
#define AUDIO_GAP   5000
#define TTC_GAP     20000
#define HMW_GAP     10000
#define BIND_CPU_ID 0x4 //(cpu2)

#if USE_ALSA
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alsa/asoundlib.h"
#define u32 unsigned int
#define u8   unsigned char
#define u16 unsigned short
#pragma pack(push) 
#pragma pack(1)     //1字节对齐
typedef  struct {
    u32     dwSize;
    u16     wFormatTag;
    u16     wChannels;
    u32     dwSamplesPerSec;
    u32     dwAvgBytesPerSec;
    u16     wBlockAlign;
    u16     wBitsPerSample;
} WAVEFORMAT;

typedef  struct {
    u8      RiffID [4];
    u32     RiffSize;
    u8      WaveID[4];
    u8      FmtID[4];
    u32     FmtSize;
    u16     wFormatTag;
    u16     nChannels;
    u32     nSamplesPerSec;  /*采样频率*/
    u32     nAvgBytesPerSec; /*每秒所需字节数*/
    u16     nBlockAlign; /*数据块对齐单位,每个采样需要的字节数*/
    u16     wBitsPerSample;/*每个采样需要的bit数*/
    u8      DataID[4];
    u32     nDataBytes;
} WAVE_HEADER;

#pragma pack(pop) /* 恢复先前的pack设置 */

WAVE_HEADER g_wave_header;
snd_pcm_t *gp_handle;  //调用snd_pcm_open打开PCM设备返回的文件句柄，后续的操作都使用是、这个句柄操作这个PCM设备
snd_pcm_hw_params_t *gp_params;  //设置流的硬件参数
snd_pcm_uframes_t g_frames;    //snd_pcm_uframes_t其实是unsigned long类型
u8 *gp_buffer;
u32 g_bufsize;
#else //ALSA
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
 
#define MYPORT  8887
#define BUFFER_SIZE 1024
#endif //USE_ALSA

static int gAudioState = 0; 
static int gAudioExit = 0;
static unsigned int gLDBeepTime = 0;
static unsigned int gTDMAPBeepTime = 0;
static unsigned int gHMWBeepTime = 0;
static unsigned int gTTCBeepTime = 0;
static unsigned int gDSMBeepTime = 0;
static pthread_t thread_id;
static pthread_mutex_t mutex;
static pthread_cond_t cond;
static int gCurBeep = -1;
static bool isBeeping = false;

void* warn_audio_handle(void* arg);

const char* gBeepPath[13] = {
    "/wissen/ws_ld.wav",
    "/wissen/ws_hmw.wav",
    "/wissen/ws_ttc.wav",
    "/wissen/ws_tdmap.wav",
    "/wissen/ws_radar.wav",
    "/wissen/ws_dist.wav",
    "/wissen/ws_drow.wav",
    "/wissen/ws_login_fail.wav",
    "/wissen/ws_login_success.wav",
    "/wissen/ws_register_fail.wav",
    "/wissen/ws_register_success.wav",
    "/wissen/ws_calibrated.wav",
    "/wissen/ws_calibrating.wav"};

#if USE_ALSA
FILE * open_and_print_file_params(char *file_name)
{
    FILE * fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("can't open %s\n", file_name);
        return NULL;
    }

    int cacheBuf[12];
    memset(&g_wave_header, 0, sizeof(g_wave_header));
    //fread(&g_wave_header, 1, sizeof(g_wave_header), fp);
    fread(cacheBuf, 20, 1, fp);
    unsigned char* buf = (unsigned char*)cacheBuf;
    g_wave_header.RiffID[0]=buf[0];
    g_wave_header.RiffID[1]=buf[1];
    g_wave_header.RiffID[2]=buf[2];
    g_wave_header.RiffID[3]=buf[3];
    g_wave_header.RiffSize=*((int*)&buf[4]);
    g_wave_header.WaveID[0]=buf[8];
    g_wave_header.WaveID[1]=buf[9];
    g_wave_header.WaveID[2]=buf[0xa];
    g_wave_header.WaveID[3]=buf[0xb];
    g_wave_header.FmtID[0]=buf[0xc];
    g_wave_header.FmtID[1]=buf[0xd];
    g_wave_header.FmtID[2]=buf[0xe];
    g_wave_header.FmtID[3]=buf[0xf];
    g_wave_header.FmtSize=*((int*)&buf[0x10]);
    
    if(g_wave_header.FmtSize == 18)
        fread(cacheBuf, 26, 1, fp);
    else
        fread(cacheBuf, 24, 1, fp);

    g_wave_header.wFormatTag=*((short*)&buf[0]);
    g_wave_header.nChannels=*((short*)&buf[2]);
    g_wave_header.nSamplesPerSec=*((int*)&buf[4]);
    g_wave_header.nAvgBytesPerSec=*((int*)&buf[8]);
    g_wave_header.nBlockAlign=*((short*)&buf[0xc]);
    g_wave_header.wBitsPerSample=*((short*)&buf[0xe]);
    if(g_wave_header.FmtSize == 18)
    {
        g_wave_header.FmtSize = 16;
        g_wave_header.DataID[0]=buf[0x12];
        g_wave_header.DataID[1]=buf[0x13];
        g_wave_header.DataID[2]=buf[0x14];
        g_wave_header.DataID[3]=buf[0x15];
        g_wave_header.nDataBytes=buf[0x16]|buf[0x17]<<8|buf[0x18]<<6|buf[0x19]<<24;
    }else{
        g_wave_header.DataID[0]=buf[0x10];
        g_wave_header.DataID[1]=buf[0x11];
        g_wave_header.DataID[2]=buf[0x12];
        g_wave_header.DataID[3]=buf[0x13];
        g_wave_header.nDataBytes=*((int*)&buf[0x14]);
    }

    /*
    printf("RiffID:%c%c%c%c\n", g_wave_header.RiffID[0], g_wave_header.RiffID[1], g_wave_header.RiffID[2], g_wave_header.RiffID[3]);
    printf("RiffSize:%d\n", g_wave_header.RiffSize);
    printf("WaveID:%c%c%c%c\n", g_wave_header.WaveID[0], g_wave_header.WaveID[1], g_wave_header.WaveID[2], g_wave_header.WaveID[3]);
    printf("FmtID:%c%c%c%c\n", g_wave_header.FmtID[0], g_wave_header.FmtID[1], g_wave_header.FmtID[2], g_wave_header.FmtID[3]);
    printf("FmtSize:%d\n", g_wave_header.FmtSize);
    printf("wFormatTag:%d\n", g_wave_header.wFormatTag);
    printf("nChannels:%d\n", g_wave_header.nChannels);
    printf("nSamplesPerSec:%d\n", g_wave_header.nSamplesPerSec);
    printf("nAvgBytesPerSec:%d\n", g_wave_header.nAvgBytesPerSec);
    printf("nBlockAlign:%d\n", g_wave_header.nBlockAlign);
    printf("wBitsPerSample:%d\n", g_wave_header.wBitsPerSample);
    printf("DataID:%c%c%c%c\n", g_wave_header.DataID[0], g_wave_header.DataID[1], g_wave_header.DataID[2], g_wave_header.DataID[3]);
    printf("nDataBytes:%d\n", g_wave_header.nDataBytes);
    */

    return fp;
}

int set_hardware_params()
{
    int rc;

    rc = snd_pcm_open(&gp_handle, "hw:0,0", SND_PCM_STREAM_PLAYBACK, 0);    
    if (rc < 0) 
    {    
        printf("unable to open pcm device\n"); 
        return -1;   
    } 
    /* Allocate a hardware parameters object */    
    snd_pcm_hw_params_alloca(&gp_params); 

    do{
        /* Fill it in with default values. */    
        rc = snd_pcm_hw_params_any(gp_handle, gp_params);
        if (rc < 0) 
        {    
            printf("unable to Fill it in with default values.\n");    
            break;
        }
        /* Interleaved mode */    
        rc = snd_pcm_hw_params_set_access(gp_handle, gp_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        if (rc < 0) 
        {    
            printf("unable to Interleaved mode.\n");    
            break;
        }

        snd_pcm_format_t format;

        if (8 == g_wave_header.FmtSize)
        {
            format = SND_PCM_FORMAT_U8;
        }
        else if (16 == g_wave_header.FmtSize)
        {
            format = SND_PCM_FORMAT_S16_LE;
        }
        else if (24 == g_wave_header.FmtSize)
        {
            format = SND_PCM_FORMAT_U24_LE;
        }
        else if (32 == g_wave_header.FmtSize)
        {
            format = SND_PCM_FORMAT_U32_LE;
        }
        else
        {
            printf("SND_PCM_FORMAT_UNKNOWN.\n");    
            format = SND_PCM_FORMAT_UNKNOWN;
            break;
        }


        /* set format */    
        rc = snd_pcm_hw_params_set_format(gp_handle, gp_params, format);
        if (rc < 0) 
        {    
            printf("unable to set format.\n");    
            break;
        }

        /* set channels (stero) */    
        snd_pcm_hw_params_set_channels(gp_handle, gp_params, g_wave_header.nChannels);
        if (rc < 0) 
        {    
            printf("unable to set channels (stero).\n");    
            break;
        }

        /* set sampling rate */       
        u32 rate = g_wave_header.nSamplesPerSec;  
        int dir;
        rc = snd_pcm_hw_params_set_rate_near(gp_handle, gp_params, &rate, &dir); 
        if (rc < 0) 
        {    
            printf("unable to set sampling rate.\n");    
            break;
        } 

        /* Write the parameters to the dirver */    
        rc = snd_pcm_hw_params(gp_handle, gp_params);    
        if (rc < 0) {    
            printf("unable to set hw parameters: %s\n", snd_strerror(rc));    
            break;
        } 

        snd_pcm_hw_params_get_period_size(gp_params, &g_frames, &dir);
        g_bufsize = g_frames * 4;
        

    }while(0);

    return 0;
}

static int beepByAlsa(const char* filePath)
{

    FILE * fp = open_and_print_file_params((char*)filePath);
    if (fp == NULL)
    {
        printf("open_and_print_file_params error\n");
        return -1;
    }


    int ret = set_hardware_params();
    if (ret < 0)
    {
        printf("set_hardware_params error\n");
        return -1;
    }

    size_t rc;
    while (1)
    {
        rc = fread(gp_buffer, g_bufsize, 1, fp);
        if (rc <1)
            break;

        ret = snd_pcm_writei(gp_handle, gp_buffer, g_frames);    
        if (ret == -EPIPE) {    
            printf("underrun occured\n");  
            break;  
        } else if (ret < 0) {    
            printf("error from writei: %s\n", snd_strerror(ret));    
            break;
        } 
    }
 
#if USE_ALSA
    snd_pcm_drain(gp_handle);    
    snd_pcm_close(gp_handle);    
#endif //USE_ALSA

    fclose(fp);
    return 0;
}
#else //USE_ALSA
static int tcpSend(const char* sendbuf)
{
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        return -1;
    }
    send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送
    close(sock_cli);

    return 0;

}
#endif //USE_ALSA

int play_audio(int index)
{
    if(0 != gAudioState)
        return -1;
    int ret = 0;
#if USE_ALSA
    ret = beepByAlsa(gBeepPath[index]);
#else
    char cmd[128];
    sprintf(cmd, "/usr/bin/aplay  %s", gBeepPath[index]);
    //int ret = system(cmd);
    ret = tcpSend(cmd);
#endif //USE_ALSA

    return ret;
}

int play_audio(const char* filepath)
{
    if(0 != gAudioState)
        return -1;
    int ret = 0;
#if USE_ALSA
    ret = beepByAlsa(filepath);
#else
    char cmd[128];
    sprintf(cmd, "/usr/bin/aplay  %s", filepath);
    //int ret = system(cmd);
    ret = tcpSend(cmd);
#endif //USE_ALSA

    return ret;
}

int play_ttc_audio(void)
{
    const char* path = "/wissen/ws_ttc.wav";
    return play_audio(path);
}
int play_tdmap_audio(void)
{
    const char* path = "/wissen/ws_tdmap.wav";
    return play_audio(path);
}
int play_radar_audio(void)
{
    const char* path = "/wissen/ws_ld.wav";
    return play_audio(path);
}

int play_dsm_audio(void)
{
    const char* path = "/wissen/ws_dsm.wav";
    return play_audio(path);
}

int play_dsm1_audio(void)
{
    const char* path = "/wissen/ws_dsm1.wav";
    return play_audio(path);
}

int play_fc_audio(void)
{
    const char* path = "/wissen/ws_fc.wav";
    return play_audio(path);
}

int play_hmw_audio(void)
{
    const char* path = "/wissen/ws_hmw.wav";
    return play_audio(path);
}

int play_ld_audio(void)
{
    const char* path = "/wissen/ws_ld.wav";
    return play_audio(path);
}

int play_audio_init(void)
{
    gAudioExit = 0;

#if USE_ALSA
    gp_buffer = (u8 *)malloc(1024000);
    if (gp_buffer == NULL)
    {
        printf("malloc failed\n");
        return -1;
    }
#endif //USE_ALSA

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_attr_t attr;
    struct sched_param param;
    memset(&param,0,sizeof(struct sched_param));
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_init(&attr);
    if(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0)
    {
        printf("Set Audio thread inheritsched fail!\n");
    }
    if(pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0)
    {
        printf("Set Audio thread attr policy SCHED_RR fail!\n");
    }
    if(pthread_attr_setschedparam(&attr,&param)!= 0)
    {
        printf("Set Audio thread attr param fail!\n");
    }
    if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0)
    {
        printf("Set Audio thread attr detachstate fail!\n");
    }
    if(pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0)
    {
        printf("Set Audio thread attr scope fail!\n");
    }

    if(0 != pthread_create(&thread_id, &attr, warn_audio_handle, NULL))
        gAudioState = -1;

    if (pthread_setname_np(thread_id, "AUDIO") != 0)
        printf("set thread AUDIO name fail\n");


    return 0;
}

int play_audio_deinit(void)
{
    gAudioExit = 1;
    gAudioState = 0;
    void* ret = NULL;
    pthread_mutex_lock(&mutex);
    gCurBeep = -1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    if(thread_id > 0){
        pthread_join(thread_id, &ret);
    }
#if USE_ALSA
    free(gp_buffer); 
#endif //USE_ALSA
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}

int play_audio_send(int id, int level)
{
    if(isBeeping)
        return 0;

    switch(id)
    {
        case AUDIO_LD:
            if(AUDIO_GAP > (my_gettime()-gLDBeepTime))
            {
                return 0;
            }else{
                gLDBeepTime = my_gettime();
            }
            break;
        case AUDIO_HMW:
            if(TTC_GAP > (my_gettime()-gTTCBeepTime) || HMW_GAP > (my_gettime()-gHMWBeepTime))
            {
                return 0;
            }else{
                gHMWBeepTime = my_gettime();
            }
            break;
        case AUDIO_TDMAP:
            if(AUDIO_GAP > (my_gettime()-gTDMAPBeepTime) || TTC_GAP > (my_gettime()-gTTCBeepTime))
            {
                return 0;
            }else{
                gTDMAPBeepTime = my_gettime();
            }
            break;
        case AUDIO_TTC:
            if(TTC_GAP > (my_gettime()-gTTCBeepTime))
            {
                return 0;
            }else{
                gTTCBeepTime = my_gettime();
            }
            break;
        case AUDIO_DSM_DIST:
        case AUDIO_DSM_DROW:
            if(AUDIO_GAP > (my_gettime()-gDSMBeepTime))
            {
                return 0;
            }else{
                gDSMBeepTime = my_gettime();
            }
            break;
        case AUDIO_CALIBRATED:
        case AUDIO_CALIBRATING:
        case AUDIO_REGISTER_FAIL:
        case AUDIO_REGISTER_SUCCESS:
        case AUDIO_LOGIN_FAIL:
        case AUDIO_LOGIN_SUCCESS:
            break;
        default:
            return 0;
    }

    pthread_mutex_lock(&mutex);
    gCurBeep = id;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return 0;
}

void* warn_audio_handle(void* arg)
{
    int needBeep = -1;

    unsigned long mask = BIND_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("Set Audio thread affinity fail!\n");
    }

    while(!gAudioExit)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        needBeep = gCurBeep;
        pthread_mutex_unlock(&mutex);
        if(needBeep == -1) {
            break;
        }

//        audio_play(needBeep);
        isBeeping = true;
        if(play_audio(needBeep) != 0){
            printf("beep type: %d fail\n", needBeep);
        }

#if !USE_ALSA
        //It's not cost time when we use system to beep.
        sleep(2);
#endif
        isBeeping = false;

    }
    return NULL;
}

int audio_play(int id)
{
    switch(id)
    {
        case AUDIO_LD:
            play_ld_audio();
            break;
        case AUDIO_HMW:
            play_hmw_audio();
            break;
        case AUDIO_TTC:
            play_ttc_audio();
            break;
        case AUDIO_TDMAP:
            play_tdmap_audio();
            break;
        case AUDIO_RADAR:
            break;
        case AUDIO_DSM_DIST:
            play_dsm_audio();
            break;
        case AUDIO_DSM_DROW:
            play_dsm1_audio();
            break;
        default:
            break;
    }

    return 0;
}
