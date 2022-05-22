/*******************************************************************************
 * am_dptz_if.h
 *
 * History:
 *   Mar 28, 2016 - [zfgong] created file
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
#ifndef AM_DPTZ_IF_H_
#define AM_DPTZ_IF_H_

#include "am_dptz_types.h"

class AMIDPTZ
{
  public:
    virtual AM_RESULT set_ratio(AM_MULTI_VIN_CHAN_ID chan_id,
                                AM_SOURCE_BUFFER_ID buf_id,
                                AMDPTZRatio &ratio) = 0;
    virtual AM_RESULT get_ratio(AM_MULTI_VIN_CHAN_ID chan_id,
                                AM_SOURCE_BUFFER_ID buf_id,
                                AMDPTZRatio &ratio) = 0;
    virtual AM_RESULT set_size(AM_MULTI_VIN_CHAN_ID chan_id,
                               AM_SOURCE_BUFFER_ID buf_id,
                               AMDPTZSize &size) = 0;
    virtual AM_RESULT get_size(AM_MULTI_VIN_CHAN_ID chan_id,
                               AM_SOURCE_BUFFER_ID buf_id,
                               AMDPTZSize &size) = 0;

  protected:
    virtual ~AMIDPTZ(){}
};

#define VIDEO_PLUGIN_DPTZ     ("dptz")
#define VIDEO_PLUGIN_DPTZ_SO ("video-dptz.so")


#endif /* AM_DPTZ_IF_H_ */
