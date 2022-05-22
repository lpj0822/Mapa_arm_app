/*******************************************************************************
 * am_encode_eis_if.h
 *
 * History:
 *   Feb 26, 2016 - [smdong] created file
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
 *
 ******************************************************************************/
#ifndef ORYX_INCLUDE_VIDEO_AM_ENCODE_EIS_IF_H_
#define ORYX_INCLUDE_VIDEO_AM_ENCODE_EIS_IF_H_

#include "am_video_types.h"
#include <string>

enum EIS_THREAD_PRI {
  NORMAL_PRIORITY,
  HIGH_PRIORITY,
};

class AMIEncodeEIS
{
  public:
    virtual AM_RESULT set_eis_mode(int32_t mode)  = 0;
    virtual AM_RESULT get_eis_mode(int32_t &mode) = 0;
    virtual AM_RESULT apply(EIS_THREAD_PRI priority = NORMAL_PRIORITY) = 0;
    virtual AM_RESULT load_config() = 0;
    virtual AM_RESULT save_config() = 0;

  protected:
    virtual ~AMIEncodeEIS(){}
};

#define VIDEO_PLUGIN_EIS    ("eis")
#define VIDEO_PLUGIN_EIS_SO ("video-eis.so")

#endif /* ORYX_INCLUDE_VIDEO_AM_ENCODE_EIS_IF_H_ */
