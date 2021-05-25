/** mbed Microcontroller Library
  ******************************************************************************
  * @file    spdio_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed SPDIO API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */
#ifndef __SPDIO_API_H__
#define __SPDIO_API_H__

#include <osdep_service.h>

/** @addtogroup spdio_api SPDIO
 *  @ingroup    hal
 *  @brief      spdio functions
 *  @{
 */

///@name Ameba Common
///@{

#define SPDIO_API_DBG

#ifdef SPDIO_API_DBG
#define SPDIO_API_PRINTK(fmt, args...)		printf(fmt"\r\n",## args)
#define _SPDIO_API_PRINTK(fmt, args...)		printf(fmt,## args)
#else
#define SPDIO_API_PRINTK(fmt, args...)
#define _SPDIO_API_PRINTK(fmt, args...)
#endif

#define SPDIO_DMA_ALIGN_4			4
#define SPDIO_RX_BUFSZ_ALIGN(x)	((((x-1)>>6)+1)<<6) //alignement to 64

#define SPDIO_RXDESC_SZ	24

/*Don't modify this enum table*/
enum spdio_rx_data_t{
	SPDIO_RX_DATA_NULL = 0x00,
	SPDIO_RX_DATA_ETH = 0x83, //an ethernet packet received
	SPDIO_RX_DATA_ATCMD = 0x11, //an AT command packet received
	SPDIO_RX_DATA_USER = 0x41, //defined by user
};

enum spdio_tx_data_t{
	SPDIO_TX_DATA_NULL = 0x00,
	SPDIO_TX_DATA_ETH = 0x82, //an ethernet packet sent
	SPDIO_TX_DATA_ATCMDRSP = 0x10, //an AT command response packet sent
	SPDIO_TX_DATA_USER = 0x40, // defined by user
};

struct spdio_buf_t{
	void *priv; //priv data from user
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries, spdio_rx_data_t and spdio_tx_data_t
	u8 reserved;
};

struct spdio_t {
	void *priv; //not used by user
	u32 tx_bd_num; //for spdio send data to host, 2 bd for one packet, so this value must be rounded to 2
	u32 rx_bd_num; //for spdio receive data from host
	u32 rx_bd_bufsz; //buffer size = desired packet length + 24(spdio header info), must be rounded to 64
	struct spdio_buf_t *rx_buf; //buffer array for spdio receive assigned by user, rx_bd_bufsz * rx_bd_num

	/**
	 *@brief pointer to callback function defined by user, 
	 		called by spdio when one packet receive done
	 *@param priv: a pointer to spdio_t structure which is used to initilize spdio interface
	 *@param pbuf: a pointer to spdio_buf_t structure which is spdio receive buffer
	 *@param pdata: the actual received packet payload
	 *@param size: the actual payload length
	 *@param type: the received packet type, spdio_rx_data_t
	 *@retval SUCCESS or FAIL
	 */
	char (*rx_done_cb)(void *priv, void* pbuf, u8 *pdata, u16 size, u8 type); 
	
	/**
	 *@brief pointer to callback function defined by user, 
	 		called by spdio when one packet sent done
	 *@param priv: a pointer to spdio_t structure which is used to initilize spdio interface
	 *@param pbuf: a pointer to spdio_buf_t structure which carries the transmit packet
	 *@retval SUCCESS or FAIL
	 */
	char (*tx_done_cb)(void *priv, void* pbuf); 
};

/**
  * @brief  Gets example setting for spdio obj.
  * @param  obj: a pointer to an spdio_t structure which will be initialized with an example settings
  * @retval None
  */
void spdio_structinit(struct spdio_t *obj);

/**
  * @brief  Initialize spdio interface.
  * @param  obj, a pointer to a spdio_t structure which should be initialized by user,
  *		and which will be used to initialize spdio interface
  *		obj->tx_bd_num: spdio write bd number, needs 2 bd for one transaction
  *		obj->rx_bd_num: spdio read bd number
  *		obj->rx_bd_bufsz: spdio read buffer size
  *		obj->rx_buf: spdio read buffer array
  * @retval None
  */
void spdio_init(struct spdio_t *obj);

/**
  * @brief  Deinitialize spdio interface.
  * @param  obj: a pointer to spdio_t structure which is already initialized
  * @retval None
  */
void spdio_deinit(struct spdio_t *obj);

/**
  * @brief  spdio write function.
  * @param  obj: a pointer to spdio_t structure which is already initialized
  * @param  pbuf: a pointer to spdio_buf_t structure which carries the payload
  * @retval SUCCESS or FAIL
  */
s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf);

/**
  * @brief  an obj which will be used to initialize sdio interface
  *		so it must be initialized before calling HalSdioInit();
  */
extern struct spdio_t *g_spdio_priv;

///@}

/*\@}*/

#endif //#ifndef __SPDIO_API_H__