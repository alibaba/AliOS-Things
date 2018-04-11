/*********************************************************
*Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "sdk_init.h"
#include "k_config.h"
#include "lib_config.h"

#if defined (__CC_ARM) && defined(__MICROLIB)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}
#endif


void SystemTick_Config(void);
static void UARTInit(void);
//static void LEDInit(void);
//static void KeyInit(void);

void sdk_init(void)
{
   // SystemClockConfig();                
    DeviceClockAllEnable();             
    SystemTick_Config();               

    UARTInit();
//    LEDInit();
//    KeyInit();
}

void SystemTick_Config(void)
{
    SYSTICK_InitStruType x;

    x.SysTick_Value = SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND;                     
    x.SysTick_ClkSource = SysTick_ClkS_Cpu;
    x.SysTick_ITEnable = Enable;                
    SysTick_Init(&x);
    SysTick_Enable();
     /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn,0);
}

static void UARTInit(void)
{
    GPIO_InitSettingType x;
    UART_InitStruType y;

    x.Signal = GPIO_Pin_Signal_Digital;
    x.Dir = GPIO_Direction_Output;
    x.Func = GPIO_Reuse_Func1;
    x.ODE = GPIO_ODE_Output_Disable;
    x.DS = GPIO_DS_Output_Strong;
    x.PUE = GPIO_PUE_Input_Disable;
    x.PDE = GPIO_PDE_Input_Disable;
    GPIO_Init(GPIO_Pin_A23, &x);        //TXD - PA23

    x.Signal = GPIO_Pin_Signal_Digital;
    x.Dir = GPIO_Direction_Input;
    x.Func = GPIO_Reuse_Func1;
    x.ODE = GPIO_ODE_Output_Disable;
    x.DS = GPIO_DS_Output_Strong;
    x.PUE = GPIO_PUE_Input_Disable;
    x.PDE = GPIO_PDE_Input_Disable;
    GPIO_Init(GPIO_Pin_A22, &x);        //RXD - PA22

    y.UART_StopBits = UART_StopBits_1;  //停止位：1
    y.UART_TxMode = UART_DataMode_8;    //发送数据格式：8位数据
    y.UART_TxPolar = UART_Polar_Normal; //发送端口极性：正常
    y.UART_RxMode = UART_DataMode_8;    //接收数据格式：8位数据
    y.UART_RxPolar = UART_Polar_Normal; //接收端口极性：正常
    y.UART_BaudRate = 9600;             //波特率
    y.UART_ClockSet = UART_Clock_1;     //时钟选择：Pclk
    UART_Init(UART0, &y);

    UART_TBIMConfig(UART0, UART_TBIM_Byte);
    UART_RBIMConfig(UART0, UART_TBIM_Byte);
    UART_ITConfig(UART0, UART_IT_RB, Enable);
    NVIC_Init(NVIC_UART0_IRQn, NVIC_Priority_1, Enable);

    UART0_TxEnable();
    UART0_RxEnable();
}
 
//static void LEDInit(void)
//{
//    GPIO_InitStruType x;

//	x.GPIO_Signal = GPIO_Pin_Signal_Digital;
//    x.GPIO_Func = GPIO_Func_0;
//    x.GPIO_Direction = GPIO_Dir_Out;
//    x.GPIO_PUEN = GPIO_PUE_Input_Disable;
//    x.GPIO_PDEN = GPIO_PDE_Input_Disable;
//    x.GPIO_OD = GPIO_ODE_Output_Disable;
//    x.GPIO_DS = GPIO_DS_Output_Normal;

//    GPIO_Init(GPIOA,GPIO_Pin_14,&x);                
//    GPIO_Init(GPIOA,GPIO_Pin_13,&x);                
//    GPIO_Init(GPIOA,GPIO_Pin_12,&x);                
//    GPIO_Init(GPIOA,GPIO_Pin_11,&x);                    
//    
//    GPIOA_SetBit(GPIO_Pin_14);
//    GPIOA_SetBit(GPIO_Pin_13);
//    GPIOA_SetBit(GPIO_Pin_12);
//    GPIOA_SetBit(GPIO_Pin_11);
//    
//}

//static void KeyInit(void)
//{
//    GPIO_InitStruType x;

//	x.GPIO_Signal = GPIO_Pin_Signal_Digital;
//    x.GPIO_Func = GPIO_Func_0;
//    x.GPIO_Direction = GPIO_Dir_In;
//    x.GPIO_PUEN = GPIO_PUE_Input_Enable;
//    x.GPIO_PDEN = GPIO_PDE_Input_Disable;
//    x.GPIO_OD = GPIO_ODE_Output_Disable;
//    x.GPIO_DS = GPIO_DS_Output_Normal;
//    GPIO_Init(GPIOB,GPIO_Pin_7,&x);         
//    GPIO_Init(GPIOB,GPIO_Pin_2,&x);         
//    GPIO_Init(GPIOB,GPIO_Pin_8,&x);         
//    GPIO_Init(GPIOB,GPIO_Pin_3,&x);         
//    GPIO_Init(GPIOA,GPIO_Pin_20,&x);        

//    PINT_Config(PINT4, PINT_SEL2, PINT_Trig_Rise);          
//    NVIC_Init(NVIC_PINT4_IRQn,NVIC_Priority_2,ENABLE);
//    
//    PINT_Config(PINT7, PINT_SEL4, PINT_Trig_Rise);          
//    NVIC_Init(NVIC_PINT7_IRQn,NVIC_Priority_2,ENABLE);
//    
//    PINT_Config(PINT2, PINT_SEL4, PINT_Trig_Rise);          
//    NVIC_Init(NVIC_PINT2_IRQn,NVIC_Priority_2,ENABLE);    
//    
//    PINT4_MaskDisable();
//    PINT4_Enable();                       
//    PINT2_MaskDisable();
//    PINT2_Enable();                       
//    PINT7_MaskDisable();
//    PINT7_Enable();                       
//}

