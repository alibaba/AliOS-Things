/**
 * @file usbd.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_USBD_H
#define HAL_USBD_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_usbd USBD
 *  usbd hal API.
 *
 *  @{
 */

#include <stdint.h>

#include "usb_device.h"

/* Endpoint transfer status, for endpoints > 0 */
typedef enum {
    EP_COMPLETED, /**< Transfer completed */
    EP_PENDING,   /**< Transfer in progress */
    EP_INVALID,   /**< Invalid parameter */
    EP_STALLED,   /**< Endpoint stalled */
} ep_status;

/* Initialization */
/******************************************************************************************/

/**
 * @brief Initialize usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_init(void *pdev);

/**
 * @brief Deinitialize usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_deinit(void *pdev);

/**
 * @brief start usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_start(void *pdev);

/**
 * @brief stop usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_stop(void *pdev);

/**
 * @brief enable usb device interrupt
 */
void usbd_hal_connect(void);

/**
 * @brief disable usb device interrupt
 */
void usbd_hal_disconnect(void);

/**
 * @brief configure usb device info
 */
void usbd_hal_configure_device(void);

/**
 * @brief unconfigure usb device info
 */
void usbd_hal_unconfigure_device(void);

/**
 * @brief set usb device address
 *
 * @param[in]  pdev     point to usb device handler
 * @param[in]  address  the usb device address
 *
 * @return none
 */
void usbd_hal_set_address(void *pdev, uint8_t address);

/* Endpoint 0 */
/******************************************************************************************/

/**
 * @brief Endpoint0 setup(read setup packet)
 *
 * @param[in]  buffer  point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_setup(uint8_t *buffer);

/**
 * @brief Endpoint0 read packet
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_read(void *pdev);

/**
 * @brief Endpoint0 read stage
 */
void usbd_hal_ep0_read_stage(void);

/**
 * @brief Endpoint0 get read result
 *
 * @param[in]  pdev    point to usb device handler
 * @param[out] buffer  point to packet
 *
 * @return the length of read packet
 */
uint32_t usbd_hal_get_ep0_read_result(void *pdev, uint8_t *buffer);

/**
 * @brief Endpoint0 write
 *
 * @param[in]  pdev    point to usb device handler
 * @param[in]  buffer  point to packet
 * @param[in]  size    the length of write packet
 *
 * @return none
 */
void usbd_hal_ep0_write(void *pdev, uint8_t *buffer, uint32_t size);

/**
 * @brief Get endpoint0 write result
 */
void usbd_hal_get_ep0_write_result(void);

/**
 * @brief Stall endpoint0
 *
 * @param[in] pdev point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_stall(void *pdev);

/* Other endpoints */
/******************************************************************************************/

/**
 * @brief open the endpoint
 *
 * @param[in]  pdev       point to usb device handler
 * @param[in]  endpoint   the num of endpoint
 * @param[in]  maxPacket  the max size of packet
 * @param[in]  flags      options flags for configuring endpoints
 *
 * @return true is ok, false is fail
 */
bool usbd_hal_realise_endpoint(void *pdev, uint8_t endpoint, uint32_t maxPacket, uint32_t flags);

/**
 * @brief start read the endpoint data
 *
 * @param[in]  pdev         point to usb device handler
 * @param[in]  endpoint     the num of endpoint
 * @param[in]  maximumSize  amount of data to be received
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_read(void *pdev, uint8_t endpoint, uint32_t maximumSize);

/**
 * @brief read the endpoint data
 *
 * @param[in]  pdev        point to usb device handler
 * @param[in]  endpoint    the num of endpoint
 * @param[out]  data       point to receive buffer
 * @param[out]  bytesRead  amount of data be received
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_read_result(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t *bytesRead);

/**
 * @brief start write the endpoint data
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 * @param[in]  data      point to write buffer
 * @param[in]  size      amount of data to be write
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_write(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t size);

/**
 * @brief get writting endpoint data status
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_write_result(void *pdev, uint8_t endpoint);

/**
 * @brief stall the endpoint
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return none
 */
void usbd_hal_stall_endpoint(void *pdev, uint8_t endpoint);

/**
 * @brief unstall the endpoint
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return none
 */
void usbd_hal_unstall_endpoint(void *pdev, uint8_t endpoint);

/**
 * @brief get the endpoint status of stall
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return true is ok, false is false
 */
bool usbd_hal_get_endpoint_stall_state(void *pdev, uint8_t endpoint);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_USBD_H */

