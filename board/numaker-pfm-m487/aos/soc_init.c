/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include "hal/hal.h"
#include "k_config.h"
#include "soc_init.h"
#include "board.h"

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00010000;
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

uart_dev_t uart_0;
static void stduart_init(void)
{
    memset((void*)&uart_0, 0, sizeof(uart_dev_t));
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode         = MODE_TX_RX;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    hal_uart_init(&uart_0);
}

void soc_init(void);
void numicro_soc_init(void)
{
    /* Configure the system clock */
   	soc_init();

    /* Configure the Systick interrupt time */
    SysTick_Config ( SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);
    
    /* Standard I/O initialization */
    stduart_init();
}

/**
  * @brief  Retargets the C library printf function to the UART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
#if 0
//  if (ch == '\n') 
//    UART_Write(UART0, (void *)"\r", 1);
  UART_Write(UART0, (void *)&ch, 1);
#else  
//  if (ch == '\n') 
//    hal_uart_send(&console_uart, (void *)"\r", 1);
  hal_uart_send(&uart_0, (void *)&ch, 1, 0);
#endif
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the UART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the UART0 and Loop until the end of transmission */
  uint8_t ch = EOF;
  int32_t ret = -1;

#if 0
  ret = UART_Read(UART0, (void *)&ch, 1);
#else
  ret = hal_uart_recv(&uart_0, (void *)&ch, 1, 0);
#endif

  if (ret == 0)
      return ch;
  else
      return -1;
}
