
#include <aos/kernel.h>
#include "platform_bluetooth.h"

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
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = 0x08008000;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = 0xF8000,   //992k bytes
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_owner            = HAL_FLASH_QSPI;
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_description      = "RF Firmware";
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_start_addr       = 0x2000;
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_length           = 0x6E000; ///440k bytes
    hal_partitions[HAL_PARTITION_RF_FIRMWARE].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_QSPI;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x1F8000;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length           = 0x1000; // 4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_QSPI;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x1F9000;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length           = 0x4000; //16k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description         = "OTA Storage";
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr          = 0x70000;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length              = 0xF8000, //608k bytes
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner            = HAL_FLASH_QSPI;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description      = "PARAMETER3";
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr       = 0x1FD000;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length           = 0x1000; //4k bytes
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_QSPI;
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER4";
    hal_partitions[HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0x1FE000;
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
  [FLASH_PIN_QSPI_CS ]                = { GPIOC, 11 },
  [FLASH_PIN_QSPI_CLK]                = { GPIOB,  1 },
  [FLASH_PIN_QSPI_D0]                 = { GPIOA,  6 },
  [FLASH_PIN_QSPI_D1]                 = { GPIOA,  7 },
  [FLASH_PIN_QSPI_D2]                 = { GPIOC,  4 },
  [FLASH_PIN_QSPI_D3]                 = { GPIOC,  5 },

  /* GPIOs for external use */
  [MICO_GPIO_2]                       = { GPIOB,  2 },
  [MICO_GPIO_4]                       = { GPIOB, 15 },
  [MICO_GPIO_5]                       = { GPIOB, 12 },
  [MICO_GPIO_6]                       = { GPIOB, 13 },
  [MICO_GPIO_7]                       = { GPIOB, 14 },
  [MICO_GPIO_8]                       = { GPIOC,  6 },
  [MICO_GPIO_9]                       = { GPIOA, 15 },
  [MICO_GPIO_12]                      = { GPIOC,  7 },
  [MICO_GPIO_14]                      = { GPIOC,  0 },
  [MICO_GPIO_16]                      = { GPIOC, 13 },
  [MICO_GPIO_17]                      = { GPIOB,  8 },
  [MICO_GPIO_18]                      = { GPIOB,  9 },
  [MICO_GPIO_19]                      = { GPIOB, 10 },
  [MICO_GPIO_27]                      = { GPIOB,  3 },
  [MICO_GPIO_29]                      = { GPIOB,  7 },
  [MICO_GPIO_30]                      = { GPIOB,  6 },
  [MICO_GPIO_31]                      = { GPIOB,  4 },
  [MICO_GPIO_33]                      = { GPIOA, 10 },
  [MICO_GPIO_34]                      = { GPIOA,  5 },
  [MICO_GPIO_35]                      = { GPIOA, 11 },
  [MICO_GPIO_36]                      = { GPIOA, 12 },
  [MICO_GPIO_37]                      = { GPIOB,  0 },
  [MICO_GPIO_38]                      = { GPIOA,  4 },
};

const platform_pwm_t *platform_pwm_peripherals = NULL;

