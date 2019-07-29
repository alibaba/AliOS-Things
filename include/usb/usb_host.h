/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_H
#define __USB_HOST_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Initialize USB Host Stack
 *
 * @param[in]   none.
 *
 * @return 0: Success
 */
int usbh_init(void);

/**
 * @brief Deinitialize USB Host Stack
 *
 * @param[in]   none.
 *
 * @return none.
 */
void usbh_finalize(void);

/**
 * @brief Notify Usb Device Connect to Usb Host Task.
 *        need be called by HAL implementation
 *
 * @return NULL
 */
void usbh_send_device_connected_event(void);

/**
 * @brief Notify Usb Device Disconnect to Usb Host Task
 *        need be called by HAL implementation
 *
 * @return NULL
 */
void usbh_send_device_disconnected_event(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_H */
