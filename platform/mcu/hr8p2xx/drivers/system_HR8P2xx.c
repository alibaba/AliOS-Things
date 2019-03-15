/***************************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
***************************************************************/

#include "HR8P2xx.h"
#include "system_HR8P2xx.h"

uint32_t SystemCoreClock   = 16000000;

void SystemCoreClockUpdate (void)
{
    uint32_t Prot_Temp;
    uint16_t Count=0;
      
    Prot_Temp = SCU->PROT.PROT;
    if(Prot_Temp != 0)
        {SCU->PROT.Word = 0x55AA6996;}

    SCU->SCLKEN0.HRC_EN = 1;
    do
    {
        Count++;
    }
    while((SCU->SCLKEN0.HRC_ON == 0)&&(Count != 0xFFF));
    
    if(Count == 0xFFF)
    {
        return ;
    }
    
    SCU->SCLKEN0.SYSCLK_SSE = 0;
    Count = 0;
    do
    {
        Count++;
    }
    while((SCU->SCLKEN0.SYSCLK_CHG != 0)&&(Count != 0xFFF));
    
    if(Count == 0xFFF)
    {
        return ;
    }
    
    SCU->SCLKEN0.SYSCLK_DIV = 0;
    //SCU->SCLKEN0.SYSCLKOE = 1;
    
    switch(SCU->SCLKEN0.SYSCLK_SST)
    {
        case 0:
            SystemCoreClock = 16000000;   //HRC
            break;
        case 1:
            SystemCoreClock = 32000;      //LRC
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
