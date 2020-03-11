/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 
#include <stdio.h>

#include "k_config.h"
#include "board.h"

#include "aos/hal/uart.h"

#include "xmc_ccu4.h"
#include "xmc_gpio.h"
#include "xmc_uart.h"
#include "hal_uart_xmc4000.h"

#ifdef AOS_CANOPEN
#include "co_adapter.h"
#endif

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

uint32_t ticks = 0;
uart_dev_t uart_0;

//Project Macro definitions:
#define MODULE_PTR      	CCU40
#define MODULE_NUMBER   	(0U)

#define SLICE0_PTR       	CCU40_CC40
#define SLICE0_NUMBER    	(0U)

//XMC Capture/Compare Unit 4 (CCU4) Configuration:
XMC_CCU4_SLICE_COMPARE_CONFIG_t SLICE0_config =
{
  .timer_mode = (uint32_t) XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
  .monoshot = (uint32_t) false,
  .shadow_xfer_clear = (uint32_t) 0,
  .dither_timer_period = (uint32_t) 0,
  .dither_duty_cycle = (uint32_t) 0,
  .prescaler_mode = (uint32_t) XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
  .mcm_enable = (uint32_t) 0,
  .prescaler_initval = (uint32_t) 4, 	/* 144MHz/16 = 9MHz */
  .float_limit = (uint32_t) 0,
  .dither_limit = (uint32_t) 0,
  .passive_level = (uint32_t) XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
  .timer_concatenation = (uint32_t) 0
};

UART_MAPPING UART_MAPPING_TABLE[] =
{
    { PORT_UART_STD,     XMC_UART0_CH0, { USIC0_0_IRQn, 0, 1, 0 } },
    { PORT_UART_AT,      XMC_UART1_CH0, { USIC0_1_IRQn, 0, 1, 0 } },
    { PORT_UART_RS485,   XMC_UART1_CH1, { USIC1_0_IRQn, 0, 1, 0 } },
    { PORT_UART_SCANNER, XMC_UART0_CH1, { USIC1_1_IRQn, 0, 1, 0 } },
    { PORT_UART_LORA,    XMC_UART2_CH0, { USIC2_0_IRQn, 0, 1, 0 } },
};

static void gpio_init(void);
static void timer_init(void);
static void stduart_init(void);

void CCU40_0_IRQHandler(void)
{
	krhino_intrpt_enter();
	XMC_CCU4_SLICE_ClearEvent(SLICE0_PTR,XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
	HAL_UART_DMA_IRQHandler();
	krhino_intrpt_exit();
}

void xmc_soc_init(void)
{
	SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);
}

void xmc4800_peripheral_init(void)
{
	gpio_init();
	timer_init();
	stduart_init();
}

static void gpio_init(void)
{
	XMC_GPIO_SetMode(P5_8, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetHardwareControl(P5_8, XMC_GPIO_HWCTRL_DISABLED);
    XMC_GPIO_SetOutputHigh(P5_8);
}

static void timer_init(void)
{
	XMC_CCU4_Init(MODULE_PTR, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	XMC_CCU4_StartPrescaler(MODULE_PTR);
	XMC_CCU4_SetModuleClock(MODULE_PTR, XMC_CCU4_CLOCK_SCU);
	XMC_CCU4_SLICE_CompareInit(SLICE0_PTR, &SLICE0_config);

	XMC_CCU4_SLICE_SetTimerCompareMatch(SLICE0_PTR, 500-1);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE0_PTR, 9000-1); // 9000 * (1/9MHz) = 1ms

	XMC_CCU4_EnableShadowTransfer(MODULE_PTR, (uint32_t)(XMC_CCU4_SHADOW_TRANSFER_SLICE_0 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0));

	/* Enable Period match events */
	XMC_CCU4_SLICE_EnableEvent(SLICE0_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
	XMC_CCU4_SLICE_SetInterruptNode(SLICE0_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);

	/* Set NVIC priority */
	NVIC_SetPriority(CCU40_0_IRQn, 3U);
	NVIC_EnableIRQ(CCU40_0_IRQn);

	XMC_CCU4_EnableClock(MODULE_PTR, SLICE0_NUMBER);
	XMC_CCU4_SLICE_StartTimer(SLICE0_PTR);
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
    ticks++;
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
  if (ch == '\n') {
    //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    hal_uart_send(&uart_0, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&uart_0, (void *)&ch, 1, 30000);
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

#if defined(AOS_COMP_CLI) && (DEBUG_CONFIG_PANIC == 1)
void uart_reinit(void)
{
}
#endif
