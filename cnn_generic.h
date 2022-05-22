/*******************************************************************************
 *  cnn_generic.h
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

#ifndef __CNN_GENERIC_H__
#define __CNN_GENERIC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*! @function cnn_generic_create_context
 *  @brief create cnn's context
 *  @sa cnn_generic_destroy_context
 *  @param algo_ctx opt_sc_algos_context_t struct point for wrapper library
 *  @param param_file_path specify cnn parameter file's path
 *  @param conf_file_name specify cnn pipeline's conf file name
 *  @param thread_number specify active thread number for this algo
 *  @param input_width input width
 *  @param input_width input height
 *  @param compress_file_name the name of compress file
 *  @param compress_type the compress mode for parameters in cnn
 *  @param exponent_bit the exponent bits for for 'f16' format parameters in cnn
 *  @param fill_random_params fill random parameters for cnn, this is debug option, which used to fast-evaluate cnn's
 *       statistics, like needed memory, mac, etc. Without prepare "parameter files".
 *  @return cnn context
 */
void *cnn_generic_create_context(void *algo_ctx, const char *param_file_path,
    const char *net_name,
    TInteger thread_number,
    TInteger input_width,
    TInteger input_height,
    const char *compress_type,
    const char *new_param_path,
    TInteger exponent_bit,
    TInteger fill_random_params);

/*! @function cnn_generic_destroy_context
 *  @brief destroy caffe face's context
 *  @sa cnn_generic_create_context
 *  @param cnn's context
 */
void cnn_generic_destroy_context(void *context);

/*! @function cnn_generic_forward
 *  @brief do cnn forward
 *  @param context context
 *  @param input input buffer
 *  @param output output buffer
 */
int cnn_generic_forward(void *context, TFloat *input, TFloat *output);

/*! @function cnn_compare_forward
 *  @brief do cnn forward compare
 *  @param context context
 *  @param input input buffer
 *  @param comp_config compare config's point
 */
int cnn_compare_forward(void *context1, void *context2, TFloat *input, compare_config_t *comp_config);

/*! @function cnn_generic_query_data_source
 *  @brief query data source
 *  @param context context
 *  @param input_channels input channels
 *  @param input_width input_width
 *  @param input_height input_height
 */
int cnn_generic_query_data_source(void *context,
    TInteger *input_channels, TInteger *input_width, TInteger *input_height);

/*! @function cnn_generic_query_outdata_source
 *  @brief query output data source in the last layer
 *  @param context context
 *  @param output_channels input output_channels
 *  @param output_width output_width
 *  @param output_height output_height
 */
int cnn_generic_query_outdata_source(void *context,
    TInteger *output_channels, TInteger *output_width, TInteger *output_height);

/*! @function cnn_generic_query_cnn_statistics
 *  @brief query cnn's statistics, like needed memory, total mac (multiply accumulate) per forward
 *  @param p_pipeline cnn pipeline
 *  @param struct for statistics
 */
int cnn_generic_query_cnn_statistics(void *context, cnn_statistics_t *statistics);

#ifdef __cplusplus
}
#endif

#endif

