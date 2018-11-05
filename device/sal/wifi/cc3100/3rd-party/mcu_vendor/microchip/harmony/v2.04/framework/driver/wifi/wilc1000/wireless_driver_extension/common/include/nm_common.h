/*******************************************************************************
  File Name:
    nm_common.h

  Summary:
    This module contains common APIs declarations.

  Description:
    This module contains common APIs declarations.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _NM_COMMON_H_
#define _NM_COMMON_H_

#include "wdrv_wilc1000_api.h"
#include "common/include/nm_debug.h"

#define ETH_MODE
#define CONF_WILC_USE_1000_REV_B
#define NM_EDGE_INTERRUPT
#define __M2M_DMA_BUF_ATT__	
#define CONF_WILC_USE_SPI
#define NMI_API

//#define CONF_WILC_FW_IN_FLASH

/*!<
 *     Attribute used to define memory section to map Functions in host memory.
 */

#ifdef CONF_WILC_USE_3000_REV_A
#define COMPUTE_PMK_IN_HOST
#endif
#define P2P_CONCURRENT_INTERFACES
//#define INT_BASED_TX
#define CONF_MGMT
/*!<
 *     Used for code portability.
 */
#define CONST const

/*!<
 *     Void Pointer to '0' in case of NULL is not defined.
 */
#ifndef NULL
#define NULL ((void*)0)
#endif

/*!<
 *     Computes the minimum of \b x and \b y.
 */
#define BSP_MIN(x,y) ((x)>(y)?(y):(x))

/*!
 * @ingroup DataTypes
 * @typedef      unsigned char	uint8;
 * @brief        Range of values between 0 to 255
 */
typedef uint8_t		uint8;

/*!
 * @ingroup DataTypes
 * @typedef      unsigned short	uint16;
 * @brief        Range of values between 0 to 65535
 */
typedef uint16_t	uint16;

/*!
 * @ingroup Data Types
 * @typedef      unsigned long	uint32;
 * @brief        Range of values between 0 to 4294967295
 */
typedef uint32_t	uint32;

/*!
 * @ingroup Data Types
 * @typedef      signed char	sint8;
 * @brief        Range of values between -128 to 127
 */
typedef int8_t		sint8;

/*!
 * @ingroup DataTypes
 * @typedef      signed short	sint16;
 * @brief        Range of values between -32768 to 32767
 */
typedef int16_t		sint16;

/*!
 * @ingroup DataTypes
 * @typedef      signed long	sint32;
 * @brief        Range of values between -2147483648 to 2147483647
 */
typedef int32_t		sint32;

#ifdef _NM_BSP_BIG_END
#define NM_BSP_B_L_32(x) \
((((x) & 0x000000FF) << 24) + \
 (((x) & 0x0000FF00) << 8) + \
 (((x) & 0x00FF0000) >> 8) + \
 (((x) & 0xFF000000) >> 24))
#define NM_BSP_B_L_16(x) \
((((x) & 0x00FF) << 8) + \
 (((x)  & 0xFF00) >> 8))
#else
#define NM_BSP_B_L_32(x)	(x)
#define NM_BSP_B_L_16(x)	(x)
#endif

#define NM_DEBUG		CONF_WINC_DEBUG
#define NM_BSP_PRINTF	CONF_WINC_PRINTF

/**@defgroup  CommonDefines CommonDefines
 * @ingroup WlanDefines
 */

/**@{*/

#define M2M_TIME_OUT_DELAY 10000

/*states*/
#define M2M_SUCCESS         ((sint8)0)
#define M2M_ERR_SEND		((sint8)-1)
#define M2M_ERR_RCV			((sint8)-2)
#define M2M_ERR_MEM_ALLOC	((sint8)-3)
#define M2M_ERR_TIME_OUT	((sint8)-4)
#define M2M_ERR_INIT        ((sint8)-5)
#define M2M_ERR_BUS_FAIL    ((sint8)-6)
#define M2M_NOT_YET			((sint8)-7)
#define M2M_ERR_FIRMWARE	((sint8)-8)
#define M2M_SPI_FAIL		((sint8)-9)
#define M2M_ERR_FIRMWARE_bURN	 ((sint8)-10)
#define M2M_ACK				((sint8)-11)
#define M2M_ERR_FAIL		((sint8)-12)
#define M2M_ERR_FW_VER_MISMATCH         ((sint8)-13)
#define M2M_ERR_SCAN_IN_PROGRESS         ((sint8)-14)

