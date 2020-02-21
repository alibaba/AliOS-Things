/**
  ******************************************************************************
  * @file    board.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
  */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/uart.h"

#include "k_config.h"
#include "board.h"

#include "stm32f1xx_hal.h"
#include "hal_gpio_stm32f1.h"
#include "hal_uart_stm32f1.h"
#include "hal_i2c_stm32f1.h"
#include "hal_spi_stm32f1.h"

uart_dev_t uart_0;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;
UART_MAPPING UART_MAPPING_TABLE[] =
{
  {PORT_UART_STD, USART2, {UART_OVERSAMPLING_16, 64}}
};

i2c_mapping_t i2c_mapping[PORT_I2C_SIZE] = {{PORT_I2C_1,I2C1}};

SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE] = {
{PORT_SPI_1,SPI1, {SPI_DIRECTION_2LINES,SPI_DATASIZE_8BIT,SPI_POLARITY_HIGH,SPI_PHASE_2EDGE,\
    SPI_FIRSTBIT_MSB,SPI_TIMODE_DISABLE,SPI_CRCCALCULATION_DISABLE,0} }, \
{1,HAL_GPIO_5,HAL_GPIO_22,HAL_GPIO_7,HAL_GPIO_6},
};


/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{
    /*mm heap set*/
    aos_heap_set();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    //HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
}
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
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  krhino_intrpt_enter();
  USART_DMA_RX_IRQHandler(USART2);
  krhino_intrpt_exit();
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  krhino_intrpt_enter();
  USART_DMA_TX_IRQHandler(USART2);
  krhino_intrpt_exit();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
