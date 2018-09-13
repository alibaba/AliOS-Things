/**
  * @file  hal_flash.h
  * @author  XRADIO IOT WLAN Team
  */
  
/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAL_FLASH_H_
#define HAL_FLASH_H_

#include <stdlib.h>
#include <stdio.h>
#include "flashchip/flash_chip.h"
#include "driver/chip/hal_spi.h"

#define FLASH_FLASHC_ENABLE (1)
#define FLASH_SPI_ENABLE (!defined(__CONFIG_BOOTLOADER))

typedef struct FlashDrvierBase FlashDrvierBase;

typedef HAL_Status (*FlashDriverFunc)(FlashDrvierBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);

struct FlashDrvierBase
{
	/*
		attribute
	*/
	int dev;
	uint32_t sizeToDma;

	/*
		public interface
	*/
	FlashDriverFunc write;
	FlashDriverFunc read;
	HAL_Status (*open)(FlashDrvierBase *base);
	HAL_Status (*close)(FlashDrvierBase *base);
	HAL_Status (*setFreq)(FlashDrvierBase *base, uint32_t freq);
	void (*msleep)(FlashDrvierBase *base, uint32_t ms);
	void (*destroy)(FlashDrvierBase *);
};

FlashDrvierBase *FlashDriverCreator(int driver);
int FlashDriverDestory(FlashDrvierBase *base);

/*
	Flash Board Config
*/
enum FlashBoardType
{
#if FLASH_FLASHC_ENABLE
	FLASH_DRV_FLASHC,	/*!< flash controller driver */
#endif
#if FLASH_SPI_ENABLE
	FLASH_DRV_SPI        /*!< spi driver */
#endif
};

typedef struct FlashcBoardCfg
{
	uint32_t clk;	/*!< flash clock */
} FlashcBoardCfg;

typedef struct SpiBoardCfg
{
	uint32_t clk;   /*!< flash clock */
	SPI_Port port;	/*!< spi port */
	SPI_CS cs;      /*!< cs pin */
} SpiBoardCfg;

typedef struct FlashBoardCfg
{
	enum FlashBoardType type;   /*!< flash driver */
	FlashReadMode mode;         /*!< read mode to flash */
	union {
		FlashcBoardCfg flashc;	/*!< flash driver controller configuration. Notice!! flashc support all read mode */
		SpiBoardCfg spi; 		/*!< spi driver configuration. Notice!! spi only support normal read, fast read, dual output mode */
	};
} FlashBoardCfg;


/*
	Flash Global Interface
*/
typedef struct FlashDev FlashDev;

typedef enum FlashControlCmd
{
	FLASH_GET_MIN_ERASE_SIZE,
	/*TODO: tbc...*/
} FlashControlCmd;

HAL_Status HAL_Flash_Init(uint32_t flash);

HAL_Status HAL_Flash_Deinit(uint32_t flash);

HAL_Status HAL_Flash_Open(uint32_t flash, uint32_t timeout_ms);

HAL_Status HAL_Flash_Close(uint32_t flash);

HAL_Status HAL_Flash_Control(uint32_t flash, FlashControlCmd attr, uint32_t arg);

HAL_Status HAL_Flash_Overwrite(uint32_t flash, uint32_t addr, uint8_t *data, uint32_t size);

HAL_Status HAL_Flash_Write(uint32_t flash, uint32_t addr, uint8_t *data, uint32_t size);

HAL_Status HAL_Flash_Read(uint32_t flash, uint32_t addr, uint8_t *data, uint32_t size);

HAL_Status HAL_Flash_Erase(uint32_t flash, FlashEraseMode blk_size, uint32_t addr, uint32_t blk_cnt);

HAL_Status HAL_Flash_MemoryOf(uint32_t flash, FlashEraseMode size, uint32_t addr, uint32_t *start);

int HAL_Flash_Check(uint32_t flash, uint32_t addr, uint8_t *data, uint32_t size);

#endif /* HAL_FLASH_H_ */
