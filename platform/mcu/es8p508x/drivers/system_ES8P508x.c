/***************************************************************
*Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
***************************************************************/
#include "system_ES8P508x.h"

uint32_t SystemCoreClock = 20000000;

void SystemCoreClockUpdate (void)
{
    uint32_t Prot_Temp;
    uint16_t Count=0;

    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)                                      
        {SCU->PROT.Word = 0x55AA6996;}                      

    SCU->SCLKEN1.HRC_EN = 1;                                
    do
    {
        Count++;
    }
    while((SCU->SCLKEN1.HRC_RDY == 0)&&(Count != 0xFFF));   

    if(Count == 0xFFF)
    {
        return ;
    }

    SCU->SCLKEN0.SYSCLK_DIV = 0;                            

    switch(SCU->SCLKEN0.CLK_SEL)
    {
        case 0:
            SystemCoreClock = 20000000;                     
            break;
        case 1:
            SystemCoreClock = 32000;                        
            break;
        default:
            break;
    }

    if(Prot_Temp != 0)                                      
        {SCU->PROT.Word = 0x00000000;}                      
}

void SystemInit (void)
{
    SystemCoreClockUpdate();
}

/*************************END OF FILE**********************/
