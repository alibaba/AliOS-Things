/*********************************************************
*Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "board.h"
#include "k_config.h"
#include "lib_config.h"

void SystemTick_Config(void);
static void UARTInit(void);
static void LEDInit(void);
static void KeyInit(void);

void board_init(void)
{
    SystemClockConfig();
    DeviceClockAllEnable();
    SystemTick_Config();

    UARTInit();
    LEDInit();
    KeyInit();
}

void SystemTick_Config(void)
{
    SYSTICK_InitStruType x;

    x.SysTick_Value = SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND;                     
    x.SysTick_ClkSource = SysTick_ClkS_Cpu;
    x.SysTick_ITEnable = ENABLE;                
    SysTick_Init(&x);
    SysTick_Enable();
     /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn,0);
}

static void UARTInit(void)
{
    GPIO_InitStruType y;
    UART_InitStruType uart;

    y.GPIO_Signal = GPIO_Pin_Signal_Digital;
    y.GPIO_Func = GPIO_Func_2;
    y.GPIO_Direction = GPIO_Dir_Out;
    y.GPIO_PUEN = GPIO_PUE_Input_Disable;
    y.GPIO_PDEN = GPIO_PDE_Input_Disable;
    y.GPIO_OD = GPIO_ODE_Output_Disable;
    y.GPIO_DS = GPIO_DS_Output_Normal;
    GPIO_Init(GPIOA,GPIO_Pin_15,&y);                 
 
	y.GPIO_Signal = GPIO_Pin_Signal_Digital;
    y.GPIO_Func = GPIO_Func_2;
    y.GPIO_Direction = GPIO_Dir_In;
    y.GPIO_PUEN = GPIO_PUE_Input_Disable;
    y.GPIO_PDEN = GPIO_PDE_Input_Disable;
    y.GPIO_OD = GPIO_ODE_Output_Disable;
    y.GPIO_DS = GPIO_DS_Output_Normal;
    GPIO_Init(GPIOA,GPIO_Pin_16,&y);                 

    uart.UART_StopBits = UART_StopBits_1;           
    uart.UART_TxMode = UART_DataMode_8;             
    uart.UART_TxPolar = UART_Polar_Normal;          
    uart.UART_RxMode = UART_DataMode_8;             
    uart.UART_RxPolar = UART_Polar_Normal;          
    uart.UART_BaudRate = 9600;                      
    uart.UART_ClockSet = UART_Clock_1;              
    UART_Init(UART2,&uart);

    UART_TBIMConfig(UART2,UART_TRBIM_Byte);
    UART_RBIMConfig(UART2, UART_TRBIM_Byte);
    UART_ITConfig(UART2,UART_IT_RB,ENABLE);

    UART2_TxEnable();                               
    UART2_RxEnable();
}
 
static void LEDInit(void)
{
    GPIO_InitStruType x;

	x.GPIO_Signal = GPIO_Pin_Signal_Digital;
    x.GPIO_Func = GPIO_Func_0;
    x.GPIO_Direction = GPIO_Dir_Out;
    x.GPIO_PUEN = GPIO_PUE_Input_Disable;
    x.GPIO_PDEN = GPIO_PDE_Input_Disable;
    x.GPIO_OD = GPIO_ODE_Output_Disable;
    x.GPIO_DS = GPIO_DS_Output_Normal;

    GPIO_Init(GPIOA,GPIO_Pin_14,&x);                
    GPIO_Init(GPIOA,GPIO_Pin_13,&x);                
    GPIO_Init(GPIOA,GPIO_Pin_12,&x);                
    GPIO_Init(GPIOA,GPIO_Pin_11,&x);                    
    
    GPIOA_SetBit(GPIO_Pin_14);
    GPIOA_SetBit(GPIO_Pin_13);
    GPIOA_SetBit(GPIO_Pin_12);
    GPIOA_SetBit(GPIO_Pin_11);
    
}

static void KeyInit(void)
{
    GPIO_InitStruType x;

	x.GPIO_Signal = GPIO_Pin_Signal_Digital;
    x.GPIO_Func = GPIO_Func_0;
    x.GPIO_Direction = GPIO_Dir_In;
    x.GPIO_PUEN = GPIO_PUE_Input_Enable;
    x.GPIO_PDEN = GPIO_PDE_Input_Disable;
    x.GPIO_OD = GPIO_ODE_Output_Disable;
    x.GPIO_DS = GPIO_DS_Output_Normal;
    GPIO_Init(GPIOB,GPIO_Pin_7,&x);         
    GPIO_Init(GPIOB,GPIO_Pin_2,&x);         
    GPIO_Init(GPIOB,GPIO_Pin_8,&x);         
    GPIO_Init(GPIOB,GPIO_Pin_3,&x);         
    GPIO_Init(GPIOA,GPIO_Pin_20,&x);        

    PINT_Config(PINT4, PINT_SEL2, PINT_Trig_Rise);          
    NVIC_Init(NVIC_PINT4_IRQn,NVIC_Priority_2,ENABLE);
    
    PINT_Config(PINT7, PINT_SEL4, PINT_Trig_Rise);          
    NVIC_Init(NVIC_PINT7_IRQn,NVIC_Priority_2,ENABLE);
    
    PINT_Config(PINT2, PINT_SEL4, PINT_Trig_Rise);          
    NVIC_Init(NVIC_PINT2_IRQn,NVIC_Priority_2,ENABLE);    
    
    PINT4_MaskDisable();
    PINT4_Enable();                       
    PINT2_MaskDisable();
    PINT2_Enable();                       
    PINT7_MaskDisable();
    PINT7_Enable();                       
}

