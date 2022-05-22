/*
 * test_video_service_dyn_air_api.cpp
 *
 *  History:
 *    Nov 27, 2015 - [Shupeng Ren] created file
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
#include <stdint.h>
#include <getopt.h>
#include <math.h>
#include <string>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "am_base_include.h"
#include "am_log.h"
#include "am_define.h"

#include "am_api_helper.h"
#include "am_api_video.h"
#include "PublicDefine.h"

static AMAPIHelperPtr g_api_helper = nullptr;
static float xScale = 1.0f,yScale = 1.0f;

int32_t add_line(unsigned int color, int x, int y, int x1, int y1)
{
    am_service_result_t service_result = {0};
    am_overlay_data_t  data = {0};
    data.stream_id = 3;
    data.area_id = 0;
    int left,top;
    left = x>x1?x1:x;
    top = y>y1?y1:y;
    data.data_num = 1;
    data.type = 4;
    memset(data.line, 0, OVERLAY_MAX_LINE_NUM * sizeof(am_overlay_line_data_t));
    am_overlay_line_data_t &line = data.line[0];
    SET_BIT(line.enable_bits, AM_OVERLAY_DATA_ADD_EN_BIT);
    SET_BIT(line.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    line.offset_x = left;
    line.offset_y = top;
    line.width = x>x1?x-x1:(x==x1?1:x1-x);
    line.height = y>y1?y-y1:(y==y1?1:y1-y);
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_COLOR_EN_BIT);
    line.line_color = color;
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_THICKNESS_EN_BIT);
    line.line_tn = 1;
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_POINTS_EN_BIT);
    line.p_n = 2;
    line.p_x[0] = x==left?0:x-left-1;
    line.p_y[0] = y==top?0:y-top-1;
    line.p_x[1] = x1==left?0:x1-left-1;
    line.p_y[1] = y1==top?0:y1-top-1;


    g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DATA_ADD,
            &data, sizeof(data),
            &service_result, sizeof(service_result));
    AM_RESULT call_ret = AM_RESULT(service_result.ret);
    if (call_ret == AM_RESULT_ERR_PLUGIN_LOAD) {
        ERROR("Overlay plugin is not loaded!\n");
        return -1;
    } else if (call_ret != AM_RESULT_OK) {
        ERROR("failed to add data to area!\n");
        return -1;
    }
    return 0;

}


int32_t add_rect(unsigned int color,int x, int y, int wid, int hig)
{
    am_service_result_t service_result = {0};
    am_overlay_data_t  data = {0};
    data.stream_id = 3;
    data.area_id = 0;
    data.data_num = 1;
    data.type = 4;
    memset(data.line, 0, OVERLAY_MAX_LINE_NUM * sizeof(am_overlay_line_data_t));
    am_overlay_line_data_t &line = data.line[0];
    SET_BIT(line.enable_bits, AM_OVERLAY_DATA_ADD_EN_BIT);
    SET_BIT(line.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    line.offset_x = x;
    line.offset_y = y;
    line.width = wid;
    line.height = hig;
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_COLOR_EN_BIT);
    line.line_color = color;
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_THICKNESS_EN_BIT);
    line.line_tn = 1;
    SET_BIT(line.enable_bits, AM_OVERLAY_LINE_POINTS_EN_BIT);
    line.p_n = 5;
    line.p_x[0] = 0;
    line.p_y[0] = 0;
    line.p_x[1] = wid-1;
    line.p_y[1] = 0;
    line.p_x[2] = wid-1;
    line.p_y[2] = hig-1;
    line.p_x[3] = 0;
    line.p_y[3] = hig-1;
    line.p_x[4] = 0;
    line.p_y[4] = 0;


    g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DATA_ADD,
            &data, sizeof(data),
            &service_result, sizeof(service_result));
    AM_RESULT call_ret = AM_RESULT(service_result.ret);
    if (call_ret == AM_RESULT_ERR_PLUGIN_LOAD) {
        ERROR("Overlay plugin is not loaded!\n");
        return -1;
    } else if (call_ret != AM_RESULT_OK) {
        ERROR("failed to add data to area!\n");
        return -1;
    }
    return 0;

}
int32_t init_osd()
{

    /********************** init area *****************************/
    am_service_result_t service_result = {0};
    am_overlay_area_t area;

    SET_BIT(area.enable_bits, AM_OVERLAY_INIT_EN_BIT);
    area.stream_id = 3;
    SET_BIT(area.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    area.offset_x = 0;

    SET_BIT(area.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    area.offset_y = 0;
    SET_BIT(area.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    area.width = 640;
    SET_BIT(area.enable_bits, AM_OVERLAY_RECT_EN_BIT);
    area.height = 360;
    SET_BIT(area.enable_bits, AM_OVERLAY_ROTATE_EN_BIT);
    area.rotate = 0;
    SET_BIT(area.enable_bits, AM_OVERLAY_BG_COLOR_EN_BIT);
    area.bg_color = 0;
    SET_BIT(area.enable_bits, AM_OVERLAY_BUF_NUM_EN_BIT);
    area.buf_num = 1;

    g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_INIT,
            &area, sizeof(area),
            &service_result, sizeof(service_result));
    AM_RESULT ret = AM_RESULT(service_result.ret);
    if (ret == AM_RESULT_ERR_PLUGIN_LOAD) {
        ERROR("Overlay plugin is not loaded!\n");
        return -1;
    } else if (ret != AM_RESULT_OK) {
        ERROR("failed to init overlay area!\n");
        return -1;
    }

    /******************* enable area *****************************/
    am_overlay_id_s param;
    param.stream_id = 3;
    SET_BIT(param.enable_bits, AM_OVERLAY_ENABLE_EN_BIT);
    param.area_id = 0;

    g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_SET,
            &param, sizeof(param),
            &service_result, sizeof(service_result));
    ret = AM_RESULT(service_result.ret);
    if (ret == AM_RESULT_ERR_PLUGIN_LOAD) {
        ERROR("Overlay plugin is not loaded!\n");
        return -1;
    } else if (ret != AM_RESULT_OK) {
        ERROR("failed to set overlay!\n");
        return -1;
    }
    return 0;
}

