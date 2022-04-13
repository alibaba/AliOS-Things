/**
 * @file usb_host.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_H
#define __USB_HOST_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @addtogroup aos_usb_host usb_host
 * usb host API.
 *
 * @{
 */

/**
 * @brief Initialize USB Host Stack
 *
 * @param[in]   none.
 *
 * @return 0: success, otherwise: fail.
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
 * @brief Notify Usb host task that Usb Device has Connected.
 *        Need be called by hcd connect int callback.
 *
 * @return NULL
 */
void usbh_send_device_connected_event(void);

/**
 * @brief Notify Usb host task that Usb Device has Disconnected.
 *        Need be called by hcd disconnect int callback.
 *
 * @return NULL
 */
void usbh_send_device_disconnected_event(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_H */
