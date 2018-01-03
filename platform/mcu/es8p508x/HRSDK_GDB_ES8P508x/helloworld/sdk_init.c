/*********************************************************
*Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
*文件名:  sdk_init.c
*作  者:  essemi
*版  本:  V1.00
*日  期:  2017/12/22
*描  述:  按键检测演示主程序
*备  注:  适用于HRSDK-GDB-ES8P508x        
          本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
**********************************************************/

/* Includes ------------------------------------------------------------------*/
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
static void LEDInit(void);
static void KeyInit(void);

void sdk_init(void)
{
    SystemClockConfig();                //配置时钟    
    DeviceClockAllEnable();             //打开所有外设时钟
    SystemTick_Config();               //系统时钟配置

    UARTInit();
    LEDInit();
    KeyInit();
}

/***************************************************************
 函数名：void SysTickInit(void)
 描  述：系统滴答定时器配置
 输入值：无
 输出值：无
 返回值：无
***************************************************************/
void SystemTick_Config(void)
{
    SYSTICK_InitStruType x;

    x.SysTick_Value = SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND;                     
    x.SysTick_ClkSource = SysTick_ClkS_Cpu;
    x.SysTick_ITEnable = ENABLE;                //中断使能
    SysTick_Init(&x);
    SysTick_Enable();
     /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn,0);
}

/*********************************************************
函数名: void UARTInit(void)
描  述: UART初始化子程序
输入值: 无
输出值: 无
返回值: 无 
**********************************************************/
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
    GPIO_Init(GPIOA,GPIO_Pin_15,&y);                 //PA15---TxD
 
	y.GPIO_Signal = GPIO_Pin_Signal_Digital;
    y.GPIO_Func = GPIO_Func_2;
    y.GPIO_Direction = GPIO_Dir_In;
    y.GPIO_PUEN = GPIO_PUE_Input_Disable;
    y.GPIO_PDEN = GPIO_PDE_Input_Disable;
    y.GPIO_OD = GPIO_ODE_Output_Disable;
    y.GPIO_DS = GPIO_DS_Output_Normal;
    GPIO_Init(GPIOA,GPIO_Pin_16,&y);                 //PA16---RxD

    uart.UART_StopBits = UART_StopBits_1;          //停止位：1
    uart.UART_TxMode = UART_DataMode_8;            //发送数据格式：8位数据
    uart.UART_TxPolar = UART_Polar_Normal;         //发送端口极性：正常
    uart.UART_RxMode = UART_DataMode_8;            //接收数据格式：8位数据
    uart.UART_RxPolar = UART_Polar_Normal;         //接收端口极性：正常
    uart.UART_BaudRate = 9600;                     //波特率
    uart.UART_ClockSet = UART_Clock_1;             //时钟选择：Pclk
    UART_Init(UART2,&uart);

    UART_TBIMConfig(UART2,UART_TRBIM_Byte);
    UART_RBIMConfig(UART2, UART_TRBIM_Byte);
    UART_ITConfig(UART2,UART_IT_RB,ENABLE);
//    NVIC_Init(NVIC_UART2_IRQn,NVIC_Priority_1,ENABLE);
    UART2_TxEnable();                               //UART2发送使能
    UART2_RxEnable();
}
 
/*********************************************************
函数名: void LEDInit(void)
描  述: LED初始化
输入值: 无
输出值: 无
返回值: 无 
**********************************************************/
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

    GPIO_Init(GPIOA,GPIO_Pin_14,&x);     //LD1
    GPIO_Init(GPIOA,GPIO_Pin_13,&x);     //LD2
    GPIO_Init(GPIOA,GPIO_Pin_12,&x);     //LD3
    GPIO_Init(GPIOA,GPIO_Pin_11,&x);     //LD4    
    
    GPIOA_SetBit(GPIO_Pin_14);
    GPIOA_SetBit(GPIO_Pin_13);
    GPIOA_SetBit(GPIO_Pin_12);
    GPIOA_SetBit(GPIO_Pin_11);
    
}

/*********************************************************
函数名: void KeyInit(void)
描  述: 按键和按键中中断初始化子程序
输入值: 无
输出值: 无
返回值: 无 
**********************************************************/
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
    GPIO_Init(GPIOB,GPIO_Pin_7,&x);         //KL1
    GPIO_Init(GPIOB,GPIO_Pin_2,&x);         //KL2
    GPIO_Init(GPIOB,GPIO_Pin_8,&x);         //KR1
    GPIO_Init(GPIOB,GPIO_Pin_3,&x);         //KR2
    GPIO_Init(GPIOA,GPIO_Pin_20,&x);         //K5

    PINT_Config(PINT4, PINT_SEL2, PINT_Trig_Rise);          //选择SEL2中断源，上升沿触发中断
    NVIC_Init(NVIC_PINT4_IRQn,NVIC_Priority_2,ENABLE);
    
    PINT_Config(PINT7, PINT_SEL4, PINT_Trig_Rise);          //选择SEL2中断源，上升沿触发中断
    NVIC_Init(NVIC_PINT7_IRQn,NVIC_Priority_2,ENABLE);
    
    PINT_Config(PINT2, PINT_SEL4, PINT_Trig_Rise);          //选择SEL2中断源，上升沿触发中断
    NVIC_Init(NVIC_PINT2_IRQn,NVIC_Priority_2,ENABLE);    
    
    PINT4_MaskDisable();
    PINT4_Enable();                       //开启KINT中断
    PINT2_MaskDisable();
    PINT2_Enable();                       //开启KINT中断
    PINT7_MaskDisable();
    PINT7_Enable();                       //开启KINT中断
}

