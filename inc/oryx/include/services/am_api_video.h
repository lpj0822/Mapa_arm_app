/*
 * am_api_video.h
 *
 *  History:
 *    Nov 18, 2014 - [Shupeng Ren] created file
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

/*! @file am_api_video.h
 *  @brief This file defines Oryx Video Services related data structures
 */
#ifndef _AM_API_VIDEO_H_
#define _AM_API_VIDEO_H_

#include "commands/am_api_cmd_video.h"

// Setting item masks
/*! @defgroup airapi-datastructure-video Data Structure of Video Service
 *  @ingroup airapi-datastructure
 *  @brief All Oryx Video Service related method call data structures
 *  @{
 */

/*
 * Feature
 */
/*****************************Feature Parameters*******************************/
/*! @defgroup airapi-datastructure-video-feature Feature Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief feature related parameters,
 *         refer to @ref airapi-commandid-video-feature "Feature Commands" to
 *         see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_FEATURE_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_FEATURE_SET
 *  @{
 *//* Start of feature Parameter Section */

/*! @enum AM_FEATURE_CONFIG_BITS
 *  @brief feature configuration to indicate which config needs to be modified
 */
enum AM_FEATURE_CONFIG_BITS
{
  AM_FEATURE_CONFIG_MODE_EN_BIT        = 0,//!< Change feature mode
  AM_FEATURE_CONFIG_HDR_EN_BIT         = 1,//!< Change feature hdr
  AM_FEATURE_CONFIG_ISO_EN_BIT         = 2,//!< Change feature iso
  AM_FEATURE_CONFIG_DEWARP_EN_BIT      = 3,//!< Change feature dewarp
  AM_FEATURE_CONFIG_DPTZ_EN_BIT        = 4,//!< Change feature dptz
  AM_FEATURE_CONFIG_BITRATECTRL_EN_BIT = 5,//!< Change feature bitrate ctrl
  AM_FEATURE_CONFIG_OVERLAY_EN_BIT     = 6,//!< Change feature overlay
  AM_FEATURE_CONFIG_IAV_VERSION_EN_BIT = 7,//!< Change feature iav version
  AM_FEATURE_CONFIG_VIDEO_MD_EN_BIT    = 8,//!< Change feature video motion detect
  AM_FEATURE_CONFIG_HEVC_EN_BIT        = 9,//!< Change feature hevc
};


/*! @struct am_feature_config_s
 *  @brief For feature config file GET and SET
 */
/*! @typedef am_feature_config_t
 *  @brief For feature config file GET and SET
 */
typedef struct am_feature_config_s
{
    /*!
     * @sa AM_FEATURE_CONFIG_BITS
     */
    uint32_t enable_bits; //!< AM_FEATURE_CONFIG_BITS

    /*!
     * - iav_version
     */
    uint32_t version;

    /*!
     * - 0: auto
     * - 1: mode0
     * - 2: mode1
     * - 3: mode4
     * - 4: mode5
     */
    uint32_t mode;

    /*!
     * - 0: none
     * - 1: 2x
     * - 2: 3x
     * - 3: 4x
     * - 4: sensor
     */
    uint32_t hdr;

    /*!
     * - 0: normal
     * - 1: plus
     * - 2: advanced
     */
    uint32_t iso;

    /*!
     * - 0: none
     * - 1: ldc
     * - 2: full
     */
    uint32_t dewarp_func;

    /*!
     * - 0: none
     * - 1: enable
     */
    uint32_t dptz;

    /*!
     * - 0: none
     * - 1: lbr
     */
   uint32_t bitrate_ctrl;

    /*!
     * - 0: none
     * - 1: enable
     */
   uint32_t overlay;

   /*!
    * - 0: none
    * - 1: enable
    */
  uint32_t video_md;

   /*!
    * - -1: unsupported in platform
    * - 0: disable
    * - 1: enable
    */
  int32_t hevc;
} am_feature_config_t;

/*! @} */ /* End of Feature Parameters */
/******************************************************************************/

/*
 * VIN
 */
/******************************VIN Parameters**********************************/
/*! @defgroup airapi-datastructure-video-vin VIN Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief VIN related parameters,
 *         refer to @ref airapi-commandid-video-vin "VIN Commands" to see
 *         related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_VIN_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_VIN_SET
 *  @{
 *//* Start of VIN Parameter Section */

/*! @enum AM_VIN_CONFIG_BITS
 *  @brief VIN configuration type to indicate which config needs to be modified
 */
enum AM_VIN_CONFIG_BITS
{
  AM_VIN_CONFIG_WIDTH_HEIGHT_EN_BIT  = 0,//!< Change VIN Width and Height
  AM_VIN_CONFIG_FLIP_EN_BIT          = 1,//!< Change VIN Flip config
  AM_VIN_CONFIG_HDR_MODE_EN_BIT      = 2,//!< Change VIN HDR mode
  AM_VIN_CONFIG_FPS_EN_BIT           = 3,//!< Change VIN FPS config
  AM_VIN_CONFIG_BAYER_PATTERN_EN_BIT = 4,//!< Change VIN bayer pattern
};

/*! @macros VIN_MAX_CHAR_NUM
 *  @brief VIN max char number
 */
#define VIN_MAX_CHAR_NUM  (16)  //!< used to vin mode

/*! @struct am_vin_config_s
 *  @brief For VIN config file GET and SET
 */
/*! @typedef am_vin_config_t
 *  @brief For VIN config file GET and SET
 */
typedef struct am_vin_config_s
{
    /*!
     * @sa AM_VIN_CONFIG_BITS
     */
    uint32_t enable_bits; //!< AM_VIN_CONFIG_BITS

    uint32_t vin_id; //!< VIN ID

    uint32_t width; //!< VIN Width
    uint32_t height; //!< VIN Height

    uint16_t reserved0; //!< Reserved space to keep align

    /*!
     * - 0:     auto
     * - x:     x
     * - 1000:   29.97
     * - 1001:   59.94
     * - 1002:   23.976
     * - 1003:   12.5
     * - 1004:   6.25
     * - 1005:   3.125
     * - 1006:   7.5
     * - 1007:   3.75
     */
    uint16_t fps; //!< VIN FPS

    /*!
     * - 0:   not flip
     * - 1:   flipv
     * - 2:   fliph
     * - 3:   flip both V & h
     * - 255: auto
     */
    uint8_t flip; //!< VIN flip

    /*!
     * - 0: single exposure, no HDR
     * - 1: 2x hdr exposure
     * - 2: 3x hdr exposure
     * - 3: 4x hdr exposure
     */
    uint8_t hdr_mode; //!< VIN HDR Mode

    /*!
     * - 0: auto
     * - 1: RG
     * - 2: BH
     * - 3: GR
     * - 4: GB
     */
    uint8_t bayer_pattern; //!< VIN bayer pattern
    uint8_t reserved1; //!< Reserved space to keep align

    /*!
     * vin mode to use
     */
    char mode[VIN_MAX_CHAR_NUM];
} am_vin_config_t;

/*! @struct am_vin_info_s
 *  @brief For dynamically GET VIN info
 */
/*! @typedef am_vin_info_t
 *  @brief For dynamically GET VIN info
 */
typedef struct am_vin_info_s {
    uint32_t  width;  //!< VIN width
    uint32_t  height; //!< VIN height
    uint32_t  vin_id; //!< VIN ID

    /*!
     * - 0:     auto
     * - x:     x
     * - 1000:   29.97
     * - 1001:   59.94
     * - 1002:   23.976
     * - 1003:   12.5
     * - 1004:   6.25
     * - 1005:   3.125
     * - 1006:   7.5
     * - 1007:   3.75
     */
    uint16_t   max_fps; //!< VIN fps
    uint16_t   fps;     //!< The same as max_fps

    /*!
     * - 0: auto
     * - 1: Progressive
     * - 2: Interlace
     */
    uint8_t   format; //!< VIN format

    /*!
     * - 0: AUTO
     * - 1: YUV BT601
     * - 2: YUV BT656
     * - 3: YUV BT1120
     * - 4: RGB BT601
     * - 5: RGB BT656
     * - 6: RGB RAW
     * - 7: RGB BT1120
     */
    uint8_t   type;  //!< VIN type

    /*!
     * - 0: auto
     * - X: X bits
     */
    uint8_t   bits; //!< VIN bits

    /*!
     * - 0: auto
     * - 1: 4:3
     * - 2: 16:9
     */
    uint8_t   ratio; //!< VIN aspect ratio

    /*!
     * - 0: auto
     * - 1: NTSC
     * - 2: PAL
     * - 3: SECAM
     * - 4: ALL
     */
    uint8_t   system; //!< VIN system

    /*!
     * - 0: none
     * - 1: auto
     * - 2: FLIP_V
     * - 3: FLIP_H
     * - 4: FLIP both V & H
     */
    uint8_t   flip; //!< VIN flip type

    /*!
     * - 0: linear mode
     * - 1: 2x
     * - 2: 3x
     * - 3: 4x
     */
    uint8_t   hdr_mode; //!< VIN HDR mode
    uint8_t   reserved; //!< Reserved place to keep align
} am_vin_info_t;

/*! @} */ /* End of VIN Parameters */
/******************************************************************************/

/*! @defgroup airapi-datastructure-video-vout VOUT Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief VOUT related parameters,
 *         refer to @ref airapi-commandid-video-vout "VOUT Commands" to see
 *         related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_VOUT_HALT
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_VOUT_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_VOUT_GET
 *  @{
 *//* Start of VOUT Parameter Section */

/*! @enum AM_VOUT_CONFIG_BITS
 *  @brief VOUT configuration type to indicate which config needs to be modified
 */
