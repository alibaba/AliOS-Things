/**
  ******************************************************************************
  * @file    hardware/board/stm32u5_b_u585i_iot02a/startup/startup.c
  * @author  MCD Application Team
  * @brief   Starup for stm32u585
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "stdio.h"
#include "aos/init.h"
#include <k_api.h>
#include "k_config.h"
#include <stdio.h>
#include <stdlib.h>
#include "aos/debug.h"
#include <aos/kernel.h>
#include "stm32u5xx_hal.h"
/*
 * If enable this macro for local test, we don't use wifi service to trigger connection,
 * remember to mask -DALIOS_WIFI_SERVICE and set -DLWIP_SUPPORT=1 in SDK.
 */
#if LWIP_SUPPORT
#define ALIOS_BES_APP
#endif
#define BES_SDK 0
#define BES_WIFI_ONLY 1
#define BES_WIFI_BT 2
#define BES_WIFI_BT_MINI 3 // cut off nouse tasks
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOH
#define LED1_Pin GPIO_PIN_7
#define LED1_GPIO_Port GPIOH
/*
 * main task stask size(byte)
*/
#define OS_MAIN_TASK_STACK (32*1024)
#define OS_MAIN_TASK_PRI 32
static ktask_t *g_main_task;
extern void aos_maintask(void* arg);
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_USART1_UART_Init(void);

#ifdef STM32U5_CONFIG_MICROSPEECH

static void MX_ADF1_Init(void);

MDF_HandleTypeDef AdfHandle0;
MDF_FilterConfigTypeDef AdfFilterConfig0;
#endif //STM32U5_CONFIG_MICROSPEECH
UART_HandleTypeDef huart1;

