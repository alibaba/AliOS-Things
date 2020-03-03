/**
******************************************************************************
* @file    platform.c
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provides all MICO Peripherals mapping table and platform
*          specific funcgtions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/

//#include "stdio.h"
//#include "string.h"

//#include "platform.h"
//#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
//#include "MicoPlatform.h"
//#include "wlan_platform_common.h"

/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/
extern WEAK void PlatformEasyLinkButtonClickedCallback(void);
extern WEAK void PlatformStandbyButtonClickedCallback(void);
extern WEAK void PlatformEasyLinkButtonLongPressedCallback(void);
extern WEAK void bootloader_start(void);

/******************************************************
*               Variables Definitions
******************************************************/

/* This table maps STM32 pins to GPIO definitions on the schematic
* A full pin definition is provided in <WICED-SDK>/include/platforms/BCM943362WCD4/platform.h
*/

static uint32_t _default_start_time = 0;
static mico_timer_t _button_EL_timer;

platform_gpio_t platform_gpio_pins[] =
{
  /* Common GPIOs for internal use */
 // [MICO_SYS_LED]                      = { GPIOB,  0 },
 // [MICO_RF_LED]                       = { GPIOB,  1 }, //MICO_GPIO_16
 // [BOOT_SEL]                          = { GPIOB,  1 }, //MICO_GPIO_16
//  [MFG_SEL]                           = { GPIOB,  9 }, //MICO_GPIO_30
 // [EasyLink_BUTTON]                   = { GPIOA,  1 }, //MICO_GPIO_11

  /* GPIOs for external use */
  [MICO_GPIO_1]  =
  {
    .pin = PA_5,
  },

  [MICO_GPIO_2]  =
  {
    .pin = PA_12,
  },
};

/*
* Possible compile time inputs:
* - Set which ADC peripheral to use for each ADC. All on one ADC allows sequential conversion on all inputs. All on separate ADCs allows concurrent conversion.
*/
/* TODO : These need fixing */
platform_adc_t platform_adc_peripherals[] =
{
  [PORT_ADC_1]  =
  {
    .pin = AD_1,
  },

  [PORT_ADC_2] =
  {
    .pin = AD_2,
  },

  [PORT_ADC_3] =
  {
    .pin = AD_3,
  },
};


/* PWM mappings */
platform_pwm_t platform_pwm_peripherals[] =
{
  [PORT_PWM_1]  =
  {
    .pin = PA_23,
  },
  [PORT_PWM_2]  =
  {
    .pin = PA_22,
  },
  [PORT_PWM_3]  =
  {
    .pin = PA_0,
  },
  [PORT_PWM_4]  =
  {
    .pin = PA_12,
  },
  /* TODO: fill in the other options here ... */
};

platform_spi_t platform_spi_peripherals[] =
{
  [PORT_SPI_1]  =
  {
    .mosi = PA_4,
    .miso = PA_3,
    .sclk = PA_1,
    .ssel = PA_2,
  },

  [PORT_SPI_2]  =
  {
    .mosi = PA_23,
    .miso = PA_22,
    .sclk = PA_18,
    .ssel = PA_19,
  },
    /**  SPI1 can choose PORT_SPI_2 or PORT_SPI_3
  [PORT_SPI_3]  =
  {
    .mosi = PB_3,
    .miso = PB_2,
    .sclk = PB_1,
    .ssel = PB_0,
  },*/
};

platform_uart_t platform_uart_peripherals[] =
{
  [PORT_UART_1] =
  {
    .tx = PA_30,
    .rx = PA_29,
  },
  [PORT_UART_2] =
  {
    .tx = PA_23,
    .rx = PA_18,
  },
};
platform_uart_driver_t platform_uart_drivers[PORT_UART_MAX];

platform_i2c_t platform_i2c_peripherals[] =
{
  [PORT_I2C_1] =
  {
    .sda = PA_23,
    .scl = PA_18,
  },

  [PORT_I2C_2] =
  {
    .sda = PA_19,
    .scl = PA_22,
  },

};


#if 0
/* Flash memory devices */
platform_flash_t platform_flash_peripherals[] =
{
  [MICO_FLASH_SPI] =
  {
    //.flash_type                   = FLASH_TYPE_SPI,
    .flash_start_addr             = 0x00000000,
    .flash_length                 = 0x100000,
  },
};

platform_flash_driver_t platform_flash_drivers[MICO_FLASH_MAX];

