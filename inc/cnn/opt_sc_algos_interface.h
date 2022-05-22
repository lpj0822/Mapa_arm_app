/*******************************************************************************
 * opt_sc_algos_interface.h
 *
 * History:
 *  2017/07/28 - [Zhi He] create file
 *
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

#ifndef __OPT_SC_ALGOS_INTERFACE_H__
#define __OPT_SC_ALGOS_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT_API __attribute__((visibility("default")))

#define DENABLE_DEPRECATED
#ifdef DENABLE_DEPRECATED
#define attribute_deprecated __attribute__((deprecated))
#else
#define attribute_deprecated
#endif

typedef int TInteger;
typedef float TFloat;
typedef int TSI32;
typedef short TSI16;
typedef signed char TSI8;
typedef unsigned int TUI32;
typedef unsigned short TUI16;
typedef unsigned char TUI8;
typedef unsigned short TFbinary16;


//CNN limitation
#define D_MAX_CONCAT_INPUT_NUM 8
#define D_MAX_PRIOR_BOX_MINMAX_SIZE_NUM 8
#define D_MAX_PRIOR_BOX_VARIANCE_NUM 8


#define D_CF_IN_WIDTH 96
#define D_CF_IN_HEIGHT 112
#define D_CF_EIGENVALUES_NUM 512

/*! @function func_fr_create_context
 *  @brief create face recognition context
 *  @sa func_fr_destroy_context
 *  @param thread_number specify active thread number for this algo.
 *  @return face recognition context
 */
typedef void * (*func_fr_create_context)(const char *param_file, TInteger thread_number);

/*! @function func_fr_destroy_context
 *  @brief destroy face recognition context
 *  @sa func_fr_create_context
 */
typedef void * (*func_fr_destroy_context)(void *context);

/*! @function func_fr_forward
 *  @brief do face recognition forward, get the face's eigenvalues
 *  @param context face recognition context
 *  @param input input image data: 3 * 112 * 96 * float
 *  @param output output: 512 * float
 */
typedef int (*func_fr_forward)(void *context, TFloat *input, TFloat *output);

/*! @function func_fr_distance_quadratic
 *  @brief calculate face distance's quadratic, use quadratic of cosine distance.
 *     if user want face distance (cosine), they can get sqrt(this result).
 *  @param data_1 data 1: 512 * float
 *  @param data_2 data 2: 512 * float
 */
typedef TFloat (*func_fr_distance_quadratic)(TFloat *data_1, TFloat *data_2);

/*! @function func_fr_male_probability
 *  @brief calculate male propability, if >= 0.65 treat it as male, if <= 0.35 treat it as female
 *  @param eigenvalues: 512 * float, face's eigenvalues
 *  @return: probability of male
 */
typedef TFloat (*func_fr_male_probability)(TFloat* eigenvalues);

/*! @struct face_rect_t
 *  @brief include face's profile, face's landmark points, face's score
 *  @member y: face profile's vertical position (top left point)
 *  @member x: face profile's horizontal position (top left point)
 *  @member h: face profile's height
 *  @member w: face profile's width
 *  @member ldmk[]: face's five landmark point: left eye, right eye, nose, mouth's left point, mouth's right point
 *      for ldmk[][], y is first, x is second one, for example, ldmk[0][0] is left eye's y, ldmk[0][1] is left eye's x
 *  @member score: face's score
 */
typedef struct {
    TInteger y;
    TInteger x;
    TInteger h;
    TInteger w;
    TInteger ldmk[5][2];
    TFloat score;
} face_rect_t;

/*! @struct roi_t
 *  @brief include roi's profile
 *  @member y: roi profile's vertical position (top left point)
 *  @member x: roi profile's horizontal position (top left point)
 *  @member h: roi profile's height
 *  @member w: roi profile's width
 */
typedef struct {
    TInteger y;
    TInteger x;
    TInteger h;
    TInteger w;
} roi_t;

/*! @function func_fd_create_context
 *  @brief create face detection context
 *  @sa func_fd_destroy_context
 *  @param thread_number specify active thread number for this algo.
 *  @param input input image data, planed
 *  @param channel input image data's channel number
 *  @param width image width
 *  @param height image height
 *  @param mnsz mnsz
 *  @param scale_factor_num scale factor numerator
 *  @param scale_factor_den scale factor denominator
 *  @return face detection context
 */
typedef void * (*func_fd_create_context)(TInteger thread_number,
    TInteger width, TInteger height, TInteger channel, TInteger mnsz,
    TInteger scale_factor_num, TInteger scale_factor_den);

/*! @function func_fd_destroy_context
 *  @brief destroy face detection context
 *  @sa func_fd_create_context
 */