enum AM_VOUT_CONFIG_BITS
{
  AM_VOUT_CONFIG_TYPE_EN_BIT          = 0,//!< Change VOUT type
  AM_VOUT_CONFIG_VIDEO_TYPE_EN_BIT    = 1,//!< Change VOUT video type
  AM_VOUT_CONFIG_MODE_EN_BIT          = 2,//!< Change VOUT mode
  AM_VOUT_CONFIG_FLIP_EN_BIT          = 3,//!< Change VOUT flip
  AM_VOUT_CONFIG_ROTATE_EN_BIT        = 4,//!< Change VOUT rotate
};

/*! @macros VOUT_MAX_CHAR_NUM
 *  @brief VOUT max char number
 */
#define VOUT_MAX_CHAR_NUM  (16)  //!< used to vout mode

/*! @struct am_vout_config_s
 *  @brief For VOUT config file GET and SET
 */
/*! @typedef am_vout_config_t
 *  @brief For VOUT config file GET and SET
 */
typedef struct am_vout_config_s
{
    /*!
     * @sa AM_VOUT_CONFIG_BITS
     */
    uint32_t enable_bits; //!< AM_VOUT_CONFIG_BITS

    uint32_t vout_id; //!< VOUT ID

    /*!
     * - 0:   none
     * - 1:   CVBS
     * - 2:   HDMI
     * - 3:   LCD
     * - 4:   YPbPr
     */
    uint32_t type; //!< VOUT type

    /*!
     * - 0:   none
     * - 1:   YUV BT601
     * - 2:   YUV BT656
     * - 3:   YUV BT1120
     * - 4:   RGB BT601
     * - 5:   RGB BT656
     * - 6:   RGB BT1120
     */
    uint32_t video_type; //!< VOUT video type

    /*!
     * - 0:   not flip
     * - 1:   flipv
     * - 2:   fliph
     * - 3:   flip both V & h
     */
    uint32_t flip; //!< VOUT flip

    /*!
     * - 0:   not rotate
     * - 1:   rotate 90 degree
     */
    uint32_t rotate; //!< VOUT rotate

    /*!
     * - 0:     auto
     * - x:     x
     * - 1000:   29.97
     * - 1001:   59.94
     * - 1002:   23.976
     * - 1003:   12.5
     * - 1004:   6.25
     * - 1005:   3.125
     * - 1006:   7.5
     * - 1007:   3.75
     */
    uint32_t fps; //!< VOUT FPS

    /*!
     * vout mode to use
     */
    char mode[VOUT_MAX_CHAR_NUM];
} am_vout_config_t;

/*! @} */ /* End of VOUT Parameters */
/******************************************************************************/

/*
 * Stream Format
 */
/***************************Stream Format Parameters***************************/
/*! @defgroup airapi-datastructure-video-stream-fmt Stream Format Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief Stream format related parameters,
 *         refer to @ref airapi-commandid-video-stream-fmt
 *         "Stream Format Commands" to see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_STREAM_FMT_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_STREAM_FMT_SET
 *  @{
 */ /* Start of Stream Format Parameters */

/*! @enum AM_STREAM_FMT_BITS
 *  @brief Stream format type to indicate which config needs to be modified
 */
enum AM_STREAM_FMT_BITS {
  AM_STREAM_FMT_ENABLE_EN_BIT       = 0,  //!< Bit0
  AM_STREAM_FMT_TYPE_EN_BIT         = 1,  //!< Bit1
  AM_STREAM_FMT_SOURCE_EN_BIT       = 2,  //!< Bit2
  AM_STREAM_FMT_FRAME_RATE_EN_BIT   = 3,  //!< Bit3
  AM_STREAM_FMT_WIDTH_EN_BIT        = 4,  //!< Bit5
  AM_STREAM_FMT_HEIGHT_EN_BIT       = 5,  //!< Bit6
  AM_STREAM_FMT_OFFSET_X_EN_BIT     = 6,  //!< Bit7
  AM_STREAM_FMT_OFFSET_Y_EN_BIT     = 7,  //!< Bit8
  AM_STREAM_FMT_HFLIP_EN_BIT        = 8,  //!< Bit9
  AM_STREAM_FMT_VFLIP_EN_BIT        = 9,  //!< Bit10
  AM_STREAM_FMT_ROTATE_EN_BIT       = 10, //!< Bit11
};

/*! @struct am_stream_fmt_s
 *  @brief For stream format file GET and SET
 */
/*! @typedef am_stream_fmt_t
 *  @brief For stream format file GET and SET
 */
typedef struct am_stream_fmt_s {
    /*!
     * @sa AM_STREAM_FMT_BITS
     */
    uint32_t  enable_bits;

    /*!
     * Video stream ID, start from 0
     */
    uint32_t  stream_id;

    /*!
     * - 0: Disable
     * - 1: Enable
     */
    uint32_t  enable;

    /*!
     * - 0: None
     * - 1: H264
     * - 2: H265
     * - 3: MJPEG
     */
    uint32_t  type;

    /*!
     * - 0: Main buffer
     * - 1: 2nd buffer
     * - 2: 3rd buffer
     * - 3: 4th buffer
     * - 4: 5th buffer
     * - 5: PMN buffer
     * - 6: EFM buffer
     */
    uint32_t  source;

    /*!
     * Frame rate
     */
    uint32_t  frame_rate;

    /*!
     * Video width.
     * multiple of 16, -1 for auto configure
     */
    uint32_t  width;

    /*!
     * Video height.
     * multiple of 8, -1 for auto configure
     */
    uint32_t  height;

    /*!
     * Video offset x.
     * multiple of 4
     */
    uint32_t  offset_x;

    /*!
     * Video offset y.
     * multiple of 4
     */
    uint32_t  offset_y;

    /*! horizontal flip.
     * - 0: disable hflip
     * - 1: enable hflip
     */
    uint32_t  hflip;

    /*! vertical flip.
     * - 0: disable vflip
     * - 1: enable vflip
     */
    uint32_t  vflip;

    /*!
     * - 0: disable rotate
     * - 1: enable rotate
     */
    uint32_t  rotate;
} am_stream_fmt_t;

/*! @} */ /* End of Stream Format Paramaters */
/******************************************************************************/

/*
 * Stream Config
 */
/********************************Stream Config*********************************/
/*! @defgroup airapi-datastructure-video-stream-cfg Stream Config Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief Stream config related parameters,
 *         refer to @ref airapi-commandid-video-stream-cfg
 *         "Stream Config Commands" to see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_STREAM_CFG_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_STREAM_CFG_SET
 *  @{
 */ /* Start of Stream Config Parameters */

/*! @enum AM_H26x_CFG_BITS
 *  @brief H264 and H265 config type to indicate which config needs to be modified
 */
enum AM_H26x_CFG_BITS {
  AM_H26x_CFG_BITRATE_CTRL_EN_BIT         = 0,
  AM_H26x_CFG_PROFILE_EN_BIT              = 1,
  AM_H26x_CFG_AU_TYPE_EN_BIT              = 2,
  AM_H26x_CFG_CHROMA_EN_BIT               = 3,
  AM_H26x_CFG_M_EN_BIT                    = 4,
  AM_H26x_CFG_N_EN_BIT                    = 5,
  AM_H26x_CFG_IDR_EN_BIT                  = 6,
  AM_H26x_CFG_SLICE_NUM_EN_BIT            = 7,
  AM_H26x_CFG_SLICES_PER_INFO_EN_BIT      = 8,
  AM_H26x_CFG_BITRATE_EN_BIT              = 9,
  AM_H26x_CFG_MV_THRESHOLD_EN_BIT         = 10,
  AM_H26x_CFG_FLAT_AREA_IMPROVE_EN_BIT    = 11,
  AM_H26x_CFG_MULTI_REF_P_EN_BIT          = 12,
  AM_H26x_CFG_FAST_SEEK_INTVL_EN_BIT      = 13,
};

/*! @typedef am_h26x_cfg_t
 *  @brief For h264 and h265 config file GET and SET
 */
typedef struct am_h26x_cfg_s {
    /*!
     * @sa AM_H26x_CFG_BITS
     */
    uint32_t    enable_bits;

    /*!
     * Video stream ID, start from 0
     */
    uint32_t    stream_id;

    /*!
     * - 0: CBR
     * - 1: VBR
     * - 2: CBR QUALITY
     * - 3: VBR QUALITY
     * - 4: CBR2
     * - 5: VBR2
     * - 6: LBR
     */
    uint32_t    bitrate_ctrl;

    /*!
     * - 0: Baseline
     * - 1: Main
     * - 2: High
     */
    uint32_t    profile;

    /*!
     * - 0: NO_AUD_NO_SEI
     * - 1: AUD_BEFORE_SPS_WITH_SEI
     * - 2: AUD_AFTER_SPS_WITH_SEI
     * - 3: NO_AUD_WITH_SEI
     */
    uint32_t    au_type;

    /*!
     * - 0: 420
     * - 1: 422
     * - 2: Mono
     */
    uint32_t    chroma;

    /*!
     * 1 ~ 3
     */
    uint32_t    M;

    /*!
     * 15 ~ 1800
     */
    uint32_t    N;

    /*!
     * 1 ~ 4
     */
    uint32_t    idr_interval;


    /*! slice number only in HEVC
     *  0 ~ 255
     */
    uint32_t    slice_num;

    /*! slices per info only in HEVC
     * 1 ~ 255
     */
    uint32_t slices_per_info;

    /*!
     * bit per second@30FPS
     * 6400 ~ 12000000
     */
    uint32_t    target_bitrate;

    /*!
     * 0 ~ 64
     */
    uint32_t    mv_threshold;

    /*! flat area improve
     * - 1: enable
     * - 0: disable
     */
    uint32_t    flat_area_improve;

    /*!
     * enable multi-reference P frame
     *
     * - 1: enable
     * - 0: disable
     */
    uint32_t    multi_ref_p;

    /*!
     * specify fast seek P frame interval
     *
     * 0 ~ 63
     */
    uint32_t    fast_seek_intvl;
} am_h26x_cfg_t;

