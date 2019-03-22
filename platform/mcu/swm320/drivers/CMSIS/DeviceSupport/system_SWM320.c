/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include <stdint.h>
#include "SWM320.h"

#define SYS_CLK_20MHz   0
#define SYS_CLK_40MHz   1
#define SYS_CLK_32KHz   2
#define SYS_CLK_XTAL    3
#define SYS_CLK_PLL     4

#define SYS_CLK   SYS_CLK_PLL //SYS_CLK_40MHz


#define SYS_CLK_DIV_1   0
#define SYS_CLK_DIV_2   1

#define SYS_CLK_DIV     SYS_CLK_DIV_1


#define __HSI       (20000000UL)
#define __LSI       (   32000UL)
#define __HSE       (20000000UL)

#define SYS_PLL_SRC     SYS_CLK_XTAL

#define PLL_IN_DIV      5

#define PLL_FB_DIV      60


#define PLL_OUT_DIV8    0
#define PLL_OUT_DIV4    1
#define PLL_OUT_DIV2    2

#define PLL_OUT_DIV     PLL_OUT_DIV8



uint32_t SystemCoreClock  = __HSI;                  //System Clock Frequency (Core Clock)
uint32_t CyclesPerUs      = (__HSI / 1000000);      //Cycles per micro second

void SystemCoreClockUpdate(void)
{
    if(SYS->CLKSEL & SYS_CLKSEL_SYS_Msk)            //SYS_CLK  <= HFCK
    {
        if(SYS->CLKSEL & SYS_CLKSEL_HFCK_Msk)           //HFCK <= XTAL
        {
            SystemCoreClock =  __HSE;
        }
        else                                            //HFCK <= HRC
        {
            if(SYS->HRCCR & SYS_HRCCR_DBL_Msk)              //HRC = 40MHz
            {
                SystemCoreClock = __HSI*2;
            }
            else                                            //HRC = 20MHz
            {
                SystemCoreClock = __HSI;
            }
        }
    }
    else                                            //SYS_CLK  <= LFCK
    {
        if(SYS->CLKSEL & SYS_CLKSEL_LFCK_Msk)           //LFCK <= PLL
        {
            if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)            //PLL_SRC <= HRC
            {
                SystemCoreClock = __HSI;
            }
            else                                            //PLL_SRC <= XTAL
            {
                SystemCoreClock = __HSE;
            }

            SystemCoreClock = SystemCoreClock / PLL_IN_DIV * PLL_FB_DIV * 4 / (2 << (2 - PLL_OUT_DIV));
        }
        else                                            //LFCK <= LRC
        {
            SystemCoreClock = __LSI;
        }
    }

    if(SYS->CLKDIV & SYS_CLKDIV_SYS_Msk) SystemCoreClock /= 2;
}

void SystemInit(void)
{
    SYS->CLKEN |= (1 << SYS_CLKEN_ANAC_Pos);

    switch(SYS_CLK)
    {
        case SYS_CLK_20MHz:
            switchCLK_20MHz();
            break;

        case SYS_CLK_40MHz:
            switchCLK_40MHz();
            break;

        case SYS_CLK_32KHz:
            switchCLK_32KHz();
            break;

        case SYS_CLK_XTAL:
            switchCLK_XTAL();
            break;

        case SYS_CLK_PLL:
            switchCLK_PLL();
            break;
    }

    SYS->CLKDIV &= ~SYS_CLKDIV_SYS_Msk;
    SYS->CLKDIV |= (SYS_CLK_DIV << SYS_CLKDIV_SYS_Pos);

    SystemCoreClockUpdate();
}

void switchCLK_20MHz(void)
{
    uint32_t i;

    SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
                 (0 << SYS_HRCCR_DBL_Pos);          //HRC = 20MHz

    for(i = 0; i < 1000; i++) __NOP();

    SYS->CLKSEL &= ~SYS_CLKSEL_HFCK_Msk;            //HFCK  <=  HRC
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);       //SYS_CLK  <= HFCK
}

void switchCLK_40MHz(void)
{
    uint32_t i;

    SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
                 (1 << SYS_HRCCR_DBL_Pos);          //HRC = 40MHz

    for(i = 0; i < 1000; i++) __NOP();

    SYS->CLKSEL &= ~SYS_CLKSEL_HFCK_Msk;            //HFCK  <=  HRC
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);       //SYS_CLK  <= HFCK
}

void switchCLK_32KHz(void)
{
    uint32_t i;

    SYS->CLKEN |= (1 << SYS_CLKEN_RTCBKP_Pos);

    SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);

    for(i = 0; i < 100; i++) __NOP();

    SYS->CLKSEL &= ~SYS_CLKSEL_LFCK_Msk;            //LFCK  <=  LRC
    SYS->CLKSEL &= ~SYS_CLKSEL_SYS_Msk;             //SYS_CLK  <= LFCK
}

void switchCLK_XTAL(void)
{
    uint32_t i;

    SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);

    for(i = 0; i < 1000; i++) __NOP();

    SYS->CLKSEL |= (1 << SYS_CLKSEL_HFCK_Pos);      //HFCK  <=  XTAL
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);       //SYS_CLK  <= HFCK
}

void switchCLK_PLL(void)
{
    uint32_t i;

    PLLInit();
    SYS->PLLCR |= (1 << SYS_PLLCR_OUTEN_Pos);

    for(i = 0; i < 10000; i++) __NOP();

    SYS->CLKSEL |= (1 << SYS_CLKSEL_LFCK_Pos);      //LFCK  <=  PLL
    SYS->CLKSEL &= ~SYS_CLKSEL_SYS_Msk;             //SYS_CLK  <= LFCK
}

void PLLInit(void)
{
    uint32_t i;

    if(SYS_PLL_SRC == SYS_CLK_20MHz)
    {
        SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
                     (0 << SYS_HRCCR_DBL_Pos);      //HRC = 20MHz

        SYS->PLLCR |= (1 << SYS_PLLCR_INSEL_Pos);   //PLL_SRC <= HRC
    }
    else if(SYS_PLL_SRC == SYS_CLK_XTAL)
    {
        SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);

        for(i = 0; i < 20000; i++);

        SYS->PLLCR &= ~(1 << SYS_PLLCR_INSEL_Pos);  //PLL_SRC <= XTAL
    }

    SYS->PLLDIV &= ~(SYS_PLLDIV_INDIV_Msk |
                     SYS_PLLDIV_FBDIV_Msk |
                     SYS_PLLDIV_OUTDIV_Msk);
    SYS->PLLDIV |= (PLL_IN_DIV << SYS_PLLDIV_INDIV_Pos) |
                   (PLL_FB_DIV << SYS_PLLDIV_FBDIV_Pos) |
                   (PLL_OUT_DIV<< SYS_PLLDIV_OUTDIV_Pos);

    SYS->PLLCR &= ~(1 << SYS_PLLCR_OFF_Pos);

    while(SYS->PLLLOCK == 0);
}
