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
 * @file     drv_etb.h
 * @brief    header file for event trigger driver
 * @version  V1.0
 * @date     27. octorber 2017
 ******************************************************************************/

#ifndef _CSI_ETB_H_
#define _CSI_ETB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <soc.h>

typedef enum {
    ETB_SOURCE = 0,
    ETB_DEST   = 1
} etb_direct_e;

typedef enum {
    ETB_HARDWARE  = 0,   ///< etb channel inout is hardware trigger.
    ETB_SOFTWWARE       ///< etb channel inout is software trigger.
} etb_source_type_e;

typedef enum {
    ETB_ONE_TRIGGER_ONE = 0,
    ETB_ONE_TRIGGER_MORE = 1,
    ETB_MORE_TRIGGER_ONE = 2
} etb_channel_func_e;

/// definition for etb handle.
typedef void *etb_handle_t;

typedef struct {
    uint32_t busy : 1;
} etb_status_t;

typedef enum {
    ETB_EVENT_CHANNEL_BUSY = 0,   ///<
    ETB_EVENT_MODE_FAULT          ///<
} etb_event_e;

/**
\brief ETB Driver Capabilities.
*/
typedef struct {
    uint32_t sync_trigger : 1;
    uint32_t async_trigger : 1;
    uint32_t etb_31_channel : 1;
    uint32_t one_trigger_one : 1;
    uint32_t one_trigger_more : 1;
    uint32_t more_trigger_one : 1;
} etb_capabilities_t;
 
typedef void (*etb_event_cb_t)(int32_t idx, etb_event_e event);  ///< Pointer to \ref etb_event_cb_t : etb Event call back.

/**
  \brief       Initialize etb Interface. 1. Initializes the resources needed for the etb interface 2.registers event callback function.
  \param[in]   idx       etb index.
  \param[in]   cb_event  event call back function \ref etb_event_cb_t
  \return      return etb handle if success
*/
etb_handle_t csi_etb_initialize(int32_t idx, etb_event_cb_t cb_event);

/**
  \brief       De-initialize etb Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle   etb handle to operate.
  \return      error code
*/
int32_t csi_etb_uninitialize(etb_handle_t handle);

/**
  \brief       Get driver capabilities.
  \param[in]   idx       etb index.
  \return      \ref etb_capabilities_t
*/
etb_capabilities_t csi_etb_get_capabilities(int32_t idx);

/**
  \brief       config etb channel.
  \param[in]   handle       etb handle to operate.
  \param[in]   source_lo    a specific number represent a location in an source trigger location map to trigger other ip(s).
  \param[in]   dest_lo      a specific number represent an location in an dest trigger map to wait signal(s) from source ip(s) or location(s).
  \param[in]   source_type  \ref etb_source_type_e the input source is hardware trigger or software trigger.
  \param[in]   mode         \ref etb_channel_func_e channel function.
  \return      channel nubmber or error code (negative).
*/
int32_t csi_etb_channel_config(etb_handle_t  handle, uint32_t source_ip, uint32_t dest_ip, etb_source_type_e source_type, etb_channel_func_e mode);

/**
  \brief       start etb.
  \param[in]   handle    etb handle to operate.
  \param[in]   channel   etb channel number to operate.
  \return      error code
*/
int32_t csi_etb_start(etb_handle_t  handle, int32_t channel);

/**
  \brief       stop etb.
  \param[in]   handle    etb handle to operate.
  \param[in]   channel   etb channel number to operate.
  \return      error code
*/
int32_t csi_etb_stop(etb_handle_t  handle, int32_t channel);

/**
  \brief       Get ETB status.
  \param[in]   handle etb handle to operate.
  \return      ETB status \ref etb_status_t
*/
etb_status_t csi_etb_get_status(etb_handle_t handle);


#endif /* _CSI_ETB_H_ */
