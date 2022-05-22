/**
 * test_oryx_video_reader.cpp
 *
 *  History:
 *    Aug 11, 2015 - [Shupeng Ren] created file
 *    Dec 19, 2016 - [Ning Zhang] modified file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <unistd.h>
#include <sys/uio.h>

#include "am_base_include.h"
#include "am_define.h"
#include "am_log.h"
#include <signal.h>
#include <getopt.h>
#include "am_log.h"
#include "am_video_reader_if.h"
#include "am_video_address_if.h"
#include "YUV_Interface.h"
#include "PublicDefine.h"
#include "utils.h"

#define TIME_GAP 35
#define BIND_CPU_ID 0x4 //(cpu2)

static int gExit = 0;
static int gFrameNo = 0;
static pthread_t tid = -1;
static pthread_mutex_t mutex;
static char* gpCamBuf = NULL;
static char* yuvbuf[CACHE_BUF_NUM];
static bool gIsFromCam = true;
static char* gReadDir = NULL;
static int gFrameNoUped = -1;
static bool m_gdma_support = false;
static AMAddress g_usr_addr;

#define  SOURCE_BUFFER_ID  1

static AMIVideoReaderPtr greader  = nullptr;
static AMIVideoAddressPtr gaddress = nullptr;

static int dump_latest_yuv(int index) {

    AMQueryFrameDesc desc;

    if (AM_RESULT_OK != greader->query_yuv_frame(desc,
                AM_SOURCE_BUFFER_ID(SOURCE_BUFFER_ID), false)) {
        ERROR("Query yuv frame failed.");
        return -1;
    }

    int user_offset = (FRAME_WID*FRAME_HIG*index*3);
    do {
        //copy y buffer
        if (AM_RESULT_OK !=
                greader->gdmacpy_dsp2usr(
                    user_offset,
                    desc.yuv.y_offset,
                    FRAME_WID,
                    FRAME_HIG,
                    FRAME_WID,
                    desc.yuv.pitch)) {
            ERROR("Gdmacpy y buffer failed");
            break;
        }
        /*
        //copy uv buffer
        if (AM_RESULT_OK !=
                greader->gdmacpy_dsp2usr(
                    user_offset+(FRAME_WID*FRAME_HIG<<1),
                    desc.yuv.uv_offset,
                    FRAME_WID,
                    FRAME_HIG,
                    FRAME_WID,
                    desc.yuv.pitch)) {
            ERROR("Gdmacpy uv buffer failed");
            break;
        }
        */
        yuvbuf[index] = (char*)g_usr_addr.data+user_offset;
        //printf("YUV addr: %p\n", yuvbuf[index]);

    } while (0);

    return 0;
}

static int dump_latest_yuv(void* buf) {

    if (NULL == buf)
    {
        return -1;
    }
    AMQueryFrameDesc frame_desc;
    struct iovec y = {0};
    struct iovec uv = {0};
    AMAddress yaddr;
    AMAddress uvaddr;
    char* y_ptr = (char*)buf;
    char* uv_ptr = nullptr;
    int32_t uv_height = 0;

    if (AM_RESULT_OK != greader->query_yuv_frame(frame_desc,
                AM_SOURCE_BUFFER_ID(SOURCE_BUFFER_ID), false)) {
        ERROR("Query yuv frame failed.");
        return -1;
    }
    if (AM_RESULT_OK != gaddress->yuv_y_addr_get(frame_desc, yaddr)) {
        ERROR("Failed to get y address!");
        return -1;
    }
    if (AM_RESULT_OK != gaddress->yuv_uv_addr_get(frame_desc, uvaddr)) {
        ERROR("Failed to get uv address!");
        return -1;
    }
    if (frame_desc.yuv.format == AM_CHROMA_FORMAT_YUV420) {
        uv_height = frame_desc.yuv.height / 2;
    } else if (frame_desc.yuv.format == AM_CHROMA_FORMAT_YUV422) {
        //save it to YUV422 NV16 format
        uv_height = frame_desc.yuv.height;
    } else {
        ERROR("Unsupported chroma format: %d in YUV dump.", frame_desc.yuv.format);
        return -1;
    }
    //copy y buffer
    y.iov_len = frame_desc.yuv.width * frame_desc.yuv.height;
    uv_ptr = y_ptr+y.iov_len;
    uv.iov_len = frame_desc.yuv.width * uv_height;

    //printf("wid:%d, pitch:%d, yaddr:%p, uvaddr:%p\n",frame_desc.yuv.width, frame_desc.yuv.pitch, yaddr.data, uvaddr.data);
    if (frame_desc.yuv.pitch == frame_desc.yuv.width) {
        memcpy(y_ptr, yaddr.data, y.iov_len);
        memcpy(uv_ptr, uvaddr.data, uv.iov_len);
    } else {
        for (uint32_t i = 0; i < frame_desc.yuv.height ; i++) {
            memcpy(y_ptr, yaddr.data + i * frame_desc.yuv.pitch,
                    frame_desc.yuv.width);
            y_ptr += frame_desc.yuv.width;
        }
        for (int32_t i = 0; i < uv_height; i++) {
            memcpy(uv_ptr, uvaddr.data + i * frame_desc.yuv.pitch,
                    frame_desc.yuv.width);
            uv_ptr += frame_desc.yuv.width;
        }
    }

    return 0;
}

