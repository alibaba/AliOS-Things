/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup SPI_Driver_APIs SPI Driver APIs
  * @brief SPI Flash APIs
  */

/** @addtogroup SPI_Driver_APIs
  * @{
  */

typedef enum {
    SPI_FLASH_RESULT_OK,        /**< SPI Flash operating OK */
    SPI_FLASH_RESULT_ERR,       /**< SPI Flash operating fail */
    SPI_FLASH_RESULT_TIMEOUT    /**< SPI Flash operating time out */
} SpiFlashOpResult;

typedef struct{
    uint32  deviceId;
    uint32  chip_size;    // chip size in byte
    uint32  block_size;
    uint32  sector_size;
    uint32  page_size;
    uint32  status_mask;
} SpiFlashChip;

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

/**
  * @brief  Get ID info of SPI Flash.
  *
  * @param  null
  *
  * @return SPI Flash ID
  */
uint32 spi_flash_get_id(void);

/**
  * @brief  Read state register of SPI Flash.
  *
  * @param  uint32 *status : the read value (pointer) of state register.
  *
  * @return SpiFlashOpResult
  */
SpiFlashOpResult spi_flash_read_status(uint32 *status);

/**
  * @brief  Write state register of SPI Flash.
  *
  * @param  uint32 status_value : Write state register value.
  *
  * @return SpiFlashOpResult
  */
SpiFlashOpResult spi_flash_write_status(uint32 status_value);

/**
  * @brief  Erase the Flash sector.
  *
  * @param  uint16 sec : Sector number, the count starts at sector 0, 4KB per sector.
  *
  * @return SpiFlashOpResult
  */
SpiFlashOpResult spi_flash_erase_sector(uint16 sec);

/**
  * @brief  Write data to Flash.
  *
  * @param  uint32 des_addr  : destination address in Flash.
  * @param  uint32 *src_addr : source address of the data.
  * @param  uint32 size      : length of data
  *
  * @return SpiFlashOpResult
  */
SpiFlashOpResult spi_flash_write(uint32 des_addr, uint32 *src_addr, uint32 size);

/**
  * @brief  Read data from Flash.
  *
  * @param  uint32 src_addr  : source address of the data.
  * @param  uint32 *des_addr : destination address in Flash.
  * @param  uint32 size      : length of data
  *
  * @return SpiFlashOpResult
  */
SpiFlashOpResult spi_flash_read(uint32 src_addr, uint32 *des_addr, uint32 size);

/**
  * @brief  Registered function for spi_flash_set_read_func.
  *
  * @attention  used for sdk internal, don't need to care about params
  *
  * @param  SpiFlashChip *spi : spi flash struct pointer.
  * @param  uint32 src_addr  : source address of the data.
  * @param  uint32 *des_addr : destination address in Flash.
  * @param  uint32 size      : length of data
  *
  * @return SpiFlashOpResult
  */
typedef SpiFlashOpResult (* user_spi_flash_read)(
        SpiFlashChip *spi,
        uint32 src_addr,
        uint32 *des_addr,
        uint32 size);

/**
  * @brief  Register user-define SPI flash read API.
  *
  * @attention This API can be only used in SPI overlap mode, please refer to ESP8266_RTOS_SDK
  *            \examples\driver_lib\driver\spi_overlap.c
  *
  * @param  user_spi_flash_read read : user-define SPI flash read API .
  *
  * @return  none
  */
void spi_flash_set_read_func(user_spi_flash_read read);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
