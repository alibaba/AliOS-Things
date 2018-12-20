/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_scu.h"
#include "type.h"

void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type)
{
    SCU_RegUnLock();
    SCU->NMICON.NMICS = NMI_Type;
    SCU_RegLock();
    return;
}

FlagStatus  SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag)
{
    FlagStatus bitstatus = RESET;
    
    SCU_RegUnLock();
    if((SCU->PWRC.Word & (uint32_t)PWRC_Flag) != (uint32_t)RESET)
        bitstatus = SET;
    SCU_RegLock();
    
    return  bitstatus;
}

void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag)
{
    SCU_RegUnLock();
    SCU->PWRC.Word &= ~((uint32_t)PWRC_Flag);
    SCU_RegLock();
    return;
}

FlagStatus  SCU_GetLVDFlagStatus(SCU_TYPE_LVDCON LVD_Flag)
{
    FlagStatus bitstatus = RESET;
    
    SCU_RegLock();
    if((SCU->LVDCON.Word & (uint32_t)LVD_Flag) != (uint32_t)RESET)
        bitstatus = SET;
    return  bitstatus;
}

void DeviceClockAllEnable(void)
{
    SCU_RegUnLock();
    SCU->AHBEN.CLKEN_APB1 = 1;
    SCU->AHBEN.CLKEN_APB2 = 1;
    SCU->AHBEN.CLKEN_DMA = 1;
    SCU->AHBEN.CLKEN_ETH = 1;
    SCU->APB1EN.Word = 0XFFFFFFFF;
    SCU->APB2EN.Word = 0XFFFFFFFF;

    SCU_RegLock();
    return;
}

void DeviceClockAllDisable(void)
{
    SCU_RegUnLock();
    SCU->AHBEN.CLKEN_APB1 = 0;
    SCU->AHBEN.CLKEN_APB2 = 0;
    SCU->AHBEN.CLKEN_DMA = 0;
    SCU->AHBEN.CLKEN_SRAM = 0;
    SCU->AHBEN.CLKEN_USB = 0;
    SCU->APB1EN.Word = 0X0;
    SCU->APB2EN.Word = 0X0;
    SCU_RegLock();
}

void AHBPeriphClock_Config(SCU_AHBPERIPH_Typedef tppe_periph, FunctionalState NewState)
{
    uint32_t peri_num, peri_shift;
    
    peri_num = (uint32_t)tppe_periph;
    peri_shift = (0x01 << peri_num);
    SCU_RegUnLock();
    if (NewState != DISABLE)
    {
        SCU->AHBEN.Word |= peri_shift;
    }        
    else
    {
        SCU->AHBEN.Word &= ~peri_shift;
    }    
    SCU_RegLock();    
}

void APB1PeriphClock_Config(SCU_APB1PERIPH_Typedef tppe_periph, FunctionalState NewState)
{
    uint32_t peri_num, peri_shift;
    
    peri_num = (uint32_t)tppe_periph;
    peri_shift = (0x01 << peri_num);
    SCU_RegUnLock();
    if (NewState != DISABLE)
    {
        SCU->APB1EN.Word |= peri_shift;
    }        
    else
    {
        SCU->APB1EN.Word &= ~peri_shift;
    }        
    SCU_RegLock();     
}

void APB2PeriphClock_Config(SCU_APB2PERIPH_Typedef tppe_periph, FunctionalState NewState)
{
    uint32_t peri_num, peri_shift;
    
    peri_num = (uint32_t)tppe_periph;
    peri_shift = (0x01 << peri_num);
    SCU_RegUnLock();
    if (NewState != DISABLE)
    {
        SCU->APB2EN.Word |= peri_shift;
    }    
    else
    {
        SCU->APB2EN.Word &= ~peri_shift;
    }    
    SCU_RegLock();     
}

void SCU_SET_SYSCLK_LRC(void)
{
    SCU_RegUnLock();
    SCU->SCLKEN1.SYSCLK_SEL = 1;
    SCU->SCLKEN0.XO_EN = 0;
    SCU->SCLKEN0.HRC_EN = 0;
    SCU->SCLKEN1.DPLL2_EN = 0;
    SCU_RegLock();
    return;
}