#if defined ( __GNUC__) && !defined(__clang__)
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
int main(int argc, char *argv[])
{
    aos_status_t  status;
    /* maybe other board level init */
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    board_basic_init();
    HAL_Init();
    /* USER CODE BEGIN Init */
    /* USER CODE END Init */
    /* Configure the system clock */
    SystemClock_Config();
    /* Configure the System Power */
    SystemPower_Config();
    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ICACHE_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    /* kernel init, malloc can use after this! */
    // HAL_SuspendTick();

#ifdef STM32U5_CONFIG_MICROSPEECH
    MX_ADF1_Init();

    if (HAL_MDF_AcqStart(&AdfHandle0, &AdfFilterConfig0) != HAL_OK)
    {
      Error_Handler();
    }

#endif //STM32U5_CONFIG_MICROSPEECH

    aos_init();

    /* main task to run */
    aos_task_create(&g_main_task, "main_task", aos_maintask, NULL, NULL,
                    OS_MAIN_TASK_STACK, OS_MAIN_TASK_PRI, 1u);
    /* kernel start schedule! */
    aos_start();
    /* never run here */
    return 0;
}
static void MX_USART1_UART_Init(void)
{
  /* USER CODE BEGIN USART1_Init 0 */
  /* USER CODE END USART1_Init 0 */
  /* USER CODE BEGIN USART1_Init 1 */
  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  /* USER CODE END USART1_Init 2 */
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{
  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();
  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{
  /* USER CODE BEGIN ICACHE_Init 0 */
  /* USER CODE END ICACHE_Init 0 */
  /* USER CODE BEGIN ICACHE_Init 1 */
  /* USER CODE END ICACHE_Init 1 */
  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */
  /* USER CODE END ICACHE_Init 2 */
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, LED2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, LED1_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pins : LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOH, LED2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, LED1_Pin, GPIO_PIN_RESET);
}
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  while (1)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef STM32U5_CONFIG_MICROSPEECH
/**
  * @brief ADF1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADF1_Init(void)
{

  /* USER CODE BEGIN ADF1_Init 0 */

  /* USER CODE END ADF1_Init 0 */

  /* USER CODE BEGIN ADF1_Init 1 */

  /* USER CODE END ADF1_Init 1 */

  /**
    AdfHandle0 structure initialization and HAL_MDF_Init function call
  */
  AdfHandle0.Instance = ADF1_Filter0;
  AdfHandle0.Init.CommonParam.ProcClockDivider = 1;
  AdfHandle0.Init.CommonParam.OutputClock.Activation = ENABLE;
  AdfHandle0.Init.CommonParam.OutputClock.Pins = MDF_OUTPUT_CLOCK_0;
  AdfHandle0.Init.CommonParam.OutputClock.Divider = 4;
  AdfHandle0.Init.CommonParam.OutputClock.Trigger.Activation = DISABLE;
  AdfHandle0.Init.SerialInterface.Activation = ENABLE;
  AdfHandle0.Init.SerialInterface.Mode = MDF_SITF_NORMAL_SPI_MODE;
  AdfHandle0.Init.SerialInterface.ClockSource = MDF_SITF_CCK0_SOURCE;
  AdfHandle0.Init.SerialInterface.Threshold = 31;
  AdfHandle0.Init.FilterBistream = MDF_BITSTREAM0_FALLING;
  if (HAL_MDF_Init(&AdfHandle0) != HAL_OK)
  {
    Error_Handler();
  }

  /**
    AdfFilterConfig0 structure initialization
    WARNING : only structure is filled, no specific init function call for filter
  */
  AdfFilterConfig0.DataSource = MDF_DATA_SOURCE_BSMX;
  AdfFilterConfig0.Delay = 0;
  AdfFilterConfig0.CicMode = MDF_ONE_FILTER_SINC4;
  AdfFilterConfig0.DecimationRatio = 64;
  AdfFilterConfig0.Gain = 0;
  AdfFilterConfig0.ReshapeFilter.Activation = DISABLE;
  AdfFilterConfig0.HighPassFilter.Activation = DISABLE;
  AdfFilterConfig0.SoundActivity.Activation = ENABLE;
  AdfFilterConfig0.SoundActivity.Mode = MDF_SAD_VOICE_ACTIVITY_DETECTOR;
  AdfFilterConfig0.SoundActivity.FrameSize = MDF_SAD_8_PCM_SAMPLES;
  AdfFilterConfig0.SoundActivity.Hysteresis = DISABLE;
  AdfFilterConfig0.SoundActivity.SoundTriggerEvent = MDF_SAD_ENTER_DETECT;
  AdfFilterConfig0.SoundActivity.DataMemoryTransfer = MDF_SAD_MEMORY_TRANSFER_ALWAYS;
  AdfFilterConfig0.SoundActivity.MinNoiseLevel = 200;
  AdfFilterConfig0.SoundActivity.HangoverWindow = MDF_SAD_HANGOVER_4_FRAMES;
  AdfFilterConfig0.SoundActivity.LearningFrames = MDF_SAD_LEARNING_2_FRAMES;
  AdfFilterConfig0.SoundActivity.AmbientNoiseSlope = 0;
  AdfFilterConfig0.SoundActivity.SignalNoiseThreshold = MDF_SAD_SIGNAL_NOISE_18DB;
  AdfFilterConfig0.SoundActivity.SoundLevelInterrupt = DISABLE;
  AdfFilterConfig0.AcquisitionMode = MDF_MODE_ASYNC_CONT;
  AdfFilterConfig0.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
  AdfFilterConfig0.DiscardSamples = 0;
  /* USER CODE BEGIN ADF1_Init 2 */

  /* USER CODE END ADF1_Init 2 */

}
#endif //STM32U5_CONFIG_MICROSPEECH


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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while(1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
// // PUTCHAR_PROTOTYPE
// // {
// //   /* Place your implementation of fputc here */
// //   /* e.g. write a character to the USART1 and Loop until the end of transmission */
// //   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
// //   return ch;
// // }
// #ifdef __GNUC__
// int _write(int fd, char *ptr, int len)  
// {  
//   for (int i = 0; i < len; i++) {
//     HAL_UART_Transmit(&huart1, (uint8_t*)ptr, 1, 0xFFFF);
//   }
  
//   return len;
// }
// #endif