const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .port                         = I2C1,
    .pin_scl                      = &platform_gpio_pins[MICO_GPIO_17],
    .pin_sda                      = &platform_gpio_pins[MICO_GPIO_18],
    .peripheral_clock_reg         = RCC_APB1Periph_I2C1,
    .tx_dma                       = DMA1,
    .tx_dma_peripheral_clock      = RCC_AHB1Periph_DMA1,
    .tx_dma_stream                = DMA1_Stream1,
    .rx_dma_stream                = DMA1_Stream0,
    .tx_dma_stream_id             = 1,
    .rx_dma_stream_id             = 0,
    .tx_dma_channel               = DMA_Channel_1,
    .rx_dma_channel               = DMA_Channel_1,
    .gpio_af_scl                  = GPIO_AF_I2C1,
    .gpio_af_sda                  = GPIO_AF_I2C1
  },
};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_1] =
  {
    .port                         = USART6,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_8],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_12],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream6,
      .channel                    = DMA_Channel_5,
      .irq_vector                 = DMA2_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream1,
      .channel                    = DMA_Channel_5,
      .irq_vector                 = DMA2_Stream1_IRQn,
      .complete_flags             = DMA_LISR_TCIF1,
      .error_flags                = ( DMA_LISR_TEIF1 | DMA_LISR_FEIF1 | DMA_LISR_DMEIF1 ),
    },
  },
  [MICO_UART_2] =
  {
    .port                         = USART1,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_30],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_29],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
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
    .port                         = SPI2,
    .gpio_af                      = GPIO_AF_SPI2,
    .peripheral_clock_reg         = RCC_APB1Periph_SPI2,
    .peripheral_clock_func        = RCC_APB1PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[MICO_GPIO_4],
    .pin_miso                     = &platform_gpio_pins[MICO_GPIO_7],
    .pin_clock                    = &platform_gpio_pins[MICO_GPIO_6],
    .tx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream4,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream4_IRQn,
      .complete_flags             = DMA_HISR_TCIF4,
      .error_flags                = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 ),
    },
    .rx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream3,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream3_IRQn,
      .complete_flags             = DMA_LISR_TCIF3,
      .error_flags                = ( DMA_LISR_TEIF3 | DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 ),
    },
  }
};

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];


const platform_qspi_t platform_qspi_peripherals[] =
{
  [MICO_QSPI_1]  =
  {
    .port                         = QUADSPI,
    .FSelect                      = QSPI_FSelect_2,
    .peripheral_clock_reg         = RCC_AHB3Periph_QSPI,
    .peripheral_clock_func        = RCC_AHB3PeriphClockCmd,
    .pin_d0                       = &platform_gpio_pins[FLASH_PIN_QSPI_D0],
    .pin_d1                       = &platform_gpio_pins[FLASH_PIN_QSPI_D1],
    .pin_d2                       = &platform_gpio_pins[FLASH_PIN_QSPI_D2],
    .pin_d3                       = &platform_gpio_pins[FLASH_PIN_QSPI_D3],
    .pin_clock                    = &platform_gpio_pins[FLASH_PIN_QSPI_CLK],
    .pin_cs                       = &platform_gpio_pins[FLASH_PIN_QSPI_CS],
#ifdef USE_QUAD_SPI_DMA
    .dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream7,
      .channel                    = DMA_Channel_3,
      .complete_flags             = DMA_FLAG_TCIF7,
    },
#endif
    .gpio_af_d0                   = GPIO_AF10_QUADSPI,
    .gpio_af_d1                   = GPIO_AF10_QUADSPI,
    .gpio_af_d2                   = GPIO_AF10_QUADSPI,
    .gpio_af_d3                   = GPIO_AF10_QUADSPI,
    .gpio_af_clk                  = GPIO_AF9_QUADSPI,
    .gpio_af_cs                   = GPIO_AF9_QUADSPI,
  }
};

//platform_qspi_driver_t platform_qspi_drivers[MICO_QSPI_MAX];

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
  [HAL_FLASH_EMBEDDED] =
  {
    .flash_type                   = FLASH_TYPE_EMBEDDED,
    .flash_start_addr             = 0x08000000,
    .flash_length                 = 0x100000,
  },
  [HAL_FLASH_QSPI] =
  {
    .flash_type                   = FLASH_TYPE_QSPI,
    .flash_start_addr             = 0x000000,
    .flash_length                 = 0x200000,
  },
};

platform_flash_driver_t platform_flash_drivers[HAL_FLASH_MAX];


#if defined ( USE_MICO_SPI_FLASH )
const mico_spi_device_t mico_spi_flash =
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
  [WIFI_PIN_POWER      ] = { GPIOA, 9 },
  [WIFI_PIN_BOOTSTRAP_1] = { GPIOC, 10 },
  [WIFI_PIN_RESET]           = { GPIOA, 9 },
  [WIFI_PIN_32K_CLK]         = { GPIOA, 8 },
};

