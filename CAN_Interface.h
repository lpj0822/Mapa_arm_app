#ifndef __CAN_INTERFACE_H__
#define __CAN_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_OK      0
#define CAN_FAIL    1
#define CAN_START   2

    typedef struct CAN_INFO {
        float speed;
        float steel;
        unsigned char gear;
        unsigned char corneringLamp;
    }CAN_INFO_t;

    int CAN_init(void);
    int CAN_deinit(void);
    float CAN_getSpeed(void);
    CAN_INFO_t CAN_getInfo(void);

#ifdef __cplusplus
}
#endif

#endif
