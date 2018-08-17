/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include "hal/hal.h"
#include "k_config.h"
#include "Board.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

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

char g_system_stack[0x400] __attribute__ ((section (".stack"))) ;
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

#ifndef EOF
#define EOF (-1)
#endif

uart_dev_t uart_0;

static void stduart_init(void);

void ti_soc_init(void)
{
    /**Configure the Systick interrupt time */
    SysTick_Config(SystemCoreClock / (RHINO_CONFIG_TICKS_PER_SECOND / 10));
    /* Call driver init functions */
    Board_initGeneral();
    /* Call driver init functions */
    GPIO_init();
    UART_init();

    /* default uart init */
    stduart_init();
}

static void stduart_init(void)
{
    uart_0.port = STDIO_UART;
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
  if (ch == '\n') 
  {
    hal_uart_send(&uart_0, (void *)"\r", 1, AOS_WAIT_FOREVER);
  }
  hal_uart_send(&uart_0, &ch, 1, AOS_WAIT_FOREVER);
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
  ret = hal_uart_recv_II(&uart_0, &ch, 1, &recv_size, AOS_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}