/*! @enum AM_MJPEG_CFG_BITS
 *  @brief MJPEG config type to indicate which config needs to be modified
 */
enum AM_MJPEG_CFG_BITS {
  AM_MJPEG_CFG_QUALITY_EN_BIT    = 0,
  AM_MJPEG_CFG_CHROMA_EN_BIT     = 1,
};

/*! @struct am_mjpeg_cfg_s
 *  @brief For mjpeg config file GET and SET
 */
/*! @typedef am_mjpeg_cfg_t
 *  @brief For mjpeg config file GET and SET
 */
typedef struct am_mjpeg_cfg_s {
    /*!
     * @sa AM_MJPEG_CFG_BITS
     */
    uint32_t    enable_bits;

    /*!
     * Video stream ID, start from 0
     */
    uint32_t    stream_id;

    /*!
     * 1 ~ 99
     */
    uint32_t    quality;

    /*!
     * - 0: 420
     * - 1: 422
     * - 2: Mono
     */
    uint32_t    chroma;
} am_mjpeg_cfg_t;

/*! @enum AM_STREAM_CFG_BITS */
enum AM_STREAM_CFG_BITS
{
  AM_STREAM_CFG_H264_EN_BIT = 0, //!< Config stream to H.264
  AM_STREAM_CFG_MJPEG_EN_BIT = 1, //!< Config stream to MJPEG
};

/*! @struct am_stream_cfg_s
 *  @brief For stream config file GET and SET
 */
/*! @typedef am_stream_cfg_t
 *  @brief For stream config file GET and SET
 */
typedef struct am_stream_cfg_s {
    /*!
     * @sa AM_STREAM_CFG_BITS
     */
    uint32_t        enable_bits;

    uint32_t        stream_id;

    /*!
     * @sa am_h264_cfg_s
     */
    am_h26x_cfg_t   h264;

    /*!
     * @sa am_mjpeg_cfg_s
     */
    am_mjpeg_cfg_t  mjpeg;
} am_stream_cfg_t;

/*! @} */ /* End of Stream Config Parameters */
/******************************************************************************/

/*
 * Stream Dynamic Control
 */
/****************************Stream Dynamic Control****************************/
/*! @defgroup airapi-datastructure-video-stream-dynamic-control Stream Dynamic Control Parameters
 *  @ingroup airapi-datastructure-video
 *  @brief Stream dynamic control related parameters,
 *         refer to @ref airapi-commandid-video-stream-dynamic-control
 *         "Stream Dynamic Control Commands" to see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_MAX_NUM_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_STATUS_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_START
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_STOP
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_FORCE_IDR
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_PARAMETERS_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_STREAM_PARAMETERS_GET
 *  @{
 */ /* Start of Stream Dynamic Control Parameters */

/*! @enum AM_STREAM_DYN_CTRL_BITS
 *  @brief Stream dynamic control option to indicate which parameter needs to be modified
 */
enum AM_STREAM_DYN_CTRL_BITS {
  AM_STREAM_DYN_CTRL_SOURCE_EN_BIT       = 0,  //!< Bit0
  AM_STREAM_DYN_CTRL_TYPE_EN_BIT         = 1,  //!< Bit1
  AM_STREAM_DYN_CTRL_FRAMERATE_EN_BIT    = 2,  //!< Bit2
  AM_STREAM_DYN_CTRL_BITRATE_EN_BIT      = 3,  //!< Bit3
  AM_STREAM_DYN_CTRL_SIZE_EN_BIT         = 4,  //!< Bit4
  AM_STREAM_DYN_CTRL_OFFSET_EN_BIT       = 5,  //!< Bit5
  AM_STREAM_DYN_CTRL_FLIP_EN_BIT         = 6,  //!< Bit6
  AM_STREAM_DYN_CTRL_ROTATE_EN_BIT       = 7,  //!< Bit7
  AM_STREAM_DYN_CTRL_PROFILE_EN_BIT      = 8,  //!< Bit8
  AM_STREAM_DYN_CTRL_GOP_N_EN_BIT        = 9,  //!< Bit9
  AM_STREAM_DYN_CTRL_GOP_IDR_EN_BIT      = 10, //!< Bit10
  AM_STREAM_DYN_CTRL_ENC_QUALITY_EN_BIT  = 11, //!< Bit11
  AM_STREAM_DYN_CTRL_I_FRAME_SIZE_EN_BIT = 12, //!< Bit12
  AM_STREAM_DYN_CTRL_SAVE_CFG_EN_BIT     = 13, //!< Bit13
  AM_STREAM_DYN_CTRL_ABS_BITRATE_EN_BIT  = 14, //!< Bit14
};

/*! @struct am_stream_parameter_s
 *  @brief For stream dynamic control parameter GET and SET
 */
/*! @typedef am_stream_parameter_t
 *  @brief For stream dynamic control parameter GET and SET
 */
typedef struct am_stream_parameter_s {
    /*!
     * @sa AM_STREAM_DYN_CTRL_BITS
     */
    uint32_t  enable_bits;

    /*!
     * Video stream ID, start from 0
     */
    uint32_t  stream_id;

    /*!
     * - 0: Main buffer
     * - 1: 2nd buffer
     * - 2: 3rd buffer
     * - 3: 4th buffer
     * - 4: 5th buffer
     * - 5: PMN buffer
     * - 6: EFM buffer
     */
    uint32_t  source;

    /*!
     * - 0: None
     * - 1: H264
     * - 2: H265
     * - 3: MJPEG
     */
    uint32_t  type;

    /*!
     * Frame rate
     */
    uint32_t  fps;

    /*!
     * Bit rate control mode
     */
    uint32_t  bitrate_ctrl;

    /*!
     * Bit rate
     */
    uint32_t  bitrate;

    /*!
     * keep bit rate in a fixed value
     */
    uint32_t  abs_bitrate;

    /*!
     * Stream width, multiple of 16
     */
    uint32_t  width;

    /*!
     * Stream height, multiple of 8
     */
    uint32_t  height;

    /*!
     * Stream offset x, multiple of 2
     */
    uint32_t  offset_x;

    /*!
     * Stream offset y, multiple of 2
     */
    uint32_t  offset_y;

    /*!
     *  Flip mode.
     * - 0: disable flip
     * - 1: horizontal flip
     * - 2: vertical flip
     * - 3: both horizontal and vertical flip
     */
    uint32_t  flip;

    /*!
     * - 0: disable rotate
     * - 1: enable rotate
     */
    uint32_t  rotate;

    /*!
     * - 0: Baseline
     * - 1: Main
     * - 2: High
     */
    uint32_t    profile;

    /*!
     * H26x gop length: 15 ~ 1800
     */
    uint32_t    gop_n;

    /*!
     * H26x IDR frame interval: 1 ~ 4
     */
    uint32_t    idr_interval;

    /*!
     * Mjpeg encode quality: 1 ~ 99
     */
    uint32_t    quality;

    /*!
     * H26x i frame max size: 0 ~ 8192 KB
     */
    uint32_t    i_frame_max_size;

} am_stream_parameter_t;

/*! @struct am_stream_status_s
 *  @brief For stream status GET
 */
/*! @typedef am_stream_status_t
 *  @brief For stream status GET
 */
typedef struct am_stream_status_s {
    /*!
     * bit OR'ed of all streams,
     * stream[0] is at bit 0, stream[x] is at bit x,
     * bit = 1 means stream is encoding,
     * otherwise it's not encoding.
     */
    uint32_t status;
} am_stream_status_t;

typedef struct am_stream_lock_s {
    /*!
     * Video stream ID, start from 0
     */
    uint32_t stream_id;

    /*!
     * The UUID generated by apps
     */
    uint8_t uuid[16];

    /*!
     * - 0: unlock the stream
     * - 1: lock the stream
     * - 255: RESET all lock states, WARN: this is a dangerous action
     */
    uint8_t operation;

    /*!
     * - 0: operation is done
     * - 1: stream is occupied by another app
     */
    uint8_t op_result;
    uint16_t reserved;
} am_stream_lock_t;

/*! @} */ /* End of Stream Dynamic Control Parameters */
/******************************************************************************/

/*
 * Channel
 */
/************************************Channel**********************************/
/*! @defgroup airapi-datastructure-video-channel CHANNEL
 *  @ingroup airapi-datastructure-video
 *  @brief To get and set channel related parameters
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_CHANNEL_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_CHANNEL_GET
 *  @{
 */ /* Start of Channel */

/*!
 * Max source buffer number
 */
const int MAX_BUFFER_NUM = 5;

/*! @enum AM_CHANNEL_BUF_CONFIG_BITS
 *  @brief channel buffer config bits to indicate which config needs to be modified
 */
enum AM_CHANNEL_BUF_CONFIG_BITS {
  AM_CHANNEL_BUF_MAX_OUTPUT_W_EN_BIT    = 0,
  AM_CHANNEL_BUF_MAX_OUTPUT_H_EN_BIT    = 1,
  AM_CHANNEL_BUF_INPUT_X_EN_BIT         = 2,
  AM_CHANNEL_BUF_INPUT_Y_EN_BIT         = 3,
  AM_CHANNEL_BUF_INPUT_W_EN_BIT         = 4,
  AM_CHANNEL_BUF_INPUT_H_EN_BIT         = 5,
  AM_CHANNEL_BUF_OUTPUT_X_EN_BIT        = 6,
  AM_CHANNEL_BUF_OUTPUT_Y_EN_BIT        = 7,
  AM_CHANNEL_BUF_OUTPUT_W_EN_BIT        = 8,
  AM_CHANNEL_BUF_OUTPUT_H_EN_BIT        = 9,
};

/*! @struct am_channel_buffer_s
 *  @brief For channel's buffer config parameters GET and SET
 */
