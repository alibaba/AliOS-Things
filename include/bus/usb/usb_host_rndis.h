/**
 * @file usb_host_rndis.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_RNDIS_H
#define __USB_HOST_RNDIS_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @addtogroup aos_usb_rndis usb_rndis
 * usb rndis API.
 *
 * @{
 */

/**
 * @brief Try To Connect RNDIS Device
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_connect(void);

/**
 * @brief RNDIS: Send keep alive message
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_send_keep_alive(void);

/**
 * @brief RNDIS: Send device initialize message
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_device_initialize(void);

/**
 * @brief RNDIS: Set property message
 *
 * @param[in]  oid    set property oid message
 * @param[in]  buffer pointer to the property buffer which is set
 * @param[in]  length the length of the property
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_set_property(uint32_t oid, void *buffer, uint32_t length);

/**
 * @brief RNDIS: Query property message
 *
 * @param[in]   oid    query property oid message
 * @param[out]  buffer pointer to the property buffer
 * @param[in]   length the length of the property
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_query_property(uint32_t oid, void *buffer, uint32_t length);

/**
 * @brief RNDIS: read data packet (block mode)
 *
 * @param[out]  buffer pointer to the buffer which store data packet
 * @param[out]  length pointer to the length of the data packet
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_read_packet(void *buffer, uint32_t *length);

/**
 * @brief RNDIS: send data packet (block mode)
 *
 * @param[in]  buffer pointer to the data packet buffer
 * @param[in]  length the length of the data packet which will be send
 *
 * @return 0:success, otherwise is failed
 */
int usbh_rndis_send_packet(void *buffer, uint32_t length);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_RNDIS_H */
