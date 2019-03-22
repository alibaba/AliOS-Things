/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/uart.h"
#include "board.h"

#include "k_config.h"
#include "k_api.h"

#include "SWM320.h"

extern void aos_heap_set(void);

static void peripheral_init(void);
static void stduart_init(void);
static void SystemClock_Config(void);
/*
* @ default logic uart port: PORT_UART_STD to map with different physical GPIO port
*/
uart_dev_t uart_0 = {
   .port = 0,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_1 = {
   .port = 1,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_2 = {
   .port = 2,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_3 = {
   .port = 3,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
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
//    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);
    if (SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND))
    {
        /* Capture error */
        while (1);
    }

    /* peripheral init */
    peripheral_init();

    /*user code end*/
}

/*user code begin*/

/**
* @brief peripheral init example.
*/
static void peripheral_init(void)
{
    stduart_init();
}

/**
* @init the default uart init example.
*/
static void stduart_init(void)
{
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
static void SystemClock_Config(void)
{
    /* Configure the system clock */
    uint32_t prioritygroup = 0x00;

    prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));

    prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(prioritygroup, 0x0f, 0));

    SystemInit();
}
/*user code end*/