/*! @typedef am_channel_buffer_t
 *  @brief For channel's buffer config parameters GET and SET
 */
typedef struct am_channel_buffer_s {
  /*!
   * @sa AM_CHANNEL_CONFIG_BITS
   */
  uint32_t   enable_bits;

  /*!
   * @sa source buffer max_output_width
   */
  uint32_t max_output_width;

  /*!
   * @sa source buffer max_output_height
   */
  uint32_t max_output_height;

  /*!
   * @sa source buffer input window offset x
   */
  uint32_t input_x;

  /*!
   * @sa source buffer input window offset y
   */
  uint32_t input_y;

  /*!
   * @sa source buffer input window width
   */
  uint32_t input_width;

  /*!
   * @sa source buffer input window height
   */
  uint32_t input_height;

  /*!
   * @sa source buffer output offset x in canvas
   */
  uint32_t output_x;

  /*!
   * @sa source buffer output offset y in canvas
   */
  uint32_t output_y;

  /*!
   * @sa source buffer output width
   */
  uint32_t output_width;

  /*!
   * @sa source buffer output height
   */
  uint32_t output_height;
}am_channel_buffer_t;

/*! @enum AM_CHANNEL_CONFIG_BITS
 *  @brief channel config bits to indicate which config needs to be modified
 */
enum AM_CHANNEL_CONFIG_BITS {
  AM_CHANNEL_CFG_ENABLE_EN_BIT                = 0,
  AM_CHANNEL_CFG_VSRC_ID_EN_BIT               = 1,
  AM_CHANNEL_CFG_VSRC_CTX_EN_BIT              = 2,
  AM_CHANNEL_CFG_IMG_STATS_SRC_CHAN_EN_BIT    = 3,
  AM_CHANNEL_CFG_IDSP_FPS_EN_BIT              = 4,
  AM_CHANNEL_CFG_MAX_PADDING_WIDTH_EN_BIT     = 5,
  AM_CHANNEL_CFG_MAX_MAIN_INPUT_WIDTH_EN_BIT  = 6,
  AM_CHANNEL_CFG_SENSOR_CTRL_EN_BIT           = 7,
  AM_CHANNEL_CFG_SOURCE_BUFFER_EN_BIT         = 8,
};

/*! @struct am_channel_config_s
 *  @brief For channel config parameters GET and SET
 */
/*! @typedef am_channel_config_t
 *  @brief For channel config parameters GET and SET
 */
typedef struct am_channel_config_s {
  /*!
   * @sa AM_CHANNEL_CONFIG_BITS
   */
  uint32_t   enable_bits;

  /*!
   * Channel ID
   */
  uint32_t   channel_id;

  /*!
   * Video Source ID
   */
  uint32_t   vsrc_id;

  /*!
   *  Video Source Context ID
   */
  uint32_t   vsrc_ctx;

  /*!
   * 3A statistics source channel
   */
  uint32_t   img_stats_src_chan;

  /*!
   * IDSP Framerate
   */
  uint32_t   idsp_fps;

  /*!
   * max padding width
   */
  uint32_t   max_padding_width;

  /*!
   * max main input width
   */
  uint32_t   max_main_input_width;

  /*!
   * sensor control
   */
  bool       sensor_ctrl;

  /*!
   * channel enable
   */
  bool       enable;

  /*!
   * @sa am_channel_buffer_t
   */
  am_channel_buffer_t buffer[MAX_BUFFER_NUM];
}am_channel_config_t;

/*! @} */ /* End of Channel */
/******************************************************************************/

/*
 * Canvas
 */
/************************************Canvas**********************************/
/*! @defgroup airapi-datastructure-video-canvas CANVAS
 *  @ingroup airapi-datastructure-video
 *  @brief To get and set canvas related parameters
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_CANVAS_INFO_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_CANVAS_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_CFG_CANVAS_GET
 *  @{
 */ /* Start of Canvas */

/*!
 * Max canvas number
 */
const int MAX_CANVAS_NUM = 10;

/*!
 * Max Channel number
 */
const int MAX_CHANNEL_NUM = 10;

/*! @struct am_canvas_state_s
 *  @brief For canvas state GET
 */
/*! @typedef am_canvas_state_t
 *  @brief For canvas state GET
 */
typedef struct am_canvas_state_s {
    /*!
     * Canvas ID
     */
    uint32_t    canvas_id;

    /*!
     * - 0: Unknown
     * - 1: Idle
     * - 2: Busy
     * - 3: Error
     */
    uint32_t    state;
} am_canvas_state_t;

/*! @struct am_canvas_info_s
 *  @brief For canvas related information GET
 */
/*! @typedef am_canvas_info_t
 *  @brief For canvas related information GET
 */
typedef struct am_canvas_info_s {
    /*!
     * Canvas id
     */
    uint32_t    canvas_id;

    /*!
     * Canvas type
     */
    uint32_t    canvas_type;

    /*!
     * Canvas width
     */
    uint32_t    width;

    /*!
     * Canvas height
     */
    uint32_t    height;
} am_canvas_info_t;

/*! @struct am_canvas_source_s
 *  @brief For canvas source config parameters GET and SET
 */
/*! @typedef am_canvas_source_t
 *  @brief For canvas source config parameters GET and SET
 */
typedef struct am_canvas_source_s {
  /*!
   * Channel id
   */
  uint32_t    channel_id;

  /*!
   * buffer id
   */
  uint32_t    buffer_id;

  /*!
   * enable
   */
  bool        enable;
} am_canvas_source_t;

/*! @enum AM_CANVAS_CONFIG_BITS
 *  @brief canvas config bits to indicate which config needs to be modified
 */
enum AM_CANVAS_CONFIG_BITS {
  AM_CANVAS_CFG_TYPE_EN_BIT               = 0,
  AM_CANVAS_CFG_WIDTH_EN_BIT              = 1,
  AM_CANVAS_CFG_HEIGHT_EN_BIT             = 2,
  AM_CANVAS_CFG_EXTRA_DRAM_BUFF_EN_BIT    = 3,
  AM_CANVAS_CFG_CAP_SKIP_ITVL_EN_BIT      = 4,
  AM_CANVAS_CFG_VOUT_ID_EN_BIT            = 5,
  AM_CANVAS_CFG_CHANNEL_ORDER_CTRL_EN_BIT = 6,
  AM_CANVAS_CFG_AUTO_STOP_EN_BIT          = 7,
  AM_CANVAS_CFG_SOURCE_EN_BIT             = 8,
  AM_CANVAS_WORKING_NUM_EN_BIT            = 9,
};

/*! @struct am_canvas_config_s
 *  @brief For canvas config parameters GET and SET
 */
/*! @typedef am_canvas_config_t
 *  @brief For canvas config parameters GET and SET
 */
typedef struct am_canvas_config_s {
    /*!
     * @sa AM_CANVAS_CONFIG_BITS
     */
    uint32_t   enable_bits;

    /*!
     * working canvas number
     */
    uint32_t   working_canvas_num;

    /*!
     * Canvas id
     */
    uint32_t   canvas_id;

    /*!
     * Canvas type
     * - 1: encode
     * - 2: preview
     */
    uint32_t   canvas_type;

    /*!
     * Canvas width
     */
    uint32_t   width;

    /*!
     * Canvas height
     */
    uint32_t   height;

    /*!
     * Canvas extra_dram_buf
     */
    uint32_t   extra_dram_buf;

    /*!
     * Canvas cap_skip_itvl
     */
    uint32_t   cap_skip_itvl;

    /*!
     * Canvas vout_id
     */
    uint32_t   vout_id;

    /*!
     * Canvas chan_order_ctrl
     */
    bool       chan_order_ctrl;

    /*!
     * Canvas auto_stop
     */
    bool       auto_stop;

    /*!
     * Canvas source buffers
     */
    am_canvas_source_t source[MAX_CHANNEL_NUM];
} am_canvas_config_t;

/*! @} */ /* End of Canvas */
/******************************************************************************/

/*
 * Source buffer
 */
/******************************Source Buffer Format****************************/
/*! @defgroup airapi-datastructure-video-src-buf-fmt Source Buffer Format
 *  @ingroup airapi-datastructure-video
 *  @brief Source buffer format related parameters,
 *         refer to @ref airapi-commandid-video-src-buf-fmt
 *         "Source Buffer Format Commands" to see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_BUFFER_FMT_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_BUFFER_FMT_SET
 *  @{
 */ /* Start of Source Buffer Format */

/*! @struct am_buffer_id_s
 *  @brief Specify the channel id and buffer id
 */
/*! @typedef am_buffer_id_t
 *  @brief Specify the channel id and buffer id
 */
typedef struct am_buffer_id_s {
    /*!
     * Channel id number
     */
    uint32_t    channel_id;

    /*!
     * - 0: Main buffer
     * - 1: 2nd buffer
     * - 2: 3rd buffer
     * - 3: 4th buffer
     * - 4: 5th buffer
     */
    uint32_t    buffer_id;
} am_buffer_id_t;


/*! @struct am_buffer_state_s
 *  @brief For source buffer state GET
 */
/*! @typedef am_buffer_state_t
 *  @brief For source buffer state GET
 */
typedef struct am_buffer_state_s {
    /*!
     * - 0: Main buffer
     * - 1: 2nd buffer
     * - 2: 3rd buffer
     * - 3: 4th buffer
     */
    uint32_t    buffer_id;

    /*!
     * - 0: Idle
     * - 1: Busy
     * - 2: Error
     * - 3: Unknown
     */
    uint32_t    state;
} am_buffer_state_t;


/*! @enum AM_BUFFER_FMT_BITS
 *  @brief Buffer format type to indicate which config needs to be modified
 */