int32_t clear_osd()
{
    AM_RESULT ret = AM_RESULT_OK;
    am_service_result_t service_result;
    am_overlay_id_s param;
    param.stream_id = 3;
    SET_BIT(param.enable_bits, AM_OVERLAY_REMOVE_EN_BIT);
    param.area_id = 0;
    g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_SET,
    //g_api_helper->method_call(AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DESTROY, nullptr, 0,
                            &param, sizeof(param),
                            &service_result, sizeof(service_result));
    ret = AM_RESULT(service_result.ret);
    if (ret == AM_RESULT_ERR_PLUGIN_LOAD) {
        ERROR("Overlay plugin is not loaded!\n");
        return -1;
    } else if (ret != AM_RESULT_OK) {
        ERROR("failed to destroy overlay!\n");
        return -1;
    }
    return 0;
}
unsigned int my_gettime(void) {
	static struct timeval tv = { 0, 0 };
	if (!tv.tv_sec) {
		gettimeofday(&tv, NULL);
		return 0;
	}

	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);

	return (((cur_time.tv_sec - tv.tv_sec) * 1000)
			+ ((cur_time.tv_usec - tv.tv_usec) / 1000));
}


int OVERLAY_init(void)
{
    g_api_helper = AMAPIHelper::get_instance();
    if (!g_api_helper) {
        ERROR("unable to get AMAPIHelper instance\n");
        return -1;
    }
    xScale = 1.0f*OVERLAY_WID/FRAME_WID;
    yScale = 1.0f*OVERLAY_HIG/FRAME_HIG;
    return 0;
}
int OVERLAY_clean(void)
{

    clear_osd();
    init_osd();
    return 0;
}
int OVERLAY_line(unsigned int color, int x1, int y1, int x2, int y2)
{
    
    add_line(color,(int)(x1*xScale), (int)(y1*yScale),(int)(x2*xScale), (int)(y2*yScale));
    return 0;
}
int OVERLAY_rect(unsigned int color, int x, int y, int wid, int hig)
{

    add_rect(color,(int)(x*xScale), (int)(y*yScale),(int)(wid*xScale), (int)(hig*yScale));
    return 0;
}
int OVERLAY_show(void)
{
    return 0;
}
int OVERLAY_deinit(void)
{
    return 0;
}


/*
int32_t main(int32_t argc, char **argv)
{
    
    g_api_helper = AMAPIHelper::get_instance();
    if (!g_api_helper) {
        ERROR("unable to get AMAPIHelper instance\n");
        return -1;
    }
    int i = 0;
    unsigned time = my_gettime();
    for(i = 0; i < 100; i++)
    {
        time = my_gettime();
        clear_osd();
        init_osd();
        add_rect(2,i*2+300,i,50,50);
        add_rect(2,i*2+320,i+50,50,50);
        add_rect(2,i*2+400,i+100,50,50);
        add_line(2,i*2+200,100,i*2+150,150);
        printf("over show %d...\n",my_gettime()-time);
        usleep(30000);
    }

    return 0;
}
*/
