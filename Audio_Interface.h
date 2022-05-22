#ifndef __AUDIO_INTERFACE_H__
#define __AUDIO_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_LD                0
#define AUDIO_HMW               1
#define AUDIO_TTC               2
#define AUDIO_TDMAP             3
#define AUDIO_RADAR             4
#define AUDIO_DSM_DIST          5
#define AUDIO_DSM_DROW          6
#define AUDIO_LOGIN_FAIL        7
#define AUDIO_LOGIN_SUCCESS     8
#define AUDIO_REGISTER_FAIL     9
#define AUDIO_REGISTER_SUCCESS  10
#define AUDIO_CALIBRATED        11
#define AUDIO_CALIBRATING       12

int play_audio_send(int id, int level);
int play_audio_deinit(void);
int play_audio_init(void);
int audio_play(int id);

#ifdef __cplusplus
}
#endif


#endif
