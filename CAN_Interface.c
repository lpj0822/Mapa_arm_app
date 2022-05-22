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

#include "CAN_Interface.h"

#define BUF_SIZE	(256)
#define UART_DEV "/dev/ttyS2"
#define CAN_CPU_ID  0x4 //(cpu2)

#define BOTELI      0
#define USE_CAN     1

static int gExit = 0;
static int uart_fd = -1;
static pthread_t tid;
static CAN_INFO_t gCanInfo = {-1.0f, 0.0f, 0, 0};
static pthread_mutex_t mutex;
static void* UpdateCAN(void* arg);
static int gStatus = 0;

int uart_config(int fd, int speed, int flow)
{
	struct termios ti;

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &ti) < 0) {
		perror("Can't get port settings");
		return -1;
	}

	cfmakeraw(&ti);

	ti.c_cflag |= CLOCAL;
	if (flow)
		ti.c_cflag |= CRTSCTS;
	else
		ti.c_cflag &= ~CRTSCTS;

	cfsetospeed(&ti, speed);
	cfsetispeed(&ti, speed);

	if (tcsetattr(fd, TCSANOW, &ti) < 0) {
		perror("Can't set port settings");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);

	return 0;
}

int CAN_init(void)
{
    gExit = 0;
    pthread_mutex_init(&mutex, NULL);

	uart_fd = open(UART_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
	//uart_fd = open(UART_DEV, O_RDWR);
    if (uart_fd < 0) {
		printf("Can't open serial port");
        gStatus = CAN_FAIL;
        return CAN_FAIL;
	}

#if USE_CAN
    uart_config(uart_fd, B9600, 0);
#endif

    if(pthread_create(&tid, NULL, UpdateCAN, NULL) != 0)
    {
        gStatus = CAN_FAIL;
        return CAN_FAIL;
    }

#if USE_CAN
    if (pthread_setname_np(tid, "CAN") != 0)
        printf("set thread CAN name fail\n");
#else
    if (pthread_setname_np(tid, "GPS") != 0)
        printf("set thread GPS name fail\n");
#endif

    return 0;
}

int CAN_deinit(void)
{
    gExit = 1;
    void* re = NULL;
    if(tid > 0){
        pthread_join(tid,&re);
    }
    pthread_mutex_destroy(&mutex);

    if(uart_fd > 0)
        close(uart_fd);

    return 0;
}

CAN_INFO_t CAN_getInfo(void)
{
    CAN_INFO_t value;

    pthread_mutex_lock(&mutex);
    value = gCanInfo;
    pthread_mutex_unlock(&mutex);

    return value;
}

float CAN_getSpeed(void)
{
    float value = 0;

    pthread_mutex_lock(&mutex);
    value = gCanInfo.speed;
    pthread_mutex_unlock(&mutex);

    return value;
}

static void* UpdateCAN(void* arg){

    int i = 0;
	int rval = 0;
	char buf[BUF_SIZE];
    char msg[BUF_SIZE];
    //int msgHeadSize = 0;
	memset(buf, 0, BUF_SIZE);
	memset(msg, 0, BUF_SIZE);
    //bool is_get_buf_start = false;
    unsigned long mask = CAN_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("CAN set cpu core fail!\n");
    }
    while(!gExit) {
		rval = read(uart_fd, buf, BUF_SIZE);
        if (rval < 0) {
			if (errno == EAGAIN) {
				usleep(20000);
				continue;
			} else {
				break;
			}
        }

#if USE_CAN
        for(i = 0; i < rval; i++)
        {
            if(buf[i] == 0xAA && i+11 <= rval && buf[i+1] == 0x55)
            {
                pthread_mutex_lock(&mutex);
                //km/h to m/s
                gCanInfo.speed = ((float)((buf[i+9]<<8) + buf[i+8]))/3.6f;
                gCanInfo.gear = buf[i+5];
                gCanInfo.corneringLamp  = buf[i+4];
#if BOTELI
                int steel = (buf[i+7]<<8) + buf[i+6];
                if(steel != 0) 
                    gCanInfo.steel = ((float)((buf[i+7]<<8) + buf[i+6]))/16.0f-2048;
#else
                gCanInfo.steel = ((float)((buf[i+7]<<8) + buf[i+6]))/1000.0f;
#endif
                if(gCanInfo.gear == 9 && gCanInfo.speed > 0.0f)
                    gCanInfo.speed = gCanInfo.speed*-1;
                //                printf("CAN SPEED: %f -- STEEL: %f -- GEAR:%d -- CORNERING LAMP: %d\n",gCanInfo.speed, gCanInfo.steel, gCanInfo.gear, gCanInfo.corneringLamp);
                pthread_mutex_unlock(&mutex);
            }
        }
#else //USE_CAN
        
        

        if (is_get_buf_start) {
            is_get_buf_start = false;
            int time = 0;
            char state = '0';
            float latitude = 0.0f;
            float longitude = 0.0f;
            float speed = 0.0f;
            float direction = 0.0f;
            int date = 0;
            char mode = '0';
            memcpy(msg+msgHeadSize,buf,rval);

            sscanf(msg,"GNRMC,%d.000,%c,%f,N,%f,E,%f,%f,%d,,,%c*",&time,&state,&latitude,&longitude, &speed,&direction, &date,&mode);
            //printf("state: %c;speed:%f\n",state, speed*1.852);
            if('A' == state)
            {
                pthread_mutex_lock(&mutex);
                //0.515=1.852/3.6
                gCanInfo.speed = speed*0.515;
                pthread_mutex_unlock(&mutex);
            }
        } else {
            char* subBuf = buf;
            for(i = 0; i < rval; i++) {
                if(*subBuf == '$')
                    break;
                else
                    subBuf++;
            }
            if(*subBuf == '$') {
                subBuf++;
                if(strncmp("GNRMC", subBuf, 5) == 0){
                    is_get_buf_start = true;
                    memcpy(msg, subBuf,rval-i-1);
                    msgHeadSize = rval-i-1;
                }
            }
        }
 
#endif //USE_CAN
    }
    return NULL;
}


