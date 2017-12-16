/**
  ******************************************************************************
  * @file    mxconstants.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
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
#ifndef __MXCONSTANT_H
#define __MXCONSTANT_H
  /* Includes ------------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define M24SR64_Y_RF_DISABLE_Pin GPIO_PIN_2
#define M24SR64_Y_RF_DISABLE_GPIO_Port GPIOE
#define USB_OTG_OVCR_EXTI3_Pin GPIO_PIN_3
#define USB_OTG_OVCR_EXTI3_GPIO_Port GPIOE
#define M24SR64_Y_GPO_Pin GPIO_PIN_4
#define M24SR64_Y_GPO_GPIO_Port GPIOE
#define SPSGRF_915_GPIO3_EXTI5_Pin GPIO_PIN_5
#define SPSGRF_915_GPIO3_EXTI5_GPIO_Port GPIOE
#define SPBTLE_RF_IRQ_EXTI6_Pin GPIO_PIN_6
#define SPBTLE_RF_IRQ_EXTI6_GPIO_Port GPIOE
#define BUTTON_EXTI13_Pin GPIO_PIN_13
#define BUTTON_EXTI13_GPIO_Port GPIOC
#define ARD_A5_ADC_Pin GPIO_PIN_0
#define ARD_A5_ADC_GPIO_Port GPIOC
#define ARD_A4_ADC_Pin GPIO_PIN_1
#define ARD_A4_ADC_GPIO_Port GPIOC
#define ARD_A3_ADC_Pin GPIO_PIN_2
#define ARD_A3_ADC_GPIO_Port GPIOC
#define ARD_A2_ADC_Pin GPIO_PIN_3
#define ARD_A2_ADC_GPIO_Port GPIOC
#define ARD_D1_UART4_TX_Pin GPIO_PIN_0
#define ARD_D1_UART4_TX_GPIO_Port GPIOA
#define ARD_D0_UART4_RX_Pin GPIO_PIN_1
#define ARD_D0_UART4_RX_GPIO_Port GPIOA
#define ARD_D10_SPI_SSN_PWM_Pin GPIO_PIN_2
#define ARD_D10_SPI_SSN_PWM_GPIO_Port GPIOA
#define ARD_D4_Pin GPIO_PIN_3
#define ARD_D4_GPIO_Port GPIOA
#define ARD_D7_Pin GPIO_PIN_4
#define ARD_D7_GPIO_Port GPIOA
#define ARD_D12_SPI1_MISO_Pin GPIO_PIN_6
#define ARD_D12_SPI1_MISO_GPIO_Port GPIOA
#define ARD_D11_SPI1_MOSI_PWM_Pin GPIO_PIN_7
#define ARD_D11_SPI1_MOSI_PWM_GPIO_Port GPIOA
#define ARD_A1_ADC_Pin GPIO_PIN_4
#define ARD_A1_ADC_GPIO_Port GPIOC
#define ARD_A0_ADC_Pin GPIO_PIN_5
#define ARD_A0_ADC_GPIO_Port GPIOC
#define ARD_D3_PWM_INT1_EXTI0_Pin GPIO_PIN_0
#define ARD_D3_PWM_INT1_EXTI0_GPIO_Port GPIOB
#define ARD_D6_PWM_Pin GPIO_PIN_1
#define ARD_D6_PWM_GPIO_Port GPIOB
#define ARD_D8_Pin GPIO_PIN_2
#define ARD_D8_GPIO_Port GPIOB
#define ISM43362_RST_Pin GPIO_PIN_8
#define ISM43362_RST_GPIO_Port GPIOE
#define INTERNAL_I2C2_SCL_Pin GPIO_PIN_10
#define INTERNAL_I2C2_SCL_GPIO_Port GPIOB
#define INTERNAL_I2C2_SDA_Pin GPIO_PIN_11
#define INTERNAL_I2C2_SDA_GPIO_Port GPIOB
#define ISM43362_BOOT0_Pin GPIO_PIN_12
#define ISM43362_BOOT0_GPIO_Port GPIOB
#define ISM43362_WAKEUP_Pin GPIO_PIN_13
#define ISM43362_WAKEUP_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOB
#define SPSGRF_915_SDN_Pin GPIO_PIN_15
#define SPSGRF_915_SDN_GPIO_Port GPIOB
#define INTERNAL_UART3_TX_Pin GPIO_PIN_8
#define INTERNAL_UART3_TX_GPIO_Port GPIOD
#define INTERNAL_UART3_RX_Pin GPIO_PIN_9
#define INTERNAL_UART3_RX_GPIO_Port GPIOD
#define LPS22HB_INT_DRDY_EXTI10_Pin GPIO_PIN_10
#define LPS22HB_INT_DRDY_EXTI10_GPIO_Port GPIOD
#define LSM6DSL_INT1_EXTI11_Pin GPIO_PIN_11
#define LSM6DSL_INT1_EXTI11_GPIO_Port GPIOD
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_12
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define SPBTLE_RF_SPI3_CSN_Pin GPIO_PIN_13
#define SPBTLE_RF_SPI3_CSN_GPIO_Port GPIOD
#define ARD_D2_INT0_EXTI14_Pin GPIO_PIN_14
#define ARD_D2_INT0_EXTI14_GPIO_Port GPIOD
#define HTS221_DRDY_EXTI15_Pin GPIO_PIN_15
#define HTS221_DRDY_EXTI15_GPIO_Port GPIOD
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC
#define VL53L0X_GPIO1_EXTI7_Pin GPIO_PIN_7
#define VL53L0X_GPIO1_EXTI7_GPIO_Port GPIOC
#define LIS3MDL_DRDY_EXTI8_Pin GPIO_PIN_8
#define LIS3MDL_DRDY_EXTI8_GPIO_Port GPIOC
#define SPBTLE_RF_RST_Pin GPIO_PIN_8
#define SPBTLE_RF_RST_GPIO_Port GPIOA
#define ARD_D9_PWM_Pin GPIO_PIN_15
#define ARD_D9_PWM_GPIO_Port GPIOA
#define INTERNAL_SPI3_SCK_Pin GPIO_PIN_10
#define INTERNAL_SPI3_SCK_GPIO_Port GPIOC
#define INTERNAL_SPI3_MISO_Pin GPIO_PIN_11
#define INTERNAL_SPI3_MISO_GPIO_Port GPIOC
#define INTERNAL_SPI3_MOSI_Pin GPIO_PIN_12
#define INTERNAL_SPI3_MOSI_GPIO_Port GPIOC
#define PMOD_RESET_Pin GPIO_PIN_0
#define PMOD_RESET_GPIO_Port GPIOD
#define PMOD_SPI2_SCK_Pin GPIO_PIN_1
#define PMOD_SPI2_SCK_GPIO_Port GPIOD
#define PMOD_IRQ_EXTI2_Pin GPIO_PIN_2
#define PMOD_IRQ_EXTI2_GPIO_Port GPIOD
#define PMOD_UART2_CTS_SPI2_MISO_Pin GPIO_PIN_3
#define PMOD_UART2_CTS_SPI2_MISO_GPIO_Port GPIOD
#define PMOD_UART2_RTS_SPI2_MOSI_Pin GPIO_PIN_4
#define PMOD_UART2_RTS_SPI2_MOSI_GPIO_Port GPIOD
#define PMOD_UART2_TX_SPI2_CSN_Pin GPIO_PIN_5
#define PMOD_UART2_TX_SPI2_CSN_GPIO_Port GPIOD
#define PMOD_UART2_RX_Pin GPIO_PIN_6
#define PMOD_UART2_RX_GPIO_Port GPIOD
#define STSAFE_A100_RESET_Pin GPIO_PIN_7
#define STSAFE_A100_RESET_GPIO_Port GPIOD
#define ARD_D5_PWM_Pin GPIO_PIN_4
#define ARD_D5_PWM_GPIO_Port GPIOB
#define SPSGRF_915_SPI3_CSN_Pin GPIO_PIN_5
#define SPSGRF_915_SPI3_CSN_GPIO_Port GPIOB
#define ST_LINK_UART1_TX_Pin GPIO_PIN_6
#define ST_LINK_UART1_TX_GPIO_Port GPIOB
#define ST_LINK_UART1_RX_Pin GPIO_PIN_7
#define ST_LINK_UART1_RX_GPIO_Port GPIOB
#define ARD_D15_I2C1_SCL_Pin GPIO_PIN_8
#define ARD_D15_I2C1_SCL_GPIO_Port GPIOB
#define ARD_D14_I2C1_SDA_Pin GPIO_PIN_9
#define ARD_D14_I2C1_SDA_GPIO_Port GPIOB
#define ISM43362_SPI3_CSN_Pin GPIO_PIN_0
#define ISM43362_SPI3_CSN_GPIO_Port GPIOE
#define ISM43362_DRDY_EXTI1_Pin GPIO_PIN_1
#define ISM43362_DRDY_EXTI1_GPIO_Port GPIOE

#endif /* __MXCONSTANT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
