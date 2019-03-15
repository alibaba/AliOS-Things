/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_CORE_H
#define __USB_HOST_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Get Usb Host Handler
 * 
 * @return  usb host handler
 */
usb_host_t *usbh_get_instance(void);

/**
 * @brief Process Usb Enumeration
 * 
 * @param[in]  phandle pointer to the usb host handler
 * @param[in]  phelper pointer to the enumerate helper structure
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_enumerate(usb_host_t *phandle, enum_helper_t *phelper);

/**
 * @brief Notify Usb Device Connect to Usb Host Task
 *
 * @return NULL
 */
void usbh_send_device_connected_event(void);

/**
 * @brief Notify Usb Device Disconnect to Usb Host Task
 *
 * @return NULL
 */
void usbh_send_device_disconnected_event(void);

/**
 * @brief Register Callback For Device Disconnect Event
 *
 * @param[in] phandle   pointer to the usb host handler
 * @param[in] device_cb pointer to the callback function
 *
 * @return 0:success, otherwise is failed
 */
int usbh_register_disconnect_callback(usb_host_t *phandle, void (*device_cb)(void));

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_CORE_H */