typedef void (*func_fd_destroy_context)(void *context);

/*! @function func_fd_detect
 *  @brief detect face
 *  @param context context
 *  @param input input image data, planed
 *  @param threshold_1 stage 1's threshold
 *  @param threshold_2 stage 2's threshold
 *  @param threshold_3 stage 3's threshold
 *  @param p_rects output rects
 *  @param max_face_number max face number
 *  @return number of face(rect) detected
 */
typedef int (*func_fd_detect)(void *context, TFloat *p_input,
    TFloat threshold_1, TFloat threshold_2, TFloat threshold_3,
    face_rect_t *p_rects, TInteger max_face_number);

/*! @function func_color_conversion_nv12_rgb_3channel_float
 *  @brief color conversion, nv12 --> rgb 3 channels, float
 *  @param context face detection context
 *  @param width image's width, should be multiple of 16
 *  @param height image's height, should be multiple of 2
 *  @param stride image's stride
 *  @param p_y plan y's address
 *  @param p_uv plan uv's address
 *  @return p_output output buffer, minmum size should be width * height * 3 * sizeof(float)
 */
typedef void (*func_color_conversion_nv12_rgb_3channel_float)(void *context,
    TInteger width, TInteger height, TInteger stride,
    unsigned char *p_y, unsigned char *p_uv, TFloat* p_output);

/*! @function func_color_conversion_nv12_rgb_3channel_float_mt
 *  @brief color conversion, nv12 --> rgb 3 channels, float, multi thread version
 *  @param thread_pool thread pool
 *  @param thread_number thread number
 *  @param width image's width, should be multiple of 16
 *  @param height image's height, should be multiple of 2
 *  @param stride image's stride
 *  @param p_y plan y's address
 *  @param p_uv plan uv's address
 *  @return p_output output buffer, minmum size should be width * height * 3 * sizeof(float)
 */
typedef void (*func_color_conversion_nv12_rgb_3channel_float_mt)(void *thread_pool,
    TInteger thread_number,
    TInteger width, TInteger height, TInteger stride,
    unsigned char *p_y, unsigned char *p_uv, TFloat* p_output);

/*! @function color_conversion_nv12_rgb_3channel_float_crop
 *  @brief color conversion, nv12 --> rgb 3 channels, float
 *  @param thread_pool thread pool
 *  @param thread_number thread number
 *  @param width image's width, should be multiple of 16
 *  @param height image's height, should be multiple of 2
 *  @param stride image's stride
 *  @param p_y plan y's address
 *  @param p_uv plan uv's address
 *  @param roi roi's struct object
 *  @return p_output output buffer, minmum size should be width * height * 3 * sizeof(float)
 */
typedef void (*func_color_conversion_nv12_rgb_3channel_float_crop)(void *thread_pool,
    TInteger thread_number,
    TInteger width, TInteger height, TInteger stride,
    unsigned char *p_y, unsigned char *p_uv, TFloat* p_output,roi_t *roi);


/*! @function func_g_br_u8_to_rgb_3channel_float
 *  @brief g br two planes, u8 --> rgb 3 channels, float
 *  @param context context
 *  @param width image's width
 *  @param height image's height
 *  @param stride image's stride
 *  @param p_g_o plan g's address
 *  @param p_br_o plan br's address
 *  @return p_output output buffer, minmum size should be width * height * 3 * sizeof(float)
 */
typedef void (*func_g_br_u8_to_rgb_3channel_float)(void *context,
    TInteger width, TInteger height, TInteger stride,
    unsigned char *p_g_o, unsigned char *p_br_o, TFloat* p_output);

/*! @struct img_tool_similarity_transform_t
 *  @brief similarity transform context: the transform formular is
 *  x' = sc * x + ss * y + tx
 *  y' = sc * y - ss * x + ty
 *  @param tranform transform's context
 *  @param cur_dim current landmarks
 *  @param fixed_dim current landmarks
 */
typedef struct {
    TFloat sc, ss, tx, ty;
} img_tool_similarity_transform_t;

/*! @function func_img_tool_setup_similarity_transform_5
 *  @brief setup similarity transform, input are fixed landmark and current landmark
 *            each have five pairs of [x, y]
 *  @param tranform transform's context
 *  @param cur_dim current landmarks
 *  @param fixed_dim current landmarks
 */
typedef void (*func_img_tool_setup_similarity_transform_5)(img_tool_similarity_transform_t *tranform,
    TFloat *cur_dim, TFloat *fixed_dim);

/*! @function func_img_tool_similarity_transform
 *  @brief do similarity transform
 *  @param tranform transform's context
 *  @param input input buffer
 *  @param output output buffer
 *  @param input_width input width
 *  @param input_height input height
 *  @param output_width output width
 *  @param output_height output height
 */
