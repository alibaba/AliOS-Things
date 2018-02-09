/**
  ******************************************************************************
  * @file    rtl8710b_ota.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the OTA functions.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_OTA_H_
#define _RTL8710B_OTA_H_

#include "basic_types.h"

/** @addtogroup AmebaZ_Platform
  * @{
  */

#define SINGLE_IMG_OTA_UPGRADE    1						/*single image method upgrade defination */
#define SERVER_TYPE		SERVER_LOCAL						/*configure OTA demo type*/
#define CONFIG_CUSTOM_SIGNATURE 1						/*custom signature used or not*/

#define HTTP_OTA_UPDATE			//if define, using http protocol, if not, will use socket
#ifdef HTTP_OTA_UPDATE
#define HEADER_BAK_LEN			32

typedef struct {
	uint32_t	status_code;
	uint32_t	header_len;
	uint8_t		*body;
	uint32_t	body_len;
	uint8_t		*header_bak;
	uint32_t	parse_status;
} http_response_result_t;
#endif
/** @defgroup OTA
  * @brief OTA driver modules
  * @{
  */

/* Exported Types --------------------------------------------------------*/
/** @defgroup OTA_Exported_Types OTA Exported Types
  * @{
  */

#if SINGLE_IMG_OTA_UPGRADE

/** 
  * @brief  OTA firmware file header structure definition
  */
typedef struct
{
	u32	FwVer;	/*!< Specifies the OTA firmware verision.
	                         This parameter is in first Dword in the firmware file. */
	u32	HdrNum;/*!< Specifies the OTA firmware header number.
	                         This parameter indicates how many headers in firmware file. */
}update_file_hdr;

/** 
  * @brief  OTA firmware file image header structure definition
  */
typedef struct
{
	u8	ImgId[4];	/*!< Specifies the OTA image ID.
	                         	This parameter is used to identify the OTA header needed. */
	u32	ImgHdrLen;	/*!< Specifies the OTA image header length.
	                         	This parameter indicates the Image Header Length. */
	u32	Checksum;	/*!< Specifies the OTA image checksum.
	                         	This parameter is used to judge whether the image received is correct. */
	u32  ImgLen;		/*!< Specifies the OTA image length. */
	u32  Offset;		/*!< Specifies the the location in the total firmware file. */
	u32  FlashOffset;    /*!< Specifies the flash offset address of the corresponding image. */
}update_file_img_hdr;

/** 
  * @brief  OTA firmware file download information structure definition
  */
typedef struct
{
	u32 ImgId;		/*!< Specifies the Image ID.*/

	u32	FlashAddr;	/*!< Specifies the Flash Address.
	                         	This parameter is used to write the Image to the flash. */
	u32	ImgOffset;	/*!< Specifies the Image location in Firmware header.
	                         	This parameter indicates the Image location in firmware file. */
	u32	ImageLen;	/*!< Specifies the OTA image length. */
}update_dw_info;

/** 
  * @brief  OTA target image header structure definition
  */
typedef struct
{
	update_file_hdr	FileHdr;			/*!< Specifies the firmware file header. */
	update_file_img_hdr	FileImgHdr;	/*!< Specifies the target OTA image firmware file header. */
	update_file_img_hdr	FileRdpHdr;	/*!< Specifies the target RDP image firmware file header. */
	u32 RdpStatus;
}update_ota_target_hdr;

/** 
  * @brief  OTA image ID header structure definition
  */
typedef struct
{
	u8	ImgId[5];				/*!< Specifies the image ID strings. */
}update_file_img_id;

#endif
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup OTA_Exported_Constants OTA Exported Constants
  * @{
  */ 
/** @defgroup OTA_system_parameter_definitions 
  * @{
  */ 
#define OFFSET_DATA		FLASH_SYSTEM_DATA_ADDR			/*system data offset address*/
#define BACKUP_SECTOR	(FLASH_SYSTEM_DATA_ADDR - 0x1000)	/*back up system data offset address*/
#define OTA1_ADDR		0x0800B000						/*the OTA1 start address*/
#define OTA2_ADDR		0x08080000						/*the OTA2 start address*/
#define BUF_SIZE			512								/*the size of the buffer used for receiving firmware data from server*/
#define RDP_FLASH_ADDR	0x080FF000						/*the flash addresss for RDP image*/

#define OTA_IMAG			0								/*identify the OTA image*/
#define RDP_IMAG			1								/*identify the RDP image*/

/**
  * @}
  */

/** @defgroup OTA_Index_definitions 
  * @{
  */ 
#define OTA_INDEX_1			0
#define OTA_INDEX_2			1
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup OTA_Exported_Functions OTA Exported Functions
  * @{
  */
u32 OTA_Change(u32 OTAIdx);

void* ota_update_malloc(unsigned int size);
void ota_update_free(void *buf);

#if (SERVER_TYPE == SERVER_LOCAL)
void ota_platform_reset(void);
int ota_write_ota2_addr(uint32_t ota_addr);
u32 ota_get_cur_index(void);
#if SINGLE_IMG_OTA_UPGRADE
u32 recv_file_info_from_server(u8 * Recvbuf, u32 len, int socket);
u32 recv_ota_file_hdr(u8 * Recvbuf, u32 * len, update_ota_target_hdr * pOtaTgtHdr, int socket);
u32 get_ota_tartget_header(u8* buf, u32 len, update_ota_target_hdr * pOtaTgtHdr, u8 * ImgId);
void erase_ota_target_flash(u32 addr, u32 len);
s32 download_new_fw_from_server(u32 addr, int socket, update_ota_target_hdr * pOtaTgtHdr, u8 * signature);
u32 verify_ota_checksum(u32 addr, u32 len, u8 * signature, update_ota_target_hdr * pOtaTgtHdr);
u32 change_ota_signature(u32 addr, u8 * signature, u32 ota_target_index);
u32 get_ota_address(u32 ota_target_index, u32 * new_addr, update_ota_target_hdr * pOtaTgtHdr);
#endif
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef HTTP_OTA_UPDATE
int parse_http_response(uint8_t *response, uint32_t response_len, http_response_result_t *result);
int update_ota_http_connect_server(int server_socket, char *host, int port);
int http_read_socket( int socket, uint8_t *recevie_buf, int buf_len );

/*************************************************************************************************
** Function Name  : http_update_ota
** Description    : The process of OTA updating through http protocol
** Input          : cfg:struct update_cfg_local_t
** Return         : NULL
**************************************************************************************************/
int http_update_ota(char *host, int port, char *resource);
#endif

#endif //_RTL8710B_OTA_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