void SCU_SET_SYSCLK_HRC(void)
{
  SCU_RegUnLock();
  SCU->SCLKEN0.HRC_EN = 1;
  SCU->SCLKEN1.OSCCLK_SEL = 0;
  SCU->SCLKEN1.DPLL_OSC_SEL = 0;
  SCU_RegLock();
  return;
}

void SCU_SET_SYSCLK_XTAL(void)
{
  SCU_RegUnLock();
#if 1
  SCU->SCLKEN0.HRC_EN = 1;
  SCU->SCLKEN0.XO_BUF_EN = 1;
  SCU->SCLKEN1.OSCCLK_SEL = 1;
  SCU->SCLKEN1.DPLL_OSC_SEL = 0;
  SCU->SCLKEN0.HRC_EN = 0;
#else
  SCU->SCLKEN0.HRC_EN = 1;
  SCU->SCLKEN0.XO_BUF_EN = 1;
  SCU->SCLKEN0.MOSC_EN = 1;
  SCU->SCLKEN0.CLKFLT_EN = 1;
  SCU->SCLKEN0.MCU_DIV = 0;

  SCU->SCLKEN1.DPLL_OSC_SEL = 0;
  SCU->SCLKEN0.HRC_EN = 0;
#endif
  SCU_RegLock();
  return;
}

#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x05000)   /*!< Time out for HSE start up */

void SCU_SET_SYSCLK_DPLL2(uint32_t dpll2_value)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    
    SCU_RegUnLock();

    SCU->SCLKEN0.HRC_EN = 1;
    SCU->SCLKEN0.XO_BUF_EN = 1;
    SCU->SCLKEN0.MOSC_EN = 1;
    SCU->SCLKEN0.CLKFLT_EN = 1;
    SCU->SCLKEN0.MCU_DIV = 0;
    SCU->SCLKEN1.OSCCLK_SEL = 1;
//SCU->SCLKEN1.OSCCLK_SEL = 0;
    
    SCU->DPLL2CON0.PREDIV = 5;          //dpll2_prediv = 5
    SCU->DPLL2CON0.OFFSETEN = 0;
    SCU->DPLL2CON0.DNOFFSET = 3;
    SCU->DPLL2CON0.UPOFFSET = 3;
    SCU->DPLL2CON0.CPI = 12;
    SCU->DPLL2CON1.NDIV = 0x90;         //dpll2_ndiv = 144

    SCU->SCLKEN1.DPLL2_EN = 1;
    SCU->SCLKEN1.DPLL2_LDTEN = 1;
#if 1
//  while (SCU->DPLL2CON0.LOCKED == 0)
//    {
//    }
    do
    {
        HSEStatus = SCU->DPLL2CON0.LOCKED;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter < HSE_STARTUP_TIMEOUT));
    if (StartUpCounter >= HSE_STARTUP_TIMEOUT)
        SCU->SCLKEN1.OSCCLK_SEL = 0;
#else
    delay_ms(1000);
#endif    
    if(dpll2_value != 200000000)
    {
        SCU->SCLKEN1.DPLL2CLK_SEL = SCU_DPLL2_150M;//mcu 150m
    }       
    else
    {
        SCU->SCLKEN1.DPLL2CLK_SEL = SCU_DPLL2_200M;//mcu 200m
    }       
   
    SCU->SCLKEN1.DPLL_OSC_SEL = 1;
    SCU->AHBEN.ABP1_PRE1 = 2;   //HCLK/4
    SCU->AHBEN.ABP2_PRE1 = 0;   //HCLK  
    
    SCU_RegLock();
    return;
}

