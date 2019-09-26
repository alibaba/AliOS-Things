/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_REQUEST_H
#define __USB_HOST_REQUEST_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Sending Clear Feature Request
 * 
 * @param[in]  phandle pointer to the usb host handler
 * @param[in]  pipe    the index of the pipe (endpoint)
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_req_clear_feature(usb_host_t *phandle, uint8_t pipe);

/**
 * @brief Sending Get Descriptor Request
 * 
 * @param[in]   phandle pointer to the usb host handler
 * @param[in]   type    the type of the descriptor
 * @param[in]   length  the length of the descriptor
 * @param[out]  buff    pointer to the buffer which will store the descriptor
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_req_get_descriptor(usb_host_t *phandle, uint16_t type,
                            uint8_t length, uint8_t *buff);

/**
 * @brief Sending Set Device Address Request
 * 
 * @param[in]  phandle pointer to the usb host handler
 * @param[in]  address the address of the connected device
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_req_set_address(usb_host_t *phandle, uint16_t address);

/**
 * @brief Sending Set Device Configuration Request
 * 
 * @param[in]  phandle pointer to the usb host handler
 * @param[in]  conf    the index of the configuration
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_req_set_configuration(usb_host_t *phandle, uint16_t conf);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_REQUEST_H */