#include "aos/hal/flash.h"
#include <aos/kernel.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void board_init(void)
{
    
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_description      = "Bootloader";
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_start_addr       = 0x08000000;
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_length           = 0x8000;    //500k bytes
    hal_partitions[HAL_PARTITION_BOOTLOADER].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS;

    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x0800C000;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = 0x98000;    //608K bytes
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_owner            = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_description      = "RF Firmware";
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_start_addr       = 0x2000;
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_length           = 0x3E000; // 4k bytes
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x0;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = 0x2000; // 8k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0xDA000;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = 0x2000; //8k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner               = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description         = "OTA Storage";
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr          = 0x40000;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length              = 0x98000;    //608K bytes
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner            = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description      = "PARAMETER3";
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr       = 0xD8000;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_SPI;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER4";
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0xD9000;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
}



#include "platform_peripheral.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"

/******************************************************
*               Function Declarations
******************************************************/
extern WEAK void PlatformEasyLinkButtonClickedCallback(void);
extern WEAK void PlatformEasyLinkButtonLongPressedCallback(void);
extern WEAK void bootloader_start(void);

/******************************************************
*               Variables Definitions
******************************************************/

const platform_gpio_t platform_gpio_pins[] =
{
  /* Common GPIOs for internal use */
  [MICO_SYS_LED]                      = { GPIOB,  13 }, 
  [MICO_RF_LED]                       = { GPIOB,  8 }, 
  [BOOT_SEL]                          = { GPIOB,  1 }, 
  [MFG_SEL]                           = { GPIOB,  0 }, 
  [EasyLink_BUTTON]                   = { GPIOA,  1 }, 
  [STDIO_UART_RX]                     = { GPIOA,  3 },  
  [STDIO_UART_TX]                     = { GPIOA,  2 },  
  [FLASH_PIN_SPI_CS  ]                = { GPIOA, 15 },
  [FLASH_PIN_SPI_CLK ]                = { GPIOB,  3 },
  [FLASH_PIN_SPI_MOSI]                = { GPIOA,  7 },
  [FLASH_PIN_SPI_MISO]                = { GPIOB,  4 },

  /* GPIOs for external use */
  [MICO_GPIO_2]                       = { GPIOB,  2 },
  [MICO_GPIO_8]                       = { GPIOA , 2 },
  [MICO_GPIO_9]                       = { GPIOA,  1 },
  [MICO_GPIO_12]                      = { GPIOA,  3 },
  [MICO_GPIO_14]                      = { GPIOA,  0 },
  [MICO_GPIO_16]                      = { GPIOC, 13 },
  [MICO_GPIO_17]                      = { GPIOB, 10 },
  [MICO_GPIO_18]                      = { GPIOB,  9 },
  [MICO_GPIO_19]                      = { GPIOB, 12 },
  [MICO_GPIO_27]                      = { GPIOA, 12 },  
  [MICO_GPIO_29]                      = { GPIOA, 10 },
  [MICO_GPIO_30]                      = { GPIOB,  6 },
  [MICO_GPIO_31]                      = { GPIOB,  8 },
  [MICO_GPIO_33]                      = { GPIOB, 13 },
  [MICO_GPIO_34]                      = { GPIOA,  5 },
  [MICO_GPIO_35]                      = { GPIOA, 11 },
  [MICO_GPIO_36]                      = { GPIOB,  1 },
  [MICO_GPIO_37]                      = { GPIOB,  0 },
  [MICO_GPIO_38]                      = { GPIOA,  4 },
};

const platform_pwm_t *platform_pwm_peripherals = NULL;