SCU_TYPE_SYSCLK SCU_GetSysClk(void)
{
    SCU_TYPE_SYSCLK clk_tmp;

    SCU_RegUnLock();
    
    if(SCU->SCLKEN1.SYSCLK_SEL == 1)
    {
        clk_tmp = SCU_SYSCLK_LRC;
    }
    else if(SCU->SCLKEN1.DPLL_OSC_SEL != 1)
    {
        if((SCU->SCLKEN1.OSCCLK_SEL == 0) && (SCU->SCLKEN0.HRC_EN == 1))
        {
            clk_tmp = SCU_SYSCLK_HRC;
        }
        else if(SCU->SCLKEN1.OSCCLK_SEL == 1)
        {
            clk_tmp = SCU_SYSCLK_XTAL;
        }
        else
        {
            clk_tmp = WRONG_SET;
        }
    }
    else
    {
        if(SCU->SCLKEN1.DPLL2_EN == 1)
        {
            clk_tmp = SCU_SYSCLK_DPLL2;
        }
        else
        {
            clk_tmp = WRONG_SET;
        }     
        
    }
    SCU_RegLock();

    return (clk_tmp);
}

void SCU_APB1Config(SCU_APB_PRE APB1_HCLK)
{
    SCU_RegUnLock();  
    SCU->AHBEN.ABP1_PRE1 = APB1_HCLK;
    SCU_RegLock();
}

void SCU_APB2Config(SCU_APB_PRE APB2_HCLK)
{
    SCU_RegUnLock();  
    SCU->AHBEN.ABP1_PRE1 = APB2_HCLK;
    SCU_RegLock();
}

#if !defined  (DPLL2_VALUE)   
  #define DPLL2_VALUE    ((uint32_t)150000000) /*!< Value of the Internal oscillator in Hz*/
#endif  

void SCU_GetClocksFreq(SCU_ClocksTypeDef* SCU_Clocks)
{
    uint32_t tmp;   
    uint32_t SystemClock;     
    SCU_TYPE_SYSCLK scu_sysclk;
    
    scu_sysclk = SCU_GetSysClk();    
    switch(scu_sysclk)
    {
        case SCU_SYSCLK_LRC:
            SystemClock = 32000;
            break;
        case SCU_SYSCLK_HRC:
            SystemClock = 25000000;
            break;
        case SCU_SYSCLK_XTAL:
            SystemClock = 25000000;
            break;
        case SCU_SYSCLK_DPLL2:
            SystemClock = DPLL2_VALUE;
            break;
        default:
            break;
    }
    SCU_Clocks->HCLK_Frequency = SystemClock;
    
    tmp = SCU->AHBEN.ABP1_PRE1;
    switch(tmp)
    {
        case 0x00:
        case 0x01:
            SCU_Clocks->APB1_Frequency = SystemClock/2;
            break;
        case 0x02:
            SCU_Clocks->APB1_Frequency = SystemClock/4;
            break;
        case 0x03:
            SCU_Clocks->APB1_Frequency = SystemClock/8;
            break;        
    }
    tmp = SCU->AHBEN.ABP2_PRE1;
        switch(tmp)
    {
        case 0x00:
            SCU_Clocks->APB2_Frequency = SystemClock;
            break;
        case 0x01:
            SCU_Clocks->APB2_Frequency = SystemClock/2;
            break;
        case 0x02:
            SCU_Clocks->APB2_Frequency = SystemClock/4;
            break;
        case 0x03:
            SCU_Clocks->APB2_Frequency = SystemClock/8;
            break;        
    }
}

void SystemClockSelect(SCU_TYPE_SYSCLK CLKx)
{
    switch(CLKx)
    {
        case SCU_SYSCLK_LRC:
            SCU_SET_SYSCLK_LRC();
            break;
        case SCU_SYSCLK_HRC:
            SCU_SET_SYSCLK_HRC();   
            break;
        case SCU_SYSCLK_XTAL:
            SCU_SET_SYSCLK_XTAL();
            break;
        case SCU_SYSCLK_DPLL2:
            SCU_SET_SYSCLK_DPLL2(DPLL2_VALUE);
            break;
        default:
            break;
    }
}
/*************************END OF FILE**********************/
