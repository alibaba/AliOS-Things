/**
  ******************************************************************************
  * @file    rtl8721d_ota.h
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

#ifndef _RTL8721D_OTA_H_
#define _RTL8721D_OTA_H_


/** @addtogroup AmebaD_Platform
  * @{
  */
#define SERVER_LOCAL	1
#define SERVER_CLOUD	2
#define SERVER_TYPE		SERVER_LOCAL						/*configure OTA demo type*/
#define MAX_IMG_NUM		2

#define HTTP_OTA_UPDATE	
#define HTTPS_OTA_UPDATE	

#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)

#define HEADER_BAK_LEN			32

typedef struct {
	u32	status_code;
	u32	header_len;
	u8		*body;
	u32	body_len;
	u8		*header_bak;
	u32	parse_status;
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
	u32  FlashAddr;    /*!< Specifies the flash offset address of the corresponding image. */
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
	update_file_img_hdr	FileImgHdr[MAX_IMG_NUM];	/*!< Specifies the target OTA image firmware file header. */
	u8 Sign[MAX_IMG_NUM][9];			/*!< Specifies the signature of target image. */
	u8 ValidImgCnt;						/*!< Specifies valid image number in file. */
}update_ota_target_hdr;


/* Exported constants --------------------------------------------------------*/

/** @defgroup OTA_Exported_Constants OTA Exported Constants
  * @{
  */ 
/** @defgroup OTA_system_parameter_definitions 
  * @{
  */ 
#define BACKUP_SECTOR	(FLASH_RESERVED_DATA_BASE)	/*back up system data offset address*/						
#define LS_IMG2_OTA1_ADDR	0x08014000				/* KM0 OTA1 start address*/
#define LS_IMG2_OTA2_ADDR	0x08194000				/* KM0 OTA2 start address*/


#define BUF_SIZE			512								/*the size of the buffer used for receiving firmware data from server*/

#define OTA_IMAG			0								/*identify the OTA image*/

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
int  ota_readstream_user(u32 address, u32 len, u8 * data);

u32 recv_file_info_from_server(u8 * Recvbuf, u32 len, int socket);
u32 recv_ota_file_hdr(u8 * Recvbuf, u32 * len, update_ota_target_hdr * pOtaTgtHdr, int socket);
u32 get_ota_tartget_header(u8* buf, u32 len, update_ota_target_hdr * pOtaTgtHdr, u8 target_idx);
void erase_ota_target_flash(u32 addr, u32 len);
u32 download_new_fw_from_server(int socket, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx);
u32 verify_ota_checksum(update_ota_target_hdr * pOtaTgtHdr);
u32 change_ota_signature(update_ota_target_hdr * pOtaTgtHdr, u32 ota_target_index);
#endif

#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)
int  parser_url( char *url, char *host, u16 *port, char *resource);
int parse_http_response(unsigned char *response, unsigned int response_len, http_response_result_t *result);
#ifdef HTTP_OTA_UPDATE
int update_ota_http_connect_server(int server_socket, char *host, int port);
u32 recv_ota_file_hdr_http(u8 * Recvbuf, u32 writelen, u32 * len, update_ota_target_hdr * pOtaTgtHdr, int socket);
int http_read_socket( int socket, u8 *recevie_buf, int buf_len );
u32 download_new_fw_from_server_http(u8* first_buf, unsigned int firstbuf_len, int socket, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx);
int http_update_ota(char *host, int port, char *resource);
#endif
#ifdef HTTPS_OTA_UPDATE
#if !defined(MBEDTLS_CONFIG_FILE)
#include <mbedtls/config.h>
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
u32 recv_ota_file_hdr_https(u8 * Recvbuf, u32 writelen, u32 * len, update_ota_target_hdr * pOtaTgtHdr, mbedtls_ssl_context * ssl);
int https_read_socket(mbedtls_ssl_context * ssl, u8 * recevie_buf, int buf_len);
u32 download_new_fw_from_server_https(u8 * first_buf, unsigned int firstbuf_len, mbedtls_ssl_context * ssl, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx);
int https_update_ota(char *host, int port, char *resource);
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
#endif //_RTL8721D_OTA_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
