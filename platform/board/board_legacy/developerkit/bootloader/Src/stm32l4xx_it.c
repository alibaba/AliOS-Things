#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

void NMI_Handler(void)
{

}
void HardFault_Handler(void)
{
    while (1) {
        ;
    }
}
void MemManage_Handler(void)
{

    while (1) {
        ;
    }
}
void BusFault_Handler(void)
{
    while (1) {
        ;
    }
}

void UsageFault_Handler(void)
{
    while (1) {
        ;
    }
}
void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}
void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

extern UART_HandleTypeDef hlpuart1;
void LPUART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&hlpuart1);
}