typedef void (*func_img_tool_similarity_transform)(img_tool_similarity_transform_t *tranform,
    TFloat *input, TFloat *output,
    TInteger input_width, TInteger input_height,
    TInteger output_width, TInteger output_height);


/*! @struct cnn_layer_param_strings_t
 *  @brief include cnn layer's text based parameters
 *  @member name: layer's name
 *  @member layer_type_string: layer's type string
 *  @member source_result_string: indicate the layer is source layer or result or intermediate
 *  @member input_string: input layer's name
 *  @member eltwise_input_string: eltwise input layer's name
 *  @member convolution_input_channel_string: convolution input channel number
 *  @member input_channel_string: input channel number
 *  @member output_channel_string: output channel number
 *  @member convolution_kernel_size_string: convolution kernel size
 *  @member convolution_stride_string: convolution stride
 *  @member convolution_padding_string: convolution padding
 *  @member convolution_weight_string: convolution weight parameters file
 *  @member convolution_bias_string: convolution bias parameters file
 *  @member prelu_slop_string: prelu slop parameters file
 *  @member pooling_type_string: pooling type
 *  @member pooling_global_string: pooling global
 *  @member pooling_kernel_size_string: pooling kernel size
 *  @member pooling_stride_string: pooling stride
 *  @member pooling_padding_string: pooling padding
 *  @member inner_product_weight_string: inner product weight
 *  @member inner_product_bias_string: inner product bias
 *  @member scale_factor_string: scale factor
 *  @member scale_bias_string: scale bias
 *  @member batch_normalization_mean_string: mean for batch normalization
 *  @member batch_normalization_variance_string: variance for batch normalization
 *  @member batch_normalization_scale_string: scale for batch normalization
 *  @member norm_scale_string: scale for normalization
 *  @member norm_across_spatial_string: normalize's across spatial
 *  @member norm_channel_shared_string: normalize's channel shared
 *  @member concat_input_string: concat layer's inputs, max total input number is D_MAX_CONCAT_INPUT_NUM
 *  @member concat_axis_string: concat axis
 *  @member shape_dim_string: reshape layer's dimentions
 *  @member flatten_axis_string: flatten layer's axis
 *  @member permute_order_string: permute orders
 *  @member prior_box_min_size_string: prior box's min size, max number is D_MAX_PRIOR_BOX_MINMAX_SIZE_NUM
 *  @member prior_box_max_size_string: prior box's max size, max number is D_MAX_PRIOR_BOX_MINMAX_SIZE_NUM
 *  @member prior_box_flip_string: prior box flip
 *  @member prior_box_clip_string: prior box clip
 *  @member prior_box_aspect_ratio_string: prior box aspect ratio
 *  @member prior_box_variance_string: prior box variance
 *  @member prior_box_step_string: prior box step
 *  @member prior_box_offset_string: prior box offset
 *  @member deout_location_input_string: detection output's location input
 *  @member deout_confidence_input_string: detection output's confidence input
 *  @member deout_prior_box_input_string: detection output's prior box input
 *  @member deout_num_classes_string: detection output's number of classes
 *  @member deout_background_label_id_string: detection output's background lable id
 *  @member deout_share_location_string: detection output's share location
 *  @member deout_nms_threshold_string: detection output's nms threhold
 *  @member deout_nms_top_k_string: detection output's nms top k
 *  @member deout_keep_top_k_string: detection output's keep top k
 *  @member deout_confidence_threshold_string: detection output's confidence threhold
 */

