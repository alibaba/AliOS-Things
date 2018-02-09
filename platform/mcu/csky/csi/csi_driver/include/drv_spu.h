/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     drv_spu.h
 * @brief    header file for spu driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CSI_spu_H_
#define _CSI_spu_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drv_common.h>

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
    uint32_t spu_usart        :1;  ///< supports usart device
    uint32_t spu_spi          :1;  ///< spuports spi device
    uint32_t spu_i2c          :1;  ///< supports i2c device
    uint32_t spu_can          :1;  ///< supports can device
} spu_capabilities_t;

/**
  \brief       Get driver capabilities.
  \param[in]   handle  spu handle to operate.
  \return      \ref spu_capabilities_t
*/
spu_capabilities_t csi_spu_get_capabilities(spu_handle_t handle);

/**
  \brief       Initialize spu Interface. Initializes the resources needed for the spu interface 
  \param[in]   pgpio[] spu pin
  \param[in]   num spu pin number, a maximum of 32
  \return      return spu handle if success
*/
spu_handle_t csi_spu_initialize(int32_t pgpio[]);

/**
  \brief       De-initialize spu Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spu handle to operate.
  \return      error code
*/
int32_t csi_spu_uninitialize(spu_handle_t handle);

/**
  \brief       config spu mode.
  \param[in]   handle  spu handle to operate.
  \param[in]   capabilities type of device that SPU supports(uart/spi/i2c/can)
  \return      error code
*/
int32_t csi_spu_config(spu_handle_t handle,spu_capabilities_t capabilities);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_spu_H_ */
