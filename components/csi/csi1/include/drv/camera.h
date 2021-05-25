/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv_camera.h
 * @brief    header file for camera driver
 * @version  V1.2
 * @date     27. May 2019
 * @model    camera
 ******************************************************************************/
#ifndef _DRV_CAMERA_H_
#define _DRV_CAMERA_H_

#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for camera handle.
typedef void *camera_handle_t;

/**** Camera Data Types ****/
typedef enum {
    CAMERA_DATA_FORMAT_YUV420_8 = 0,
    CAMERA_DATA_FORMAT_YUV420_10,
    CAMERA_DATA_FORMAT_YUV420_8_LEG,
    CAMERA_DATA_FORMAT_YUV420_8_SHIFT,
    CAMERA_DATA_FORMAT_YUV420_10_SHIFT,
    CAMERA_DATA_FORMAT_YUV422_8,
    CAMERA_DATA_FORMAT_YUV422_10,
    CAMERA_DATA_FORMAT_RGB444,
    CAMERA_DATA_FORMAT_RGB555,
    CAMERA_DATA_FORMAT_RGB565,
    CAMERA_DATA_FORMAT_RGB666,
    CAMERA_DATA_FORMAT_RGB888,
    CAMERA_DATA_FORMAT_RAW6,
    CAMERA_DATA_FORMAT_RAW7,
    CAMERA_DATA_FORMAT_RAW8,
    CAMERA_DATA_FORMAT_RAW10,
    CAMERA_DATA_FORMAT_RAW12,
    CAMERA_DATA_FORMAT_RAW14,
} camera_data_format_e;

/**** Camera work mode ****/
typedef enum {
    CAMERA_OUTPUT_MODE_MIPI = 0,
    CAMERA_OUTPUT_MODE_DVP,
    CAMERA_OUTPUT_MODE_FPD,
    CAMERA_OUTPUT_MODE_GMSL,
} camera_output_mode_e;

/**
\brief Camera config structure
*/
typedef struct {
    camera_data_format_e    format; ///< output image format
    camera_output_mode_e    mode;   ///< camera work mode
    uint32_t                width;  ///< output image width
    uint32_t                height; ///< output image height
} camera_config_t;

/****** CAMERA Event *****/
typedef enum {
    CAMERA_EVENT_READY = 0, ///< camera Ready
    CAMERA_EVENT_ERROR,     ///< Read/Write camera Error
} camera_event_e;

typedef void (*camera_event_cb_t)(camera_handle_t handle, camera_event_e event, void *user_data);   ///< Pointer to \ref camera_event_cb_t : CAMERA Event call back.

// Function documentation

/**
  \brief       Initialize CAMERA Interface. 1. Initializes the resources needed for the CAMERA interface 2.registers event callback function
  \param[in]   idx  device id
  \param[in]   cb_event  Pointer to \ref camera_event_cb_t
  \return      pointer to camera handle
*/
camera_handle_t csi_camera_initialize(int32_t idx, camera_event_cb_t cb_event, void *user_data);

/**
  \brief       De-initialize CAMERA Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  camera handle to operate.
  \return      error code
*/
int32_t csi_camera_uninitialize(camera_handle_t handle);

/**
  \brief       Start and config the camera.
  \param[in]   handle  camera handle to operate.
  \param[in]   config  the config structure of camera
  \return      error code
*/
int32_t csi_camera_start(camera_handle_t handle, camera_config_t *config);

/**
  \brief       Stop the camera.
  \param[in]   handle  camera handle to operate.
  \return      error code
*/
int32_t csi_camera_stop(camera_handle_t handle);
#ifdef __cplusplus
}
#endif

#endif /* _DRV_CAMERA_H_ */