typedef struct cnn_layer_param_strings_s {
    const char *name;

    const char *layer_type_string;
    const char *source_result_string;

    const char *input_string;
    const char *eltwise_input_string;

    const char *convolution_input_channel_string;

    const char *input_channel_string;
    const char *output_channel_string;

    //convolution parameters
    const char *convolution_kernel_size_string;
    const char *convolution_stride_string;
    const char *convolution_padding_string;
    const char *convolution_bias_term_string;
    const char *convolution_weight_string;
    const char *convolution_bias_string;

    //prelu parameters
    const char *prelu_slop_string;

    //pooling parameters
    const char *pooling_type_string;
    const char *pooling_global_string;
    const char *pooling_kernel_size_string;
    const char *pooling_stride_string;
    const char *pooling_padding_string;

    //inner product parameters
    const char *inner_product_weight_string;
    const char *inner_product_bias_string;

    //scale parameters
    const char *scale_factor_string;
    const char *scale_bias_string;

    //batch normalization parameters
    const char *batch_normalization_mean_string;
    const char *batch_normalization_variance_string;
    const char *batch_normalization_scale_string;

    //normalization parameters
    const char *norm_scale_string;
    const char *norm_across_spatial_string;
    const char *norm_channel_shared_string;

    //concat parameters
    const char *concat_input_string[D_MAX_CONCAT_INPUT_NUM];
    const char *concat_axis_string;

    //reshape parameters
    const char *shape_dim_string;

    //flatten parameters
    const char *flatten_axis_string;

    //permute parameters
    const char *permute_order_string[4];

    //priorbox parameters
    const char *prior_box_min_size_string[D_MAX_PRIOR_BOX_MINMAX_SIZE_NUM];
    const char *prior_box_max_size_string[D_MAX_PRIOR_BOX_MINMAX_SIZE_NUM];
    const char *prior_box_flip_string;
    const char *prior_box_clip_string;
    const char *prior_box_aspect_ratio_string;
    const char *prior_box_variance_string[D_MAX_PRIOR_BOX_VARIANCE_NUM];
    const char *prior_box_step_string;
    const char *prior_box_offset_string;

    //detection output parameters
    const char *deout_location_input_string;
    const char *deout_confidence_input_string;
    const char *deout_prior_box_input_string;
    const char *deout_num_classes_string;
    const char *deout_background_label_id_string;
    const char *deout_share_location_string;
    const char *deout_nms_threshold_string;
    const char *deout_nms_top_k_string;
    const char *deout_keep_top_k_string;
    const char *deout_confidence_threshold_string;

    struct cnn_layer_param_strings_s *p_next;
} cnn_layer_param_strings_t;

/*! @struct cnn_preallocated_buffer_f32_t
 *  @brief include cnn's preallocate buffer, it's ping-pong for different layers
 *  @member max_intermediate_a_buffer_size: intermediate a's max size
 *  @member max_intermediate_b_buffer_size: intermediate b's max size
 *  @member max_intermediate_staged_buffer_size: intermediate staged buffer max size
 *  @member max_result_buffer_size: result buffer max size
 *  @member p_intermediate_a_buffer: intermediate a's address
 *  @member p_intermediate_b_buffer: intermediate b's address
 *  @member p_intermediate_staged_buffer: intermediate staged buffer's address
 *  @member p_result_buffer: result buffer's address
 */

typedef struct {
    TInteger max_intermediate_a_buffer_size;
    TInteger max_intermediate_b_buffer_size;
    TInteger max_intermediate_staged_buffer_size;
    TInteger max_result_buffer_size;
    TFloat *p_intermediate_a_buffer;
    TFloat *p_intermediate_b_buffer;
    TFloat *p_intermediate_staged_buffer;
    TFloat *p_result_buffer;
} cnn_preallocated_buffer_f32_t;

/*! @struct cnn_statistics_t
 *  @brief cnn's statistics
 *  @member pipeline_used_memory: pipeline used memory
 *  @member params_used_memory: parameters used memory
 *  @member total_actual_memory: intermediate staged buffer max size
 *  @member total_mac: mac(multiply accumulate) of cnn
 */

typedef struct {
    unsigned int pipeline_used_memory;
    unsigned int params_used_memory;
    unsigned int total_actual_memory;

    unsigned long total_mac;
} cnn_statistics_t;

/*! @struct cnn_layer_info_t
 *  @brief layer's information
 *  @member layer_type: layer type
 *  @member p_convolution_weight: convolution weight
 *  @member p_convolution_bias: convolution bias
 *  @member p_prelu_slop: prelu slop
 *  @member p_inner_product_weight: inner product weight
 *  @member p_inner_product_bias: inner product bias
 *  @member p_scale_factor: scale's factor
 *  @member p_scale_bias: scale's bias
 *  @member input_buf: layer's input buffer address
 *  @member lt_input_buf: layer's eltwise input buffer address
 *  @member output_buf: layer's output buffer address
 *  @member input_channel: input channel number
 *  @member input_width: input width
 *  @member input_height: input height
 *  @member output_channel: output channel number
 *  @member output_width: output width
 *  @member output_height: output height
 *  @member convolution_kernel_size: convolution kernel size
 *  @member convolution_stride: convolution stride
 *  @member convolution_padding: convolution padding
 *  @member pooling_type: pooling type
 *  @member pooling_kernel_size: pooling kernel size
 *  @member pooling_stride: pooling stride
 *  @member pooling_padding: pooling padding
 */

