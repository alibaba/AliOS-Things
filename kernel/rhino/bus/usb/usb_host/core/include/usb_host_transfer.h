/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_TRANSFER_H
#define __USB_HOST_TRANSFER_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief USB Host Control Transfer
 * 
 * @param[in]      phandle pointer to the usb host handler
 * @param[in]      setup   pointer to the setup packet
 * @param[in/out]  buff    pointer to the data buffer which will send or recv
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_ctrl_transfer(usb_host_t *phandle, usb_setup_pkt_t *setup, 
                          uint8_t *buff);

/**
 * @brief USB Host Bulk Transfer
 * 
 * @param[in]      phandle pointer to the usb host handler
 * @param[in]      pipe    pointer to the pipe (endpoint)
 * @param[in/out]  buff    pointer to the data buffer which will send or recv
 * @param[in]      length  the length of the data buffer
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_bulk_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length);

/**
 * @brief USB Host Intrrupt Transfer
 * 
 * @param[in]      phandle pointer to the usb host handler
 * @param[in]      pipe    pointer to the pipe (endpoint)
 * @param[in/out]  buff    pointer to the data buffer which will send or recv
 * @param[in]      length  the length of the data buffer
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_intr_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length);

/**
 * @brief USB Host Isoc Transfer
 * 
 * @param[in]      phandle pointer to the usb host handler
 * @param[in]      pipe    pointer to the pipe (endpoint)
 * @param[in/out]  buff    pointer to the data buffer which will send or recv
 * @param[in]      length  the length of the data buffer
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_isoc_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_TRANSFER_H */