enum AM_BUFFER_FMT_BITS {
  AM_BUFFER_FMT_TYPE_EN_BIT           = 0,
  AM_BUFFER_FMT_INPUT_CROP_EN_BIT     = 1,
  AM_BUFFER_FMT_INPUT_WIDTH_EN_BIT    = 2,
  AM_BUFFER_FMT_INPUT_HEIGHT_EN_BIT   = 3,
  AM_BUFFER_FMT_INPUT_X_EN_BIT        = 4,
  AM_BUFFER_FMT_INPUT_Y_EN_BIT        = 5,
  AM_BUFFER_FMT_WIDTH_EN_BIT          = 6,
  AM_BUFFER_FMT_HEIGHT_EN_BIT         = 7,
  AM_BUFFER_FMT_OUTPUT_X_EN_BIT       = 8,
  AM_BUFFER_FMT_OUTPUT_Y_EN_BIT       = 9,
  AM_BUFFER_FMT_PREWARP_EN_BIT        = 10,
  AM_BUFFER_FMT_CAP_SKIP_ITVL_EN_BIT  = 11,
  AM_BUFFER_FMT_AUTO_STOP_EN_BIT      = 12,
  AM_BUFFER_FMT_SAVE_CFG_EN_BIT       = 13,
};

/*! @struct am_buffer_fmt_s
 *  @brief For buffer format file GET and SET
 */
/*! @typedef am_buffer_fmt_t
 *  @brief For buffer format file GET and SET
 */
typedef struct am_buffer_fmt_s {
    /*!
     * @sa AM_BUFFER_FMT_BITS
     */
    uint32_t    enable_bits;

    /*!
     * Channel ID, start from 0
     */
    uint32_t    channel_id;

    /*!
     * - 0: Main buffer
     * - 1: 2nd buffer
     * - 2: 3rd buffer
     * - 3: 4th buffer
     * - 4: 5th buffer
     *
     * - 5: pre-main buffer
     * - 6: efm buffer
     */
    uint32_t    buffer_id;

    /*!
     * Canvas ID, start from 0
     */
    uint32_t    canvas_id;

    /*!
     * - 0: off
     * - 1: encode
     * - 2: preview
     */
    uint32_t    type;

    /*!
     * - 0: false
     * - 1: true
     */
    uint32_t    input_crop;

    /*!
     * input window:
     * {input_width, input_height, input_offset_x, input_offset_y}
     * Only use in input_crop is true
     */
    uint32_t    input_width;

    /*!
     * input window:
     * {input_width, input_height, input_offset_x, input_offset_y}
     * Only use in input_crop is true
     */
    uint32_t    input_height;

    /*!
     * input window:
     * {input_width, input_height, input_offset_x, input_offset_y}
     * Only use in input_crop is true
     */
    uint32_t    input_offset_x;

    /*!
     * input window:
     * {input_width, input_height, input_offset_x, input_offset_y}
     * Only use in input_crop is true
     */
    uint32_t    input_offset_y;

    /*!
     * buffer size: (width, height)
     */
    uint32_t    width;

    /*!
     * buffer size: (width, height)
     */
    uint32_t    height;

    /*!
     * Source buffer output offset X in corresponding canvas
     */
    uint32_t    output_offset_x;

    /*!
     * Source buffer output offset Y in corresponding canvas
     */
    uint32_t    output_offset_y;

    /*!
     * - 0: false
     * - 1: true
     */
    uint32_t    prewarp;

    /*!
     * capture one frame every N+1 frame in yuv capture
     */
    uint32_t    cap_skip_itvl;

    /*!
     * auto stop capture yuv after source buffer is full
     */
    uint32_t    auto_stop;
} am_buffer_fmt_t;
/*! @} */ /* End of Source Buffer Format */
/******************************************************************************/

/*
 * DPTZ
 */
/********************************DPTZ *****************************************/
/*! @defgroup airapi-datastructure-video-dptz DPTZ
 *  @ingroup airapi-datastruture-video
 *  @brief DPTZ Warp related parameters used,
 *         refer to @ref airapi-commandid-video-dptz "DPTZ Commands"
 *         to see related commands.
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_DPTZ_RATIO_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_DPTZ_RATIO_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_DPTZ_SIZE_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_DPTZ_SIZE_SET
 *  @{
 */ /* Start of DPTZ */

/*! @enum AM_DPTZ_BITS
 *  @brief DPTZ type to indicate which configure needs to be modified
 */
enum AM_DPTZ_BITS {
  AM_DPTZ_PAN_RATIO_EN_BIT    = 0,
  AM_DPTZ_TILT_RATIO_EN_BIT   = 1,
  AM_DPTZ_ZOOM_RATIO_EN_BIT   = 2,
  AM_DPTZ_SIZE_X_EN_BIT       = 3,
  AM_DPTZ_SIZE_Y_EN_BIT       = 4,
  AM_DPTZ_SIZE_W_EN_BIT       = 5,
  AM_DPTZ_SIZE_H_EN_BIT       = 6
};

/*! @typedef am_dptz_ratio_t
 *  @brief For DPTZ RATIO GET and SET
 */
typedef struct am_dptz_ratio_s {
    /*!
     * @sa AM_DPTZ_BITS
     */
    uint32_t enable_bits;
    /*!
     * specify the channel id
     */
    uint32_t channel_id;
    /*!
     * 0~3, 0: main buffer (DPTZ-I),  1-3: sub buffer (DPTZ-II)
     * */
    uint32_t buffer_id;
    /*!
     * -1.0~1.0, -1.0: left most,  1.0: right most
     * */
    float pan_ratio;
    /*!
     * -1.0~1.0, -1.0: top most,  1.0: bottom most
     * */
    float tilt_ratio;
    /*!
     * 0.1~8.0, 0.1: zoom in most, 1: original, 8.0: zoom out most
     */
    float zoom_ratio;
} am_dptz_ratio_t;

/*! @typedef am_dptz_size_t
 *  @brief For DPTZ Size GET and SET
 */
typedef struct am_dptz_size_s {
    /*!
     * @sa AM_DPTZ_SIZE_BITS
     */
    uint32_t enable_bits;
    /*!
     * specify the channel id
     */
    uint32_t channel_id;
    /*!
     * 0~3, 0: main buffer (DPTZ-I),  1-3: sub buffer (DPTZ-II)
     * */
    uint32_t buffer_id;

    uint32_t w;//!< input window width
    uint32_t h;//!< input window height
    uint32_t x;//!< input window x
    uint32_t y;//!< input window y

} am_dptz_size_t;

/*! @} */ /* End of DPTZ */
/******************************************************************************/

/*
 * Warp
 */
/********************************Warp *****************************************/
/*! @defgroup airapi-datastructure-video-warp Warp
 *  @ingroup airapi-datastruture-video
 *  @brief DPTZ Warp related parameters used,
 *         refer to @ref airapi-commandid-video-warp "Warp Commands"
 *         to see related commands.
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_WARP_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_DYN_WARP_SET
 *  @{
 */ /* Start of Warp */

/*! @enum AM_WARP_BITS
 *  @brief Warp type to indicate which configure needs to be modified
 */
enum AM_WARP_BITS {
  AM_WARP_REGION_ID_EN_BIT = 0,
  AM_WARP_LDC_STRENGTH_EN_BIT,
  AM_WARP_PANO_HFOV_DEGREE_EN_BIT,
  AM_WARP_VER_PANO_FOV_DEGREE_EN_BIT,
  AM_WARP_REGION_YAW_PITCH_EN_BIT,
  AM_WARP_REGION_ROTATE_EN_BIT,
  AM_WARP_LDC_WARP_MODE_EN_BIT,
  AM_WARP_MAX_RADIUS_EN_BIT,
  AM_WARP_SAVE_CFG_EN_BIT,
  AM_WARP_HOR_VER_ZOOM_EN_BIT,
  AM_WARP_REGION_DPTZ_EN_BIT,
  AM_WARP_PAN_ANGLE_EN_BIT,
  AM_WARP_TILT_ANGLE_EN_BIT,
  AM_WARP_SUB_ROI_OFFSET_EN_BIT,
  AM_WARP_ZOOM_EN_BIT,
};

const int MAX_WARP_AREAS = 12;

/*! @typedef warp_region_dptz_t
 *  @brief For Warp region DPTZ set
 */
typedef struct warp_region_dptz_s {
  uint32_t input_w        = 0; //!< input window width
  uint32_t input_h        = 0; //!< input window height
  uint32_t input_x        = 0; //!< input window offset x
  uint32_t input_y        = 0; //!< input window offset y
  uint32_t output_w       = 0; //!< output size width
  uint32_t output_h       = 0; //!< output size height
  uint32_t output_x       = 0; //!< output offset x
  uint32_t output_y       = 0; //!< output offset y
  bool input_size_flag    = false; //!< input_size_flag
  bool input_offset_flag  = false; //!< input_offset_flag
  bool output_size_flag   = false; //!< output_size_flag
  bool output_offset_flag = false; //!< output_offset_flag
} warp_region_dptz_t;

/*! @typedef am_warp_t
 *  @brief For Warp GET and SET
 */
