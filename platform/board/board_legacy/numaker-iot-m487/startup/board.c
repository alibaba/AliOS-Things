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
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00028000;
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
size_t g_iram1_start = 0x20000000;
size_t g_iram1_total_size = 0x00028000;
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
        .pin_tx     = PB_3,
        .pin_rx     = PB_2,
        .pin_rts    = NC,
        .pin_cts    = NC
    },
};
const int i32BoardMaxUartNum = sizeof( board_uart ) / sizeof( board_uart[0] );

/* I2C */
struct i2c_s board_i2c[] = {
    { /* I2C PORT 0 */
        .i2c        = I2C_0,
        .pin_scl    = I2C_SCL,
        .pin_sda    = I2C_SDA
    },
    { /* I2C PORT 1 */
        .i2c        = I2C_1,
        .pin_scl    = PB_1,
        .pin_sda    = PB_0
    },
};
const int i32BoardMaxI2CNum = sizeof( board_i2c ) / sizeof( board_i2c[0] );

/* Analog-In */
struct analogin_s board_analogin [] = 
{
    /* Analog-In port */
    { .adc = ADC_0_6,   .pin = A0   },
    { .adc = ADC_0_7,   .pin = A1   },
    { .adc = ADC_0_8,   .pin = A2   },
    { .adc = ADC_0_9,   .pin = A3   },
    { .adc = ADC_0_0,   .pin = A4   },
    { .adc = ADC_0_1,   .pin = A5   },
};
const int i32BoardMaxADCNum  = sizeof( board_analogin ) / sizeof( board_analogin[0] );

/* Analog-Out */
struct analogout_s board_analogout [] = 
{
    /* Analog-In port */
    { .dac = DAC_0, .pin = PB_12    },
    { .dac = DAC_1, .pin = PB_13    },
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
    { .pin = SW2 }, 
    { .pin = SW3 }, 
    { .pin = LED_RED }, 
    { .pin = LED_YELLOW },
    { .pin = LED_GREEN },
};
const int i32BoardMaxGPIONum  = sizeof( board_gpio ) / sizeof( board_gpio[0] );

/* PWM port-M & channel-N */
struct pwmout_s board_pwm [] = 
{
    {   .pwm = PWM_0_0,  .pin = EPWM0_CH0   },
    {   .pwm = PWM_0_1,  .pin = EPWM0_CH1   },
    {   .pwm = PWM_0_2,  .pin = EPWM0_CH2   },
    {   .pwm = PWM_0_3,  .pin = EPWM0_CH3   },
    {   .pwm = PWM_0_4,  .pin = EPWM0_CH4   },
    {   .pwm = PWM_0_5,  .pin = EPWM0_CH5   },

    {   .pwm = PWM_1_0,  .pin = EPWM1_CH0   },
    {   .pwm = PWM_1_1,  .pin = EPWM1_CH1   },
    {   .pwm = PWM_1_2,  .pin = EPWM1_CH2   },
    {   .pwm = PWM_1_3,  .pin = EPWM1_CH3   },

    {   .pwm = PWM_0_3,  .pin = D6  }, //The same with EPWM0_CH3.
    {   .pwm = PWM_0_2,  .pin = D10 }, //The same with EPWM0_CH2.
};
const int i32BoardMaxPWMNum  = sizeof( board_pwm ) / sizeof( board_pwm[0] );

/* SPI */
struct spi_s board_spi [] = 
{
    {   /* SPI0 */
        .spi        = SPI_0,
        .pin_mosi   = PA_0,
        .pin_miso   = PA_1,
        .pin_sclk   = PA_2,
        .pin_ssel   = PA_3,
    },
    {   /* SPI2 */
        .spi        = SPI_2,
        .pin_mosi   = PA_8,
        .pin_miso   = PA_9,
        .pin_sclk   = PA_10,
        .pin_ssel   = PA_11,
    },
};
const int i32BoardMaxSPINum  = sizeof( board_spi ) / sizeof( board_spi[0] );


/* QSPI */
struct qspi_s board_qspi [] = 
{
    {   
        .qspi       = QSPI_0,
        .pin_d0     = PA_0,     //QSPI0-MOSI0
        .pin_d1     = PA_1,     //QSPI0-MISO0
        .pin_d2     = PA_4,     //QSPI0-MOSI1
        .pin_d3     = PA_5,     //QSPI0-MISO1
        .pin_sclk   = PA_2,
        .pin_ssel   = PA_3,
    },
};
const int i32BoardMaxQSPINum  = sizeof( board_qspi ) / sizeof( board_qspi[0] );

/* SD */
struct sdh_s board_sdh [] = 
{
    {   
        .sdh            = SDH_0,
        .pin_dat0 = PE_2,
        .pin_dat1   = PE_3,
        .pin_dat2   = PB_4,
        .pin_dat3   = PB_5,
        .pin_cmd    = PE_7,
        .pin_clk    = PE_6,
        .pin_cdn    = PD_13,
    },
};
const int i32BoardMaxSDHNum  = sizeof( board_sdh ) / sizeof( board_sdh[0] );

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

    /* Enable HIRC clock (Internal RC 22.1184MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    /* Enable LIRC for lp_ticker */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
    /* Enable LXT for RTC */
    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    /* Wait for LIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
    /* Wait for LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    
    /* Set core clock as 192000000 from PLL */
    CLK_SetCoreClock(192000000);
    
    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2); // PCLK divider set 2
    
//#if DEVICE_ANALOGIN
//    /* Vref connect to internal */
    SYS->VREFCTL = (SYS->VREFCTL & ~SYS_VREFCTL_VREFCTL_Msk) | SYS_VREFCTL_VREF_3_0V;
//#endif
    
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}
