/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_spu.h
 * @brief    header file for spu driver
 * @version  V1.0
 * @date     02. June 2017
 * @model    spu
 ******************************************************************************/

#ifndef _CSI_spu_H_
#define _CSI_spu_H_


#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif
/// definition for spu handle.
typedef void *spu_handle_t;

/****** spu specific error codes *****/
typedef enum {
    EDRV_SPU_INST_OVERFLOW  = (DRV_ERROR_SPECIFIC + 1), ///< INST_RAM size not enough
    EDRV_SPU_PROG_OVERFLOW,  ///<PROG_RAM size not enough
} drv_spu_error_e;

/**
\brief spu Driver Capabilities.
*/
typedef struct  {
    uint32_t spu_usart        : 1; ///< supports usart device
    uint32_t spu_spi          : 1; ///< spuports spi device
    uint32_t spu_iic          : 1; ///< supports iic device
    uint32_t spu_can          : 1; ///< supports can device
    uint32_t spu_i2s          : 1; ///< supports i2s device
} spu_capabilities_t;

/**
  \brief       Get driver capabilities.
  \param[in]   handle  spu handle to operate.
  \return      \ref spu_capabilities_t
*/
spu_capabilities_t drv_spu_get_capabilities(spu_handle_t handle);

/**
  \brief       Initialize spu Interface. Initializes the resources needed for the spu interface
  \param[in]   idx spu index
  \param[in]   num spu pin number, a maximum of 32
  \return      return spu handle if success
*/
spu_handle_t drv_spu_initialize(int32_t idx);

/**
  \brief       De-initialize spu Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spu handle to operate.
  \return      error code
*/
int32_t drv_spu_uninitialize(spu_handle_t handle);

/**
  \brief       config spu mode.
  \param[in]   handle  spu handle to operate.
  \param[in]   capabilities type of device that SPU supports(uart/spi/i2c/can)
  \return      error code
*/
int32_t drv_spu_config(spu_handle_t handle, spu_capabilities_t capabilities);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_spu_H_ */
