/*******************************************************************************
  WILC1000 spi flash interface.

  File Name:
    spi_flash.h

  Summary:
    WILC1000 SPI Flash Interface

  Description:
    WILC1000 SPI Flash Interface
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

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__
#include "common/include/nm_common.h"
#include "driver/source/nmbus.h"
#include "driver/source/nmasic.h"
#include "spi_flash/include/spi_flash_map.h"

/**
 *	@fn		spi_flash_enable
 *	@brief	Enable spi flash operations
 *	@version	1.0
 */
sint8 spi_flash_enable(uint8 enable);
/** \defgroup SPIFLASHAPI Function
 *   @ingroup SPIFLASH
 */

 /** @defgroup SPiFlashGetFn spi_flash_get_size
 *  @ingroup SPIFLASHAPI
 */
  /**@{*/
/*!
 * @fn             uint32 spi_flash_get_size(void);
 * @brief         Returns with \ref uint32 value which is total flash size\n
 * @note         Returned value in Mb (Mega Bit).
 * @return      SPI flash size in case of success and a ZERO value in case of failure.
 */
uint32 spi_flash_get_size(void);
 /**@}*/

  /** @defgroup SPiFlashRead spi_flash_read
 *  @ingroup SPIFLASHAPI
 */
  /**@{*/
/*!
 * @fn             sint8 spi_flash_read(uint8 *, uint32, uint32);
 * @brief          Read a specified portion of data from SPI Flash.\n
 * @param [out]    pu8Buf
 *                 Pointer to data buffer which will fill in with data in case of successful operation.
 * @param [in]     u32Addr
 *                 Address (Offset) to read from at the SPI flash.
 * @param [in]     u32Sz
 *                 Total size of data to be read in bytes
 * @warning	       
 *                 - Address (offset) plus size of data must not exceed flash size.\n
 *                 - No firmware is required for reading from SPI flash.\n
 *                 - In case of there is a running firmware, it is required to pause your firmware first 
 *                   before any trial to access SPI flash to avoid any racing between host and running firmware on bus using 
 *                   @ref m2m_wifi_download_mode
 * @note           
 *                 - It is blocking function\n
 * @sa             m2m_wifi_download_mode, spi_flash_get_size
 * @return        The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
 */
sint8 spi_flash_read(uint8 *pu8Buf, uint32 u32Addr, uint32 u32Sz);
 /**@}*/

  /** @defgroup SPiFlashWrite spi_flash_write
 *  @ingroup SPIFLASHAPI
 */
  /**@{*/
/*!
 * @fn             sint8 spi_flash_write(uint8 *, uint32, uint32);
 * @brief          Write a specified portion of data to SPI Flash.\n
 * @param [in]     pu8Buf
 *                 Pointer to data buffer which contains the required to be written.
 * @param [in]     u32Offset
 *                 Address (Offset) to write at the SPI flash.
 * @param [in]     u32Sz
 *                 Total number of size of data bytes
 * @note           
 *                 - It is blocking function\n
 *                 - It is user's responsibility to verify that data has been written successfully 
 *                   by reading data again and compare it with the original.
 * @warning	       
 *                 - Address (offset) plus size of data must not exceed flash size.\n
 *                 - No firmware is required for writing to SPI flash.\n
 *                 - In case of there is a running firmware, it is required to pause your firmware first 
 *                   before any trial to access SPI flash to avoid any racing between host and running firmware on bus using 
 *                   @ref m2m_wifi_download_mode.
 *                 - Before writing to any section, it is required to erase it first.
 * @sa             m2m_wifi_download_mode, spi_flash_get_size, spi_flash_erase
 * @return       The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
 
 */
sint8 spi_flash_write(uint8* pu8Buf, uint32 u32Offset, uint32 u32Sz);
 /**@}*/

  /** @defgroup SPiFlashErase spi_flash_erase
 *  @ingroup SPIFLASHAPI
 */
  /**@{*/
/*!
 * @fn             sint8 spi_flash_erase(uint32, uint32);
 * @brief          Erase a specified portion of SPI Flash.\n
 * @param [in]     u32Offset
 *                 Address (Offset) to erase from the SPI flash.
 * @param [in]     u32Sz
 *                 Size of SPI flash required to be erased.
 * @note         It is blocking function \n  
* @warning	       
*                 - Address (offset) plus size of data must not exceed flash size.\n
*                 - No firmware is required for writing to SPI flash.\n
 *                 - In case of there is a running firmware, it is required to pause your firmware first 
 *                   before any trial to access SPI flash to avoid any racing between host and running firmware on bus using 
 *                   @ref m2m_wifi_download_mode
 *                 - It is blocking function\n
 * @sa             m2m_wifi_download_mode, spi_flash_get_size
 * @return       The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.

 */
sint8 spi_flash_erase(uint32 u32Offset, uint32 u32Sz);
 /**@}*/
#endif	//__SPI_FLASH_H__
