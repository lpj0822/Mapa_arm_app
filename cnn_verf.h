#ifndef __CNN_VERF_H__
#define __CNN_VERF_H__

#ifdef __cplusplus
extern "C" {
#endif

int cnn_before();
int cnn_init(int id);
int cnn_main(int id, void* buf, unsigned char *result);
int cnn_release();
int cnn_after();

#ifdef __cplusplus
}
#endif


#endif
