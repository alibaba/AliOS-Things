/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>

#include "k_api.h"
#include "k_config.h"

#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "aos/hal/uart.h"

#include "NuMicro.h"

#include "board.h"

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

/* User-defined */
/* UART */
struct serial_s board_uart[] = {
#if 1
    { /* UART PORT 0, STDIO UART, Tx/Rx over Nu-Link's VCOM */
        .uart       = UART_0,
        .pin_tx     = STDIO_UART_TX,
        .pin_rx     = STDIO_UART_RX,
        .pin_rts    = NC,
        .pin_cts    = NC
    },
#endif  
    { /* UART PORT 1, UART TX/RX in UDO IF */
        .uart       = UART_1,
        .pin_tx     = PB_5,
        .pin_rx     = PB_4,
        .pin_rts    = NC,
        .pin_cts    = NC
    },
};
const int i32BoardMaxUartNum = sizeof( board_uart ) / sizeof( board_uart[0] );

/* I2C */
struct i2c_s board_i2c[] = {
    { /* I2C PORT 0 */
        .i2c        = I2C_1,
        .pin_scl    = I2C_SCL,
        .pin_sda    = I2C_SDA
    },
    { /* I2C PORT 1 */
        .i2c        = I2C_0,
        .pin_scl    = PA_5,
        .pin_sda    = PA_4
    },
};
const int i32BoardMaxI2CNum = sizeof( board_i2c ) / sizeof( board_i2c[0] );

/* Analog-In */
struct analogin_s board_analogin [] = 
{
    /* Analog-In port */
    { .adc = ADC_0_0,   .pin = A0   },
    { .adc = ADC_0_1,   .pin = A1   },
    { .adc = ADC_0_2,   .pin = A2   },
    { .adc = ADC_0_3,   .pin = A3   },
    { .adc = ADC_0_4,   .pin = A4   },
    { .adc = ADC_0_5,   .pin = A5   },
};
const int i32BoardMaxADCNum  = sizeof( board_analogin ) / sizeof( board_analogin[0] );

/* Analog-Out */
struct analogout_s board_analogout [] = 
{
    /* Analog-In port */
    { .dac = DAC_0, .pin = PC_6 },
    { .dac = DAC_1, .pin = PC_7 },
};
const int i32BoardMaxDACNum  = sizeof( board_analogout ) / sizeof( board_analogout[0] );

/* GPIO */
struct gpio_s board_gpio [] = 
{
    { .pin = D0 },
    { .pin = D1 },
    { .pin = D2 },
    { .pin = D3 },
    { .pin = D4 },
    { .pin = D5 },
    { .pin = D6 },
    { .pin = D7 },
    { .pin = D8 },
    { .pin = D9 },
    { .pin = D10 },
    { .pin = D11 },
    { .pin = D12 },
    { .pin = D13 },
    { .pin = D14 },
    { .pin = D15 }, 
    { .pin = SW1 }, 
    { .pin = SW2 }, 
    { .pin = LED_RED }, 
    { .pin = LED_YELLOW },
    { .pin = LED_GREEN },
};
const int i32BoardMaxGPIONum  = sizeof( board_gpio ) / sizeof( board_gpio[0] );

/* PWM port-M & channel-N */
struct pwmout_s board_pwm [] = 
{
    {   .pwm = PWM_0_0,  .pin = PWM0_CH0    },
    {   .pwm = PWM_0_1,  .pin = PWM0_CH1    },
    {   .pwm = PWM_0_2,  .pin = PWM0_CH2    },
    {   .pwm = PWM_0_3,  .pin = PWM0_CH3    },

    {   .pwm = PWM_1_0,  .pin = PWM1_CH0    },
    {   .pwm = PWM_1_1,  .pin = PWM1_CH1    },
    {   .pwm = PWM_1_2,  .pin = PWM1_CH2    },
    {   .pwm = PWM_1_3,  .pin = PWM1_CH3    },
};
const int i32BoardMaxPWMNum  = sizeof( board_pwm ) / sizeof( board_pwm[0] );

