/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "k_config.h"
#include "string.h"


#if defined                                         (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE           int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE           int fgetc(FILE *f)
#elif defined(__ICCARM__)           
#define PUTCHAR_PROTOTYPE           int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE           int fgetc(FILE *f)

#else

/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE           int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE           int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */
uart_dev_t uart_0;

void Init_SysTick(uint32_t ticksPerSecond)
{
    SysTick_Config(SYSTEM_CLK / ticksPerSecond);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
    SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void AnalogIO(GPIOx_Type * GPIOx, uint32_t PinNum)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.Pin != PinNum) ||
         (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) ||
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_En) ||
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis) ||
         (GPIO_InitStructureRun.PxFCR != GPIO_FCR_ANA))
    {
        GPIO_InitStructure.Pin = PinNum;
        GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
        GPIO_InitStructure.PxODEN = GPIO_OD_En;
        GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
        GPIO_InitStructure.PxFCR = GPIO_FCR_ANA;

        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}

#define IN_NORMAL                           0
#define IN_PULLUP                           1

void InputIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.Pin != PinNum) ||
         (GPIO_InitStructureRun.PxINEN != GPIO_IN_En) ||
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_En) || ((Type == IN_NORMAL) &&
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis)) || ((Type == IN_PULLUP) &&
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_En)) ||
         (GPIO_InitStructureRun.PxFCR != GPIO_FCR_IN))
    {
        GPIO_InitStructure.Pin = PinNum;
        GPIO_InitStructure.PxINEN = GPIO_IN_En;
        GPIO_InitStructure.PxODEN = GPIO_OD_En;
        if (Type == IN_NORMAL) GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
        else GPIO_InitStructure.PxPUEN = GPIO_PU_En;
        GPIO_InitStructure.PxFCR = GPIO_FCR_IN;

        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}

#define OUT_PUSHPULL                        0
#define OUT_OPENDRAIN                   1

void OutputIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.Pin != PinNum) ||
         (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) || ((Type == OUT_PUSHPULL) &&
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_Dis)) || ((Type == OUT_OPENDRAIN) &&
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_En)) ||
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis) ||
         (GPIO_InitStructureRun.PxFCR != GPIO_FCR_OUT))
    {
        GPIO_InitStructure.Pin = PinNum;
        GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
        if (Type == OUT_PUSHPULL) GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
        else GPIO_InitStructure.PxODEN = GPIO_OD_En;
        GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
        GPIO_InitStructure.PxFCR = GPIO_FCR_OUT;

        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}

#define ALTFUN_NORMAL                   0
#define ALTFUN_OPENDRAIN                1
#define ALTFUN_PULLUP                   2
#define ALTFUN_OPENDRAIN_PULLUP 3

void AltFunIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.Pin != PinNum) ||
         (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) || (((Type & 0x01) == 0) &&
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_Dis)) || (((Type & 0x01) != 0) &&
         (GPIO_InitStructureRun.PxODEN != GPIO_OD_En)) || (((Type & 0x02) == 0) &&
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis)) || (((Type & 0x02) != 0) &&
         (GPIO_InitStructureRun.PxPUEN != GPIO_PU_En)) ||
         (GPIO_InitStructureRun.PxFCR != GPIO_FCR_DIG))
    {
        GPIO_InitStructure.Pin = PinNum;
        GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
        if ((Type & 0x01) == 0) GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
        else GPIO_InitStructure.PxODEN = GPIO_OD_En;
        if ((Type & 0x02) == 0) GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
        else GPIO_InitStructure.PxPUEN = GPIO_PU_En;
        GPIO_InitStructure.PxFCR = GPIO_FCR_DIG;

        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}

void CloseIO(GPIOx_Type * GPIOx, uint32_t PinNum)
{
    GPIO_InitTypeDef GPIO_InitStructureRun;

    GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

    if ((GPIO_InitStructureRun.PxFCR != GPIO_FCR_OUT))
    {
        GPIO_SetBits(GPIOx, PinNum);
        OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
    }
    else 
    {
        OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
        GPIO_SetBits(GPIOx, PinNum);
    }
}

void Init_Pad_Io(void)
{
    GPIOx_DO_Write(GPIOA, 0x0000);                                      //
    GPIOx_DO_Write(GPIOB, 0x0000);                                      //
    GPIOx_DO_Write(GPIOC, 0x0000);                                      //
    GPIOx_DO_Write(GPIOD, 0x0000);                                      //
    GPIOx_DO_Write(GPIOE, 0x0000);                                      //
    GPIOx_DO_Write(GPIOF, 0x0000);                                      //
    GPIOx_DO_Write(GPIOG, 0x0000);                                      //
}

