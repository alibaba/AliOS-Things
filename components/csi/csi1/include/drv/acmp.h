/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     drv_acmp.h
 * @brief    Header File for acmp Driver
 * @version  V1.0
 * @date     02. June 2017
 * @model    acmp
 ******************************************************************************/
#ifndef _CSI_ACMP_H_
#define _CSI_ACMP_H_


#include <stdint.h>
#include <drv/errno.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/****** acmp specific error codes *****/
#define ACMP_ERROR_MODE                (DRV_ERROR_SPECIFIC + 1)     ///< Specified Mode not supported

/// definition for acmp handle.
typedef void *acmp_handle_t;

/*----- acmp Control Codes: Mode -----*/
typedef enum {
    ACMP_MODE_FAST                   = 0,   ///< fast mode
    ACMP_MODE_SLOW,                         ///< slow mode
} acmp_mode_e;

/**
\brief acmp Status
*/
typedef struct {
    uint32_t output_state               : 1;        ///< Analog comparator outputs 0 or 1
} acmp_status_t;

/**
\brief acmp Device Driver Capabilities.
*/
typedef struct {
    uint32_t fast_mode      : 1;      ///< supports fast mode
    uint32_t slow_mode      : 1;      ///< supports slow mode
    uint32_t hysteresis_fun : 1;      ///< supports Programming Hysteresis
} acmp_capabilities_t;


/**
  \brief       Initialize acmp Interface. 1. Initializes the resources needed for the acmp interface 2.registers event callback function
  \param[in]   idx  device id
  \param[in]   cb_event event callback function \ref acmp_event_cb_t
  \return      return acmp handle if success
*/
acmp_handle_t drv_acmp_initialize(int32_t idx);

/**
  \brief       De-initialize acmp Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_uninitialize(acmp_handle_t handle);

/**
  \brief       control acmp power.
  \param[in]   handle  acmp handle to operate.
  \param[in]   state   power state.\ref drv_power_stat_e.
  \return      error code
*/
int32_t drv_acmp_power_control(acmp_handle_t handle, csi_power_stat_e state);

/**
  \brief       Get driver capabilities.
  \param[in]   idx  device id
  \return      \ref acmp_capabilities_t
*/
acmp_capabilities_t drv_acmp_get_capabilities(int32_t idx);

/**
  \brief       config acmp mode.
  \param[in]   handle  acmp handle to operate.
  \param[in]   mode      \ref acmp_mode_e
  \param[in]   standard  \ref acmp_standard_acmp_e
  \return      error code
*/
int32_t drv_acmp_config(acmp_handle_t handle, acmp_mode_e mode);

/**
  \brief       start comparison
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_comp_start(acmp_handle_t handle);

/**
  \brief       stop comparison.
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_comp_stop(acmp_handle_t handle);

/**
  \brief       Get acmp status.
  \param[in]   handle  acmp handle to operate.
  \return      acmp status \ref acmp_status_t
*/
acmp_status_t drv_acmp_get_status(acmp_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_acmp_H_ */

