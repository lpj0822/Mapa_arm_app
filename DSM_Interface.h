#ifndef __DSM_INTERFACE_H__
#define __DSM_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif
#define DSM_EVENT_LOGIN_FAIL        0
#define DSM_EVENT_LOGIN_SUCCESS     1
#define DSM_EVENT_REGISTER_FAIL     2
#define DSM_EVENT_REGISTER_SUCCESS  3
#define DSM_EVENT_CALIB_FAIL        4
#define DSM_EVENT_CALIB_SUCCESS     5
#define DSM_EVENT_HAND_OK           6

    typedef void(*DSM_EVENT_HANDLE_f)(int);

    typedef struct DSM_RESULT {
        unsigned char sleep;
        unsigned char hasFace;
        unsigned char distraction;
        int faceX;
        int faceY;
        int faceWid;
        int faceHig;
    }DSM_RESULT_t;

    int DSM_init(void);
    int DSM_deinit(void);
    int DSM_getresult(DSM_RESULT_t* result);
    int DSM_register_event_handle(DSM_EVENT_HANDLE_f handle);


#ifdef __cplusplus
}
#endif

#endif