typedef struct am_warp_s {
    /*!
     * @sa AM_WARP_BITS
     */
    uint32_t enable_bits;
    /*!
     * Fisheye correction region number
     */
    int region_id;
    /*!
     * 0~8, warp mode
     */
    int warp_mode;
    /*!
     * Full FOV circle radius (pixel) in vin
     */
    int max_radius;
    /*!
     * -90~90, Lens warp region yaw in degree
     */
    int warp_region_yaw;
    /*!
     * -90~90, Lens warp region pitch in degree
     */
    int warp_region_pitch;
    /*!
     * -20~20, Lens warp region rotate in degree
     */
    int warp_region_rotate;
    /*!
     * warp zoom ratio parameter,
     * low 16bit is denominator, high 16bit is numerator
     */
    uint warp_zoom;
    /*!
     * horizontal zoom ratio parameter,
     * low 16bit is denominator, high 16bit is numerator
     */
    uint hor_zoom;
    /*!
     * vertical zoom ratio parameter,
     * low 16bit is denominator, high 16bit is numerator
     */
    uint ver_zoom;
    /*!
     * 0.0~20.0, 0.0: do not do lens distortion calibration,
     *  20: maximum lens distortion calibration
     */
    float ldc_strength;
    /*!
     * 1.0~180.0, only for panorama mode
     */
    float pano_hfov_degree;
    /*!
     * 1.0~180.0, only for vertical panorama and transverse mercator mode
     */
    float ver_pano_fov_degree;
    /*!
     * set buffer id for the DPTZ area, 1 for 2nd buffer, 3 for 4th buffer
     */
    int buffer_id;
    /*!
     * (Wall Subregion)Pan angle. -90~90 for wall mount.
     */
    float pan_angle;
    /*!
     * (Wall Subregion)Tilt angle. -90~90 for wall mount.
     */
    float tilt_angle;
    /*!
     * (Wall Subregion) ROI center offset x to the circle center
     */
    float sub_roi_offset_x;
    /*!
     * (Wall Subregion) ROI center offset y to the circle center
     */
    float sub_roi_offset_y;
    /*!
     * specify DPTZ area input and output
     */
    warp_region_dptz_t warp_region_dptz[MAX_WARP_AREAS];

} am_warp_t;

/*! @} */ /* End of Warp */
/******************************************************************************/

/*
 * H.264 or H.265 encode low bite rate control
 */
/**********************************LBR Control*********************************/
/*! @defgroup airapi-datastructure-video-lbr LBR Control
 *  @ingroup airapi-datastructure-video
 *  @brief H.264 or H.265 LBR control related parameters used,
 *         refer to @ref airapi-commandid-video-lbr "LBR Control Commands"
 *         to see related commands.
 *
 *  @sa _AM_IPC_MW_CMD_VIDEO_DYN_LBR_GET
 *  @sa _AM_IPC_MW_CMD_VIDEO_DYN_LBR_SET
 *  @{
 */ /* Start of H264 or H265 LBR Control */

/*! @enum AM_ENCODE_H264_LBR_CTRL_BITS
 *  @brief H.264 LBR control type to indicate which config needs to be modified
 */
enum AM_ENCODE_LBR_CTRL_BITS {
  AM_ENCODE_LBR_ENABLE_LBR_EN_BIT              = 0,
  AM_ENCODE_LBR_AUTO_BITRATE_CEILING_EN_BIT    = 1,
  AM_ENCODE_LBR_BITRATE_CEILING_EN_BIT         = 2,
  AM_ENCODE_LBR_DROP_FRAME_EN_BIT              = 3,
  AM_ENCODE_LBR_SAVE_CURRENT_CONFIG_EN_BIT     = 4,
};

/*! @struct am_encode_lbr_ctrl_s
 *  @brief For lbr control GET and SET
 */
/*! @typedef am_encode_lbr_ctrl_t
 *  @brief For lbr control GET and SET
 */
typedef struct am_encode_lbr_ctrl_s {
    /*!
     * @sa AM_ENCODE_LBR_CTRL_BITS
     */
    uint32_t enable_bits;
    uint32_t stream_id;

    /*!
     * when auto_bitrate_seiling is false, you can set it.
     */
    uint32_t bitrate_ceiling;

    /*!
     * - 0: disable LBR and use CBR
     * - 1: enable LBR
     */
    bool enable_lbr;

    /*!
     * - false: close auto bitrate ceiling and need to set manually
     * - true: auto bitrate ceiling, can not set bitrate ceiling now.
     */
    bool auto_bitrate_ceiling;

    /*!
     * - false: do not drop frame when motion is large
     * - true: drop frame auto when motion is large
     */
    bool drop_frame;
} am_encode_lbr_ctrl_t;
/*! @} */ /* End of H264 or H265 LBR Control */
/******************************************************************************/

/*
 * EIS mode control
 */
/********************************EIS mode Control******************************/
/*! @defgroup airapi-datastructure-video-EIS EIS Control
 *  @ingroup airapi-datastructure-video
 *  @brief EIS control related parameters used,
 *         refer to @ref airapi-commandid-video-eis "EIS Control Commands"
 *         to see related commands.
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_ENCODE_EIS_GET
 *  @sa AM_IPC_MW_CMD_VIDEO_ENCODE_EIS_SET
 *  @{
 */ /* Start of EIS Control */

/*! @enum AM_ENCODE_EIS_CTRL_BITS
 *  @brief EIS control type to indicate which config needs to be modified
 */
enum AM_ENCODE_EIS_CTRL_BITS {
  AM_ENCODE_EIS_MODE_EN_BIT  = 0,
  AM_ENCODE_EIS_SAVE_CFG_EN_BIT = 1,
};

/*! @struct am_encode_eis_ctrl_s
 *  @brief For eis control GET and SET
 */
/*! @typedef am_encode_eis_ctrl_t
 *  @brief For eis control GET and SET
 */
typedef struct am_encode_eis_ctrl_s {
  /*!
   * @sa AM_ENCODE_EIS_CTRL_BITS
   */
  uint32_t enable_bits;

  /*!
   * 5: eis_full, 4: rotate + pitch, 3: yaw, 2: rotate, 1: pitch, 0: disable
   */
  int32_t eis_mode;
} am_encode_eis_ctrl_t;
/*! @} */ /* End of EIS Control */
/******************************************************************************/

/*
 * Overlay
 */
/************************************Overlay***********************************/
/*! @defgroup airapi-datastructure-video-overlay Video Overlay
 *  @ingroup airapi-datastruture-video
 *  @brief Overlay related parameters used,
 *         refer to @ref airapi-commandid-video-overlay "Overlay Commands"
 *         to see related commands.
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_MAX_NUM_GET
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DESTROY
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_SAVE
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_INIT
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DATA_ADD
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DATA_UPDATE
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_SET
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_GET
 * @sa AM_IPC_MW_CMD_VIDEO_DYN_OVERLAY_DATA_GET
 *  @{
 */ /* Start of Overlay Control */

/*! @enum AM_OVERLAY_BITS
 *  @brief Overlay control type to indicate which option needs to be modified
 */
enum AM_OVERLAY_BITS {
  AM_OVERLAY_REMOVE_EN_BIT = 0,          //!< remove a overlay area
  AM_OVERLAY_ENABLE_EN_BIT = 1,          //!< enable a overlay area
  AM_OVERLAY_DISABLE_EN_BIT = 2,         //!< disable a overlay area
  AM_OVERLAY_INIT_EN_BIT = 3,            //!< init a area
  AM_OVERLAY_DATA_ADD_EN_BIT = 4,        //!< add a data block to area
  AM_OVERLAY_DATA_UPDATE_EN_BIT = 5,     //!< update a data block for a area
  AM_OVERLAY_DATA_REMOVE_EN_BIT = 6,     //!< remove a data block for a area
  AM_OVERLAY_ROTATE_EN_BIT = 7,          //!< area whether rotated with stream
  AM_OVERLAY_BG_COLOR_EN_BIT = 8,        //!< area or text background color
  AM_OVERLAY_BUF_NUM_EN_BIT = 9,         //!< area buffer number
  AM_OVERLAY_RECT_EN_BIT = 10,           //!< area or data block size and offset
  AM_OVERLAY_DATA_TYPE_EN_BIT = 11,      //!< data block type
  AM_OVERLAY_STRING_EN_BIT = 12,         //!< string to be add or update
  AM_OVERLAY_TIME_EN_BIT = 13,           //!< prefix/suffix string or enable msec
                                         //!< display for time type data block
  AM_OVERLAY_CHAR_SPACING_EN_BIT = 14,   //!< char spacing for text
  AM_OVERLAY_FONT_TYPE_EN_BIT = 15,      //!< font type for text data type
  AM_OVERLAY_FONT_SIZE_EN_BIT = 16,      //!< font size for text data type
  AM_OVERLAY_FONT_COLOR_EN_BIT = 17,     //!< font color for text data type
  AM_OVERLAY_FONT_OUTLINE_EN_BIT = 18,   //!< font outline for text data type
  AM_OVERLAY_FONT_BOLD_EN_BIT = 19,      //!< font hor and ver bold
  AM_OVERLAY_FONT_ITALIC_EN_BIT = 20,    //!< font italic for text data type
  AM_OVERLAY_BMP_EN_BIT = 21,            //!< bmp file to be add or update
  AM_OVERLAY_BMP_COLOR_EN_BIT = 22,      //!< color key and range used for transparent
  AM_OVERLAY_ANIMATION_EN_BIT = 23,      //!< animation parameter
  AM_OVERLAY_LINE_COLOR_EN_BIT = 24,     //!< color for line
  AM_OVERLAY_LINE_POINTS_EN_BIT = 25,    //!< points for line
  AM_OVERLAY_LINE_THICKNESS_EN_BIT = 26, //!< line thickness
  AM_OVERLAY_RTG_COLOR_EN_BIT = 27,      //!< color for rectangle
  AM_OVERLAY_RTG_POINTS_EN_BIT = 28,     //!< points for rectangle
  AM_OVERLAY_RTG_THICKNESS_EN_BIT = 29,  //!< line thickness for rectangle
};


/*! @macros OVERLAY_MAX_STRING
 *  @brief Overlay max char number which overlay type is string
 */
#define OVERLAY_MAX_STRING    (128) //!< user can modify it, but if it's a large value,
                                    //!< the total size maybe large than
                                    //!< AM_MAX_IPC_MESSAGE_SIZE which may cause
                                    //!< AIR API call failed

/*! @macros OVERLAY_MAX_FILENAME
  *  @brief Overlay max size number of bmp file and font file full name
 */
