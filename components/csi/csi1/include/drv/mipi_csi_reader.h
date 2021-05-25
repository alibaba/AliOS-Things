/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv_mipi_csi_reader.h
 * @brief    header file for mipi csi reader
 * @version  V1.4
 * @date     27. May 2019
 ******************************************************************************/

#ifndef _DRV_MIPI_CSI_READER_H_
#define _DRV_MIPI_CSI_READER_H_

#include <drv/common.h>
#include <drv/mipi_csi.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for mipi_csi_reader handle.
typedef void *mipi_csi_reader_handle_t;

/****** MIPI CSI READER Event *****/
typedef enum {
    MIPI_CSI_READER_EVENT_RECEIVE_ERROR  = 0, ///< Receive data error
    MIPI_CSI_READER_EVENT_FRAME_READY    = 1, ///< Receive data ready
} mipi_csi_reader_event_e;

typedef enum {
    MIPI_CSI_READER_NO_ERROR        = 0,    ///< No error
    MIPI_CSI_READER_ERROR_CRC       = 1,    ///< occur CRC error
    MIPI_CSI_READER_ERROR_PIXEL_NUM = 2,    ///< occur pixel number
} mipi_csi_reader_error_e;

typedef struct {
    mipi_csi_data_format_e format;       ///< frame format
    uint32_t               height;       ///< frame height
    uint32_t               width;        ///< frame width
    uint8_t                **frame_list; ///< output frame address list
    uint8_t                frame_count;  ///< frame count
    uint8_t                mipi_id;      ///< the mipi id to connect
} mipi_csi_reader_config_t;

typedef struct {
    uint32_t               index;  ///< frame index
    uint8_t                *addr;  ///< frame address
    mipi_csi_data_format_e format; ///< frame format
    uint32_t               height; ///< frame height
    uint32_t               width;  ///< frame width
    uint32_t               size;   ///< frame size
} mipi_csi_reader_frame_t;

typedef void (*mipi_csi_reader_event_cb_t)(mipi_csi_reader_handle_t handle, mipi_csi_reader_event_e event, void *user_data); ///< Pointer to \ref mipi_csi_reader_event_cb_t : MIPI_CSI_READER Event call back.

/**
  \brief       Initialize mipi csi reader Interface. 1. Initializes the resources needed for the mipi csi reader interface 2.registers event callback function
  \param[in]   idx mipi_csi_reader index
  \param[in]   cb_event  event call back function \ref mipi_csi_reader_event_cb_t
  \return      return mipi csi reader handle if success
*/
mipi_csi_reader_handle_t csi_mipi_csi_reader_initialize(int32_t idx, mipi_csi_reader_event_cb_t cb_event, void *user_data);

/**
  \brief       De-initialize mipi csi reader Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mipi csi reader handle to operate.
  \return      error code
*/
int32_t csi_mipi_csi_reader_uninitialize(mipi_csi_reader_handle_t handle);

/**
  \brief       Config the mipi csi reader and start to work
  \param[in]   handle  mipi csi reader handle to operate.
  \param[in]   input   the config of mipi csi reader.
  \return      error code
*/
int32_t csi_mipi_csi_reader_start(mipi_csi_reader_handle_t handle, mipi_csi_reader_config_t *input);

/**
  \brief       Stop the mipi csi reader
  \param[in]   handle  mipi csi reader handle to operate.
  \return      error code
*/
int32_t csi_mipi_csi_reader_stop(mipi_csi_reader_handle_t handle);

/**
  \brief       Hold the specific frame.
  \param[in]   handle  mipi csi reader handle to operate.
  \param[in]   frame  the frame to hold.
  \return      error code.
*/
int32_t csi_mipi_csi_reader_hold_frame(mipi_csi_reader_handle_t handle, uint8_t *frame);

/**
  \brief       Release the specific frame.
  \param[in]   handle  mipi csi reader handle to operate.
  \param[in]   frame  the frame to release.
  \return      error code.
*/
int32_t csi_mipi_csi_reader_release_frame(mipi_csi_reader_handle_t handle, uint8_t *frame);

/**
  \brief       Get the frame idx when frame ready.
  \param[in]   handle  mipi csi reader handle to operate.
  \param[out]  frame  the ready frame.
  \return      error code.
*/
int32_t csi_mipi_csi_reader_get_frame(mipi_csi_reader_handle_t handle, mipi_csi_reader_frame_t *frame);

/**
  \brief       When get frame error, output the error type.
  \param[in]   handle  mipi csi reader handle to operate.
  \param[out]  error  output the error type.
  \param[out]  frame  output the error frame address, when *fram is NULL, it represent unkonw frame.
  \return      error code.
*/
int32_t csi_mipi_csi_reader_get_error_info(mipi_csi_reader_handle_t handle, mipi_csi_reader_error_e *error, uint8_t **frame);
#ifdef __cplusplus
}
#endif

#endif /* _DRV_MIPI_CSI_READER_H_ */
