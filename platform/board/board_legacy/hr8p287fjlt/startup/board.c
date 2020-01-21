/**
  ******************************************************************************
  * @file    board.c
  * @brief   Hardware Initialization Source File.
  *
  * @version V0.01
  * @data    2018-7-4
  * @author  Eastsoft AE Team
  * @note
  * 
  *
  * Copyright (C) 2018 Shanghai Eastsoft Microelectronics Co., Ltd. ALL rights reserved.
  *******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/ 
#include "board.h"
#include "k_config.h"

#include "lib_config.h" 

#include "HR8P287.h"
#include "system_HR8P2xx.h"

/* Private constants ----------------------------------------------------------*/
/* Private types --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
void SystemTick_Config(void);
static void UART0_Init(void);

/* Public functions -----------------------------------------------------------*/

/**
  * @brief  board_init.
  * @param  None
  * @retval None
  */
void board_init(void)
{
    DeviceClockAllEnable();
    SystemTick_Config();

    UART0_Init();
}

/**
  * @brief  SystemTick_Config.
  * @param  None
  * @retval None
  */
void SystemTick_Config(void)
{
    SYSTICK_InitStruType x;
    /* systic configuration */
    x.SysTick_Value = SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND;
    x.SysTick_ClkSource = SysTick_ClkS_Cpu;
    x.SysTick_ITEnable = ENABLE;
    SysTick_Init(&x);
    SysTick_Enable();
    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn,0);
}

/**
  * @brief  UART0_Init.
  * @param  None
  * @retval None
  */
static void UART0_Init(void)
{
    GPIO_InitStruType y;
    UART_InitStruType uart0;
    /* gpio configuration */
    GPIO_RegUnLock();
    y.GPIO_Func = GPIO_Func_2;
    y.GPIO_Direction = GPIO_Dir_In;
    y.GPIO_PUEN = DISABLE;
    y.GPIO_PDEN = DISABLE;
    y.GPIO_OD = DISABLE;
    GPIO_Init(GPIOA,GPIO_Pin_0,&y);         //PA0---RxD
    y.GPIO_Direction = GPIO_Dir_Out;
    GPIO_Init(GPIOA,GPIO_Pin_1,&y);         //PA1---TxD
    GPIO_RegLock();
    /* uart0 configuration */
    uart0.UART_StopBits = UART_StopBits_1;
    uart0.UART_TxMode = UART_DataMode_8;
    uart0.UART_TxPolar = UART_Polar_Normal;
    uart0.UART_RxMode = UART_DataMode_8;
    uart0.UART_RxPolar = UART_Polar_Normal;
    uart0.UART_BaudRate = 115200;
    uart0.UART_ClockSet = UART_Clock_1;
    UART_Init(UART0,&uart0);

    UART_TBIMConfig(UART0,UART_TBIM_Byte);
    UART_RBIMConfig(UART0, UART_TBIM_Byte);
    /* enable tx\rx */
    UART0_TxEnable();
    UART0_RxEnable();
}

/******************* (C) COPYRIGHT Eastsoft Microelectronics *****END OF FILE****/