void IO_AnalogFunSet(void)
{
    GPIO_ANASEL_PE4ANS_Set(GPIO_ANASEL_PE4ANS_ACMP1_INP1);

    GPIO_ANASEL_PE3ANS_Set(GPIO_ANASEL_PE3ANS_ACMP1_INN1);

    GPIO_ANASEL_PC15ANS_Set(GPIO_ANASEL_PC15ANS_ACMP1_INP0);

    GPIO_ANASEL_PC14ANS_Set(GPIO_ANASEL_PC14ANS_ACMP1_INN0);

    GPIO_ANASEL_PC13ANS_Set(GPIO_ANASEL_PC13ANS_ADC_IN2);

    GPIO_ANASEL_PC12ANS_Set(GPIO_ANASEL_PC12ANS_ADC_IN1);
}

void Init_RCC_PERIPH_clk(void)
{
    //PERCLKCON1
    RCC_PERCLK_SetableEx(DCUCLK, ENABLE); 
    RCC_PERCLK_SetableEx(EXTI2CLK, ENABLE);
    RCC_PERCLK_SetableEx(EXTI1CLK, ENABLE);
    RCC_PERCLK_SetableEx(EXTI0CLK, ENABLE);
    RCC_PERCLK_SetableEx(PDCCLK, ENABLE);
    RCC_PERCLK_SetableEx(ANACCLK, ENABLE);
    RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);
    RCC_PERCLK_SetableEx(SCUCLK, ENABLE);
    RCC_PERCLK_SetableEx(PMUCLK, ENABLE);
    RCC_PERCLK_SetableEx(RTCCLK, ENABLE); 
    RCC_PERCLK_SetableEx(LPTFCLK, ENABLE);
    RCC_PERCLK_SetableEx(LPTRCLK, ENABLE);

    //PERCLKCON2 SETTING
    RCC_PERCLK_SetableEx(ADCCLK, ENABLE);
    RCC_PERCLK_SetableEx(WWDTCLK, ENABLE);
    RCC_PERCLK_SetableEx(RAMBISTCLK, DISABLE);
    RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);
    RCC_PERCLK_SetableEx(DMACLK, ENABLE);
    RCC_PERCLK_SetableEx(LCDCLK, ENABLE);
    RCC_PERCLK_SetableEx(AESCLK, ENABLE);
    RCC_PERCLK_SetableEx(TRNGCLK, ENABLE);
    RCC_PERCLK_SetableEx(CRCCLK, ENABLE);

    //PERCLKCON3 SETTING
    RCC_PERCLK_SetableEx(I2CCLK, ENABLE);
    RCC_PERCLK_SetableEx(U7816CLK1, ENABLE);
    RCC_PERCLK_SetableEx(U7816CLK0, ENABLE);
    RCC_PERCLK_SetableEx(UARTCOMCLK, ENABLE);
    RCC_PERCLK_SetableEx(UART5CLK, ENABLE);
    RCC_PERCLK_SetableEx(UART4CLK, ENABLE);
    RCC_PERCLK_SetableEx(UART3CLK, ENABLE);
    RCC_PERCLK_SetableEx(UART2CLK, ENABLE);
    RCC_PERCLK_SetableEx(UART1CLK, ENABLE);
    RCC_PERCLK_SetableEx(UART0CLK, ENABLE);
    RCC_PERCLK_SetableEx(HSPICLK, ENABLE);
    RCC_PERCLK_SetableEx(SPI2CLK, ENABLE);
    RCC_PERCLK_SetableEx(SPI1CLK, ENABLE);

    //PERCLKCON4 SETTING
    RCC_PERCLK_SetableEx(ET4CLK, ENABLE);
    RCC_PERCLK_SetableEx(ET3CLK, ENABLE);
    RCC_PERCLK_SetableEx(ET2CLK, ENABLE);
    RCC_PERCLK_SetableEx(ET1CLK, ENABLE);
    RCC_PERCLK_SetableEx(BT2CLK, ENABLE);
    RCC_PERCLK_SetableEx(BT1CLK, ENABLE);
}

void Init_PLL(void)
{
    RCC_PLL_InitTypeDef PLL_InitStruct;

    PLL_InitStruct.PLLDB = 499;
    PLL_InitStruct.PLLINSEL = RCC_PLLCON_PLLINSEL_XTLF;
    PLL_InitStruct.PLLOSEL = RCC_PLLCON_PLLOSEL_MUL1;
    PLL_InitStruct.PLLEN = DISABLE;

    RCC_PLL_Init(&PLL_InitStruct);
    RCC_PLLCON_PLLEN_Setable(DISABLE);
}

void Init_SysClk(void)
{
    RCC_RCHF_InitTypeDef RCHF_InitStruct;
    RCC_SYSCLK_InitTypeDef SYSCLK_InitStruct;

    RCHF_InitStruct.FSEL = SYSCLKdef;
    RCHF_InitStruct.RCHFEN = ENABLE;

    RCC_RCHF_Init(&RCHF_InitStruct);

    SYSCLK_InitStruct.SYSCLKSEL = RCC_SYSCLKSEL_SYSCLKSEL_RCHF;
    SYSCLK_InitStruct.AHBPRES = RCC_SYSCLKSEL_AHBPRES_DIV1;
    SYSCLK_InitStruct.APBPRES = RCC_SYSCLKSEL_APBPRES_DIV1;
    SYSCLK_InitStruct.EXTICKSEL = RCC_SYSCLKSEL_EXTICKSEL_AHBCLK;
    SYSCLK_InitStruct.SLP_ENEXTI = ENABLE;
    SYSCLK_InitStruct.LPM_RCLP_OFF = DISABLE;

    RCC_SysClk_Init(&SYSCLK_InitStruct);
}

