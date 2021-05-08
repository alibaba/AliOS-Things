/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_mailbox.h
 * @brief    header file for mailbox driver
 * @version  V1.0
 * @date     28. Jan 2019
 * @model    mailbox
 ******************************************************************************/

#ifndef _CSI_MAILBOX_H_
#define _CSI_MAILBOX_H_


#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int count;
    struct {
        uint32_t  mailbox_id;
        uint32_t  tx_chnl;
        uint32_t  rx_chnl;
     } config[];
} mailbox_chnl_config_t;

/// definition for mailbox handle.
typedef void *mailbox_handle_t;

/****** MAILBOX Event *****/
typedef enum {
    MAILBOX_EVENT_SEND_COMPLETE       = 0,  ///< Send completed; however mailbox may still transmit data
    MAILBOX_EVENT_RECEIVED            = 1, ///< Data Received, only in mailbox buf, call memcpy() get the data
} mailbox_event_e;

typedef void (*mailbox_event_cb_t)(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event);   ///< Pointer to \ref mailbox_event_cb_t : MAILBOX Event call back.

/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t csi_mailbox_initialize(mailbox_event_cb_t cb_event);

/**
  \brief       De-initialize MAILBOX Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t csi_mailbox_uninitialize(mailbox_handle_t handle);

/**
  \brief       Start sending data to MAILBOX transmitter,(received data is ignored).
               This function is non-blocking,\ref mailbox_event_e is signaled when operation completes or error happens.
               \ref csi_mailbox_get_status can get operation status.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   mailbox_id index of dest mailbox
  \param[in]   data  Pointer to buffer with data to send to MAILBOX transmitter.
  \param[in]   num Number of data items to send
  \return      sent number of data
*/
int32_t csi_mailbox_send(mailbox_handle_t handle, int32_t mailbox_id, const void *data, uint32_t num);

/**
  \brief       Start Receiving data from Mailbox receiver.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   mailbox_id index of dest mailbox
  \param[out]  data  Pointer to buffer with data to receive from mailbox.
  \param[in]   num   Number of data items to receive
  \return      received number or  error code
*/
int32_t csi_mailbox_receive(mailbox_handle_t handle, int32_t mailbox_id, void *data, uint32_t num);

/**
  \brief       enable mailbox ch irq
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
int32_t csi_mailbox_chnl_enable(mailbox_handle_t handle, int32_t mailbox_id);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_MAILBOX_H_ */
