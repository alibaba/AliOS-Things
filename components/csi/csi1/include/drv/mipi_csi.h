/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv_mipi_csi.h
 * @brief    header file for mipi_csi driver
 * @version  V1.2
 * @date     27. May 2019
 ******************************************************************************/

#ifndef _DRV_MIPI_CSI_H_
#define _DRV_MIPI_CSI_H_

#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for mipi csi handle.
typedef void *mipi_csi_handle_t;

/**** IPI Data Types ****/
typedef enum {
    MIPI_YUV420_8           = 0x18,
    MIPI_YUV420_10          = 0x19,
    MIPI_YUV420_8_LEG       = 0x1A,
    MIPI_YUV420_8_SHIFT     = 0x1C,
    MIPI_YUV420_10_SHIFT    = 0x1D,
    MIPI_YUV422_8           = 0x1E,
    MIPI_YUV422_10          = 0x1F,
    MIPI_RGB444             = 0x20,
    MIPI_RGB555             = 0x21,
    MIPI_RGB565             = 0x22,
    MIPI_RGB666             = 0x23,
    MIPI_RGB888             = 0x24,
    MIPI_RAW6               = 0x28,
    MIPI_RAW7               = 0x29,
    MIPI_RAW8               = 0x2A,
    MIPI_RAW10              = 0x2B,
    MIPI_RAW12              = 0x2C,
    MIPI_RAW14              = 0x2D,
} mipi_csi_data_format_e;

/**
\brief mipi config structure
*/
typedef struct {
    mipi_csi_data_format_e  data_format;   ///< Output format
    uint16_t                lane_count;    ///< the count of lane
} mipi_csi_config_t;

/****** MIPI Event *****/
typedef enum {
    MIPI_CSI_EVENT_RECEIVED_ERROR   = 0,  ///< Receive data error.
} mipi_csi_event_e;

typedef void (*mipi_csi_event_cb_t)(mipi_csi_handle_t handle, mipi_csi_event_e event, void *user_data); //< Pointer to \ref mipi_event_cb_t : MIPI Event call back.

/**
  \brief       Initialize MIPI CSI Interface. 1. Initializes the resources needed for the MIPI interface 2.registers event callback function
  \param[in]   idx  mipi index
  \param[in]   cb_event  event call back function \ref mipi_event_cb_t
  \return      return mipi handle if success
*/
mipi_csi_handle_t csi_mipi_csi_initialize(int32_t idx, mipi_csi_event_cb_t cb_event, void *user_data);

/**
  \brief       De-initialize MIPI CSI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mipi handle to operate.
  \return      error code
*/
int32_t csi_mipi_csi_uninitialize(mipi_csi_handle_t handle);

/**
  \brief       Start the mipi csi.
  \param[in]   handle  mipi csi handle to operate.
  \param[in]   config  Pointer of mipi_config_t with data to set to MIPI conteroller.
  \return      error code
*/
int32_t csi_mipi_csi_start(mipi_csi_handle_t handle, mipi_csi_config_t *config);

/**
  \brief       Stop the mipi.
  \param[in]   handle  mipi handle to operate.
  \return      error code
*/
int32_t csi_mipi_csi_stop(mipi_csi_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_MIPI_CSI_H_ */
