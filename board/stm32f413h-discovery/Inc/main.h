/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/
#include <aos/aos.h>
#include "stm32f4xx_hal.h"

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define QSPI_BK1_IO2_Pin GPIO_PIN_2
#define QSPI_BK1_IO2_GPIO_Port GPIOE
#define LED1_RED_Pin GPIO_PIN_3
#define LED1_RED_GPIO_Port GPIOE
#define MEMS_LED_Pin GPIO_PIN_4
#define MEMS_LED_GPIO_Port GPIOE
#define LCD_BL_CTRL_Pin GPIO_PIN_5
#define LCD_BL_CTRL_GPIO_Port GPIOE
#define ARD_D5_Pin GPIO_PIN_6
#define ARD_D5_GPIO_Port GPIOE
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define PSRAM_A0_Pin GPIO_PIN_0
#define PSRAM_A0_GPIO_Port GPIOF
#define PSRAM_A1_Pin GPIO_PIN_1
#define PSRAM_A1_GPIO_Port GPIOF
#define PSRAM_A2_Pin GPIO_PIN_2
#define PSRAM_A2_GPIO_Port GPIOF
#define PSRAM_A3_Pin GPIO_PIN_3
#define PSRAM_A3_GPIO_Port GPIOF
#define PSRAM_A4_Pin GPIO_PIN_4
#define PSRAM_A4_GPIO_Port GPIOF
#define PSRAM_A5_Pin GPIO_PIN_5
#define PSRAM_A5_GPIO_Port GPIOF
#define ARD_D0_Pin GPIO_PIN_6
#define ARD_D0_GPIO_Port GPIOF
#define ARD_D1_Pin GPIO_PIN_7
#define ARD_D1_GPIO_Port GPIOF
#define QSPI_BK1_IO0_Pin GPIO_PIN_8
#define QSPI_BK1_IO0_GPIO_Port GPIOF
#define QSPI_BK1_IO1_Pin GPIO_PIN_9
#define QSPI_BK1_IO1_GPIO_Port GPIOF
#define ARD_D3_Pin GPIO_PIN_10
#define ARD_D3_GPIO_Port GPIOF
#define STLK_MCO_Pin GPIO_PIN_0
#define STLK_MCO_GPIO_Port GPIOH
#define WIFI_RST_Pin GPIO_PIN_1
#define WIFI_RST_GPIO_Port GPIOH
#define ARD_A0_Pin GPIO_PIN_0
#define ARD_A0_GPIO_Port GPIOC
#define CTP_INT_Pin GPIO_PIN_1
#define CTP_INT_GPIO_Port GPIOC
#define CODEC_ext_SD_Pin GPIO_PIN_2
#define CODEC_ext_SD_GPIO_Port GPIOC
#define CODEC_SD_Pin GPIO_PIN_3
#define CODEC_SD_GPIO_Port GPIOC
#define B_USER_Pin GPIO_PIN_0
#define B_USER_GPIO_Port GPIOA
#define ARD_A1_Pin GPIO_PIN_1
#define ARD_A1_GPIO_Port GPIOA
#define ARD_A2_Pin GPIO_PIN_2
#define ARD_A2_GPIO_Port GPIOA
#define CODEC_MCK_Pin GPIO_PIN_3
#define CODEC_MCK_GPIO_Port GPIOA
#define ARD_D8_Pin GPIO_PIN_4
#define ARD_D8_GPIO_Port GPIOA
#define ARD_A3_Pin GPIO_PIN_5
#define ARD_A3_GPIO_Port GPIOA
#define SD_CMD_Pin GPIO_PIN_6
#define SD_CMD_GPIO_Port GPIOA
#define DFSDM2_DATIN1_Pin GPIO_PIN_7
#define DFSDM2_DATIN1_GPIO_Port GPIOA
#define ARD_A5_Pin GPIO_PIN_4
#define ARD_A5_GPIO_Port GPIOC
#define LED2_GREEN_Pin GPIO_PIN_5
#define LED2_GREEN_GPIO_Port GPIOC
#define ARD_D6_Pin GPIO_PIN_0
#define ARD_D6_GPIO_Port GPIOB
#define ARD_A4_Pin GPIO_PIN_1
#define ARD_A4_GPIO_Port GPIOB
#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define SD_Detect_Pin GPIO_PIN_11
#define SD_Detect_GPIO_Port GPIOF
#define PSRAM_A6_Pin GPIO_PIN_12
#define PSRAM_A6_GPIO_Port GPIOF
#define PSRAM_A7_Pin GPIO_PIN_13
#define PSRAM_A7_GPIO_Port GPIOF
#define PSRAM_A8_Pin GPIO_PIN_14
#define PSRAM_A8_GPIO_Port GPIOF
#define PSRAM_A9_Pin GPIO_PIN_15
#define PSRAM_A9_GPIO_Port GPIOF
#define PSRAM_A10_Pin GPIO_PIN_0
#define PSRAM_A10_GPIO_Port GPIOG
#define PSRAM_A11_Pin GPIO_PIN_1
#define PSRAM_A11_GPIO_Port GPIOG
#define LCD_PSRAM_D4_Pin GPIO_PIN_7
#define LCD_PSRAM_D4_GPIO_Port GPIOE
#define LCD_PSRAM_D5_Pin GPIO_PIN_8
#define LCD_PSRAM_D5_GPIO_Port GPIOE
#define LCD_PSRAM_D6_Pin GPIO_PIN_9
#define LCD_PSRAM_D6_GPIO_Port GPIOE
#define LCD_PSRAM_D7_Pin GPIO_PIN_10
#define LCD_PSRAM_D7_GPIO_Port GPIOE
#define LCD_PSRAM_D8_Pin GPIO_PIN_11
#define LCD_PSRAM_D8_GPIO_Port GPIOE
#define LCD_PSRAM_D9_Pin GPIO_PIN_12
#define LCD_PSRAM_D9_GPIO_Port GPIOE
#define LCD_PSRAM_D10_Pin GPIO_PIN_13
#define LCD_PSRAM_D10_GPIO_Port GPIOE
#define LCD_PSRAM_D11_Pin GPIO_PIN_14
#define LCD_PSRAM_D11_GPIO_Port GPIOE
#define LCD_PSRAM_D12_Pin GPIO_PIN_15
#define LCD_PSRAM_D12_GPIO_Port GPIOE
#define ARD_D15_Pin GPIO_PIN_10
#define ARD_D15_GPIO_Port GPIOB
#define ARD_D14_Pin GPIO_PIN_11
#define ARD_D14_GPIO_Port GPIOB
#define ARD_D13_Pin GPIO_PIN_12
#define ARD_D13_GPIO_Port GPIOB
#define LCD_CTP_RST_Pin GPIO_PIN_13
#define LCD_CTP_RST_GPIO_Port GPIOB
#define LCD_TE_Pin GPIO_PIN_14
#define LCD_TE_GPIO_Port GPIOB
#define WIFI_WKUP_Pin GPIO_PIN_15
#define WIFI_WKUP_GPIO_Port GPIOB
#define LCD_PSRAM_D13_Pin GPIO_PIN_8
#define LCD_PSRAM_D13_GPIO_Port GPIOD
#define LCD_PSRAM_D14_Pin GPIO_PIN_9
#define LCD_PSRAM_D14_GPIO_Port GPIOD
#define LCd_PSRAM_D15_Pin GPIO_PIN_10
#define LCd_PSRAM_D15_GPIO_Port GPIOD
#define PSRAM_A16_Pin GPIO_PIN_11
#define PSRAM_A16_GPIO_Port GPIOD
#define PSRAM_A17_Pin GPIO_PIN_12
#define PSRAM_A17_GPIO_Port GPIOD
#define QSPI_BK1_IO3_Pin GPIO_PIN_13
#define QSPI_BK1_IO3_GPIO_Port GPIOD
#define LCD_PSRAM_D0_Pin GPIO_PIN_14
#define LCD_PSRAM_D0_GPIO_Port GPIOD
#define LCD_PSRAM_D1_Pin GPIO_PIN_15
#define LCD_PSRAM_D1_GPIO_Port GPIOD
#define PSRAM_A12_Pin GPIO_PIN_2
#define PSRAM_A12_GPIO_Port GPIOG
#define PSRAM_A13_Pin GPIO_PIN_3
#define PSRAM_A13_GPIO_Port GPIOG
#define PSRAM_A14_Pin GPIO_PIN_4
#define PSRAM_A14_GPIO_Port GPIOG
#define PSRAM_A15_Pin GPIO_PIN_5
#define PSRAM_A15_GPIO_Port GPIOG
#define QSPI_BK1_NCS_Pin GPIO_PIN_6
#define QSPI_BK1_NCS_GPIO_Port GPIOG
#define USB_OTG_FS_OVRCR_Pin GPIO_PIN_7
#define USB_OTG_FS_OVRCR_GPIO_Port GPIOG
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_8
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOG
#define I2CFMP1_SCL_Pin GPIO_PIN_6
#define I2CFMP1_SCL_GPIO_Port GPIOC
#define I2CFMP_SDA_Pin GPIO_PIN_7
#define I2CFMP_SDA_GPIO_Port GPIOC
#define SD_D0_Pin GPIO_PIN_8
#define SD_D0_GPIO_Port GPIOC
#define SD_D1_Pin GPIO_PIN_9
#define SD_D1_GPIO_Port GPIOC
#define DFSDM1_CKOUT_Pin GPIO_PIN_8
#define DFSDM1_CKOUT_GPIO_Port GPIOA
#define USB_OTG_FS_VBUS_Pin GPIO_PIN_9
#define USB_OTG_FS_VBUS_GPIO_Port GPIOA
#define USB_OTG_FS_ID_Pin GPIO_PIN_10
#define USB_OTG_FS_ID_GPIO_Port GPIOA
#define USB_OTG_FS_DM_Pin GPIO_PIN_11
#define USB_OTG_FS_DM_GPIO_Port GPIOA
#define USB_OTG_FS_DP_Pin GPIO_PIN_12
#define USB_OTG_FS_DP_GPIO_Port GPIOA
#define STLINK_JTMS_SWDIO_Pin GPIO_PIN_13
#define STLINK_JTMS_SWDIO_GPIO_Port GPIOA
#define STLINK_JTMS_SWCLK_Pin GPIO_PIN_14
#define STLINK_JTMS_SWCLK_GPIO_Port GPIOA
#define ARD_D10_Pin GPIO_PIN_15
#define ARD_D10_GPIO_Port GPIOA
#define SD_D2_Pin GPIO_PIN_10
#define SD_D2_GPIO_Port GPIOC
#define SD_D3_Pin GPIO_PIN_11
#define SD_D3_GPIO_Port GPIOC
#define SD_CLK_Pin GPIO_PIN_12
#define SD_CLK_GPIO_Port GPIOC
#define LCD_PSRAM_D2_Pin GPIO_PIN_0
#define LCD_PSRAM_D2_GPIO_Port GPIOD
#define LCD_PSRAM_D3_Pin GPIO_PIN_1
#define LCD_PSRAM_D3_GPIO_Port GPIOD
#define DFSDM2_CKOUT_Pin GPIO_PIN_2
#define DFSDM2_CKOUT_GPIO_Port GPIOD
#define CODEC_CK_Pin GPIO_PIN_3
#define CODEC_CK_GPIO_Port GPIOD
#define LCD_PSRAM_NOE_Pin GPIO_PIN_4
#define LCD_PSRAM_NOE_GPIO_Port GPIOD
#define LCD_PSRAM_NWE_Pin GPIO_PIN_5
#define LCD_PSRAM_NWE_GPIO_Port GPIOD
#define DFSDM1_DATIN1_Pin GPIO_PIN_6
#define DFSDM1_DATIN1_GPIO_Port GPIOD
#define PSRAM_NE1_Pin GPIO_PIN_7
#define PSRAM_NE1_GPIO_Port GPIOD
#define VCP_RX_Pin GPIO_PIN_9
#define VCP_RX_GPIO_Port GPIOG
#define LCD_NE3_Pin GPIO_PIN_10
#define LCD_NE3_GPIO_Port GPIOG
#define WIFI_SPI_CSN_Pin GPIO_PIN_11
#define WIFI_SPI_CSN_GPIO_Port GPIOG
#define WIFI_DRDY_Pin GPIO_PIN_12
#define WIFI_DRDY_GPIO_Port GPIOG
#define ARD_D2_Pin GPIO_PIN_13
#define ARD_D2_GPIO_Port GPIOG
#define VCP_TX_Pin GPIO_PIN_14
#define VCP_TX_GPIO_Port GPIOG
#define CODEC_INT_Pin GPIO_PIN_15
#define CODEC_INT_GPIO_Port GPIOG
#define STLINK_JTDO_SWCLK_Pin GPIO_PIN_3
#define STLINK_JTDO_SWCLK_GPIO_Port GPIOB
#define ARD_D12_Pin GPIO_PIN_4
#define ARD_D12_GPIO_Port GPIOB
#define ARD_D11_Pin GPIO_PIN_5
#define ARD_D11_GPIO_Port GPIOB
#define ARD_D4_Pin GPIO_PIN_6
#define ARD_D4_GPIO_Port GPIOB
#define DFSDM2_DATIN7_Pin GPIO_PIN_7
#define DFSDM2_DATIN7_GPIO_Port GPIOB
#define ARD_D9_Pin GPIO_PIN_8
#define ARD_D9_GPIO_Port GPIOB
#define CODEC_WS_Pin GPIO_PIN_9
#define CODEC_WS_GPIO_Port GPIOB
#define PSRAM_NBL0_Pin GPIO_PIN_0
#define PSRAM_NBL0_GPIO_Port GPIOE
#define PSRAM_NBL1_Pin GPIO_PIN_1
#define PSRAM_NBL1_GPIO_Port GPIOE

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
