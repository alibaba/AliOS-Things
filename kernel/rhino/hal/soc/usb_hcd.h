/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_USB_HCD_H
#define __HAL_USB_HCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Initialize The USB Host Controller
 * 
 * @param[in]  phost pointer to the usb host handler
 * @param[in]  phcd  pointer to the usb hcd pointer
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_init(void *phost, void **phcd);

/**
 * @brief Finalize The USB Host Controller
 * 
 * @param[in]  hcd pointer to the usb hcd
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_finalize(void *hcd);

/**
 * @brief Reset Host Controller's Port
 * 
 * @param[in]  hcd pointer to the usb hcd
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_port_reset(void *hcd);

/**
 * @brief Get Device Speed
 * 
 * @param[in]  hcd pointer to the usb hcd
 * 
 * @return the usb host controller's speed (0, 1, 2)
 */
int hal_usbh_get_speed(void *hcd);

/**
 * @brief Free The Host Controll's Pipe
 * 
 * @param[in]  hcd      pointer to the usb hcd
 * @param[in]  pipe_num the index of the pipe
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_pipe_free(void *hcd, uint8_t pipe_num);

/**
 * @brief Configure The Host Controller's Pipe
 * 
 * @param[in]  hcd      pointer to the usb hcd
 * @param[in]  index    the index of the pipe
 * @param[in]  ep_addr  the endpoint address
 * @param[in]  dev_addr the device address
 * @param[in]  speed    the device speed
 * @param[in]  token    the transmit token
 * @param[in]  ep_type  the endpoint type
 * @param[in]  mps      the max packet size
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_pipe_configure(void *hcd, uint8_t index, 
                            uint8_t ep_addr, uint8_t dev_addr, 
                            uint8_t speed, uint8_t ep_type, uint16_t mps);

/**
 * @brief Submit The Urb
 * 
 * @param[in]      hcd       pointer to the usb hcd
 * @param[in]      pipe_num  the index of the pipe
 * @param[in]      direction the transmit direction
 * @param[in]      ep_type   the endpoint type
 * @param[in]      token     the transmit token
 * @param[in/out]  buf       pointer to the buffer which will be send or recv
 * @param[in]      length    the length of buffer
 * 
 * @return 0:success, otherwise is failed
 */
int hal_usbh_submit_urb(void *hcd, uint8_t pipe_num, 
                           uint8_t direction, uint8_t ep_type, 
                           uint8_t token, uint8_t *buf, uint16_t length);

/**
 * @brief Get The Urb Transmit State
 * 
 * @param[in]  hcd      pointer to the usb hcd
 * @param[in]  pipe_num the index of the pipe
 * 
 * @return 0:Idle, 1:Done, 2:Not Ready, 3:Nyet, 4:Error, 5:Stall
 */
int hal_usbh_get_urb_state(void *hcd, uint8_t pipe_num);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_USB_HCD_H */
