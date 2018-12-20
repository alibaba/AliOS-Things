/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "system_p131.h"
#include "lib_scu.h"

uint32_t SystemCoreClock = 25000000;
//uint32_t SystemCoreClock = 150000000;

void SystemInit(void)
{
    SystemClockSelect(SCU_SYSCLK_DPLL2);
//    SystemClockSelect(SCU_SYSCLK_LRC);
    DeviceClockAllEnable();
    
    SCU_RegUnLock();
    SCU->ETHCON.ETH_CS = 1;
    SCU->ETHCON.ETH_RT = 0;
    SCU->ETHCON.ETH_FV = 0x03;
    SCU_RegLock();
}
