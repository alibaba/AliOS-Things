/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "systick.h"
#include <stdlib.h>
#include "system_P131.h"

//volatile uint32_t g_ui32Flags;
__IO uint32_t uwTick;
unsigned int SSC1667_GLOBAL_TIMER = 0;

static volatile uint32_t delay_cnt; /* using for delay_ms()*/
void Sys_IncTick(void);

struct timer_t
{
  unsigned int index;
  unsigned int overtime;
  void (*action)(unsigned int timerid);
};

void systick_init(void)
{
    SysTick_Config(SystemCoreClock / 100);  /* 10ms */
}


extern void bsp_RunPer10ms(void);
//static void ms_tick(void)
//{
////    static uint32_t count = 0;
//    
//  if (delay_cnt > 0)
//      --delay_cnt;
//    SSC1667_GLOBAL_TIMER += 10;
//      if (SSC1667_GLOBAL_TIMER == 0xFFFFFFFF)
//      SSC1667_GLOBAL_TIMER = 0;
////    Sys_IncTick();

////    if (++count >= 1000)
////    {
////       count = 0;
////       g_ui32Flags |= SYS_SECOND;  
////    }
//  return;
//}
//void SysTick_Handler(void)
//{
//  ms_tick();
//}



uint32_t sys_now(void)
{
  return SSC1667_GLOBAL_TIMER;
}

// void Sys_IncTick(void)
//{
//    uwTick++;
//}

//uint32_t Sys_GetTick(void)
//{
//  return uwTick;
//}

uint32_t get_sys_tick(void)
{
  return SSC1667_GLOBAL_TIMER;
}
void delay_10ms(uint32_t n)
{
    if (n == 0) {
        return;
    }
    else if (n == 1) {
        n = 2;
    }

    delay_cnt = n;

    while (1) { 
        /* waiting */
        if (delay_cnt == 0)
            break;
    }
}


void delay_us(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000);

    tcnt = 0;
    told = SysTick->VAL;

    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            }
            else {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            if (tcnt >= ticks) {
                break;
            }
        }
    }
}


