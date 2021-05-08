/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv_vpu.h
 * @brief    header file for VPU driver
 * @version  V1.6
 * @date     27. May 2019
 * @model    vpu
 ******************************************************************************/

#ifndef _DRV_VPU_H_
#define _DRV_VPU_H_

#include <stdint.h>
#include <io.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for vpu device handle.
typedef void *vpu_handle_t;

/// definition for vpu instance handle.
typedef void *vpu_instance_t;

/* vpu_inst_config_t is a composite struct pointer, different instance type
 * using different private config struct, for example:
 * when type=VPU_INSTANCE_TYPE_ENCODER_H264: vpu_h264enc_config_t*
 */
typedef void *vpu_inst_config_t;

/****** VPU events *****/
typedef enum {
    VPU_EVENT_ENCODE_DONE  = 0, ///< encode finished
    VPU_EVENT_ENCODE_ERROR,     ///< encode error
} vpu_event_e;

/****** VPU instance types *****/
typedef enum {
    VPU_INSTANCE_TYPE_ENCODER_H264 = 0, ///< VPU instance type: H264 encoder
    VPU_INSTANCE_TYPE_DECODER_H264,     ///< VPU instance type: H264 decoder
} vpu_inst_type_e;

/****** VPU specific error number *****/
typedef enum {
    VPU_ERROR_MEMORY_ALLOC = (DRV_ERROR_SPECIFIC + 1),
    VPU_ERROR_CONFIG_PARAMETER,
} vpu_errno_e;

/****** H264 encode level ******/
typedef enum {
    VPU_H264ENC_LEVEL_1   = 10,   /* QCIF  (176x144)   75kbits */
    VPU_H264ENC_LEVEL_1_b = 99,
    VPU_H264ENC_LEVEL_1_1 = 11,
    VPU_H264ENC_LEVEL_1_2 = 12,
    VPU_H264ENC_LEVEL_1_3 = 13,
    VPU_H264ENC_LEVEL_2   = 20,   /* CIF   (352x288)   2.4Mbits\n" */
    VPU_H264ENC_LEVEL_2_1 = 21,
    VPU_H264ENC_LEVEL_2_2 = 22,
    VPU_H264ENC_LEVEL_3   = 30,   /* SD    (720x576)   12Mbits */
    VPU_H264ENC_LEVEL_3_1 = 31,
    VPU_H264ENC_LEVEL_3_2 = 32,
    VPU_H264ENC_LEVEL_4   = 40,   /* 1080p (1920x1080) 24Mbits */
    VPU_H264ENC_LEVEL_4_1 = 41,
    VPU_H264ENC_LEVEL_4_2 = 42,
    VPU_H264ENC_LEVEL_5   = 50,
    VPU_H264ENC_LEVEL_5_1 = 51
} vpu_h264enc_level_e;

/****** VPU input color format ******/
typedef enum {
    VPU_INPUT_FORMAT_YUV420_PLANAR = 0, ///< YYYYYYYY..UU..VV..
    VPU_INPUT_FORMAT_YUV420_SEMIPLANAR, ///< YYYYYYYY..UVUV..
    VPU_INPUT_FORMAT_YUYV422,           ///< YUYVYUYV..
    VPU_INPUT_FORMAT_UYVY422,           ///< UYVYUYVY..

    VPU_INPUT_FORMAT_RGB565,
    VPU_INPUT_FORMAT_BGR565,
    VPU_INPUT_FORMAT_RGB555,
    VPU_INPUT_FORMAT_BGR555,
    VPU_INPUT_FORMAT_RGB444,
    VPU_INPUT_FORMAT_BGR444,
    VPU_INPUT_FORMAT_RGB888,
    VPU_INPUT_FORMAT_BGR888,
    VPU_INPUT_FORMAT_RGB101010,
    VPU_INPUT_FORMAT_BGR101010,
} vpu_input_format_e;

/****** VPU encode output frame type ******/
typedef enum {
    VPU_FRAME_TYPE_IFRAME,      /* I-Frame */
    VPU_FRAME_TYPE_PFRAME,      /* P-Frame */
} vpu_frame_type_e;

