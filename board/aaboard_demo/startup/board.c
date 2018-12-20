/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "board.h"
#include "aos/hal/uart.h"

/*
* @ default logic uart port: PORT_UART_STD to map with different physical GPIO port
*/
uart_dev_t uart_0;

UART_MAPPING UART_MAPPING_TABLE[] =
{
    /*user code begin*/
    { PORT_UART_STD,     USART3,   { UART_OVERSAMPLING_16, 1024} },
    { PORT_UART_AT,      USART6,   { UART_OVERSAMPLING_16, 2048} },
    { PORT_UART_RS485,   UART7,    { UART_OVERSAMPLING_16, 512}  },
    { PORT_UART_SCANNER, UART4,    { UART_OVERSAMPLING_16, 512}  },
    { PORT_UART_LORA,    UART5,    { UART_OVERSAMPLING_16, 512}  }
    /*user code end*/
};

/**
  * @general board init entry board_init
  * @retval None
  */
void board_init(void)
{
    /*mm heap set*/
    aos_heap_set();

    /*choosen to init flash partition , for kv¡¢flash¡¢ota and so on*/
    flash_partition_init();

    /*user code begin*/

    /* Configure the system clock */
    SystemClock_Config();

    /* Configure the Systick interrupt time ,use RHINO_CONFIG_TICKS_PER_SECOND to init tick clock*/
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);

    /* peripheral init */
    peripheral_init();

    /*user code end*/
}

/*user code begin*/

/**
* @brief peripheral init example.
*/
void peripheral_init(void)
{
    stduart_init();
}

/**
* @init the default uart init example.
*/
void stduart_init(void)
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
* @System tick handler example.
shoule include:
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
*/
void SysTick_Handler(void)
{
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
}

/**
  * @brief System Clock Configuration example
  */
void SystemClock_Config(void)
{
}
/*user code end*/
