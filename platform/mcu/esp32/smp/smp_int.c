/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "xtensa_rtos.h"
#include "k_api.h"
#include "esp_log.h"

#define portENTER_CRITICAL()                
#define portEXIT_CRITICAL()                 

#define REASON_YIELD (1<<0)

/*static portMUX_TYPE reasonSpinlock = portMUX_INITIALIZER_UNLOCKED;*/
static volatile uint32_t reason[ portNUM_PROCESSORS ];

static char TAG[]="crosscore_int";



typedef struct xt_handler_table_entry {
    void * handler;
    void * arg;
} xt_handler_table_entry;

extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS*portNUM_PROCESSORS];
extern void xt_unhandled_interrupt(void * arg);

void os_set_int_handle(int n,void *f)
{
    xt_handler_table_entry * entry;
    
    n = n * portNUM_PROCESSORS + xPortGetCoreID();
    entry = _xt_interrupt_table + n;
    entry->handler = f;    
}

/*xt_set_interrupt_handler in libesp32.a is set only one core,it is should be modified*/

void os_modify_int_handle(int n,void *f)
{
    /*os_clear_int_handle(n,NULL);*/
    os_set_int_handle(n,f);
}

/*

void os_set_int_muticore(int n)
{
    
    return;
    xt_handler_table_entry * entry;
    void * oldhanler;
    int intnum;
    
    intnum = n * 1 + xPortGetCoreID();
    entry = _xt_interrupt_table + intnum;
    oldhanler = entry->handler;
    entry->handler = xt_unhandled_interrupt;

    intnum = n * portNUM_PROCESSORS + xPortGetCoreID();
    entry = _xt_interrupt_table + intnum;
    entry->handler = oldhanler;  

}*/


void  os_crosscore_isr(void *arg) {
    krhino_intrpt_enter();

    //Clear the interrupt first.
    if (xPortGetCoreID()==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, 0);
    }

    krhino_intrpt_exit();
}


/*Initialize the crosscore interrupt on this core. Call this once
on each active core.*/
void os_crosscore_int_init() {
    portENTER_CRITICAL();

    reason[xPortGetCoreID()]=0;
    portEXIT_CRITICAL();

    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "os_crosscore_int_init in,core:%d\r\n",xPortGetCoreID());
    if (xPortGetCoreID()==0) {
        //os_set_int_handle(17,os_crosscore_isr);
        /*err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, os_crosscore_isr, (void*)&reason[xPortGetCoreID()], NULL);*/
    } else {
        intr_matrix_set(xPortGetCoreID(),ETS_FROM_CPU_INTR1_SOURCE,24);
        err = esp_intr_alloc(ETS_FROM_CPU_INTR1_SOURCE, ESP_INTR_FLAG_IRAM, os_crosscore_isr, (void*)&reason[xPortGetCoreID()], NULL);
        os_modify_int_handle(24,os_crosscore_isr);
    }

    assert(err == ESP_OK);
}

void  os_crosscore_int_send_yield(int coreId) {
    assert(coreId<portNUM_PROCESSORS);
    //Mark the reason we interrupt the other CPU
    portENTER_CRITICAL();
    reason[coreId]|=REASON_YIELD;
    portEXIT_CRITICAL();
    //Poke the other CPU.
    if (coreId==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, DPORT_CPU_INTR_FROM_CPU_1);
    }
}

void cpu_signal(uint8_t cpu_num)
{
     extern uint64_t g_cpu_flag;
     if(g_cpu_flag & (1UL << cpu_num))
     {
         os_crosscore_int_send_yield((int)cpu_num);
     }
}