typedef struct {
    TInteger layer_type;

    TFloat *p_convolution_weight;
    TFloat *p_convolution_bias;
    TFloat *p_prelu_slop;

    TFloat *p_inner_product_weight;
    TFloat *p_inner_product_bias;

    TFloat *p_scale_factor;
    TFloat *p_scale_bias;

    TFloat *input_buf;
    TFloat *lt_input_buf;
    TFloat *output_buf;

    TInteger input_channel;
    TInteger input_width;
    TInteger input_height;

    TInteger output_channel;
    TInteger output_width;
    TInteger output_height;

    TInteger convolution_kernel_size;
    TInteger convolution_stride;
    TInteger convolution_padding;

    TInteger pooling_type;
    TInteger pooling_kernel_size;
    TInteger pooling_stride;
    TInteger pooling_padding;
} cnn_layer_info_t;

/*! @struct cnn_param_compress_strings_s
 *  @brief include cnn compress's text based parameters
 *  @member param_type_string: input format of parameter files in cnn
 *  @member quantization_level_string: quantization level for conversion operation in float <--> s8/s16.
 *  @member exponent_bit_string: exponent bits in f16 format (Sign bit: 1 bit + Exponent: x bits + mantissa: 15-x bits)
 *  @member mantissa_bit_string: mantissa bits in f16 format
 */

typedef struct cnn_param_compress_strings_s {
    const char *param_type_string;

    const char *quantization_level_s8_string;
    const char *quantization_level_s16_string;

    const char *exponent_bit_string;
    const char *mantissa_bit_string;
} cnn_param_compress_strings_t;

/*! @struct cnn_compress_s
 *  @brief include cnn compress's text based parameters
 *  @member param_type: input format of parameter files in cnn
 *  @member compress_type: parameter's cmopress mode
 *  @member quantization_level: quantization level for conversion operation in float <--> s8/s16.
 *  @member exponent_bit_string: exponent bits in f16 format (Sign bit: 1 bit + Exponent: x bits + mantissa: 15-x bits)
 *  @member mantissa_bit_string: mantissa bits in f16 format
 */

typedef struct cnn_compress_s {
    TInteger param_type;
    TInteger compress_type;

    TInteger quantization_level_s8;
    TInteger quantization_level_s16;

    TInteger exponent_bit;
    TInteger mantissa_bit;
} cnn_compress_t;

/*! @enum param_type
 *  @brief include input format of parameter files in cnn
 *  @member ECNNParamType_F32: input format for float, default
 *  @member ECNNParamType_F16: input format for half-precision float(16bit)
 *  @member ECNNParamType_S8:  input format for signed integers(8bit)
 *  @member ECNNParamType_S16: input format for signed integers(16bit)
 */
enum param_type {
    ECNNParamType_F32 = 0x00,
    ECNNParamType_F16 = 0x01,
    ECNNParamType_S8 = 0x02,
    ECNNParamType_S16 = 0x03,
};

/*! @enum compress_type
 *  @brief include parameter's compress mode supplied in cnn
 *  @member ECNNCompressType_F32: compress mode for float->float, no compress
 *  @member ECNNCompressType_F16: compress mode for float->half-precision float(16bit)
 *  @member ECNNCompressType_S8:  compress mode for float->signed integers(8bit)
 *  @member ECNNCompressType_S16: compress mode for float->signed integers(16bit)
 */
enum compress_type {
    ECNNCompressType_F32 = 0x00,
    ECNNCompressType_F16 = 0x01,
    ECNNCompressType_S8 = 0x02,
    ECNNCompressType_S16 = 0x03,
};

/*! @struct cnn_config_t
 *  @brief cnn's description structure, from config file
 *  @member p_conf_file_content: string buffer for reading config file
 *  @member cnn_layer_param_strings_t: layer config list's header
 *  @member path: config file's path (float)
 *  @member net_name: cnn's net name
 *  @member new_path: new compress param files's path
 *  @member compress_type_string: parameter's cmopress mode
 *  @member p_compress_file_content: string buffer for reading compress file
 *  @member p_compress_header: compress string list's header
 *  @member p_compress: cnn's compress description structure, from compress file
 */

typedef struct {
    char *p_conf_file_content;
    cnn_layer_param_strings_t *p_layer_header;
    int number_of_layer;
    const char *path;
    const char *net_name;

    //compress related parameters
    const char *new_path;
    const char *compress_type_string;
    char *p_compress_file_content;
    cnn_param_compress_strings_t *p_compress_header;
    cnn_compress_t p_compress;
} cnn_config_t;

/*! @function read_cnn
 *  @brief read cnn compress file and config file, will allocate related resources
 *  @sa clean_cnn_config
 *  @param path compress file, config file and param file's path
 *  @param net_name cnn's net name
 *  @param config config's pointer
 *  @return 0 means success, otherwise means fail
 */
