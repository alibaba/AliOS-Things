/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_scu.h"
#include "lib_gpio.h"
#include "system_HR8P2xx.h"

void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type)
{
    SCU->NMIC.NMICS = NMI_Type;
}

FlagStatus  SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag)
{
    FlagStatus bitstatus = RESET;
    if((SCU->PWRC.Word & (uint32_t)PWRC_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag)
{
    SCU->PWRC.Word &= ~((uint32_t)PWRC_Flag);
}

FlagStatus  SCU_GetLVDFlagStatus(SCU_TYPE_LVD0CON LVD_Flag)
{
    FlagStatus bitstatus = RESET;
    if((SCU->LVDC.Word & (uint32_t)LVD_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

uint32_t SCU_GetCFGWord(void)
{
    return (SCU->CFGWORD0.Word);
}

void SCU_SysClkSelect(SCU_TYPE_SYSCLK Sysclk)
{
    SCU->SCLKEN0.SYSCLK_SSE= Sysclk;
}

SCU_TYPE_SYSCLK SCU_GetSysClk(void)
{
    return (SCU_TYPE_SYSCLK)(SCU->SCLKEN0.SYSCLK_SST);
}

FlagStatus SCU_SysClkChangeBusy(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN0.SYSCLK_CHG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

FlagStatus  SCU_GetHRC1Flag(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN0.HRC_ON) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

FlagStatus  SCU_GetLRCFlag(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN0.LRC_ON) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

FlagStatus  SCU_HOSCReadyFlag(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN1.HOSC_RDY) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

FlagStatus  SCU_LOSCReadyFlag(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN1.LOSC_RDY) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;

}

FlagStatus  SCU_PLLReadyFlag(void)
{
    FlagStatus bitstatus = RESET;
    if((SCU->SCLKEN1.PLL_RDY) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void SystemClockConfig(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)
    {SCU_RegUnLock();}
    SCU_HRC_Enable();
    while(SCU_GetHRC1Flag() != SET);
    SCU_SysClkSelect(SCU_SysClk_HRC);
    while(SCU_SysClkChangeBusy() != RESET);
    SCU_SysClk_Div1();
    SCU_ADC32K_RtcOsc();
    SCU_LCD32K_RtcOsc();
    SCU_WDT32K_RtcOsc();
    SystemCoreClock = 16000000;
    if(Prot_Temp != 0)
    {SCU_RegLock();}
}

void DeviceClockAllEnable(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)
    {SCU_RegUnLock();}

    SCU->PCLKEN0.Word = 0x000000f7;
    SCU->PCLKEN1.Word = 0x111F070F;

    if(Prot_Temp != 0)
    {SCU_RegLock();}
}

void DeviceClockAllDisable(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)
    {SCU_RegUnLock();}

    SCU->PCLKEN0.Word = 0x00000000;
    SCU->PCLKEN0.Word = 0x00000000;

    if(Prot_Temp != 0)
    {SCU_RegLock();}
}

void SystemClockSelect(SCU_TYPE_SYSCLK CLKx)
{
    uint32_t sysclk;
    sysclk=CLKx;
    SCU_RegUnLock();
    switch(sysclk)
    {
        case 0:
            SCU_HRC_Enable();
            while(SCU_GetHRC1Flag() != SET);
            break;
        case 1:
            SCU_LRC_Enable();
            while(SCU_GetLRCFlag() != SET); 
            break;
        case 2:
            SCU_HOSC_Enable();
            while(SCU_HOSCReadyFlag() != SET); 
            break;
        case 3:
            SCU_PLL_Enable();
            while(SCU_PLLReadyFlag() != SET);
            break;
        case 4:
            SCU_LOSCLP_Enable();
            while(SCU_LOSCReadyFlag() != SET);
            break;
        default:break;
    }
    SCU_SysClkSelect(CLKx);   
    while(SCU_SysClkChangeBusy() != RESET);
    SCU_RegLock();
}

void SysclkPLL(PLL_TYPE_CLK CLKx,PLL_TYPE_UNLOCK mode)
{
    SCU_RegUnLock();
    SCU->SCLKEN1.PLL_SSE=CLKx;
    SCU->SCLKEN1.PLL_ULOCK_SET=mode;
    SCU_PLL_Enable();
    while(SCU->SCLKEN1.PLL_RDY==RESET);
    SCU_SysClkSelect(SCU_SysClk_PLLCLK);
    while(SCU_SysClkChangeBusy() != RESET); 
    SCU_RegLock();
}

/*************************END OF FILE**********************/

