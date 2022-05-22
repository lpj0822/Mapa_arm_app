/*******************************************************************************
 * test_cnn_generic.cpp
 *
 * History:
 *  2017/11/20 - [Zhi He] create file
 *
 *
 * Copyright (C) 2015 Ambarella, Inc.
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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "opt_sc_algos_interface.h"
#include "cnn_verf.h"
#include "cnn_generic.h"

#define CNN_LENET_OUTPUT_CHANNEL_NUM    (2)
#define INSTANCE_COUNT  4
    
static int status = 0;
static opt_sc_algos_context_t algo_ctx;

typedef struct {
    void *cnn_generic_ctx;
    const char *param_file_path;
    const char *new_param_path;
    const char *net_name;
    const char *compress_type;
    int input_width;
    int input_height;
    int thread_number;
    int exponent_bit;
    int loop_count;
    unsigned char b_print_out;
} test_cnn_generic_context_t;

test_cnn_generic_context_t context[INSTANCE_COUNT];
float data_out[INSTANCE_COUNT][CNN_LENET_OUTPUT_CHANNEL_NUM];

static unsigned char parse_gen_result(float *data_out) {
    float max_prob = 0;
    int max_prob_object = 0;
    unsigned char i;

    for (i = 0; i < CNN_LENET_OUTPUT_CHANNEL_NUM; i++) {
        if(max_prob < data_out[i]){
            //find highest score of the digital item
            max_prob = data_out[i];
            max_prob_object = i;
        }
    }

    //printf("Digit is %d, Score is %f\n", max_prob_object, max_prob);

    return max_prob_object;
}

int cnn_before()
{

    int ret = 1;
    ret = initialize_opt_sc_algos_context(&algo_ctx);
    if (ret) {
        printf("initialize_opt_sc_algos_context fail\n");
        status = -2;
        return (-2);
    }

    return 0;
}

int cnn_init(int id)
{

    if(status != 0)
    {
        printf("cnn init fail\n");
        return status;
    }
    int input_channels, input_width, input_height;
    char paramsPath[128];
    sprintf(paramsPath, "/wissen/car_params%d",id);
    memset(&context[id], 0x0, sizeof(test_cnn_generic_context_t));

    context[id].thread_number = 2;
    context[id].loop_count = 1;
    context[id].input_height = 25;
    context[id].input_width = 25;
    context[id].param_file_path = paramsPath;
    context[id].net_name = "car";

    context[id].cnn_generic_ctx = cnn_generic_create_context(&algo_ctx, context[id].param_file_path,
            context[id].net_name,
            context[id].thread_number,
            context[id].input_width,
            context[id].input_height,
            context[id].compress_type,
            context[id].new_param_path,
            context[id].exponent_bit, 0);

    if (!context[id].cnn_generic_ctx) {
        printf("error: load cnn[%d] fail\n", id);
        return (-3);
    }

    cnn_generic_query_data_source(context[id].cnn_generic_ctx, &input_channels, &input_width, &input_height);

    return 0;
}

int cnn_main(int id, void* buf, unsigned char *result){
    int ret = 0;

    if(status != 0)
    {
        printf("cnn init fail\n");
        *result = 0;
        return status;
    }

    //struct timeval tv1, tv2;
    //long us;

    //gettimeofday(&tv1, NULL);

    cnn_generic_forward(context[id].cnn_generic_ctx, (float*) buf, data_out[id]);

    //gettimeofday(&tv2, NULL);
    //us = (long) 1000000 * (long) (tv2.tv_sec - tv1.tv_sec) + (long) (tv2.tv_usec - tv1.tv_usec);

    //printf("do forward use total %ld us\n", us);
    *result = parse_gen_result(data_out[id]);

    return ret;
}


int cnn_release(){
    for(int i = 0; i < INSTANCE_COUNT; i++){
        if(context[i].cnn_generic_ctx != NULL){
            cnn_generic_destroy_context(context[i].cnn_generic_ctx);
            context[i].cnn_generic_ctx = NULL;
        }
    }
    return 0;
}

int cnn_after()
{
    destroy_opt_sc_algos_context(&algo_ctx);
    return 0;
}