typedef int (*func_read_cnn)(const char *path, const char *net_name, cnn_config_t *config);

/*! @function clean_cnn
 *  @brief clean cnn config, will release related resouces
 *  @sa read_cnn
 *  @param config config's pointer
 */
typedef void (*func_clean_cnn)(cnn_config_t *config);

/*! @function func_read_cnn_config_file
 *  @brief read cnn config file, will allocate related resources, in config
 *  @sa func_clean_cnn_config
 *  @param path config file and param file's path
 *  @param config_file config file name
 *  @param config config's pointer
 *  @return 0 means success, otherwise means fail
 */
typedef int (*func_read_cnn_config_file)(const char *path, const char *config_file, cnn_config_t *config);

/*! @function func_clean_cnn_config
 *  @brief clean cnn config, will release related resouces
 *  @sa func_read_cnn_config_file
 *  @param config config's pointer
 */
typedef void (*func_clean_cnn_config)(cnn_config_t *config);

/*! @function cnn_compress_param_file
*   @brief compress parameter files in cnn(float->float(no compress)/f16/s8/s16),
*       and setup a cnn pipeline, refer to cnn compress file and config file.
*   @sa cnn_destroy_pipeline
*   @param width input width
*   @param height input height
*   @param preallocate_buffer pre allocate buffer, maybe shared between different cnn pipeline
*   @param thread_number_hint thread_number_hint for parallel
*   @param fill_random_params fill random parameters for cnn, this is debug option, which used to fast-evaluate cnn's
*       statistics, like needed memory, mac, etc. Without prepare "parameter files".
*   @sa cnn_pipeline_query_cnn_statistics
*   @return cnn pipeline
*/
typedef void *(*func_cnn_compress_param_file)(cnn_config_t *config,
    TInteger width, TInteger height,
    cnn_preallocated_buffer_f32_t *preallocate_buffer,
    TInteger thread_number_hint, TInteger fill_random_params);

/*! @function cnn_decompress_and_setup_pipeline
*   @brief decompress parameter files in cnn(float(no decompress)/f16/s8/s16->float),
*       and setup a cnn pipeline(when input format was float, correspond to API "cnn_setup_pipeline"),
*       refer to cnn compress file and config file.
*   @sa cnn_destroy_pipeline
*   @param width input width
*   @param height input height
*   @param preallocate_buffer pre allocate buffer, maybe shared between different cnn pipeline
*   @param thread_number_hint thread_number_hint for parallel
*   @param fill_random_params fill random parameters for cnn, this is debug option, which used to fast-evaluate cnn's
*       statistics, like needed memory, mac, etc. Without prepare "parameter files".
*   @sa cnn_pipeline_query_cnn_statistics
*   @return cnn pipeline
*/
typedef void *(*func_cnn_decompress_and_setup_pipeline)(cnn_config_t *config,
    TInteger width, TInteger height,
    cnn_preallocated_buffer_f32_t *preallocate_buffer,
    TInteger thread_number_hint, TInteger fill_random_params);

/*! @function func_cnn_execute_pipeline
 *  @brief execute the cnn pipeline, do a forward
 *  @param thread_pool thread pool
 *  @param p_pipeline cnn pipeline
 *  @param p_input input data
 *  @param p_output output buffer, last layer's output buffer
 */
typedef void (*func_cnn_execute_pipeline)(void *thread_pool, void *p_pipeline, TFloat *p_input, TFloat *p_output);

/*! @struct compare_config_t
 *  @brief control printing cnn's compare results for output and parameters
 *  @member b_print_diff: print the different compare results(beyond the threshold) for output and parameters
 *  @member b_print_all: print all the compare results for output and parameters
 *  @member b_dump_binary: store the results of each layer in binary files
 *  @member reserved: reserved
 *  @member diff_ratio: set the print threshold, error precision
 */
typedef struct {
    unsigned char b_print_diff;
    unsigned char b_print_all;
    unsigned char b_dump_binary;
    unsigned char reserved;
    float diff_ratio;
} compare_config_t;

/*! @function cnn_compare
 *  @brief compare cnn's output and parameters, print the diff range and mean absolute difference, do a forward
 *  @param thread_pool thread pool
 *  @param p_pipeline cnn pipeline
 *  @param p_input input data
 *  @param config config's pointer
 *  @param comp_config compare config's pointer
 */
typedef int (*func_cnn_compare)(void *thread_pool1, void *thread_pool2, void *p_pipeline1, void *p_pipeline2, TFloat *p_input, cnn_config_t *config, compare_config_t *comp_config);

