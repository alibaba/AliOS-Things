/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "aos/hal/uart.h"
#include "aos/hal/i2c.h"
#include "aos/hal/spi.h"

#include "k_config.h"
#include "board.h"

#include "stm32l4xx_hal.h"
#include "hal_uart_stm32l4.h"
#include "hal/hal_i2c_stm32l4.h"
#include "hal_spi_stm32l4.h"
#include "stm32l4xx_hal_spi.h"

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

#if defined (__CC_ARM)
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00018000;
#endif

uart_dev_t uart_0;
i2c_dev_t brd_i2c1_dev = {1, {0}, NULL};

static void stduart_init(void);

UART_MAPPING UART_MAPPING_TABLE[] =
{
    { PORT_UART_STD,     USART2, { UART_OVERSAMPLING_16, UART_ONE_BIT_SAMPLE_DISABLE, UART_ADVFEATURE_NO_INIT, 512} },
    { PORT_UART_AT,      USART1,  { UART_OVERSAMPLING_16, UART_ONE_BIT_SAMPLE_DISABLE, UART_ADVFEATURE_NO_INIT, 1024} },
    {PORT_UART_ARDUINO,  USART3,   {UART_OVERSAMPLING_16, UART_ONE_BIT_SAMPLE_DISABLE, UART_ADVFEATURE_NO_INIT, 1024}}
};


void stm32_soc_init(void)
{
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /**Configure the Systick interrupt time */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    MX_GPIO_Init();

    MX_DMA_Init();
}

void stm32_soc_peripheral_init(void)
{
    /*default uart init*/
    stduart_init();
    hal_i2c_init(&brd_i2c1_dev);
}

static void stduart_init(void)
{
    uart_0.port = 0;
    uart_0.config.baud_rate = 115200;
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

extern int32_t hal_spi_init(spi_dev_t *spi);
int32_t hal_spi_init(spi_dev_t *spi) {
}

extern int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout);
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout) {
}
extern int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout) {
}

extern int32_t hal_spi_finalize(spi_dev_t *spi);
int32_t hal_spi_finalize(spi_dev_t *spi) {
}

