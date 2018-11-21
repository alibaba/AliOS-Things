/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_DEVICE_H
#define __USB_HOST_DEVICE_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Parse Device Descriptor
 * 
 * @param[in] device pointer to the connected device
 * @param[in] data   pointer to the device descriptor buffer
 *
 * @return none
 */
void usbh_parse_dev_desc(usbh_device_t *device, uint8_t *data);


/**
 * @brief Parse Configuration Descriptor
 * 
 * @param[in]  device  pointer to the connected device
 * @param[in]  data    pointer to the configuration descriptor buffer
 * @param[in]  len     the length of the configuration descriptor buffer
 * @param[in]  phelper pointer to the enumerate helper functions structure
 *
 * @return none
 */
void usbh_parse_conf_desc(usbh_device_t *device, uint8_t *data, 
                          uint16_t len, enum_helper_t *phelper);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_DEVICE_H */