#define OVERLAY_MAX_FILENAME  (128) //!< user can modify it, but if it's a large value,
                                    //!< the total size maybe large than
                                    //!< AM_MAX_IPC_MESSAGE_SIZE which may cause
                                    //!< AIR API call failed

/*! @macros OVERLAY_MAX_POINT
 *  @brief Overlay max point number used for type line
 */
#define OVERLAY_MAX_POINT    (5)    //!< user can modify it, but if it's a large value,
                                    //!< the total size maybe large than
                                    //!< AM_MAX_IPC_MESSAGE_SIZE which may cause
                                    //!< AIR API call failed

/*! @macros OVERLAY_RTG_MAX_POINT_PAIR
 *  @brief Overlay rectangle max point pair number
 */
#define OVERLAY_RTG_MAX_POINT_PAIR    (100) //!< user can modify it, but if the value is too large,
                                           //!< the total size maybe larger than
                                           //!< AM_MAX_IPC_MESSAGE_SIZE which may cause
                                           //!< AIR API call failed
/*! @macros OVERLAY_MAX_STR_NUM
 *  @brief Overlay max string data number user can add/update per time
 */
#define OVERLAY_MAX_STR_NUM   (2)   //!< max string type data user can add/update per time

/*! @macros OVERLAY_MAX_PIC_NUM
 *  @brief Overlay max picture data number user can add/update per time
 */
#define OVERLAY_MAX_PIC_NUM   (2)   //!< max picture type data user can add/update per time

/*! @macros OVERLAY_MAX_TIME_NUM
 *  @brief Overlay max time data number user can add/update per time
 */
#define OVERLAY_MAX_TIME_NUM (1)

/*! @macros OVERLAY_MAX_RTG_NUM
 *  @brief Overlay max rectangle data number user can add/update per time
 */
#define OVERLAY_MAX_RTG_NUM (1)

/*! @macros OVERLAY_MAX_LINE_NUM
 *  @brief Overlay max line data number user can add/update per time
 */
#define OVERLAY_MAX_LINE_NUM  (24)  //!< max line type data user can add/update per time

/*! @macros OVERLAY_MAX_DATA_NUM
 *  @brief Overlay max data number user can add/update per time
 */
#define OVERLAY_MAX_DATA_NUM (24)
/*! @macros OVERLAY_MAX_INDEX_NUM
 *  @brief Overlay max index number which is used in remove data cmd
 */
#define OVERLAY_MAX_INDEX_NUM (64)

/*! @struct am_overlay_id_s
 *  @brief For overlay area SET/GET and data GET
 */
/*! @typedef am_overlay_id_t
 *  @brief For overlay area SET/GET and data GET
 */
typedef struct am_overlay_id_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits = 0;
    /*!
     * index number
     */
    uint16_t index_num = 0;
    /*!
     * encode stream id
     */
    uint8_t stream_id = 0;
    /*!
     * area id in stream
     */
    uint8_t area_id = 0;
    /*!
     * data block index in area
     */
    int16_t data_index[OVERLAY_MAX_INDEX_NUM] = {0};
    /*!
     * remove/update success or not, 0 : success, -1 : fail
     */
    int8_t index_ret[OVERLAY_MAX_INDEX_NUM] = {0};
} am_overlay_id_t;

/*! @struct am_overlay_limit_val_s
 *  @brief For get overlay platform and user defined limit value
 */
/*! @typedef am_overlay_limit_val_t
 *  @brief For get overlay platform and user defined limit value
 */
typedef struct am_overlay_limit_val_s {
    /*!
     * platform support max encode stream number
     */
    uint8_t platform_stream_num_max = 0;

    /*!
     * platform support max overlay area number per stream
     */
    uint8_t platform_overlay_area_num_max = 0;

    /*!
     * user defined max stream number for overlay
     */
    uint8_t user_def_stream_num_max = 0;

    /*!
     * user defined max overlay area number per stream
     */
    uint8_t user_def_overlay_area_num_max = 0;

} am_overlay_limit_val_t;

/*! @struct am_overlay_time_data_s
 *  @brief For time type overlay
 */
/*! @typedef am_overlay_time_data_t
 *  @brief For time type overlay
 */
typedef struct am_overlay_time_data_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits;
    /*!
     * text back ground color
     */
    uint32_t bg_color;
    /*!
     * font color : 0-7 is predefine color : 0 white, 1 black, 2 red, 3 blue,
     * 4 green, 5 yellow, 6 cyan, 7 magenta; > 7 user custom color(YUV format)
     */
    uint32_t font_color;
    /*!
     * font outline color
     */
    uint32_t font_outline_color;
    /*!
     * data block width
     */
    uint16_t width;
    /*!
     * data block height
     */
    uint16_t height;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y;
    /*!
     * data block index in the area
     */
    int16_t data_index;
    /*!
     * char spacing
     */
    uint8_t spacing;
    /*!
     * font size
     */
    uint8_t font_size;
    /*!
     * font outline width
     */
    uint8_t font_outline_w;
    /*!
     * font horizontal bold size
     * 0 : no bold at all; positive is bold, negative is thin in the horizontal.
     */
    int8_t  font_hor_bold;
    /*!
     * font vertical bold
     * 0 : no bold at all; positive is bold, negative is thin in vertical.
     */
    int8_t  font_ver_bold;
    /*!
     * font italic
     */
    uint8_t font_italic;
    /*!
     * enable msecond display or ont when type is time
     */
    uint8_t msec_en;
    /*!
     * time display format:
     * 0:YYYY-MM-DD hh:mm:ss
     * 1:MM/DD/YYYY hh:mm:ss
     * 2:DD/MM/YYYY hh:mm:ss
     */
    uint8_t time_format;
    /*!
     * whether use 12H notation to display time.
     */
    uint8_t is_12h;
    uint8_t reserved;
    /*!
     * prefix string to be insert when type is time
     */
    char pre_str[OVERLAY_MAX_STRING];
    /*!
     * suffix string to be insert when type is time
     */
    char suf_str[OVERLAY_MAX_STRING];
    /*!
     * font type name
     */
    char font_type[OVERLAY_MAX_FILENAME];
} am_overlay_time_data_t;

/*! @struct am_overlay_str_data_s
 *  @brief For string type overlay
 */
/*! @typedef am_overlay_str_data_t
 *  @brief For string type overlay
 */
typedef struct am_overlay_str_data_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits;
    /*!
     * text back ground color
     */
    uint32_t bg_color;
    /*!
     * font color : 0-7 is predefine color : 0 white, 1 black, 2 red, 3 blue,
     * 4 green, 5 yellow, 6 cyan, 7 magenta; > 7 user custom color(YUV format)
     */
    uint32_t font_color;
    /*!
     * font outline color
     */
    uint32_t font_outline_color;
    /*!
     * data block width
     */
    uint16_t width;
    /*!
     * data block height
     */
    uint16_t height;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y;
    /*!
     * data block index in the area
     */
    int16_t data_index;
    /*!
     * char spacing
     */
    uint8_t spacing;
    /*!
     * font size
     */
    uint8_t font_size;
    /*!
     * font outline width
     */
    uint8_t font_outline_w;
    /*!
     * font horizontal bold size
     * 0 : no bold at all; positive is bold, negative is thin in the horizontal.
     */
    int8_t  font_hor_bold;
    /*!
     * font vertical bold
     * 0 : no bold at all; positive is bold, negative is thin in vertical.
     */
    int8_t  font_ver_bold;
    /*!
     * font italic
     */
    uint8_t font_italic;
    /*!
     * string to be insert
     */
    char str[OVERLAY_MAX_STRING];
    /*!
     * font type name
     */
    char font_type[OVERLAY_MAX_FILENAME];
} am_overlay_str_data_t;

/*! @struct am_overlay_pic_data_s
 *  @brief For picture and animation type overlay
 */
typedef struct am_overlay_pic_data_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits;
    /*!
     * color used to transparent in picture type(VUYA format):
     * v:bit[24-31],u:bit[16-23],y:bit[8-15],a:bit[0-7]
     */
    uint32_t color_key;
    /*!
     * data block width
     */
    uint16_t width;
    /*!
     * data block height
     */
    uint16_t height;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y;
    /*!
     * data block index in area
     */
    int16_t data_index;
    /*!
     * color range used to transparent with color_key
     */
    uint8_t color_range;
    /*!
     * bmp number in the animation file
     */
    uint8_t bmp_num;
    /*!
     * frame interval to update bmp when type is animation
     */
    uint8_t interval;
    uint8_t reserved0;
    uint8_t reserved1;
    uint8_t reserved2;
    /*!
     * bmp full path name
     */
    char bmp[OVERLAY_MAX_FILENAME];
} am_overlay_pic_data_t;

/*! @struct am_overlay_line_data_s
 *  @brief For line type overlay
 */
typedef struct am_overlay_line_data_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits;
    /*!
     * line color:0-7 is predefine color: 0,white;1,black;2,red;3,blue;
     * 4,green;5,yellow;6,cyan;7,magenta;>7,user custom color(VUYA format)
     */
    uint32_t line_color;
    /*!
     * data block width
     */
    uint16_t width;
    /*!
     * data block height
     */
    uint16_t height;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y;
    /*!
     * data block index in area
     */
    int16_t data_index;
    /*!
     * line thickness
     */
    uint8_t line_tn;
    /*!
     * point number
     */
    uint8_t p_n;
    /*!
     * point x value for draw linen
     */
    uint16_t p_x[OVERLAY_MAX_POINT];
    /*!
     * point y value for draw line
     */
    uint16_t p_y[OVERLAY_MAX_POINT];

} am_overlay_line_data_t;

/*! @struct am_overlay_rtg_data_s
 *  @brief For rectangle type overlay
 */