int read_yuv_from_cam(void* buf)
{

    if ((dump_latest_yuv(buf))
            != AM_RESULT_OK) {
        ERROR("Failed to dump yuv.");
        return -1;
    }

    return 0;
}

int read_yuv_deinit(void){
    int Reval = 0;
    
    if(gaddress != NULL)
    {
    }

    return Reval;
}

int read_yuv_from_file(int size, void* buf, const char* path)
{
    if(!buf)
        return -1;

    int fd = open(path, O_RDWR, 0777);
    if(!fd)
    {
        printf("open file error\n");
        return -1;
    }

    if(0 >= read(fd, buf, size))
    {
        printf("read file error\n");
        return -1;
    }
    close(fd);

    return 0;
}

void* read_cam_loop(void* arg) {

    int bufIndex = 0;
    unsigned int time = 0;
    int costTime = 0;
    unsigned long mask = BIND_CPU_ID;
    if( 0 != sched_setaffinity(0,sizeof(mask),(cpu_set_t*)(&mask)))
    {
        printf("MAIN READ YUV set cpu core fail!\n");
    }

    while(!gExit) {

        bufIndex = gFrameNo%CACHE_BUF_NUM;

        if (gIsFromCam) {
            if(m_gdma_support) {
                if ((dump_latest_yuv(bufIndex)) != AM_RESULT_OK) {
                    ERROR("Failed to dump yuv.");
                }
            } else {
                if ((dump_latest_yuv(yuvbuf[bufIndex])) != AM_RESULT_OK) {
                    ERROR("Failed to dump yuv.");
                }
            }
        } else {
            char name[128];
            sprintf(name, "/sdcard/%s/image%d.dat", gReadDir, gFrameNo);
            if(read_yuv_from_file(FRAME_WID*FRAME_HIG, yuvbuf[bufIndex], name) == -1)
            {
                printf("read error no:%d\n",gFrameNo);
                pthread_mutex_lock(&mutex);
                gFrameNo = -1;
                pthread_mutex_unlock(&mutex);
                break;
            }
        }

        pthread_mutex_lock(&mutex);
        gFrameNo++;
        pthread_mutex_unlock(&mutex);

        costTime = my_gettime() - time;
        time = my_gettime();
        if(costTime < TIME_GAP)
            usleep((TIME_GAP-costTime)*1000);
        //printf("Get YUV frame cost time : %d ms\n", costTime);

    }

    return NULL;
}

int YUV_init(bool isFromCam, const char* dirName) {

    gExit = 0;
    gReadDir = (char *)dirName;
    gIsFromCam = isFromCam;
    if(isFromCam) {
        if (!(greader = AMIVideoReader::get_instance())) {
            ERROR("Failed to create greader!");
            return -1;
        }

        if (!(gaddress = AMIVideoAddress::get_instance())) {
            ERROR("Failed to get instance of VideoAddress!");
            return -1;
        }

        m_gdma_support = greader->is_gdmacpy_support();
    }

    printf("DMA SUPPORT: %d\n", m_gdma_support);
    if(!m_gdma_support) {
        //FRAME_WID * (FRAME_HIG *2) * CACHE_BUF_NUM * 3 / 2)
        gpCamBuf = (char*)malloc(FRAME_WID*FRAME_HIG*CACHE_BUF_NUM*3);
        memset(gpCamBuf, 255, FRAME_WID*FRAME_HIG*CACHE_BUF_NUM*3);
        int i = 0;
        for(i = 0; i < CACHE_BUF_NUM; i++) {
            yuvbuf[i] = (char*)(gpCamBuf+(FRAME_WID*FRAME_HIG*i*3));
        }
    } else {
        if (AM_RESULT_OK != gaddress->usr_addr_get(g_usr_addr)) {
            ERROR("Failed to get usr addr.");
            return -1;
        }
    }

    pthread_mutex_init(&mutex, NULL);
    if(pthread_create(&tid, NULL, read_cam_loop,NULL) != 0)
        return -1;

    if (pthread_setname_np(tid, "readCam") != 0)
        printf("set thread readCam name fail\n");

    return 0;
}

int YUV_getBuffer(char** buf, int *frameNo) {

    pthread_mutex_lock(&mutex);
    *frameNo = gFrameNo-1;
    pthread_mutex_unlock(&mutex);

    if(gFrameNoUped == *frameNo) {
        *buf = NULL;
        if (*frameNo < 0)
            *frameNo = 0;
    } else {
        gFrameNoUped = *frameNo;
        int bufIndex = (*frameNo)%CACHE_BUF_NUM;
        *buf = yuvbuf[bufIndex];
    }

    return 0;
}

int YUV_deinit(void) {

    gExit = 1;
    void* re = NULL;
    if(tid > 0){
        pthread_join(tid,&re);
    }
    pthread_mutex_destroy(&mutex);

    if(gpCamBuf)
    {
        free(gpCamBuf);
        gpCamBuf = NULL;
    }

    return 0;
}
