/**
  ******************************************************************************
  * @file    swm320xx_hal_conf.h
  * @brief   HAL configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 SYNWITMicroelectronics</center></h2>
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
#ifndef __SWM320xx_HAL_CONF_H
#define __SWM320xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

//#include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ########################## Module Selection ############################## */
/**
  * @brief This is the list of modules to be used in the HAL driver
  */

#define HAL_MODULE_ENABLED
//#define HAL_ADC_MODULE_ENABLED
/*#define HAL_CRYP_MODULE_ENABLED   */
/*#define HAL_CAN_MODULE_ENABLED   */
/*#define HAL_COMP_MODULE_ENABLED   */
/*#define HAL_CRC_MODULE_ENABLED   */
/*#define HAL_CRYP_MODULE_ENABLED   */
/*#define HAL_DAC_MODULE_ENABLED   */
//#define HAL_DCMI_MODULE_ENABLED
/*#define HAL_DMA2D_MODULE_ENABLED   */
//#define HAL_DFSDM_MODULE_ENABLED
/*#define HAL_DSI_MODULE_ENABLED   */
/*#define HAL_FIREWALL_MODULE_ENABLED   */
/*#define HAL_GFXMMU_MODULE_ENABLED   */
/*#define HAL_HCD_MODULE_ENABLED   */
/*#define HAL_HASH_MODULE_ENABLED   */
/*#define HAL_I2S_MODULE_ENABLED   */
/*#define HAL_IRDA_MODULE_ENABLED   */
/*#define HAL_IWDG_MODULE_ENABLED   */
/*#define HAL_LTDC_MODULE_ENABLED   */
/*#define HAL_LCD_MODULE_ENABLED   */
/*#define HAL_LPTIM_MODULE_ENABLED   */
/*#define HAL_NAND_MODULE_ENABLED   */
/*#define HAL_NOR_MODULE_ENABLED   */
/*#define HAL_OPAMP_MODULE_ENABLED   */
/*#define HAL_OSPI_MODULE_ENABLED   */
/*#define HAL_OSPI_MODULE_ENABLED   */
//#define HAL_PCD_MODULE_ENABLED
/*#define HAL_QSPI_MODULE_ENABLED   */
//#define HAL_QSPI_MODULE_ENABLED
/*#define HAL_RNG_MODULE_ENABLED   */
/*#define HAL_RTC_MODULE_ENABLED   */
//#define HAL_SAI_MODULE_ENABLED
//#define HAL_SD_MODULE_ENABLED
/*#define HAL_SMBUS_MODULE_ENABLED   */
/*#define HAL_SMARTCARD_MODULE_ENABLED   */
//#define HAL_SPI_MODULE_ENABLED
//#define HAL_SRAM_MODULE_ENABLED
/*#define HAL_SWPMI_MODULE_ENABLED   */
/*#define HAL_TIM_MODULE_ENABLED   */
/*#define HAL_TSC_MODULE_ENABLED   */
#define HAL_UART_MODULE_ENABLED
/*#define HAL_USART_MODULE_ENABLED   */
/*#define HAL_WWDG_MODULE_ENABLED   */
/*#define HAL_EXTI_MODULE_ENABLED   */
//#define HAL_GPIO_MODULE_ENABLED
//#define HAL_I2C_MODULE_ENABLED
//#define HAL_DMA_MODULE_ENABLED
//#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
//#define HAL_PWR_MODULE_ENABLED
//#define HAL_CORTEX_MODULE_ENABLED
//#define HAL_PORT_MODULE_ENABLED


/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file
  */
#include "SWM320.h"

#ifdef HAL_RCC_MODULE_ENABLED
  #include "hal_rcc_swm320.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_EXTI_MODULE_ENABLED
  #include "hal_exit_swm320.h"
#endif /* HAL_EXTI_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "hal_gpio_swm320.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
  #include "hal_dma_swm320.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_ADC_MODULE_ENABLED
  #include "hal_adc_swm320.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_CAN_MODULE_ENABLED
  #include "hal_can_swm320.h"
#endif /* HAL_CAN_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
  #include "hal_crc_swm320.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "hal_flash_swm320.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_NOR_MODULE_ENABLED
  #include "hal_nor_swm320.h"
#endif /* HAL_NOR_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
  #include "hal_i2c_swm320.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_LCD_MODULE_ENABLED
  #include "hal_lcd_swm320.h"
#endif /* HAL_LCD_MODULE_ENABLED */

#ifdef HAL_RTC_MODULE_ENABLED
  #include "hal_rtc_swm320.h"
#endif /* HAL_RTC_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
  #include "hal_spi_swm320.h"
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
  #include "hal_timr_swm320.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
  #include "hal_uart_swm320.h"
#endif /* HAL_UART_MODULE_ENABLED */

//#ifdef HAL_PORT_MODULE_ENABLED
//  #include "hal_port_swm320.h"
//#endif
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
