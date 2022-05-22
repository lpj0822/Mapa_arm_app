#ifndef __USB_CAM_INTERFACE_H__
#define __USB_CAM_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

int USB_CAM_Init(int wid, int hig);
int USB_CAM_Capture(void** buf);
int USB_CAM_Release();

#ifdef __cplusplus
}
#endif

#endif
