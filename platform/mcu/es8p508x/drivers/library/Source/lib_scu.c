/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_scu.h"

void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type)
{
    SCU->NMICON.NMICS = NMI_Type;
}

FlagStatus  SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag)
{
    FlagStatus bitstatus = RESET;

    if((SCU->PWRC.Word & (uint32_t)PWRC_Flag) != (uint32_t)RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag)
{
    SCU->PWRC.Word &= ~((uint32_t)PWRC_Flag);
}

FlagStatus  SCU_GetLVDFlagStatus(SCU_TYPE_LVD0CON LVD_Flag)
{
    FlagStatus bitstatus = RESET;

    if((SCU->LVDCON.Word & (uint32_t)LVD_Flag) != (uint32_t)RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

void SCU_SysClkSelect(SCU_TYPE_SYSCLK Sysclk)
{
    SCU->SCLKEN0.CLK_SEL = Sysclk;
}

SCU_TYPE_SYSCLK SCU_GetSysClk(void)
{
    return (SCU_TYPE_SYSCLK)(SCU->SCLKEN0.CLK_SEL);
}

FlagStatus SCU_HRCReadyFlag(void)
{
    FlagStatus bitstatus = RESET;

    if((SCU->SCLKEN1.HRC_RDY) != (uint32_t)RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

FlagStatus  SCU_XTALReadyFlag(void)
{
    FlagStatus bitstatus = RESET;

    if((SCU->SCLKEN1.XTAL_RDY) != (uint32_t)RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

FlagStatus  SCU_PLLReadyFlag(void)
{
    FlagStatus bitstatus = RESET;

    if((SCU->SCLKEN1.PLL_RDY) != (uint32_t)RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

void SystemClockConfig(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;

    if(Prot_Temp != 0)                      
        SCU_RegUnLock();                    

    SCU_HRC_Enable();                       
    while(SCU_HRCReadyFlag() != SET);       
    SCU_SysClkSelect(SCU_SysClk_HRC);       
	
    SCU_SysClk_Div1();                      
    
    SystemCoreClock = 20000000;

    if(Prot_Temp != 0)                      
        SCU_RegLock();                     
}

void DeviceClockAllEnable(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)                      
        SCU_RegUnLock();                    

    SCU->PCLKEN0.Word = 0xFFFFFFFF;
    SCU->PCLKEN1.Word = 0xFFFFFFFF;         

    if(Prot_Temp != 0)                      
        SCU_RegLock();                      
}

void DeviceClockAllDisable(void)
{
    uint32_t Prot_Temp;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)                      
        SCU_RegUnLock();                    

    SCU->PCLKEN0.Word = 0x00000000;         
    SCU->PCLKEN1.Word = 0x00000000;

    if(Prot_Temp != 0)                      
        SCU_RegLock();                  
}

void SystemClockSelect(SCU_TYPE_SYSCLK SYSCLKx , SCU_TYPE_CLK_SEL CLK_SEL)
{
    SCU_RegUnLock();                        

    switch(SYSCLKx)
    {
        case 0:
            SCU_HRC_Enable();
            while(SCU_HRCReadyFlag() != SET);
            break;
        case 1:
            SCU_XTAL_Enable();
            while(SCU_XTALReadyFlag() != SET);
            break;
        case 2:
            SCU_PLL_Enable();
            while(SCU_PLLReadyFlag() != SET);
            break;
        default:break;
    }

    SCU->SCLKEN0.SYSCLK_DIV = 0;
    SCU->SCLKEN0.CLK_SEL = 2;

    SCU_RegLock();
}

void SysclkPLL(PLL_TYPE_CLK CLKx)
{
    SCU_RegUnLock();                        

    SCU->SCLKEN0.CLK_SEL = CLKx;
    SCU_PLL_Enable();
    while(SCU_PLLReadyFlag() != SET);
    SCU_SysClkSelect(SCU_SysClk_PLL);

    SCU_RegLock();                                   
}

void PLLClock_Config(TYPE_FUNCEN pll_en , SCU_PLL_Origin  pll_origin ,SCU_PLL_Out pll_out,TYPE_FUNCEN sys_pll)
{
    SCU_RegUnLock();

    if(pll_en == DISABLE)  
    {
        SCU->SCLKEN1.PLL_EN = 0;
        return;
    }

    if((pll_origin == SCU_PLL_HRC))         
    {
        if(SCU->SCLKEN1.HRC_RDY == 0)
        {
            SCU->SCLKEN1.HRC_EN = 1;
            while(SCU->SCLKEN1.HRC_RDY == 0);  
        }
    }

    if((pll_origin == SCU_PLL_XTAL_32K)
       || (pll_origin == SCU_PLL_XTAL_4M)
       ||(pll_origin == SCU_PLL_XTAL_8M)
       || (pll_origin == SCU_PLL_XTAL_16M)
       || (pll_origin == SCU_PLL_XTAL_20M))          
    {
        if(SCU->SCLKEN1.XTAL_RDY == 0)
        {
            SCU->SCLKEN1.XTAL_EN = 1;
					  SCU->SCLKEN0.XTAL_LP = 0;
            while(SCU->SCLKEN1.XTAL_RDY == 0);  
        }
    }

    switch(pll_origin)
    {
        case SCU_PLL_HRC:
            SCU->SCLKEN1.PLL_REF_SEL = 0x00;
            break;
        case SCU_PLL_LRC:
            SCU->SCLKEN1.PLL_REF_SEL = 0x02;
            break;
        case SCU_PLL_XTAL_32K:
            SCU->SCLKEN1.PLL_REF_SEL = 0x03;
            break;
        case SCU_PLL_XTAL_4M:
            SCU->SCLKEN1.PLL_REF_SEL = 0x04;
            break;
        case SCU_PLL_XTAL_8M:
            SCU->SCLKEN1.PLL_REF_SEL = 0x05;
            break;
        case SCU_PLL_XTAL_16M:
            SCU->SCLKEN1.PLL_REF_SEL = 0x06;
            break;
        case SCU_PLL_XTAL_20M:
            SCU->SCLKEN1.PLL_REF_SEL = 0x07;
            break;
        default:
            break;
    }

    SCU->SCLKEN1.PLL_48M_SEL = pll_out;   

    SCU->SCLKEN1.PLL_EN = 1;
    while(SCU->SCLKEN1.PLL_RDY == 0);

    if(sys_pll == ENABLE){
        SCU->SCLKEN0.PLL_MUX = 1;
			  SCU->SCLKEN1.PLL_BYLOCK = 1;
			  SCU->SCLKEN0.CLKFLT_BY = 0x55; 
		}
    else{
        SCU->SCLKEN0.PLL_MUX = 0;
			  SCU->SCLKEN1.PLL_BYLOCK = 0;
			  SCU->SCLKEN0.CLKFLT_BY = 0x00;
		}

    SCU_RegLock(); 
}

/*************************END OF FILE**********************/



