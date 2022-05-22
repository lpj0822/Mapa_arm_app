#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kd.h>

#define DEFAULT_FREQ 440
#define DEFAULT_LENGTH 200
#define DEFAULT_REPS 1
#define DEFAULT_DELAY 100

typedef struct {
    int freq;
    int length;
    int reps;
    int delay;
}BEEP_PARMS_t;

void usage_bail(const char* executable_name){
    exit(1);
}

int console_fd = -1;

int Beep_Init(){
    int ReVal = 0;

    console_fd = open("/dev/console", O_WRONLY);
    if(console_fd == -1){
        printf("Failed to open console.\n");
        return -1;
    }

    return ReVal;
}

int Beep_Play(int freq, int reps, int delay, int length){
    int ReVal = 0;

    if(console_fd == -1)
    {
        printf("Cannot connect console device.\n");
        return -1;
    }
    for(int i = 0; i < reps; i++){
        int magical_fairy_number = 1190000/freq;
        ioctl(console_fd, KIOCSOUND, magical_fairy_number);/* beep start */
        usleep(1000*length); /* wait */
        ioctl(console_fd, KIOCSOUND, 0); /* beep stop */
        usleep(1000*delay); /* wait */
    }

    return ReVal;
}

int Beep_Uninit(){
    int ReVal = 0;

    if(console_fd != -1)
    {
        close(console_fd);
    }

    return ReVal;
}