/*! @function func_cnn_setup_pipeline
 *  @brief setup a cnn pipeline, from a cnn config
 *  @sa func_cnn_destroy_pipeline
 *  @param width input width
 *  @param height input height
 *  @param preallocate_buffer pre allocate buffer, maybe shared between different cnn pipeline
 *  @param thread_number_hint thread_number_hint for parallel
 *  @param fill_random_params fill random parameters for cnn, this is debug option, which used to fast-evaluate cnn's
 *       statistics, like needed memory, mac, etc. Without prepare "parameter files".
 *  @sa cnn_pipeline_query_cnn_statistics
 *  @return cnn pipeline
 */
typedef void *(*func_cnn_setup_pipeline)(cnn_config_t *config,
    TInteger width, TInteger height,
    cnn_preallocated_buffer_f32_t *preallocate_buffer,
    TInteger thread_number_hint, TInteger fill_random_params);

/*! @function func_cnn_duplicate_pipeline
 *  @brief duplicate a cnn pipeline, from a master cnn pipeline, they will share parameters,
 *     typically it's used for a pyramid set of cnn pipelines.
 *  @sa func_cnn_destroy_pipeline
 *  @param width input width
 *  @param height input height
 *  @return duplicated cnn pipeline
 */
typedef void *(*func_cnn_duplicate_pipeline)(void *p_master_pipeline,
    TInteger width, TInteger height);

/*! @function func_cnn_destroy_pipeline
 *  @brief destroy a cnn pipeline
 *  @sa func_cnn_setup_pipeline
 *  @param p_pipeline cnn pipeline
 */
typedef void (*func_cnn_destroy_pipeline)(void *p_pipeline);

/*! @function func_cnn_pipeline_query_number_of_layers
 *  @brief query number of layers
 *  @param p_pipeline cnn pipeline
 *  @return number of layers
 */
typedef int (*func_cnn_pipeline_query_number_of_layers)(void *p_pipeline);

/*! @function func_cnn_pipeline_query_layer_info
 *  @brief query layer in a cnn pipeline
 *  @param p_pipeline cnn pipeline
 *  @param layer_index layer index
 *  @param info layer info
 */
typedef int (*func_cnn_pipeline_query_layer_info)(void *p_pipeline, TInteger layer_index, cnn_layer_info_t *info);

/*! @function func_cnn_pipeline_query_preallocated_buffer
 *  @brief query preallocated buffer of cnn pipeline
 *  @param p_pipeline cnn pipeline
 *  @return pre-allocated buffer structure
 */
typedef cnn_preallocated_buffer_f32_t *(*func_cnn_pipeline_query_preallocated_buffer)(void *p_pipeline);

/*! @function func_cnn_pipeline_query_cnn_statistics
 *  @brief query cnn's statistics, like needed memory, total mac (multiply accumulate) per forward
 *  @param p_pipeline cnn pipeline
 *  @param struct for statistics
 */
typedef void (*func_cnn_pipeline_query_cnn_statistics)(void *p_pipeline, cnn_statistics_t *statistics);

//simple thread pool
typedef int (*TFTaskFunction) (void *task_context, void *param);

/*! @struct simple_task_t
 *  @brief task for thread pool, work thread in thread pool will execute function(context, param), for this task
 *  @member function: task's function
 *  @member context: task's context
 *  @member param: task's parameters
 *  @member ret: task's function's return value
 *  @member task_id: internal debug use, ignore it
 */

typedef struct {
    TFTaskFunction function;
    void *context;
    void *param;

    int ret;
    unsigned int task_id;
} simple_task_t;

/*! @function func_create_simple_thread_pool
 *  @brief create a thread pool, with number of work thread
 *  @sa func_destroy_simple_thread_pool
 *  @param thread_num work thread number
 *  @return thread pool
 */
typedef void *(*func_create_simple_thread_pool)(unsigned int thread_num);

/*! @function func_destroy_simple_thread_pool
 *  @brief destory a thread pool
 *  @sa func_create_simple_thread_pool
 *  @param thread_pool thread pool
 */
typedef void (*func_destroy_simple_thread_pool)(void *thread_pool);

/*! @function func_simple_thread_pool_add_task
 *  @brief add a task into thread pool, when multiple tasks are added into thread pool,
 *     thread pool does not guarantee those task's execution order
 *  @sa func_simple_thread_pool_wait_task_done
 *  @param thread_pool thread pool
 *  @param task task
 *  @return 0 means success, otherwise fail
 */
typedef int (*func_simple_thread_pool_add_task)(void *thread_pool, simple_task_t *task);

/*! @function func_simple_thread_pool_wait_task_done
 *  @brief wait task finished since when multiple tasks are added into thread pool,
 *     thread pool does not guarantee those task's execution order, so the typical usage is:
 *     add a number of task into thread pool, then invoke this API that number of times, to guarantee
 *     all of the task are finished.
 *  @sa func_simple_thread_pool_add_task
 *  @param thread_pool thread pool
 *  @return finished task
 */
