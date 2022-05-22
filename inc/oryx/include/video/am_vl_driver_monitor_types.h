/*******************************************************************************
 * am_vl_driver_monitor_types.h
 *
 * History:
 *   Jan 17, 2018 - [Guohua Zheng] created file
 *
 * Copyright (c) 2018 Ambarella, Inc.
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
 *
 ******************************************************************************/
#ifndef ORYX_INCLUDE_VIDEO_AM_VL_DRIVER_MONITOR_TYPES_H_
#define ORYX_INCLUDE_VIDEO_AM_VL_DRIVER_MONITOR_TYPES_H_

#include "am_video_types.h"

#define DM_MAX_DETECT_FACE_NUM 10
#define VL_MARKS_NUM_5 5
#define VL_MARKS_NUM_68 68


struct AMVLRect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

struct AMVLDetection {
    AMVLRect rect;
    float score;
};

struct AMVLAngles {
    float pitch;
    float yaw;
    float roll;
};

struct AMVLEyeStates {
    int32_t left;
    int32_t right;
};

struct AMVLScores {
    float eye_bow;
    float mouth;
    float smile;
};

struct AMVLFDINFO {
    AMVLDetection detection;
    AMVLAngles angles;
    AMVLEyeStates eye_states;
    AMVLScores scores;
    AMPoint landmarks68[VL_MARKS_NUM_68];
    AMPoint landmarks5[VL_MARKS_NUM_5];
};

struct AMDMSFDMessage {
    int face_num;
    AMVLFDINFO face_info;
    AMRect capture_window;
};

typedef int (*AMDMSFDCallback)(void *owner, AMDMSFDMessage *event_msg);
#endif /* ORYX_INCLUDE_VIDEO_AM_FACE_DETECT_TYPES_H_ */
