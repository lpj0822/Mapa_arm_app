#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#ifdef __UCLIBC__
#include <bits/getopt.h>
#else
#include <getopt.h>
#endif
#include <termios.h>

#include "GPS_Interface.h"

#define UART_DEV "/dev/ttyS2"

static int fd;
static int gExit = 0;
static pthread_t tid = -1;
static float gSpeed = -1.0f;
static pthread_mutex_t mutex;
static void* UpdateGPS(void* arg);
static int gStatus = 0;

int GPS_init(void)
{

    gExit = 0;
    pthread_mutex_init(&mutex, NULL);
    //fd = open(UART_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
    fd = open(UART_DEV, O_RDWR);
    if (fd < 0) {
        gStatus = GPS_FAIL;
        return GPS_FAIL;
    }

    if(pthread_create(&tid, NULL, UpdateGPS,NULL) != 0)
        return GPS_FAIL;

    if (pthread_setname_np(tid, "GPS") != 0)
        printf("set thread GPS name fail\n");


    return GPS_OK;
}

int GPS_deinit(void)
{
    int ReVal = 0;

    if(gStatus == GPS_FAIL)
    {
        printf("GPS status fail\n");
        return -1;
    }

    gExit = 1;
    void* re = NULL;
    if(tid > 0){
        pthread_join(tid,&re);
    }
    pthread_mutex_destroy(&mutex);

    if(fd>0)
        close(fd);

    return ReVal;
}

float GPS_getSpeed(void)
{
    float value = 0;

    pthread_mutex_lock(&mutex);
    value = gSpeed;
    pthread_mutex_unlock(&mutex);

    return value;
}

static void* UpdateGPS(void* arg){
    int res = 0;
    char  buf[256];
    while(!gExit) {
        res = read(fd, buf, 256);

        if (res < 0) {
            if (errno == EAGAIN) {
  //              usleep(50000);
                printf("GPS read null\n");
                continue;
            } else {
                printf("GPS read fail and exit\n");
                break;
            }
        }

        char *splitBuf = NULL;
        for(int i = 0; i < res; i ++)
        {
            splitBuf = &buf[i];
            if(('a' <= splitBuf[0] && 'z' >= splitBuf[0]) || ('A' <= splitBuf[0] && 'Z' >= splitBuf[0]))
                break;
        }

        if(strncmp("GNRMC", splitBuf, 5) == 0){
            int time = 0;
            char state = '0';
            float latitude = 0.0f;
            float longitude = 0.0f;
            float speed = 0.0f;
            float direction = 0.0f;
            int date = 0;
            char mode = '0';
            char gpsBuf[128];
            int index = 0;
            for(int i=0;i<res;i++)
            {
                if(splitBuf[i] != '\n')
                    gpsBuf[index++] = splitBuf[i]; 
            }
            gpsBuf[index] = '\0';
//            printf("GPS: %s\n", gpsBuf);

            sscanf(gpsBuf,"GNRMC,%d.000,%c,%f,N,%f,E,%f,%f,%d,,,%c*",&time,&state,&latitude,&longitude, &speed,&direction, &date,&mode);
            //printf("state: %c;speed:%f\n",state, speed*1.852);
            if('A' == state)
            {
                pthread_mutex_lock(&mutex);
                //0.515=1.852/3.6
                gSpeed = speed*0.515;
                pthread_mutex_unlock(&mutex);
            }
//            usleep(800000);
 //       }else{
 //           usleep(10000);
        }
    }
    return NULL;
}


