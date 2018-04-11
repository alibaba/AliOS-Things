
#ifndef __IRQHANDLER_H__
#define __IRQHANDLER_H__

#include "HR8P506.h"

void    NMI_Handler(void);
void    HardFault_Handler(void);
void    SVC_Handler(void);
void    PendSV_Handler(void);
void    SysTick_Handler(void);


#endif

