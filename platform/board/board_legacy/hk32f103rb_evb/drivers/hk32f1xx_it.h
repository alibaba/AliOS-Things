#ifndef __HK32F1xx_IT_H
#define __HK32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "HK32f1xx_hal.h"
#include "board.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
//void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void RCC_IRQHandler(void);
//void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F1xx_IT_H */

