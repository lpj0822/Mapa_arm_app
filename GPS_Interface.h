#ifndef __GPS_INTERFACE_H__
#define __GPS_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GPS_OK      0
#define GPS_FAIL    1
#define GPS_START   2

int GPS_init(void);
int GPS_deinit(void);
float GPS_getSpeed(void);

#ifdef __cplusplus
}
#endif

#endif
