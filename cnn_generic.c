/*******************************************************************************
 *  cnn_generic.c
 *
 * History:
 *    2017/11/20 - [Zhi He] Create
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ( "Software" ) are protected by intellectual
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

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "signal.h"

#include "math.h"
#include "float.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"

#include "opt_sc_algos_interface.h"

#include "cnn_generic.h"

typedef struct {
    TInteger thread_number;
    void *p_thread_pool;
    void *p_cnn_pipeline;
    void *p_cnn_param_compress;
    void *p_cnn_param_decompress;
    cnn_config_t cnn_config;

    opt_sc_algos_context_t *algo_ctx;
} cnn_generic_context_t;

static void __cnn_generic_clean_context(cnn_generic_context_t *thiz)
{
    if (thiz) {
        //thiz->algo_ctx->clean_cnn(&thiz->cnn_config);
        if (thiz->p_thread_pool) {
            thiz->algo_ctx->destroy_simple_thread_pool(thiz->p_thread_pool);
            thiz->p_thread_pool = NULL;
        }
        //thiz->algo_ctx.cnn_destroy_pipeline(thiz->p_cnn_pipeline);
        if (thiz->p_cnn_pipeline) {
            thiz->algo_ctx->cnn_destroy_pipeline(thiz->p_cnn_pipeline);
            thiz->p_cnn_pipeline = NULL;
        }
        if (thiz->p_cnn_param_compress) {
            thiz->algo_ctx->cnn_destroy_pipeline(thiz->p_cnn_param_compress);
            thiz->p_cnn_param_compress = NULL;
        }
        if (thiz->p_cnn_param_decompress) {
            thiz->algo_ctx->cnn_destroy_pipeline(thiz->p_cnn_param_decompress);
            thiz->p_cnn_param_decompress = NULL;
        }
        free(thiz);
    }
}

void *cnn_generic_create_context(void *algo_ctx, const char *param_file_path,
    const char *net_name,
    TInteger thread_number,
    TInteger input_width,
    TInteger input_height,
    const char *compress_type,
    const char *new_param_path,
    TInteger exponent_bit,
    TInteger fill_random_params)
{
    int ret = 0;
    cnn_compress_t *compress = NULL;
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) malloc(sizeof(cnn_generic_context_t));

    if (!thiz) {
        printf("no memory\n");
        return NULL;
    }

    thiz->algo_ctx = (opt_sc_algos_context_t *) algo_ctx;

    memset(&thiz->cnn_config.p_compress, 0x0, sizeof(thiz->cnn_config.p_compress));
    thiz->cnn_config.path = param_file_path;
    thiz->cnn_config.new_path = new_param_path;
    thiz->cnn_config.net_name = net_name;
    thiz->cnn_config.compress_type_string = compress_type;

    if (thiz->cnn_config.new_path) {
        if (access(new_param_path, 0) != 0)
        {
            ret = mkdir(new_param_path, 0777);
            if (ret)
            {
                printf("mkdir new path error\n");
                 __cnn_generic_clean_context(thiz);
                return NULL;
            }
        }
    }

    ret = thiz->algo_ctx->read_cnn(param_file_path, net_name, &thiz->cnn_config);
    if (ret) {
        printf("error: read cnn file fail\n");
        __cnn_generic_clean_context(thiz);
        return NULL;
    }

    thiz->thread_number = thread_number;
    if (thiz->thread_number < 1) {
        printf("thread number %d less than 1?\n", thiz->thread_number);
        thiz->thread_number = 1;
    } else {
        thiz->p_thread_pool = thiz->algo_ctx->create_simple_thread_pool(thiz->thread_number);
        if (!thiz->p_thread_pool) {
            printf("error: thread pool create fail\n");
            thiz->thread_number = 1;
        }
    }

    compress = &thiz->cnn_config.p_compress;
    if (compress->param_type == ECNNParamType_F32) {
        if (compress->compress_type == ECNNCompressType_F32) {
            thiz->p_cnn_pipeline = thiz->algo_ctx->cnn_setup_pipeline(&thiz->cnn_config, input_width, input_height,
                NULL, thiz->thread_number, fill_random_params);
            thiz->p_cnn_param_compress = NULL;
            thiz->p_cnn_param_decompress = NULL;
        } else {
            if (exponent_bit) {
                compress->exponent_bit = exponent_bit;
            }
            thiz->p_cnn_param_compress = thiz->algo_ctx->cnn_compress_param_file(&thiz->cnn_config, input_width, input_height,
                NULL, thiz->thread_number, fill_random_params);
            thiz->p_cnn_pipeline = NULL;
            thiz->p_cnn_param_decompress = NULL;
        }
    } else {
        thiz->p_cnn_param_decompress = thiz->algo_ctx->cnn_decompress_and_setup_pipeline(&thiz->cnn_config, input_width, input_height,
            NULL, thiz->thread_number, fill_random_params);
            thiz->p_cnn_pipeline = NULL;
            thiz->p_cnn_param_compress = NULL;
    }

    if (!thiz) {
        printf("error: create cnn pipeline fail\n");
        __cnn_generic_clean_context(thiz);
        return NULL;
    }

    return thiz;
}

void cnn_generic_destroy_context(void *context)
{
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) context;
    if (thiz) {
        __cnn_generic_clean_context(thiz);
        /*if (&thiz->algo_ctx) {
            destroy_opt_sc_algos_context(&thiz->algo_ctx);
        }*/
    }
}

