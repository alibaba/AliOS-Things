/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/init.h"
#include "aos/hal/uart.h"

#include "k_config.h"
#include "board.h"
#include "stm32f1xx_hal.h"
#include "hal_gpio_stm32f1.h"
#include "hal_uart_stm32f1.h"
#include "hal_i2c_stm32f1.h"
#include "hal_spi_stm32f1.h"
#include "hal_adc_stm32f1.h"
#include "hal_timer_stm32f1.h"
#include "hal_pwm_stm32f1.h"

/* HAL PINS MAP*/
/*
* @ default logic uart port: PORT_UART_STD to map with different physical GPIO port
*/
uart_dev_t uart_0;

static gpio_uart_pin_config_t usart2_pin_conf[] = {
    {UART_TX, HAL_GPIO_2},
    {UART_RX, HAL_GPIO_3}
};

static gpio_uart_pin_config_t usart1_pin_conf[] = {
    {UART_TX, HAL_GPIO_9},
    {UART_RX, HAL_GPIO_10}
};

/*
UART logic port maps to phy dev;
UART_MAPPING is defined in hal uart head file for mcu;
PORT_UART_STD is defined in board.h
*/
UART_MAPPING UART_MAPPING_TABLE[] =
{
  {PORT_UART_STD, USART2, {UART_OVERSAMPLING_16, 64}, usart2_pin_conf,
   sizeof(usart2_pin_conf)/sizeof(usart2_pin_conf[0])},
  {PORT_UART_AT, USART1, {UART_OVERSAMPLING_16, 64}, usart1_pin_conf,
   sizeof(usart1_pin_conf)/sizeof(usart1_pin_conf[0])}
};

/*
I2C logic port maps to phy dev;
I2C_MAPPING is defined in hal i2c head file for mcu;
PORT_I2C_1 is defined in board.h
*/
I2C_MAPPING i2c_mapping[PORT_I2C_SIZE] = {
    {PORT_I2C_1,I2C1,HAL_I2C_GPIO_NEED_MAP,{HAL_GPIO_24,HAL_GPIO_25}}
};

#ifdef HAL_ADC_MODULE_ENABLED
static gpio_adc_pin_config_t adc1_conf[] = {
    {HAL_ADC_CHANNEL_10, HAL_GPIO_32},
    {HAL_ADC_CHANNEL_11, HAL_GPIO_33}
};

/*
ADC logic port maps to phy dev;
ADC_MAPPING is defined in hal adc head file for mcu;
PORT_ADC_1 is defined in board.h
*/
ADC_MAPPING ADC_MAPPING_TABLE[PORT_ADC_SIZE] = {
    {PORT_ADC_1, HAL_ADC_1, adc1_conf, sizeof(adc1_conf)/sizeof(adc1_conf[0])},
};
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
static gpio_spi_pin_config_t spi1_pin_conf[] = {
    {SPI_PIN_CS, HAL_GPIO_22},
    {SPI_PIN_CLK, HAL_GPIO_5},
    {SPI_PIN_MISO, HAL_GPIO_6},
    {SPI_PIN_MOSI, HAL_GPIO_7},
};

/*
SPI logic port maps to phy dev;
SPI_MAPPING is defined in hal spi head file for mcu;
PORT_SPI_1 is defined in board.h
*/
SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE] = {
    {
        PORT_SPI_1,
        SPI1,
        .attr = {
            .Direction = SPI_DIRECTION_2LINES,
            .DataSize = SPI_DATASIZE_8BIT,
            .CLKPolarity = SPI_POLARITY_HIGH,
            .CLKPhase = SPI_PHASE_2EDGE,
            .NSS = SPI_NSS_SOFT,
            .FirstBit = SPI_FIRSTBIT_MSB,
            .TIMode = SPI_TIMODE_DISABLE,
            .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
            .CRCPolynomial = 7
        },
        HAL_SPI_GPIO_NEED_MAP,
        spi1_pin_conf,
        sizeof(spi1_pin_conf)/sizeof(spi1_pin_conf[0]),
    }
};
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
/*
TIMER logic port maps to phy dev;
TIMER_MAPPING is defined in hal timer head file for mcu;
PORT_TIMER_1 is defined in board.h
*/
TIMER_MAPPING TIMER_MAPPING_TABLE[PORT_TIMER_SIZE] =
{
    {PORT_TIMER_DEMO, HAL_TIMER_4},
    {PORT_TIMER_1, HAL_TIMER_2},
};
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
static struct stm32_pwmchan_s pwm3chan[] = {
    {
        .channel = TIM_CHANNEL_3,
        .mode = TIM_OCMODE_PWM1,
        .out1 = {
            .pol = TIM_OCPOLARITY_LOW,
            .pin = HAL_GPIO_16,
        },
    }
};

