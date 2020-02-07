/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "k_config.h"
#include "board.h"

#include "aos/hal/uart.h"
#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "aos/hal/can.h"
#include "aos/hal/timer.h"

#include "stm32f4xx_hal.h"
#include "hal_uart_stm32f4.h"
#include "hal_gpio_stm32f4.h"
#include "hal_can_stm32f4.h"
#include "hal_timer_stm32f4.h"
#include "hal_pwm_stm32f4.h"

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

uart_dev_t uart_0;

const gpio_mapping_t gpio_mapping_table[TOTAL_GPIO_NUM] =
{
    /*  logic port      group    pin            speed               val   */
    {LED0,              GPIOB, GPIO_PIN_0,  GPIO_SPEED_FREQ_LOW, GPIO_PULLDOWN},
    {LED1,              GPIOB, GPIO_PIN_7,  GPIO_SPEED_FREQ_LOW, GPIO_PULLDOWN},
    {LED2,              GPIOB, GPIO_PIN_14, GPIO_SPEED_FREQ_LOW, GPIO_PULLDOWN},
    {KEY0,              GPIOC, GPIO_PIN_13, GPIO_SPEED_FREQ_LOW, GPIO_PULLDOWN},
    {HAL_GPIO_8,        GPIOA, GPIO_PIN_8,  GPIO_SPEED_FREQ_LOW, GPIO_PULLUP},
    {HAL_GPIO_9,        GPIOA, GPIO_PIN_9,  GPIO_SPEED_FREQ_LOW, GPIO_PULLUP},
    {ON_BOARD_TIM4_CH4, GPIOD, GPIO_PIN_15, GPIO_SPEED_FREQ_LOW, GPIO_PULLUP}
};

static gpio_dev_t board_gpio_table[] =
{
    {ON_BOARD_LED01, OUTPUT_PUSH_PULL, NULL},
    {ON_BOARD_LED02, OUTPUT_PUSH_PULL, NULL},
    {ON_BOARD_LED03, OUTPUT_PUSH_PULL, NULL},
};

CAN_MAPPING CAN_MAPPING_TABLE[] =
{
#ifdef AOS_CANOPEN
    { PORT_CAN_CANOPEN, CAN1, can_dispatch, NULL, NULL},
#endif
    { PORT_CAN_CANOPEN, NULL, NULL, NULL, NULL},
};

TIMER_MAPPING TIMER_MAPPING_TABLE[] =
{
    {PORT_TIMER_CANOPEN, TIM3},
    {PORT_TIMER_3, TIM3},
    {PORT_TIMER_4, TIM4},
    {PORT_TIMER_5, TIM5},
};

PWM_MAPPING PWM_MAPPING_TABLE[] =
{
    {PORT_PWM_4, TIM4, HAL_TIM_ACTIVE_CHANNEL_4, GROUP_GPIOD, HAL_GPIO_15},
};


UART_MAPPING UART_MAPPING_TABLE[] =
{
    { PORT_UART_STD,     USART3, { UART_OVERSAMPLING_16, 1024} },
    { PORT_UART_AT,      USART6,  { UART_OVERSAMPLING_16, 2048} },
    { PORT_UART_RS485,   UART7, { UART_OVERSAMPLING_16, 512} },
    { PORT_UART_SCANNER, UART4,  { UART_OVERSAMPLING_16, 512} },
    { PORT_UART_LORA,    UART5,  { UART_OVERSAMPLING_16, 512} },
};

void* i2c_mapping_table[] = { I2C1, I2C2, I2C3};

static void stduart_init(void);
static void I2C1_init();

void Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    printf("Fatel error, file:%s, line:%d\r\n",file, line);
    while(1) {

    }
    /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
  }

}

/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{
    /*mm heap set*/
    aos_heap_set();

    /* Configure the system clock */
    SystemClock_Config();

    HAL_Init();
}

void board_tick_init(void)
{
    /**
     * Configure the Systick interrupt time
     */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);
}

uint32_t hal_timer_getcounter(timer_dev_t *tim)
{
    uint32_t counter = 0;
    if (tim != NULL)
    {
         counter =  __HAL_TIM_GET_COUNTER((TIM_HandleTypeDef *)tim->priv);
    }
    return counter;
}

void hal_timer_setcounter(timer_dev_t *tim, uint32_t counter)
{
    if (tim != NULL)
    {
        __HAL_TIM_SET_COUNTER((TIM_HandleTypeDef *)tim->priv, counter);
        __HAL_TIM_ENABLE((TIM_HandleTypeDef *)tim->priv);
    }
}

int32_t board_gpio_init(void)
{
    int32_t i;
    int32_t ret = 0;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();


    for (i = 0; i < TOTAL_GPIO_NUM; ++i) {
        ret = hal_gpio_init(&board_gpio_table[i]);
        if (ret) {
            printf("gpio %d in gpio table init fail \r\n", i);
        }
    }
}

void board_stduart_init(void)
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

void board_dma_init(void)
{
    MX_DMA_Init();
}

void board_wifi_init(void)
{

}

static void I2C1_init()
{
    i2c_dev_t i2c_1 = {
        .port                 = 1,
        .config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT,
        .config.freq          = I2C_BUS_BIT_RATES_100K,
        .config.mode          = I2C_MODE_MASTER,
    };

    if (hal_i2c_init(&i2c_1)) {
        printf("i2c bus 1 init fail \r\n");
    }
}


void stm32_peripheral_init(void)
{
    /*default uart init*/
    stduart_init();
    /*gpio init*/
    brd_gpio_init();
    /*i2c pre init*/
    hal_i2c_pre_init();
    /*i2c bus 1 init*/
    I2C1_init();
#ifndef WITH_SAL
    /*enable ethernet*/
    //MX_ETH_Init();
    //lwip_tcpip_init();
#endif
    /*default can init*/
    #ifdef PT_SENSOR
    CAN_init();
    #endif
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  HAL_IncTick();
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

