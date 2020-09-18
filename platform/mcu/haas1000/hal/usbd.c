/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/usbd.h"

/* Initialization */
/******************************************************************************************/

/**
 * @brief Initialize usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_init(void *pdev)
{
	return 0;
}

/**
 * @brief Deinitialize usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_deinit(void *pdev)
{
	return 0;
}

/**
 * @brief enable usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_start(void *pdev)
{
	return 0;
}

/**
 * @brief disable usb device driver
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return the operation status, USBD_OK is OK, USBD_BUSY is BUSY, others is error
 */
usbd_stat_t usbd_hal_stop(void *pdev)
{
	return 0;
}

/**
 * @brief enable usb device interrupt
 */
void usbd_hal_connect(void)
{
	return 0;
}

/**
 * @brief disable usb device interrupt
 */
void usbd_hal_disconnect(void)
{
	return 0;
}

/**
 * @brief configure usb device info
 */
void usbd_hal_configure_device(void)
{
	return 0;
}

/**
 * @brief unconfigure usb device info
 */
void usbd_hal_unconfigure_device(void)
{
	return 0;
}

/**
 * @brief unconfigure usb device info
 *
 * @param[in]  pdev     point to usb device handler
 * @param[in]  address  the usb device address
 *
 * @return none
 */
void usbd_hal_set_address(void *pdev, uint8_t address)
{
	return 0;
}

/* Endpoint 0 */
/******************************************************************************************/

/**
 * @brief Endpoint0 setup(read setup packet)
 *
 * @param[in]  buffer  point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_setup(uint8_t *buffer)
{
	return 0;
}

/**
 * @brief Endpoint0 read packet
 *
 * @param[in]  pdev  point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_read(void *pdev)
{
	return 0;
}

/**
 * @brief Endpoint0 read stage
 */
void usbd_hal_ep0_read_stage(void)
{
	return 0;
}

/**
 * @brief Endpoint0 get read result
 *
 * @param[in]  pdev    point to usb device handler
 * @param[in]  buffer  point to packet
 *
 * @return the length of read packet
 */
uint32_t usbd_hal_get_ep0_read_result(void *pdev, uint8_t *buffer)
{
	return 0;
}

/**
 * @brief Endpoint0 write
 *
 * @param[in]  pdev    point to usb device handler
 * @param[in]  buffer  point to packet
 * @param[in]  size    the length of write packet
 *
 * @return none
 */
void usbd_hal_ep0_write(void *pdev, uint8_t *buffer, uint32_t size)
{
	return 0;
}

/**
 * @brief Get endpoint0 write result
 */
void usbd_hal_get_ep0_write_result(void)
{
	return 0;
}

/**
 * @brief Stall endpoint0
 *
 * @param[in] pdev point to usb device handler
 *
 * @return none
 */
void usbd_hal_ep0_stall(void *pdev)
{
	return 0;
}

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
 * @return true is ok, false is false
 */
bool usbd_hal_realise_endpoint(void *pdev, uint8_t endpoint, uint32_t maxPacket, uint32_t flags)
{
	return 0;
}

/**
 * @brief start read the endpoint data
 *
 * @param[in]  pdev         point to usb device handler
 * @param[in]  endpoint     the num of endpoint
 * @param[in]  maximumSize  amount of data to be received
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_read(void *pdev, uint8_t endpoint, uint32_t maximumSize)
{
	return 0;
}

/**
 * @brief read the endpoint data
 *
 * @param[in]  pdev       point to usb device handler
 * @param[in]  endpoint   the num of endpoint
 * @param[in]  data       point to receive buffer
 * @param[in]  bytesRead  amount of data be received
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_read_result(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t *bytesRead)
{
	return 0;
}

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
ep_status usbd_hal_endpoint_write(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t size)
{
	return 0;
}

/**
 * @brief write the endpoint data
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return endpoint status
 */
ep_status usbd_hal_endpoint_write_result(void *pdev, uint8_t endpoint)
{
	return 0;
}

/**
 * @brief stall the endpoint
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return none
 */
void usbd_hal_stall_endpoint(void *pdev, uint8_t endpoint)
{
	return 0;
}

/**
 * @brief unstall the endpoint
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return none
 */
void usbd_hal_unstall_endpoint(void *pdev, uint8_t endpoint)
{
	return 0;
}

/**
 * @brief get the endpoint status of stall
 *
 * @param[in]  pdev      point to usb device handler
 * @param[in]  endpoint  the num of endpoint
 *
 * @return true is ok, false is false
 */
bool usbd_hal_get_endpoint_stall_state(void *pdev, uint8_t endpoint)
{
	return 0;
}

