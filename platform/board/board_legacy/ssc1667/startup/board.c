/**
  ******************************************************************************
  * @file    sdk_init.c
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
#include "system_p131.h"
#include "k_api.h"

/* Private constants ----------------------------------------------------------*/
/* Private types --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
void SystemTick_Config(void);
static void UART1_Init(void);

/* Public functions -----------------------------------------------------------*/

/**
  * @brief  board_init.
  * @param  None
  * @retval None
  */
void board_init(void)
{
//  DeviceClockAllEnable();
    SystemTick_Config();
    SCU->PROT.Word = 0x55aa6996;
    SCU->SCLKEN1.DPLL_OSC_SEL = 0;
    SCU->SCLKEN1.OSCCLK_SEL = 1;
    SCU->PROT.Word = 0;

    UART1_Init();
}

/**
  * @brief  SystemTick_Config.
  * @param  None
  * @retval None
  */
void SystemTick_Config(void)
{
    SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);
}

/**
  * @brief  UART1_Init.
  * @param  None
  * @retval None
  */
static void UART1_Init(void)
{
    GPIO_InitSettingType x;
    UART_InitStruType y;

    x.Dir = GPIO_Direction_Output;
    x.DS = GPIO_DS_Output_Normal;
    x.Func = GPIO_Func2;
    x.Signal = GPIO_Signal_Digital;
    x.ODE = GPIO_ODE_Output_Disable;
    x.PDE = GPIO_PDE_Input_Disable;
    x.PUE = GPIO_PUE_Input_Disable;
    GPIO_Init(GPIO_Pin_A23,&x);    //PA23-TX
    x.PUE = GPIO_PUE_Input_Enable;;
    x.Dir = GPIO_Direction_Input;
    GPIO_Init(GPIO_Pin_A22,&x);   //PA22-RX
  
    y.UART_BaudRate = 115200;
    y.UART_ClockSet = UART_Clock_1;
    y.UART_RxMode   = UART_DataMode_8;
    y.UART_RxPolar  = UART_Polar_Normal;
    y.UART_StopBits = UART_StopBits_1;
    y.UART_TxMode   = UART_DataMode_8;
    y.UART_TxPolar  = UART_Polar_Normal;
    UART_Init(UART1, &y);

    UART1_TxEnable();
}

/******************* (C) COPYRIGHT Eastsoft Microelectronics *****END OF FILE****/
