/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include "hal/hal.h"
#include "k_config.h"
#include "board.h"
#include "stm32f4xx_hal.h"
#include "hal_uart_stm32f4.h"
#include "hal_i2c_stm32f4.h"
#include "hal_gpio_stm32f4.h"

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00030000;
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

const gpio_mapping_t GPIO_MAPPINGConfig[GPIO_TABLE_SZ] =
{
    {GPIO_OUTPUT_PP, ON_BOARD_LED01, GPIOB, GPIO_PIN_0,  /*IRQ_NULL,*/GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_PinState_Reset},
    {GPIO_OUTPUT_PP, ON_BOARD_LED02, GPIOB, GPIO_PIN_7,  /*IRQ_NULL,*/GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_PinState_Reset},
    {GPIO_OUTPUT_PP, ON_BOARD_LED03, GPIOB, GPIO_PIN_14, /*IRQ_NULL,*/GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_PinState_Reset},
	{GPIO_INPUT, HAL_GPIO_22, GPIOB, GPIO_PIN_6,  GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_PinState_Reset}
};

uart_dev_t uart_0;
uart_dev_t uart_at;

const UART_MAPPING UART_MAPPING_TABLE[] =
{
    { PORT_UART_STD,     USART6, { USART6_IRQn,  0, 1,UART_OVERSAMPLING_16 } },
    { PORT_UART_AT,     UART7, { UART7_IRQn,  0, 0,UART_OVERSAMPLING_16 } },
};

void* i2c_mapping_table[] = { I2C1, I2C2, I2C3};

i2c_dev_t brd_i2c2_dev = {
	.port = 2,
	.config = {
		.freq = I2C_BUS_BIT_RATES_400K,
		.mode = I2C_MODE_MASTER
	},
	NULL
};

#if defined(AOS_SENSOR_TEMP_SENSIRION_HTS221) || defined(AOS_SENSOR_HUMI_SENSIRION_HTS221)
extern i2c_dev_t HTS221_ctx;
#endif

static void stduart_init(void);


static void atuart_init(void)
{
    uart_at.port = PORT_UART_AT;
    uart_at.config.baud_rate = 9600;
    uart_at.config.data_width = DATA_WIDTH_8BIT;
    uart_at.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_at.config.mode = MODE_TX_RX;
    uart_at.config.parity = NO_PARITY;
    uart_at.config.stop_bits = STOP_BITS_1;

    hal_uart_init(&uart_at);
}

void stm32_soc_init(void)
{
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    board_core_init();

    /**Configure the Systick interrupt time 
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);

    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 0x0f, 0);

    /* rest init */
    stduart_init();
    hal_i2c_pre_init();
	hal_i2c_init(&brd_i2c2_dev);
	atuart_init();
#if defined(AOS_SENSOR_TEMP_SENSIRION_HTS221) || defined(AOS_SENSOR_HUMI_SENSIRION_HTS221)
	HTS221_ctx.port = 2;
#endif
}

static void stduart_init(void)
{
    uart_0.port = PORT_UART_STD;
    uart_0.config.baud_rate = STDIO_UART_BUADRATE;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode = MODE_TX_RX;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;

    hal_uart_init(&uart_0);
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    krhino_intrpt_enter();

    HAL_IncTick();
    krhino_tick_proc();

#ifdef LITTLEVGL_GUIAPP
    lv_tick_inc(1);
#endif

    krhino_intrpt_exit();

}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  if (ch == '\n') {
    //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    hal_uart_send(&uart_0, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&uart_0, &ch, 1, 30000);
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = EOF;
  int32_t ret = -1;
  
  uint32_t recv_size;
  ret = hal_uart_recv_II(&uart_0, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}


