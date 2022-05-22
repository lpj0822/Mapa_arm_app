#ifndef __LOG_INTERFACE_H__
#define __LOG_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "SE_Interface.h"

int LOG_init(void);
int LOG_deinit(void);
int LOG_write(int frameNo, SE_UI_RESULT_s* result);
int LOG_recordErr(int code, const char* msg);

#ifdef __cplusplus
}
#endif


#endif