typedef simple_task_t *(*func_simple_thread_pool_wait_task_done)(void *thread_pool);

/*! @function func_convolution_float_accumulate_add_offset
 *  @brief convolution, accumulate channels, add offset.
 *  @param input input buffer
 *  @param width input width
 *  @param height input height
 *  @param output output buffer
 *  @param param convolution weight
 *  @param offset convolution bias
 *  @return 0 means success, otherwise fail
 */
typedef int (*func_convolution_float_accumulate_add_offset)(TFloat *input, TInteger width,
    TInteger height, TFloat *output, TFloat *param, TFloat offset);

typedef struct {
    //face recognition
    func_fr_create_context fr_create_context;
    func_fr_destroy_context fr_destroy_context;
    func_fr_forward fr_forward;
    func_fr_distance_quadratic fr_distance_quadratic;
    func_fr_male_probability fr_male_probability;

    //face detection
    func_fd_create_context fd_create_context;
    func_fd_destroy_context fd_destroy_context;
    func_fd_detect fd_detect;

    //color convertion, for face detection only, need face detection's context
    func_color_conversion_nv12_rgb_3channel_float color_conversion_nv12_rgb_3channel_float;

    //image tool for similarity transform
    func_img_tool_setup_similarity_transform_5 img_tool_setup_similarity_transform_5;
    func_img_tool_similarity_transform img_tool_similarity_transform;

    void *lib;

    //color convertion, generic purpose version, specify thread pool to enable multi-thread
    func_color_conversion_nv12_rgb_3channel_float_mt color_conversion_nv12_rgb_3channel_float_mt;
    func_color_conversion_nv12_rgb_3channel_float_crop color_conversion_nv12_rgb_3channel_float_crop;

    //low level API for generic cnn: read config file
    func_read_cnn read_cnn;
    func_clean_cnn clean_cnn;
    func_read_cnn_config_file read_cnn_config_file attribute_deprecated;
    func_clean_cnn_config clean_cnn_config attribute_deprecated;

    //compress parameter files in cnn: float->f16/s8/s16
    func_cnn_compress_param_file cnn_compress_param_file;
    //decompress parameter files in cnn: f16/s8/s16->float
    func_cnn_decompress_and_setup_pipeline cnn_decompress_and_setup_pipeline;
    //compare cnn (forward)
    func_cnn_compare cnn_compare;

    //low level API for generic cnn: setup cnn and execute cnn (forward)
    func_cnn_execute_pipeline cnn_execute_pipeline;
    func_cnn_setup_pipeline cnn_setup_pipeline;
    func_cnn_duplicate_pipeline cnn_duplicate_pipeline;
    func_cnn_destroy_pipeline cnn_destroy_pipeline;

    //low level API for generic cnn: query layer and pre-allocated buffer
    func_cnn_pipeline_query_number_of_layers cnn_pipeline_query_number_of_layers;
    func_cnn_pipeline_query_layer_info cnn_pipeline_query_layer_info;
    func_cnn_pipeline_query_preallocated_buffer cnn_pipeline_query_preallocated_buffer;
    func_cnn_pipeline_query_cnn_statistics cnn_pipeline_query_cnn_statistics;

    //simple thread pool
    func_create_simple_thread_pool create_simple_thread_pool;
    func_destroy_simple_thread_pool destroy_simple_thread_pool;
    func_simple_thread_pool_add_task simple_thread_pool_add_task;
    func_simple_thread_pool_wait_task_done simple_thread_pool_wait_task_done;

} opt_sc_algos_context_t;

/*! @function initialize_opt_sc_algos_context
 *  @brief initialize library context
 *  @param context library's context
 */
EXPORT_API int initialize_opt_sc_algos_context(opt_sc_algos_context_t *context);

/*! @function destroy_opt_sc_algos_context
 *  @brief destroy library context
 *  @param context library's context
 */
EXPORT_API void destroy_opt_sc_algos_context(opt_sc_algos_context_t *context);

typedef struct {
    unsigned char ver_major;
    unsigned char ver_minor;
    unsigned char ver_patch;
    unsigned char ver_reserved;

    unsigned short date_year;
    unsigned char date_month;
    unsigned char date_day;
} opt_sc_algos_version_t;

/*! @function get_opt_sc_algos_version
 *  @brief get library version
 *  @param version library's version
 */
EXPORT_API void get_opt_sc_algos_version(opt_sc_algos_version_t *version);

#ifdef __cplusplus
}
#endif

#endif

