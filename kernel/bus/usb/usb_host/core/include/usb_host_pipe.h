/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_PIPE_H
#define __USB_HOST_PIPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Allocate a PIPE (Endpoint)
 * 
 * @param[in]  phandle pointer to the usb host handler
 * 
 * @return the pointer of the pipe, NULL is failed
 */
usbh_pipe_t *usbh_pipe_alloc(usb_host_t *phandle);

/**
 * @brief Free a PIPE (Endpoint)
 * 
 * @param[in]  phandle  pointer to the usb host handler
 * @param[in]  pipe_num the index of the pipe
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_pipe_free(usb_host_t *phandle, uint8_t pipe_num);


/**
 * @brief Configure a PIPE (Endpoint)
 * 
 * @param[in]  phandle  pointer to the usb host handler
 * @param[in]  pipe     pointer to the pipe (endpoint)
 * @param[in]  ep_addr  the endpoint address
 * @param[in]  dev_addr the device address
 * @param[in]  speed    the device speed
 * @param[in]  token    the transmit token
 * @param[in]  ep_type  the endpoint type
 * @param[in]  mps      the max packet size
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_pipe_configure(usb_host_t *phandle, usbh_pipe_t *pipe, 
                        uint8_t ep_addr, uint8_t dev_addr, 
                        uint8_t speed, uint8_t ep_type, uint16_t mps);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_PIPE_H */