/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 **********************************************************/
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "lib_config.h"

void SysTickInit(void);
void Delay_100us(__IO uint32_t nTime);
void Delay1ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#endif
