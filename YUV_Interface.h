#ifndef __YUV_INTERFACE_H__
#define __YUV_INTERFACE_H__

int YUV_init(bool isFromCam, const char* dirName);
int YUV_getBuffer(char** buf, int *frameNo);
int YUV_deinit(void);

#endif
