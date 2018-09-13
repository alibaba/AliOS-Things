/**
  * @file  hal_sdhost.h
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

#ifndef _DRIVER_CHIP_HAL_SDMMC_SDHOST_H_
#define _DRIVER_CHIP_HAL_SDMMC_SDHOST_H_

#include "driver/chip/hal_def.h"
#ifdef __CONFIG_ARCH_APP_CORE
#include "driver/chip/hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @bried Detect card by gpio irq or D3. */
//#define CONFIG_DETECT_CARD      1

typedef enum
{
	SDCGPIO_BAS = 0,
	SDCGPIO_DET = 1,
} HAL_SDCGPIOType;

typedef struct {
	uint8_t   data_bits;
	int8_t    has_detect_gpio;
	GPIO_Port detect_port;
	GPIO_Pin  detect_pin;
} HAL_SDCGPIOCfg;

/** @bried Detect card callback if used CONFIG_DETECT_CARD. */
typedef void (*card_detect_cb)(uint32_t present);

/** @bried SDC Init Structure definition. */
typedef struct {
#ifdef CONFIG_DETECT_CARD
	uint32_t                cd_mode;
/* NOTE: The specification advise that CARD_DETECT_BY_D3 is not a preferred
 *       mechanism for card detection. Moreover it won't work with MMC cards.
 *       And, this won't work with external pull-up resistors on the card interface.
 *       The preferred card detection mechanism is a mechanical switch on the card connector.
*/
#define CARD_DETECT_BY_GPIO_IRQ  (2)    /* mmc detected by gpio irq */
#define CARD_ALWAYS_PRESENT      (3)    /* mmc always present, without detect pin */
#define CARD_DETECT_BY_FS        (4)    /* mmc insert/remove by fs */
#define CARD_DETECT_BY_D3        (5)    /* mmc detected by data3 */

	card_detect_cb          cd_cb;  /* NOTE: should delay 500ms before rescan card to wait Voltage stable */
#endif
} SDC_InitTypeDef;

/**
 * @brief Initializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id->SDC ID.
 * @param param:
 *        @arg param->[in] The configuration information.
 * @retval  SDC handler.
 */
extern struct mmc_host *HAL_SDC_Init(uint32_t sdc_id, SDC_InitTypeDef *param);
#else
/**
 * @brief Initializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id->SDC ID.
 * @retval  SDC handler.
 */
extern struct mmc_host *HAL_SDC_Init(uint32_t sdc_id);
#endif

/**
 * @brief DeInitializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id-> SDC ID.
 * @retval  None.
 */
extern int32_t HAL_SDC_Deinit(uint32_t sdc_id);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_SDMMC_SDHOST_H_ */