/*
PWM logic port maps to phy dev;
PWM_MAPPING is defined in hal pwm head file for mcu;
PORT_PWM_3 is defined in board.h
*/
PWM_MAPPING PWM_MAPPING_TABLE[PORT_PWM_SIZE] =
{
    {PORT_PWM_3, HAL_TIMER_3, pwm3chan, sizeof(pwm3chan)/sizeof(pwm3chan[0])},
};
#endif /* HAL_TIM_MODULE_ENABLED */


/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{
	/*user code begin*/
    /*mm heap set*/
    aos_heap_set();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    /* Configure the system clock */
    SystemClock_Config();

    /*user code end*/
}

/*user code begin*/

/**
  * @general board tick init entry board_tick_init
  * @retval None
  */
void board_tick_init(void)
{
    /* Configure the Systick interrupt time ,use RHINO_CONFIG_TICKS_PER_SECOND to init tick clock*/
  /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);

  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0x0f, 0x0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
* @init the default uart init example.
*/
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

/**
  * Enable DMA controller clock
  */
void board_dma_init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

int32_t board_get_gpio_group(uint32_t gpio_pin, GPIO_TypeDef **GPIOx)
{
    uint16_t group = 0;
    int32_t ret = 0;

    if (GPIOx == NULL) {
        return -1;
    }

    group = gpio_pin / PINS_IN_GROUP;

    switch (group) {
    case GROUP_GPIOA:
        *GPIOx = GPIOA;
        break;
    case GROUP_GPIOB:
        *GPIOx = GPIOB;
        break;
    case GROUP_GPIOC:
        *GPIOx = GPIOC;
        break;
#if defined(GROUP_GPIOD) && defined(GPIOD)
    case GROUP_GPIOD:
        *GPIOx = GPIOD;
        break;
#endif
#if defined(GROUP_GPIOE) && defined(GPIOE)
    case GROUP_GPIOE:
        *GPIOx = GPIOE;
        break;
#endif
#if defined(GROUP_GPIOF) && defined(GPIOF)
    case GROUP_GPIOF:
        *GPIOx = GPIOF;
        break;
#endif
#if defined(GROUP_GPIOG) && defined(GPIOG)
    case GROUP_GPIOG:
        *GPIOx = GPIOG;
        break;
#endif

    default:
        ret = -1;
        break;
    }

    return ret;
}

uint32_t board_get_gpio_pin(uint32_t gpio_pin)
{
    uint32_t result = 1;
    uint8_t pin_t = gpio_pin % PINS_IN_GROUP;

    return (uint32_t)(1<<pin_t);
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void board_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_TypeDef *GPIOx = NULL;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  board_get_gpio_group(LED2, &GPIOx);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOx, board_get_gpio_pin(LED2), GPIO_PIN_RESET);

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = board_get_gpio_pin(KEY1);//KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  board_get_gpio_group(KEY1, &GPIOx);
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = board_get_gpio_pin(LED2);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  board_get_gpio_group(LED2, &GPIOx);
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/**
  * @brief WIFI Initialization Function
  * @param None
  * @retval None
  */
void board_wifi_init(void)
{

}

/**
  * @brief including WIFI\ETH\BLUETOOTH and so on.
  * @param None
  * @retval None
  */
void board_network_init(void)
{
    board_wifi_init();
}

/**
  * @brief kernel init args.
  * @param None
  * @retval None
  */
void board_kinit_init(kinit_t* init_args)
{
    return;
}

/**
  * @brief base flash partitions and so on.
  * @param None
  * @retval None
  */
void board_flash_init(void)
{
    flash_partition_init();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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
