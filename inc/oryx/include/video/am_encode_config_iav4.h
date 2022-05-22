/*******************************************************************************
 * am_encode_config_iav4.h
 *
 * History:
 *   Jul 4, 2017 - [Dong Shiming] created file
 *
 * Copyright (c) 2017 Ambarella, Inc.
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
#ifndef ORYX_INCLUDE_VIDEO_AM_ENCODE_IAV_CONFIG_H_
#define ORYX_INCLUDE_VIDEO_AM_ENCODE_IAV_CONFIG_H_

#include "am_video_types.h"

struct AMBufferPlatformConfig
{
  std::pair<bool, AM_MULTI_VIN_CHAN_ID> channel_id = {false, AM_MULTI_VIN_CHAN_ID_INVALID};
  std::pair<bool, AMRect>               output     = {false, {}};
  std::pair<bool, AMResolution>         max_output = {false, {}};
  std::pair<bool, uint16_t>             canvas_id  = {false, AM_INVALID_CANVAS_ID};
  std::pair<bool, bool>                 need_apply = {false, false};
  std::pair<bool, int32_t>              cap_skip_itvl = {false, 0};
  std::pair<bool, bool>                 auto_stop = {false, false};
};

#endif