/* Logic partition on flash devices */
const mico_logic_partition_t mico_partitions[] =
{
  [MICO_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0x00000000,
    .partition_length          =     0x4000,    //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_APPLICATION] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Application",
    .partition_start_addr      = 0x000C0000,
    .partition_length          =    0x54000,   //336k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x000C0000,
    .partition_length          = 0x40000,  //256k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x000C0000,
    .partition_length          = 0x60000, //384k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x000C0000,
    .partition_length          = 0x4000, // 16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x000C0000,
    .partition_length          = 0x4000, //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_NONE,
  }
};
#endif
/******************************************************
*           Interrupt Handler Definitions
******************************************************/
#if 0
MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[PORT_UART_1] );
}

MICO_RTOS_DEFINE_ISR( USART6_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[PORT_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream7_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[PORT_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[PORT_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[PORT_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream1_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[PORT_UART_2] );
}
#endif
/******************************************************
*               Function Definitions
******************************************************/
#if 0
static void _button_EL_irq_handler( void* arg )
{
  (void)(arg);
  int interval = -1;

  if ( MicoGpioInputGet( (mico_gpio_t)EasyLink_BUTTON ) == 0 ) {
    _default_start_time = mico_get_time()+1;
    mico_start_timer(&_button_EL_timer);
  } else {
    interval = mico_get_time() + 1 - _default_start_time;
    if ( (_default_start_time != 0) && interval > 50 && interval < RestoreDefault_TimeOut){
      /* EasyLink button clicked once */
      PlatformEasyLinkButtonClickedCallback();
    }
    mico_stop_timer(&_button_EL_timer);
    _default_start_time = 0;
  }
}

static void _button_STANDBY_irq_handler( void* arg )
{
  (void)(arg);
  PlatformStandbyButtonClickedCallback();
}

static void _button_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  _default_start_time = 0;
  PlatformEasyLinkButtonLongPressedCallback();
}

#endif
bool watchdog_check_last_reset( void )
{
#if 0
  if ( RCC->CSR & RCC_CSR_WDGRSTF )
  {
    /* Clear the flag and return */
    RCC->CSR |= RCC_CSR_RMVF;
    return true;
  }
#endif
  return false;
}

void platform_init_peripheral_irq_priorities( void )
{
#if 0
  /* Interrupt priority setup. Called by WICED/platform/MCU/STM32F2xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN SPI DMA        */
  NVIC_SetPriority( USART1_IRQn      ,  6 ); /* PORT_UART_1         */
  NVIC_SetPriority( USART6_IRQn      ,  6 ); /* PORT_UART_2         */
  NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* PORT_UART_1 TX DMA  */
  NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* PORT_UART_1 RX DMA  */
  NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* PORT_UART_2 TX DMA  */
  NVIC_SetPriority( DMA2_Stream1_IRQn,  7 ); /* PORT_UART_2 RX DMA  */
  NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
#endif
}

void init_platform( void )
{
#if 0
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

  //  Initialise EasyLink buttons
  MicoGpioInitialize( (mico_gpio_t)EasyLink_BUTTON, INPUT_PULL_UP );
  mico_init_timer(&_button_EL_timer, RestoreDefault_TimeOut, _button_EL_Timeout_handler, NULL);
  MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_BOTH_EDGES, _button_EL_irq_handler, NULL );

  //  Initialise Standby/wakeup switcher
  MicoGpioInitialize( Standby_SEL, INPUT_PULL_UP );
  MicoGpioEnableIRQ( Standby_SEL , IRQ_TRIGGER_FALLING_EDGE, _button_STANDBY_irq_handler, NULL);
#endif
}

void init_platform_bootloader( void )
{
#if 0
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

  MicoGpioInitialize((mico_gpio_t)BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize((mico_gpio_t)MFG_SEL, INPUT_HIGH_IMPEDANCE);
#endif
}

void MicoSysLed(bool onoff)
{
#if 0
  if (onoff) {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  } else {
    MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  }
#endif
}

void MicoRfLed(bool onoff)
{
#if 0
  if (onoff) {
    MicoGpioOutputLow( (mico_gpio_t)MICO_RF_LED );
  } else {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  }
#endif
}

#if 0
bool MicoShouldEnterMFGMode(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==false)
    return true;
  else
    return false;
}

bool MicoShouldEnterBootloader(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==true)
    return true;
  else
    return false;
}
#endif