const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .port                         = I2C2,
    .pin_scl                      = &platform_gpio_pins[MICO_GPIO_17],
    .pin_sda                      = &platform_gpio_pins[MICO_GPIO_18],
    .peripheral_clock_reg         = RCC_APB1Periph_I2C2,
    .tx_dma                       = DMA1,
    .tx_dma_peripheral_clock      = RCC_AHB1Periph_DMA1,
    .tx_dma_stream                = DMA1_Stream7,
    .rx_dma_stream                = DMA1_Stream5,
    .tx_dma_stream_id             = 7,
    .rx_dma_stream_id             = 5,
    .tx_dma_channel               = DMA_Channel_1,
    .rx_dma_channel               = DMA_Channel_1,
    .gpio_af_scl                  = GPIO_AF_I2C2,
    .gpio_af_sda                  = GPIO_AF9_I2C2
  },
};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_1] =
  {
    .port                         = USART2,
    .pin_tx                       = &platform_gpio_pins[STDIO_UART_TX],
    .pin_rx                       = &platform_gpio_pins[STDIO_UART_RX],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream6,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream5,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
  },
  [MICO_UART_2] =
  {
    .port                         = USART1,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_30],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_29],
    .pin_cts                      = &platform_gpio_pins[MICO_GPIO_35],
    .pin_rts                      = &platform_gpio_pins[MICO_GPIO_27],
    .tx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream7,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream7_IRQn,
      .complete_flags             = DMA_HISR_TCIF7,
      .error_flags                = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream2,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream2_IRQn,
      .complete_flags             = DMA_LISR_TCIF2,
      .error_flags                = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
    },
  },
};

platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];

const platform_spi_t platform_spi_peripherals[] =
{
  [MICO_SPI_1]  =
  {
    .port                         = SPI1,
    .gpio_af                      = GPIO_AF_SPI1,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI1,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[FLASH_PIN_SPI_MOSI],
    .pin_miso                     = &platform_gpio_pins[FLASH_PIN_SPI_MISO],
    .pin_clock                    = &platform_gpio_pins[FLASH_PIN_SPI_CLK],
    .tx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream5,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 ),
    },
    .rx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream0,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream0_IRQn,
      .complete_flags             = DMA_LISR_TCIF0,
      .error_flags                = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
    },
  }
};

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
  [HAL_FLASH_EMBEDDED] =
  {
    .flash_type                   = FLASH_TYPE_EMBEDDED,
    .flash_start_addr             = 0x08000000,
    .flash_length                 = 0x80000,
  },
  [HAL_FLASH_SPI] =
  {
    .flash_type                   = FLASH_TYPE_SPI,
    .flash_start_addr             = 0x000000,
    .flash_length                 = 0x200000,
  },
};


platform_flash_driver_t platform_flash_drivers[HAL_FLASH_MAX];


#if defined ( USE_MICO_SPI_FLASH )
const platforom_spi_device_t mico_spi_flash =
{
  .port        = MICO_SPI_1,
  .chip_select = FLASH_PIN_SPI_CS,
  .speed       = 40000000,
  .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_USE_DMA | SPI_MSB_FIRST ),
  .bits        = 8
};

#endif
const platform_adc_t platform_adc_peripherals[] =
{
  [MICO_ADC_1] = { ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_38] },
  [MICO_ADC_2] = { ADC1, ADC_Channel_5, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_34] },
};

/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
* SDIO: EMW1062_PIN_BOOTSTRAP[1:0] = b'00
* gSPI: EMW1062_PIN_BOOTSTRAP[1:0] = b'01
*/
const platform_gpio_t wifi_control_pins[] =
{
  [WIFI_PIN_RESET]           = { GPIOB, 14 },
};

/* Wi-Fi SDIO bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
  [WIFI_PIN_SDIO_OOB_IRQ] = { GPIOA,  0 },
  [WIFI_PIN_SDIO_CLK    ] = { GPIOB, 15 },
  [WIFI_PIN_SDIO_CMD    ] = { GPIOA,  6 },
  [WIFI_PIN_SDIO_D0     ] = { GPIOB,  7 },
  [WIFI_PIN_SDIO_D1     ] = { GPIOA,  8 },
  [WIFI_PIN_SDIO_D2     ] = { GPIOA,  9 },
  [WIFI_PIN_SDIO_D3     ] = { GPIOB,  5 },
};


/******************************************************
*           Interrupt Handler Definitions
******************************************************/
    
MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( USART2_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream7_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream5_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}


/******************************************************
*               Function Definitions
******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F2xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  //NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN SPI DMA        */
  NVIC_SetPriority( USART1_IRQn      ,  6 ); /* MICO_UART_1         */
  NVIC_SetPriority( USART2_IRQn      ,  6 ); /* MICO_UART_2         */
  NVIC_SetPriority( DMA1_Stream6_IRQn,  7 ); /* MICO_UART_1 TX DMA  */
  NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* MICO_UART_1 RX DMA  */
  NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* MICO_UART_2 TX DMA  */
  NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* MICO_UART_2 RX DMA  */
  NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
}

void init_platform( void )
{
  
}

