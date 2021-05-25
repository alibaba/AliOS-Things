/**
  ******************************************************************************
  * @file    rtl8710b_inic.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the USB & SDIO INIC firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_INIC_H_
#define _RTL8721D_INIC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup INIC 
  * @brief INIC driver modules
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup INIC_Exported_Types INIC Exported Types
  * @{
  */

/** 
  * @brief  INIC TX DESC structure definition
  * @note: Ameba1 is 6 dword, but AmebaZ is 4 dwords
  */
typedef struct {
	/* u4Byte 0 */
	u32	txpktsize:16;       // bit[15:0]
	u32	offset:8;    		// bit[23:16], store the sizeof(INIC_TX_DESC)
	u32	bus_agg_num:8;		// bit[31:24], the bus aggregation number

	/* u4Byte 1 */
	u32 type:8;             // bit[7:0], the packet type
	u32 data:8;             // bit[8:15], the value to be written to the memory
	u32 reply:1;            // bit[16], request to send a reply message
	u32 rsvd0:15;

	/* u4Byte 2 */
	u32	start_addr;        // 1) memory write/read start address 2) RAM start_function address

	/* u4Byte 3 */
	u32 data_len:16;      // bit[15:0], the length to write/read
	u32 rsvd2:16;           // bit[31:16]
} INIC_TX_DESC, *PINIC_TX_DESC;

/** 
  * @brief  INIC RX DESC structure definition
  * @note: Ameba1 is 6 dword, but AmebaZ is 4 dwords
  */
typedef struct {
	/* u4Byte 0 */
	u32	pkt_len:16;		// bit[15:0], the packet size
	u32	offset:8;			// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc    	
	u32	rsvd0:6;			// bit[29:24]
	u32	icv:1; 			//icv error
	u32	crc:1; 			// crc error

	/* u4Byte 1 */
	/************************************************/
	/*****************receive packet type*********************/
	/*	0x82:	802.3 packet						      */
	/*	0x80:	802.11 packet						      */
	/*	0x10:	C2H command					      */
	/*	0x50:	Memory Read						      */
	/*	0x52:	Memory Write						      */
	/*	0x54:	Memory Set						      */
	/*	0x60:	Indicate the firmware is started		      */
	u32	type:8;			// bit[7:0], the type of this packet
	u32	rsvd1:24;		 // bit[31:8]

	/* u4Byte 2 */
	u32	start_addr;

	/* u4Byte 3 */
	u32 data_len:16;		// bit[15:0], the type of this packet
	u32 result:8;			// bit[23:16], the result of memory write command
	u32 rsvd2:8;			// bit[31:24]
} INIC_RX_DESC, *PINIC_RX_DESC;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup INIC_Exported_Constants INIC Exported Constants
  * @{
  */ 

/** @defgroup INIC_packet_type_definitions 
  * @{
  */
#define TX_PACKET_802_3		(0x83)
#define TX_PACKET_802_11		(0x81)
#define TX_H2C_CMD				(0x11)
#define TX_MEM_READ			(0x51)
#define TX_MEM_WRITE			(0x53)
#define TX_MEM_SET				(0x55)
#define TX_FM_FREETOGO			(0x61)
#define TX_PACKET_USER			(0x41)
#define TX_REG_WRITE			(0xE3)

#define TX_FLASH_READ			(0xF1)
#define TX_FLASH_WRITE			(0xF3)
#define TX_FLASH_SECERASE		(0xF5)
#define TX_FLASH_CHECKSUM		(0xF7)

#define RX_PACKET_802_3		(0x82)
#define RX_PACKET_802_11		(0x80)
#define RX_C2H_CMD				(0x10)
#define RX_MEM_READ			(0x50)
#define RX_MEM_WRITE			(0x52)
#define RX_MEM_SET				(0x54)
#define RX_FM_FREETOGO			(0x60)
#define RX_PACKET_USER			(0x40)
#define RX_REG_WRITE			(0xE4)

#define RX_FLASH_READ			(0xF2)
#define RX_FLASH_WRITE			(0xF4)
#define RX_FLASH_SECERASE		(0xF6)
#define RX_FLASH_CHECKSUM		(0xF8)
/**
  * @}
  */

/** @defgroup INIC_DESC_Size_definitions 
  * @{
  */
#define SIZE_RX_DESC	(sizeof(INIC_RX_DESC))
#define SIZE_TX_DESC	(sizeof(INIC_TX_DESC))
/**
  * @}
  */

 /**
  * @}
  */
  

/** @defgroup INIC_Exported_Functions INIC Exported Functions
  * @{
  */
//_LONG_CALL_
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */ 

#endif //_RTL8710B_INIC_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
