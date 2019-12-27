/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SOC_INIT_H
#define __SOC_INIT_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "aos/hal/i2c.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "aos/hal/uart.h"

#include "hal/hal_gpio_stm32l4.h"
#include "hal/hal_i2c_stm32l4.h"
#include "hal/hal_uart_stm32l4.h"

#ifdef ARDUINO_SPI_I2C_ENABLED
#include "hal/hal_spi_stm32l4.h"
#endif
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

typedef enum {
    GPIO_ALS_INT,
    GPIO_AUDIO_CTL,
    GPIO_AUDIO_RST,
    GPIO_AUDIO_WU,
    GPIO_CAM_PD,
    GPIO_CAM_RST,
    GPIO_LED_1,
    GPIO_LED_2,
    GPIO_LED_3,
    GPIO_KEY_1,
    GPIO_KEY_2,
    GPIO_KEY_3,
    GPIO_LCD_DCX,
    GPIO_LCD_PWR,
    GPIO_LCD_RST,
    GPIO_PCIE_RST,
    GPIO_SECURE_CLK,
    GPIO_SECURE_IO,
    GPIO_SECURE_RST,
    GPIO_SIM_DET,
    GPIO_USB_PCIE_SW,
    GPIO_WIFI_RST,
    GPIO_WIFI_WU,
    GPIO_ZIGBEE_INT,
    GPIO_ZIGBEE_RST,
    MAX_GPIO_NUM
} BOARD_GPIO;

typedef enum {
	BOARD_HW_UNKNOW = 0,
	BOARD_HW_VER12,
	BOARD_HW_VER13,
	BOARD_HW_END
} BOARD_HW_VERSION;

extern gpio_dev_t brd_gpio_table[];
extern i2c_dev_t brd_i2c2_dev;
extern i2c_dev_t brd_i2c3_dev;
extern i2c_dev_t brd_i2c4_dev;
extern uart_dev_t uart_0;

extern BOARD_HW_VERSION get_devloperkit_hwver(void);
extern int get_devloperkit_atuart(void);

#define KIDS_A10_PRT(fmt, args...)  \
  printf("%s: [%s-->%d]=> "fmt,   \
  __FILE__, __FUNCTION__,         \
  __LINE__, ##args)
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

#endif /* __SOC_INIT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