/**
  \brief H264 encoder configure parameters
*/
typedef struct {
    uint32_t input_width;       ///< input picture width
    uint32_t input_height;      ///< input picture height
    vpu_input_format_e format;  ///< input picture format

    uint32_t output_width;      ///< output picture width
    uint32_t output_height;     ///< output picture height
    vpu_h264enc_level_e level;  ///< encode encode level
    uint32_t framerate_numer;   ///< encode frame rate's numer
    uint32_t framerate_denom;   ///< encode frame rate's denom
    uint32_t gop_len;           ///< encode group of picture length
} vpu_h264enc_config_t;

typedef struct {
    uint8_t          *input_addr;   ///< input data address info
    vpu_frame_type_e  frame_type;   ///< encode frame type
} vpu_picture_t;

typedef struct {
    uint8_t          *addr;  ///< output stream buffer address
    uint32_t          size;  ///< Output stream buffer size
} vpu_es_frame_t;

///< Pointer to \ref vpu_event_cb_t : VPU Event call back.
typedef void (*vpu_event_cb_t)(vpu_instance_t vpu_inst, vpu_event_e event, void *user_data);

/**
  \brief       Initialize VPU Interface.
                Initialize the resources needed for the VPU interface
  \param[in]   idx  vpu index
  \param[in]   cb_event  event call back function \ref vpu_event_cb_t
  \return      pointer to vpu instance
*/
vpu_handle_t csi_vpu_initialize(int32_t idx, vpu_event_cb_t cb_event, void *user_data);

/**
  \brief       De-initialize VPU Interface.
                Stop operation and release the resources used by the interface
  \param[in]   handle  vpu handle to operate.
  \return      error code
*/
int32_t csi_vpu_uninitialize(vpu_handle_t handle);

/**
  \brief       Create vpu decoder/encoder instance with configuration parameter
  \param[in]   handle  vpu handle to operate.
  \param[in]   type    The instance type
  \param[in]   config  The instance configuration parameter.
                         For H264 encoder, config should be vpu_h264enc_config_t
                         For JPEG encoder, config should be vpu_jpeg_config_t
  \return      error code
*/
vpu_instance_t csi_vpu_create_instance(vpu_handle_t handle,
                                       vpu_inst_type_e type,
                                       vpu_inst_config_t config);

/**
  \brief       Destory vpu decoder/encoder instance
  \param[in]   type  The instance type
  \return      error code
*/
int32_t csi_vpu_destory_instance(vpu_instance_t vpu_inst);

/**
  \brief       Get vpu instance configuration info
  \param[in]   vpu_inst The VPU instance handle
  \param[out]  type     The instance type
  \param[out]  config   The instance configuration parameter.
                         For H264 encoder, config should be vpu_h264enc_config_t
                         For JPEG encoder, config should be vpu_jpeg_config_t
  \return      error code
*/
int32_t csi_vpu_get_config(vpu_instance_t vpu_inst,
                           vpu_inst_type_e *type,
                           vpu_inst_config_t **config);

/**
  \brief       start H264 encoder and get ES stream header
  \param[in]   vpu_inst The VPU instance handle
  \param[out]  output   The pointer to output data information
  \return      error code
*/
int32_t csi_vpu_h264enc_start(vpu_instance_t vpu_inst,
                              vpu_es_frame_t *output);
/**
  \brief       Encode pictures
  \param[in]   vpu_inst The VPU instance handle
  \param[in]   input    The pointer to input data information
  \return      error code
*/
int32_t csi_vpu_h264enc_encode(vpu_instance_t vpu_inst,
                               vpu_picture_t *input);

/**
  \brief       Get encoded output stream
  \param[in]   vpu_inst The VPU instance handle
  \param[out]  input     The pointer to input data information
  \param[out]  output    The pointer to output data information
  \return      error code
*/
int32_t csi_vpu_h264enc_get_stream(vpu_instance_t vpu_inst,
                                   vpu_picture_t **input,
                                   vpu_es_frame_t *output);

/**
  \brief       stop H264 encoder and get ES stream tail
  \param[in]   vpu_inst The VPU instance handle
  \param[out]  output   The pointer to output data information
  \return      error code
*/
int32_t csi_vpu_h264enc_stop(vpu_instance_t vpu_inst,
                             vpu_es_frame_t *output);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_VPU_H_ */