void SCU_Init(uint8_t Mode)
{
    if (Mode == 1) //debug
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(ENABLE);
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(ENABLE);
    }
    else //release
    {
        SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(DISABLE);
        SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(DISABLE);
    }

    SCU_MCUDBGCR_DBG_ET4_STOP_Setable(DISABLE);
    SCU_MCUDBGCR_DBG_ET3_STOP_Setable(DISABLE);
    SCU_MCUDBGCR_DBG_ET2_STOP_Setable(DISABLE);
    SCU_MCUDBGCR_DBG_ET1_STOP_Setable(DISABLE);
    SCU_MCUDBGCR_DBG_BT2_STOP_Setable(DISABLE);
    SCU_MCUDBGCR_DBG_BT1_STOP_Setable(DISABLE);
}

void Init_SysClk_Gen(void)
{
    if (RCHFCLKCFG >= 24) FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_1CYCLE);
    else FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_0CYCLE);

    Init_PLL();

    Init_SysClk(); 

    Init_RCC_PERIPH_clk();

    RCC_MPRIL_MPRIL_Set(RCC_MPRIL_MPRIL_DMA);

    ANAC_PDRCON_PDREN_Setable(ENABLE);
    ANAC_BORCON_OFF_BOR_Setable(DISABLE);

#ifdef __DEBUG
    SCU_Init(1);

#else

    SCU_Init(0);
#endif
}

void IWDT_Init(void)
{
    RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);
    IWDT_Clr();
    IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_2s);
    IWDT_IWDTCFG_IWDTSLP4096S_Setable(DISABLE);
}

void Init_IO(void)
{
    OutputIO(LED1_Port, LED1_Pin, 0);
    OutputIO(LED2_Port, LED2_Pin, 0);
    OutputIO(LED3_Port, LED3_Pin, 0);
    OutputIO(LED4_Port, LED4_Pin, 0);
    AltFunIO(DBGCOMRX_Port, DBGCOMRX_Pin, 1);
    AltFunIO(DBGCOMTX_Port, DBGCOMTX_Pin, 1);
}

static void configDelay(void)
{
    __IO uint32_t count;

    for (count = SYSTEM_CLK / 100; count > 0; count--,IWDT_Clr());
}

void Init_System(void)
{
    __disable_irq();

    IWDT_Init();
    IWDT_Clr();
    configDelay();
    Init_SysClk_Gen();
    RCC_Init_RCHF_Trim(clkmode);

    uart_0.port = DBGUART;
    uart_0.config.baud_rate=9600;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;
    uart_0.config.mode=MODE_TX_RX;
    hal_uart_init(&uart_0);

    __enable_irq();
}

void board_init(void)
{

    Init_System();
    Init_IO();

    Init_SysTick(RHINO_CONFIG_TICKS_PER_SECOND);
    RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE);
    FLASH_Deinit();
    NVIC_SetPriority (NVMIF_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
}

void SysTick_Handler(void)
{

    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
    IWDT_Clr();
}


/**
    * @brief    This function is executed in case of error occurrence.
    * @param    None
    * @retval None
    */
__attribute__((weak)) void _Error_Handler(char * file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
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
void assert_failed(uint8_t * file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}

#endif


/**
    * @brief    Retargets the C library printf function to the USART.
    * @param    None
    * @retval None
    */
PUTCHAR_PROTOTYPE
{
    if (ch == '\n')
    {
        //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
//      while (UART_UARTIF_RxTxIF_ChkEx(DBGUART, TxInt) == 0);

//      UARTx_TXREG_Write(DBGUART, '\r');
    hal_uart_send(&uart_0,"\r",1,1);
    }
  hal_uart_send(&uart_0,&ch,1,1);

//  while (UART_UARTIF_RxTxIF_ChkEx(DBGUART, TxInt) == 0);

//  UARTx_TXREG_Write(DBGUART, ch);

    return ch;
}

/**
    * @brief    Retargets the C library scanf function to the USART.
    * @param    None
    * @retval None
    */
GETCHAR_PROTOTYPE
{
    /* Place your implementation of fgetc here */
    /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
//  if (UART_UARTIF_RxTxIF_ChkEx(DBGUART, RxInt) == 0)
//  {
//      return - 1;
//  }
//  else 
//  {
//      return UARTx_RXREG_Read(DBGUART);
//  }
  char c;
  if(hal_uart_recv(&uart_0,&c,1,1)==0)
  {
    return c;
  }
  return  -1;
}

