/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_etb.h
 * @brief    header file for event trigger driver
 * @version  V1.0
 * @date     27. octorber 2017
 * @model    etb
 ******************************************************************************/

#ifndef _CSI_ETB_H_
#define _CSI_ETB_H_


#include <stdint.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    ETB_SOURCE = 0,
    ETB_DEST   = 1
}
etb_direct_e;

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
  \brief       Get driver capabilities.
  \param[in]   idx       etb index.
  \return      \ref etb_capabilities_t
*/
etb_capabilities_t drv_etb_get_capabilities(int32_t idx);

/**
  \brief       config etb channel.
  \param[in]   source_lo    a specific number represent a location in an source trigger location map to trigger other ip(s).
  \param[in]   dest_lo      a specific number represent an location in an dest trigger map to wait signal(s) from source ip(s) or location(s).
  \param[in]   source_type  \ref etb_source_type_e the input source is hardware trigger or software trigger.
  \param[in]   mode         \ref etb_channel_func_e channel function.
  \return      channel nubmber or error code (negative).
*/
int32_t drv_etb_channel_config(uint32_t source_ip, uint32_t dest_ip, etb_source_type_e source_type, etb_channel_func_e mode);

/**
  \brief       start etb.
  \param[in]   channel   etb channel number to operate.
  \return      none
*/
void drv_etb_start(int32_t channel);

/**
  \brief       stop etb.
  \param[in]   channel   etb channel number to operate.
  \return      none
*/
void drv_etb_stop(int32_t channel);

/**
  \brief       Get ETB status.
  \param[in]   ch etb channel to operate.
  \return      ETB status \ref etb_status_t
*/
etb_status_t drv_etb_get_status(int32_t channel);

#endif /* _CSI_ETB_H_ */
