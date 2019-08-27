#include "systick_delay.h"
#ifdef ALIOS_SUPPORT
#include <stdio.h>
#include <k_config.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_time.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
static u8  fac_us=0;
static u16 fac_ms=0;

#ifdef ALIOS_SUPPORT
void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

uint32_t lega_systick_csr_get()
{
    uint32_t ul_systick_ctrl;
    ul_systick_ctrl = SysTick->CTRL;
    return ul_systick_ctrl;
}
void lega_systick_csr_set(uint32_t ctrl)
{
    SysTick->CTRL = ctrl;
}

void delay_init(u8 SYSCLK)
{
    u32 reload;
    //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    fac_us=SYSCLK;
    reload=SYSCLK;
    reload= reload*1000000/RHINO_CONFIG_TICKS_PER_SECOND;

    fac_ms=1000/RHINO_CONFIG_TICKS_PER_SECOND;
    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD=reload;
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
}

#else

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}


void delay_init(u8 SYSCLK)
{
    u32 reload;
    //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    fac_us=SYSCLK;
    reload=SYSCLK;
    reload= reload*1000000/configTICK_RATE_HZ;

    fac_ms=1000/configTICK_RATE_HZ;

    SysTick_Config(reload);
}
#endif

void delay_us(u32 nus)
{
    u32 ticks;
    u32 told,tnow,tcnt=0;
    u32 reload=SysTick->LOAD;
    ticks=nus*fac_us;
    told=SysTick->VAL;
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)tcnt+=told-tnow;
            else tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)break;
        }
    };
}

#ifndef ALIOS_SUPPORT
void delay_ms(u32 nms)
{
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
    {
        if(nms>=fac_ms)
        {
            vTaskDelay(nms/fac_ms);
        }
        nms%=fac_ms;
    }
    delay_us((u32)(nms*1000));
}
#endif

void delay_xms(u32 nms)
{
    u32 i;
    for(i=0;i<nms;i++) delay_us(1000);
}
