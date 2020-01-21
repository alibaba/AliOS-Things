/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "k_config.h"
#include "HC32L136.h"
#include "base_types.h"
#include <stdio.h>

extern uint32_t SystemCoreClock;
extern  void EnableNvic(IRQn_Type enIrq,uint8_t u8Level,boolean_t bEn);

volatile uint32_t uwTick;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void GPIO_Init(void);
void UART0_Init(void);
void UART0_SendChar(unsigned char temp);

void board_driver_init(void)
{
    UART0_Init();
    SystemClock_Config();
    GPIO_Init();
    LED_ON();
}

void board_init(void)
{
    M0P_SYSCTRL->RCH_CR_f.TRIM = (*((volatile uint16_t*) (0x00100C00)));  //24M
    while(!M0P_SYSCTRL->RCH_CR_f.STABLE );   //wait RCL stable
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    SystemCoreClock = 24000000;
    SysTick_Config(SystemCoreClock/1000U);
    EnableNvic(SysTick_IRQn,0,TRUE);
}
void GPIO_Init(void)
{
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1; 
   
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;

    M0P_GPIO->PADIR = 0x400;  //PA10 RX
    M0P_GPIO->PBDIR = 0;
    M0P_GPIO->PCDIR = 0;
    M0P_GPIO->PDDIR = 0;

    M0P_GPIO->PAOUT = 0;
    M0P_GPIO->PBOUT = 0;
    M0P_GPIO->PCOUT = 0;
    M0P_GPIO->PDOUT = 0x10;

    M0P_GPIO->PAPU = 0;
    M0P_GPIO->PBPU = 0;
    M0P_GPIO->PCPU = 0;
    M0P_GPIO->PDPU = 0;

    M0P_GPIO->PAPD = 0xFF;
    M0P_GPIO->PBPD = 0xFF;
    M0P_GPIO->PCPD = 0xFF;
    M0P_GPIO->PDPD = 0xFF;  
    
}

/* USART0 init function */
void UART0_Init(void)
{
    M0P_SYSCTRL->PERI_CLKEN_f.UART0 = 1;  
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1; 

    M0P_GPIO->PADIR_f.PA09 = 0;   //txd
    M0P_GPIO->PADIR_f.PA10 = 1;   //rxd
    M0P_GPIO->PA09_SEL = 1;
    M0P_GPIO->PA10_SEL = 1;

    M0P_UART0->SCON = 0x00;
    
    M0P_UART0->SCON_f.OVER = 0;
    M0P_UART0->SCON_f.SM = 1;  
    M0P_UART0->SCON_f.B8CONT = 0;  

    M0P_UART0->SADDR = 0x00;
    M0P_UART0->SADEN = 0x00;

    M0P_UART0->SCNT = 13;                           ///*115200bps @24M*///  

//  M0P_UART0->SCON_f.RCIE = 1;  
//  M0P_UART0->SCON_f.REN = 1;       ///* UART0 *///    
//  EnableNvic(UART0_IRQn,3,TRUE);
  M0P_UART0->ICR = 0x00;
}
void UART0_SendChar(unsigned char temp)
{
    M0P_UART0->SBUF = temp;
    while(!M0P_UART0->ISR_f.TC);
    M0P_UART0->ICR_f.TCCF = 0;
}
void Print(unsigned char *pd)
{
    while((*pd)!='\0') 
    {
        UART0_SendChar(*pd); 
        pd++;  
    }
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



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
            M0P_UART0->SBUF = (uint8_t)ch;
      while(!M0P_UART0->ISR_f.TC);
      M0P_UART0->ICR_f.TCCF = 0;
  return ch;
}
/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
int fgetc(FILE *f)
{
  /* Place your implementation of fgetc here */
    uint8_t ch = 0;
    uint32_t ret = 0xFFFFFF;
    
     while(ret>0)
     {
            if(M0P_UART0->ISR_f.RC)   
                {
                  break;
                }
                else
                {
                  ret--;
                }
         }
    if (ret > 0) {
              M0P_UART0->ICR = 0x00;
              ch = M0P_UART0->SBUF ;
        return ch;
    } else {
        return -1;
    }
}


/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
 * @note This function is declared as __weak to be overwritten in case of other 
  *      implementations in user file.
  * @retval None
  */
void HAL_IncTick(void)
{
  uwTick++;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other 
  *       implementations in user file.
  * @retval tick value
  */
 uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/**
  * @brief This function provides accurate delay (in ms) based on a variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note ThiS function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(volatile uint32_t Delay)
{
  uint32_t tickstart = 0U;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}

/**
  * @brief Suspends the Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_SuspendTick()
  *       is called, the the SysTick interrupt will be disabled and so Tick increment 
  *       is suspended.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief Resumes the Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the the SysTick interrupt will be enabled and so Tick increment 
  *       is resumed.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief Returns the HAL revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
  return 0x55aa55aa;
}