typedef struct am_overlay_rtg_data_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits;
    /*!
     * rtg color:0-7 is predefine color: 0,white;1,black;2,red;3,blue;
     * 4,green;5,yellow;6,cyan;7,magenta;>7,user custom color(VUYA format)
     */
    uint32_t rtg_color;
    /*!
     * data block width
     */
    uint16_t width;
    /*!
     * data block height
     */
    uint16_t height;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y;
    /*!
     * data block index in area
     */
    int16_t data_index;
    /*!
     * rtg line thickness
     */
    uint8_t rtg_line_tn;
    /*!
     * point pair number
     */
    uint8_t point_pair_num;
    /*!
     * rectangle type
     */
    uint8_t rtg_type;
    uint8_t reserved;
    /*!
     * rectangle first point x value
     */
    uint16_t first_x[OVERLAY_RTG_MAX_POINT_PAIR];
    /*!
     * rectangle first point y value
     */
    uint16_t first_y[OVERLAY_RTG_MAX_POINT_PAIR];
    /*!
     * rectangle second point x value
     */
    uint16_t second_x[OVERLAY_RTG_MAX_POINT_PAIR];
    /*!
     * rectangle second point y value
     */
    uint16_t second_y[OVERLAY_RTG_MAX_POINT_PAIR];

} am_overlay_rtg_data_t;

/*! @struct am_overlay_data_s
 *  @brief For overlay data ADD/UPDATE
 */
typedef struct am_overlay_data_s {
    /*!
     * encode stream id
     */
    uint8_t stream_id;
    /*!
     * area id in stream
     */
    uint8_t area_id;
    /*!
     * data block type, 0:text; 1:picture; 2:time(a special text type);
     * 3:animation; 4:line; 5:rectangle;
     */
    uint8_t type;
    /*!
     * data block number
     */
    uint8_t data_num;
    /*!
     * data block parameter
     * @sa am_overlay_str_data_s
     * @sa am_overlay_pic_data_s
     * @sa am_overlay_line_data_s
     * @sa am_overlay_time_data_s
     */
    union {
        am_overlay_str_data_t   str[OVERLAY_MAX_STR_NUM];
        am_overlay_pic_data_t   pic[OVERLAY_MAX_PIC_NUM];
        am_overlay_line_data_t  line[OVERLAY_MAX_LINE_NUM];
        am_overlay_time_data_t  time[OVERLAY_MAX_TIME_NUM];
        am_overlay_rtg_data_t   rtg[OVERLAY_MAX_RTG_NUM];
    };
} am_overlay_data_t;

/*! @struct am_overlay_data_get_s
 *  @brief For overlay data GET
 */
typedef struct am_overlay_data_get_s {
    /*!
     * data block width
     */
    uint16_t width = 0;
    /*!
     * data block height
     */
    uint16_t height = 0;
    /*!
     * data block offset x in the area
     */
    uint16_t offset_x = 0;
    /*!
     * data block offset y in the area
     */
    uint16_t offset_y = 0;
    /*!
     * data index
     */
    uint16_t index = 0;
    /*!
     * data block type, 0:text; 1:picture; 2:time(a special text type);
     * 3:animation; 4:line; 5:rectangle;
     */
    uint8_t type = 0;
    /*!
     * Because of the data size limitation, the detail info can be writen into
     * a temp file and send back the file name.
     */
    char info_file_name[48] = {0};
} am_overlay_data_get_t;

/*! @struct am_overlay_area_s
 *  @brief For Overlay area INIT and GET
 */
/*! @typedef am_overlay_area_t
 *  @brief For Overlay area INIT and GET
 */
typedef struct am_overlay_area_s {
    /*!
     * @sa AM_OVERLAY_BITS
     */
    uint32_t enable_bits = 0;
    /*!
     * area background color
     */
    uint32_t  bg_color = 0;
    /*!
     * area width
     */
    uint16_t width = 0;
    /*!
     * area height
     */
    uint16_t height = 0;
    /*!
     * area offset x in the stream
     */
    uint16_t offset_x = 0;
    /*!
     * area offset y in the stream
     */
    uint16_t offset_y = 0;
    /*!
     * number of data block in the area, used for GET
     */
    int16_t data_num = 0;
    /*!
     * encode stream id
     */
    uint8_t stream_id = 0;
    /*!
     * area state, 0 = disable, 1 = enable, other value = not created
     */
    uint8_t enable = 0;
    /*!
     * area rotate attribute
     */
    uint8_t  rotate = 0;
    /*!
     * number of buffer in the area
     */
    uint8_t buf_num = 0;
    uint8_t reserved0 = 0;
    uint8_t reserved1 = 0;
} am_overlay_area_t;

/*! @} */ /* End of Overlay */

/*! @enum Defines motion detection ROI maximum number
 *
 *  ROI: Region Of Interest\n
 *  Only motions happening in ROI will trigger motion event.
 */
enum AM_MD_ROI_ID
{
  MD_ROI_ID_0 = 0, //!< ROI region 0
  MD_ROI_ID_1 = 1, //!< ROI region 1
  MD_ROI_ID_2 = 2, //!< ROI region 2
  MD_ROI_ID_3 = 3, //!< ROI region 3
  MD_MAX_ROI,      //!< ROI region number
};

/*! @enum Defines motion level
 *  Currently there are 3 levels of motion:\n
 *  - 0: Means no motion
 *  - 1: Small motion
 *  - 2: Big motion
 */
enum AM_MOTION_LEVEL_NUMBER
{
  MOTION_LEVEL_0 = 0, //!< No motion
  MOTION_LEVEL_1 = 1, //!< Small motion
  MOTION_LEVEL_2 = 2, //!< Big motion
  MOTION_LEVEL_NUM    //!< Motion level number
};

/*! @struct am_video_md_threshold_s
 *  This structure describes motion detection related info
 */
struct am_video_md_threshold_s
{
    /*!
     * Threshold of motion level
     */
    uint32_t threshold[MOTION_LEVEL_NUM - 1];

    /*!
     * @sa AM_MD_ROI_ID
     */
    uint32_t roi_id;
};

/*! @struct am_video_md_level_change_delay_s
 *  This structure describes motion level change number
 *
 *  We must make sure that the motion change is surely happened, if a motion
 *  level change happened a number of time described in this data structure,
 *  we can say that the motion level has changed.
 */
struct am_video_md_level_change_delay_s
{
    /*!
     *  Motion level change times
     */
    uint32_t lc_delay[MOTION_LEVEL_NUM - 1];

    /*!
     * @sa AM_MD_ROI_ID
     */
    uint32_t roi_id;
};

/*! @struct am_video_md_roi_s
 *  This structure describes motion detect ROI
 *
 *  We use ROI(Region Of Interest) to specify a rectangle. We focus on the motion
 *  info of this rectangle. Different ROI are independent.
 */
struct am_video_md_roi_s
{
    /*!
     * @sa AM_MD_ROI_ID
     */
    uint32_t roi_id;

    /*!
     * width-offset of left border line
     */
    uint32_t left;

    /*!
     * width-offset of right border line
     */
    uint32_t right;

    /*!
     * height-offset of top border line
     */
    uint32_t top;

    /*!
     * height-offset of bottom border line
     */
    uint32_t bottom;

    /*!
     * - true: this ROI is valid
     * - false: this ROI is invalid
     */
    bool valid;
};

/*! @enum AM_video_MD_CONFIG_BITS
 *
 */
enum AM_VIDEO_MD_CONFIG_BITS
{
  /*! Config enabled */
  AM_VIDEO_MD_CONFIG_ENABLE = 0,

  /*! Config @ref am_video_md_threshold_s */
  AM_VIDEO_MD_CONFIG_THRESHOLD0 = 1,

  /*! Config @ref am_video_md_threshold_s */
  AM_VIDEO_MD_CONFIG_THRESHOLD1 = 2,

  /*! Config @ref am_video_md_level_change_delay_s */
  AM_VIDEO_MD_CONFIG_LEVEL0_CHANGE_DELAY = 3,

  /*! Config @ref am_video_md_level_change_delay_s */
  AM_VIDEO_MD_CONFIG_LEVEL1_CHANGE_DELAY = 4,

  /*! Config buffer ID */
  AM_VIDEO_MD_CONFIG_BUFFER_ID = 5,

  /*! Config buffer type */
  AM_VIDEO_MD_CONFIG_BUFFER_TYPE = 6,

  /*! Config @ref am_video_md_roi_s */
  AM_VIDEO_MD_CONFIG_ROI = 7,

  /*! Save current config */
  AM_VIDEO_MD_SAVE_CURRENT_CONFIG = 8,
};

/*! @struct am_video_md_config_s
 *  This data structure defines motion detection related config
 *
 *  @sa AM_IPC_MW_CMD_VIDEO_MOTION_DETECT_CONFIG_SET
 *  @sa AM_IPC_MW_CMD_VIDEO_MOTION_DETECT_CONFIG_GET
 */
struct am_video_md_config_s
{
    /*
     * @sa am_video_md_roi_s
     */
    am_video_md_roi_s roi;

    /*!
     * @sa am_video_md_threshold_s
     */
    am_video_md_threshold_s threshold;

    /*!
     * @sa am_video_md_level_change_delay_s
     */
    am_video_md_level_change_delay_s level_change_delay;

    /*!
     * buffer ID
     * - 0: main buffer
     * - 1: second buffer
     * - 2: third buffer
     * - 3: fourth buffer
     */
    uint32_t buffer_id;

    /*!
     * buffer type
     * - 1: YUV buffer
     * - 3: ME0 buffer
     * - 4: ME1 buffer
     */
    uint32_t buffer_type;

    /*!
     * @sa AM_VIDEO_MD_CONFIG_BITS
     */
    uint32_t enable_bits;

    /*!
     * - true: enable motion detection
     * - false: disable motion detection
     */
    bool enable;
};
/*! @} */


/******************************************************************************/
/*! @} */ /* End of defgroup airapi-video */

#endif /* _AM_API_VIDEO_H_ */
