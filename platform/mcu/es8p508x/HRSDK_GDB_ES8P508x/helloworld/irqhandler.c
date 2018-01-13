#include "irqhandler.h"
#include "k_api.h"


void NMI_Handler(void)
{
    
}

void HardFault_Handler(void)
{
    while(1)
    {
    }
}

void SVC_Handler(void)
{
    
}


void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();    
}

