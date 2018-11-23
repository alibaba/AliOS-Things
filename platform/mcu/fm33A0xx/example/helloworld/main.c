/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>

/* rhino task definition */
#define DEMO_TASK_STACKSIZE    256
#define DEMO_TASK_PRIORITY     20

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];


/* Private variables ---------------------------------------------------------*/
//UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
ktask_t *g_aos_init;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART4_UART_Init(void);

/* Private function prototypes -----------------------------------------------*/
void hal_reboot(void) {}

extern int application_start(void);
void aos_app_entry(void *args)
{
//    HW_Init();
    //DBG_Init();
    application_start();
}

int main(void)
{

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 // HAL_Init();

  /* Configure the system clock */
 // SystemClock_Config();

  /* Initialize all configured peripherals */
 // MX_GPIO_Init();
 // MX_USART4_UART_Init();

  /* aos initialization */
  aos_init();

  /* aos rhino task creation */
  krhino_task_create(&demo_task_obj, "aos app", 0,DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, (task_entry_t)aos_app_entry, 1);

  /* aos start */
  aos_start();

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif


