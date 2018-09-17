/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include "esp_types.h"
#include "rom/ets_sys.h"
#include "espos_task.h"
#include "espos_scheduler.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "test_espos_run_in_isr.h"

#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   16               /*!< Hardware timer clock divider */

static void IRAM_ATTR timer_isr(void *p)
{
    espos_test_isr_routine routine = (espos_test_isr_routine) p;

    espos_isr_enter();

    TIMERG0.int_clr_timers.t0 = 1;

    routine();

    espos_isr_exit();
}

void espos_test_run_in_isr(espos_test_isr_routine routine)
{

    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_0;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 0;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, 1);
    timer_set_auto_reload(timer_group, timer_idx, TIMER_AUTORELOAD_DIS);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_isr,
                       (void *) routine, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}