/* Wi-Fi SDIO bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
  [WIFI_PIN_SDIO_OOB_IRQ] = { GPIOC,  0 },
  [WIFI_PIN_SDIO_CLK    ] = { GPIOC, 12 },
  [WIFI_PIN_SDIO_CMD    ] = { GPIOD,  2 },
  [WIFI_PIN_SDIO_D0     ] = { GPIOC,  8 },
  [WIFI_PIN_SDIO_D1     ] = { GPIOC,  9 },
  [WIFI_PIN_SDIO_D2     ] = { GPIOC, 10 },
  [WIFI_PIN_SDIO_D3     ] = { GPIOB,  5 },
};


/* Bluetooth control pins.*/
static const platform_gpio_t internal_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [MICO_BT_PIN_POWER      ] = { GPIOC,  3 },
    [MICO_BT_PIN_HOST_WAKE  ] = { GPIOC,  2 },
    [MICO_BT_PIN_DEVICE_WAKE] = { GPIOC,  1 }
};

const platform_gpio_t* mico_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [MICO_BT_PIN_POWER      ] = &internal_bt_control_pins[MICO_BT_PIN_POWER      ],
    [MICO_BT_PIN_HOST_WAKE  ] = &internal_bt_control_pins[MICO_BT_PIN_HOST_WAKE  ],
    [MICO_BT_PIN_DEVICE_WAKE] = &internal_bt_control_pins[MICO_BT_PIN_DEVICE_WAKE],
    [MICO_BT_PIN_RESET      ] = NULL,
};

/* Bluetooth UART pins.*/
static const platform_gpio_t internal_bt_uart_pins[] =
{
    [MICO_BT_PIN_UART_TX ] = { GPIOA,  2 },
    [MICO_BT_PIN_UART_RX ] = { GPIOA,  3 },
    [MICO_BT_PIN_UART_CTS] = { GPIOA,  0 },
    [MICO_BT_PIN_UART_RTS] = { GPIOA,  1 },
};

const platform_gpio_t* mico_bt_uart_pins[] =
{
    [MICO_BT_PIN_UART_TX ] = &internal_bt_uart_pins[MICO_BT_PIN_UART_TX ],
    [MICO_BT_PIN_UART_RX ] = &internal_bt_uart_pins[MICO_BT_PIN_UART_RX ],
    [MICO_BT_PIN_UART_CTS] = &internal_bt_uart_pins[MICO_BT_PIN_UART_CTS],
    [MICO_BT_PIN_UART_RTS] = &internal_bt_uart_pins[MICO_BT_PIN_UART_RTS],
};

static const platform_uart_t internal_bt_uart_peripheral =
{
  .port                         = USART2,
  .pin_tx                       = &internal_bt_uart_pins[MICO_BT_PIN_UART_TX ],
  .pin_rx                       = &internal_bt_uart_pins[MICO_BT_PIN_UART_RX ],
  .pin_cts                      = &internal_bt_uart_pins[MICO_BT_PIN_UART_CTS ],
  .pin_rts                      = &internal_bt_uart_pins[MICO_BT_PIN_UART_RTS ],
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
};

static platform_uart_driver_t internal_bt_uart_driver;
const platform_uart_t*        mico_bt_uart_peripheral = &internal_bt_uart_peripheral;
platform_uart_driver_t*       mico_bt_uart_driver     = &internal_bt_uart_driver;


/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t mico_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_CTS_RTS, //FLOW_CONTROL_DISABLED,
};

/*BT chip specific configuration information*/
const platform_bluetooth_config_t mico_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 115200,
    .featured_baud_rate          = 3000000
};

/******************************************************
*           Interrupt Handler Definitions
******************************************************/

MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( USART6_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( USART2_IRQHandler )
{
  platform_uart_irq( mico_bt_uart_driver );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream7_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream1_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( mico_bt_uart_driver );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream5_IRQHandler )
{
  platform_uart_rx_dma_irq( mico_bt_uart_driver );
}


/******************************************************
*               Function Definitions
******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F4xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  NVIC_SetPriority( USART6_IRQn      ,  6 ); /* MICO_UART_1         */
  NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* MICO_UART_1 TX DMA  */
  NVIC_SetPriority( DMA2_Stream1_IRQn,  7 ); /* MICO_UART_1 RX DMA  */
  NVIC_SetPriority( USART2_IRQn      ,  6 ); /* BT UART             */
  NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* BT UART RX DMA      */
  NVIC_SetPriority( DMA1_Stream6_IRQn,  7 ); /* BT UART TX DMA      */
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