/*i2c MAASTER ERR*/
#define I2C_ERR_LARGE_ADDRESS 	  0xE1UL	/*the address exceed the max addressing mode in i2c flash*/
#define I2C_ERR_TX_ABRT 		  0xE2UL	/*NO ack from slave*/
#define I2C_ERR_OVER_SIZE 		  0xE3UL	/**/
#define ERR_PREFIX_NMIS		      0xE4UL	/*wrong first four byte in flash NMIS*/
#define ERR_FIRMEWARE_EXCEED_SIZE 0xE5UL	/*Total size of firmware exxed the max size 256k*/
/**/
#define PROGRAM_START		0x26961735UL
#define BOOT_SUCCESS		0x10add09eUL
#define BOOT_START		    0x12345678UL     


#define NBIT31				(0x80000000)
#define NBIT30				(0x40000000)
#define NBIT29				(0x20000000)
#define NBIT28				(0x10000000)
#define NBIT27				(0x08000000)
#define NBIT26				(0x04000000)
#define NBIT25				(0x02000000)
#define NBIT24				(0x01000000)
#define NBIT23				(0x00800000)
#define NBIT22				(0x00400000)
#define NBIT21				(0x00200000)
#define NBIT20				(0x00100000)
#define NBIT19				(0x00080000)
#define NBIT18				(0x00040000)
#define NBIT17				(0x00020000)
#define NBIT16				(0x00010000)
#define NBIT15				(0x00008000)
#define NBIT14				(0x00004000)
#define NBIT13				(0x00002000)
#define NBIT12				(0x00001000)
#define NBIT11				(0x00000800)
#define NBIT10				(0x00000400)
#define NBIT9				(0x00000200)
#define NBIT8				(0x00000100)
#define NBIT7				(0x00000080)
#define NBIT6				(0x00000040)
#define NBIT5				(0x00000020)
#define NBIT4				(0x00000010)
#define NBIT3				(0x00000008)
#define NBIT2				(0x00000004)
#define NBIT1				(0x00000002)
#define NBIT0				(0x00000001)

#define M2M_MAX(A,B)					((A) > (B) ? (A) : (B))
#define M2M_SEL(x,m1,m2,m3)				((x>1)?((x>2)?(m3):(m2)):(m1))
#define WORD_ALIGN(val) 				(((val) & 0x03) ? ((val) + 4 - ((val) & 0x03)) : (val))



#define DATA_PKT_OFFSET	4

#ifndef CONF_WILC_BIG_ENDIAN
#define BYTE_0(word)   					((uint8)(((word) >> 0 	) & 0x000000FFUL))
#define BYTE_1(word)  	 				((uint8)(((word) >> 8 	) & 0x000000FFUL))
#define BYTE_2(word)   					((uint8)(((word) >> 16) & 0x000000FFUL))
#define BYTE_3(word)   					((uint8)(((word) >> 24) & 0x000000FFUL))
#else
#define BYTE_0(word)   					((uint8)(((word) >> 24) & 0x000000FFUL))
#define BYTE_1(word)  	 				((uint8)(((word) >> 16) & 0x000000FFUL))
#define BYTE_2(word)   					((uint8)(((word) >> 8 	) & 0x000000FFUL))
#define BYTE_3(word)   					((uint8)(((word) >> 0 	) & 0x000000FFUL))
#endif


typedef enum{
	M2M_REQ_GRP_MAIN = 0, M2M_REQ_GRP_WIFI, M2M_REQ_GRP_HIF
}tenuM2mReqGrp;


typedef enum{
	M2M_REQ_CONFIG_PKT,
	M2M_REQ_DATA_PKT = NBIT7
}tenuM2mReqPkt;
#ifdef __cplusplus
     extern "C" {
 #endif
NMI_API void m2m_memcpy(uint8* pDst,uint8* pSrc,uint32 sz);
NMI_API void m2m_memset(uint8* pBuf,uint8 val,uint32 sz);
NMI_API uint16 m2m_strlen(uint8 * pcStr);
NMI_API sint8 m2m_memcmp(uint8 *pu8Buff1,uint8 *pu8Buff2 ,uint32 u32Size);
NMI_API uint8 m2m_strncmp(uint8 *pcS1, uint8 *pcS2, uint16 u16Len);
NMI_API uint8 * m2m_strstr(uint8 *pcIn, uint8 *pcStr);
NMI_API uint8 m2m_checksum(uint8* buf, int sz);
void nm_sleep(uint32 u32TimeMsec);
void nm_reset(void);


#ifdef __cplusplus
}
 #endif
#endif	/*_NM_COMMON_H_*/