/* SPI */
struct spi_s board_spi [] = 
{
    {   /* SPI0 */
        .spi        = SPI_0,
        .pin_mosi   = PC_3,
        .pin_miso   = PC_2,
        .pin_sclk   = PC_1,
        .pin_ssel   = PC_0,
    }
};
const int i32BoardMaxSPINum  = sizeof( board_spi ) / sizeof( board_spi[0] );

// FIXME
gpio_dev_t board_gpio_table[] = 
{
    {0,  OUTPUT_PUSH_PULL, NULL},//0
    {1,  OUTPUT_PUSH_PULL, NULL},      
    {2,  OUTPUT_PUSH_PULL, NULL},     
    {3,  OUTPUT_PUSH_PULL, NULL},     
    {4,  OUTPUT_PUSH_PULL, NULL},       
    {5,  OUTPUT_PUSH_PULL, NULL},       
    {6,  OUTPUT_PUSH_PULL, NULL},        
    {7,  OUTPUT_PUSH_PULL, NULL},       
    {8,  OUTPUT_PUSH_PULL, NULL},        
    {9,  OUTPUT_PUSH_PULL, NULL},//9          
    {10, OUTPUT_PUSH_PULL, NULL},          
    {11, OUTPUT_PUSH_PULL, NULL},            
    {12, OUTPUT_PUSH_PULL, NULL},      
    {13, OUTPUT_PUSH_PULL, NULL},
    {14, OUTPUT_PUSH_PULL, NULL},
    {15, OUTPUT_PUSH_PULL, NULL},
    {16, IRQ_MODE,               NULL},//16
    {17, IRQ_MODE,               NULL},
    {18, OUTPUT_PUSH_PULL, NULL},
    {19, OUTPUT_PUSH_PULL, NULL},
    {20, OUTPUT_PUSH_PULL, NULL},
};

uart_dev_t uart_0;
void stduart_init(void)
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

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void board_init(void)
{
    aos_heap_set();

    // NOTE: Support singleton semantics to be called from other init functions
    static int inited = 0;
    if (inited) {
        return;
    }
    inited = 1;
    
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (internal OSC 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC_EN_Msk);
    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk);
    /* Enable LIRC clock (OSC 10KHz) for lp_ticker */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRC_EN_Msk);
    /* Enable LXT clock (XTAL 32KHz) for RTC */
    CLK_EnableXtalRC(CLK_PWRCTL_LXT_EN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HIRC_STB_Msk);
    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HXT_STB_Msk);
    /* Wait for LIRC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_LIRC_STB_Msk);
    /* Wait for LXT clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_LXT_STB_Msk);

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_HCLK_CLK_DIVIDER(1));

    /* Select HXT/HIRC to clock PLL
     *
     * Comparison between HXT/HIRC-clocked PLL:
     * 1. Spare HXT on board if only HIRC is used.
     * 2. HIRC has shorter stable time.
     * 3. HXT has better accuracy. USBD requires HXT-clocked PLL.
     * 4. HIRC has shorter wake-up time from power-down mode. 
     *    Per test, wake-up time from power-down mode would take:
     *    T1. 1~13 ms (proportional to deep sleep time) with HXT-clocked PLL as HCLK clock source
     *    T2. <1 ms with HIRC-clocked PLL as HCLK clock source
     *    T1 will fail Greentea test which requires max 10 ms wake-up time.
     *
     *    If we just call CLK_SetCoreClock(FREQ_42MHZ) to configure HCLK to 42 MHz,
     *    it will go T1 with HXT already enabled in front. So we manually configure
     *    it to choose HXT/HIRC-clocked PLL.
     */
#define NU_HXT_PLL      1
#define NU_HIRC_PLL     2

#ifndef NU_CLOCK_PLL
#define NU_CLOCK_PLL    NU_HIRC_PLL
#endif

#if (NU_CLOCK_PLL == NU_HXT_PLL)
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HXT, FREQ_42MHZ*2);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_HCLK_CLK_DIVIDER(2));
#elif (NU_CLOCK_PLL == NU_HIRC_PLL)
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HIRC, FREQ_42MHZ*2);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_HCLK_CLK_DIVIDER(2));
#endif

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}
