#ifndef __BEEP_INTERFACE_H__
#define __BEEP_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

int Beep_Init();

int Beep_Play(int freq, int reps, int delay, int length);

int Beep_Uninit();

#ifdef __cplusplus
}
#endif


#endif