int cnn_generic_forward(void *context, TFloat *input, TFloat *output)
{
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) context;
    if (thiz && input) {
        if (thiz->p_cnn_pipeline) {
            thiz->algo_ctx->cnn_execute_pipeline(thiz->p_thread_pool, thiz->p_cnn_pipeline, input, output);
            return 0;
        }
        if (thiz->p_cnn_param_compress) {
            thiz->algo_ctx->cnn_execute_pipeline(thiz->p_thread_pool, thiz->p_cnn_param_compress, input, output);
            return 0;
        }
        if (thiz->p_cnn_param_decompress) {
            thiz->algo_ctx->cnn_execute_pipeline(thiz->p_thread_pool, thiz->p_cnn_param_decompress, input, output);
            return 0;
        }
    }
    printf("error: bad parameters\n");
    return (-1);
}

int cnn_compare_forward(void *context1, void *context2, TFloat *input, compare_config_t *comp_config)
{
    int ret = 0;
    cnn_generic_context_t *thiz1 = (cnn_generic_context_t *) context1;
    cnn_generic_context_t *thiz2 = (cnn_generic_context_t *) context2;
    if (thiz1 && thiz2 && input && comp_config) {
        if (thiz1->p_cnn_pipeline) {
            if (thiz2->p_cnn_pipeline) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_pipeline, thiz2->p_cnn_pipeline, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_compress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_pipeline, thiz2->p_cnn_param_compress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_decompress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_pipeline, thiz2->p_cnn_param_decompress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            }
        }
        if (thiz1->p_cnn_param_compress) {
            if (thiz2->p_cnn_pipeline) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_compress, thiz2->p_cnn_pipeline, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_compress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_compress, thiz2->p_cnn_param_compress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_decompress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_compress, thiz2->p_cnn_param_decompress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            }
        }
        if (thiz1->p_cnn_param_decompress) {
            if (thiz2->p_cnn_pipeline) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_decompress, thiz2->p_cnn_pipeline, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_compress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_decompress, thiz2->p_cnn_param_compress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            } else if (thiz2->p_cnn_param_decompress) {
                ret = thiz1->algo_ctx->cnn_compare(thiz1->p_thread_pool, thiz2->p_thread_pool, thiz1->p_cnn_param_decompress, thiz2->p_cnn_param_decompress, input, &thiz1->cnn_config, comp_config);
                if (ret) {
                    printf("error: compare cnn failed.\n");
                    return (-1);
                }
                return 0;
            }
        }
    }

    printf("error: bad parameters\n");
    return (-1);
}

int cnn_generic_query_data_source(void *context,
    TInteger *input_channels, TInteger *input_width, TInteger *input_height)
{
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) context;

    if ((!context) || (!input_channels) || (!input_width) || (!input_height)) {
        printf("error: null parameters\n");
        return (-1);
    }

    if (thiz) {
        cnn_layer_info_t layer_info;
        //thiz->algo_ctx.cnn_pipeline_query_layer_info(thiz->p_cnn_pipeline, 0, &layer_info);
        if (thiz->p_cnn_pipeline) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_pipeline, 0, &layer_info);
            *input_channels = layer_info.input_channel;
            *input_width = layer_info.input_width;
            *input_height = layer_info.input_height;
            return 0;
        }
        if (thiz->p_cnn_param_compress) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_param_compress, 0, &layer_info);
            *input_channels = layer_info.input_channel;
            *input_width = layer_info.input_width;
            *input_height = layer_info.input_height;
            return 0;
        }
        if (thiz->p_cnn_param_decompress) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_param_decompress, 0, &layer_info);
            *input_channels = layer_info.input_channel;
            *input_width = layer_info.input_width;
            *input_height = layer_info.input_height;
            return 0;
        }

    }
    printf("error: bad parameters\n");
    return (-1);
}

int cnn_generic_query_outdata_source(void *context,
    TInteger *output_channels, TInteger *output_width, TInteger *output_height)
{
    int num = 0;
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) context;

    if ((!context) || (!output_channels) || (!output_width) || (!output_height)) {
        printf("error: null parameters\n");
        return (-1);
    }

    if (thiz) {
        cnn_layer_info_t layer_info;
        num = thiz->cnn_config.number_of_layer -1;
        if (thiz->p_cnn_pipeline) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_pipeline, num, &layer_info);
            *output_channels = layer_info.output_channel;
            *output_width = layer_info.output_width;
            *output_height = layer_info.output_height;
            return 0;
        }
        if (thiz->p_cnn_param_compress) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_param_compress, num, &layer_info);
            *output_channels = layer_info.output_channel;
            *output_width = layer_info.output_width;
            *output_height = layer_info.output_height;
            return 0;
        }
        if (thiz->p_cnn_param_decompress) {
            thiz->algo_ctx->cnn_pipeline_query_layer_info(thiz->p_cnn_param_decompress, num, &layer_info);
            *output_channels = layer_info.output_channel;
            *output_width = layer_info.output_width;
            *output_height = layer_info.output_height;
            return 0;
        }

    }
    printf("error: bad parameters\n");
    return (-1);
}

int cnn_generic_query_cnn_statistics(void *context, cnn_statistics_t *statistics)
{
    cnn_generic_context_t *thiz = (cnn_generic_context_t *) context;

    if ((!context) || (!statistics)) {
        printf("error: null parameters\n");
        return (-1);
    }

    if (thiz->p_cnn_pipeline) {
        thiz->algo_ctx->cnn_pipeline_query_cnn_statistics(thiz->p_cnn_pipeline, statistics);
    }
    if (thiz->p_cnn_param_compress) {
        thiz->algo_ctx->cnn_pipeline_query_cnn_statistics(thiz->p_cnn_param_compress, statistics);
    }
    if (thiz->p_cnn_param_decompress) {
        thiz->algo_ctx->cnn_pipeline_query_cnn_statistics(thiz->p_cnn_param_decompress, statistics);
    }
    return 0;
}

