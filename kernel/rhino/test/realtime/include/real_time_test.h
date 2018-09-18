/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef REAL_TIME_TEST_H
#define REAL_TIME_TEST_H

#include <k_api.h>
#include "real_time_default_config.h"
#include "global_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/* rttest hal */
float soc_hr_hw_freq_mhz(void);
float soc_cpu_clock_freq_mhz(void);
void hal_rttest_trigger_highintrpt(hr_timer_t* trig_time);
void hal_rttest_trigger_lowintrpt(hr_timer_t* trig_time);

#define CPU_CLOCK_REQ         soc_cpu_clock_freq_mhz()  /* MHz */
#define HR_COUNT_FREQ()       soc_hr_hw_freq_mhz()  /* MHz */
#define HR_TIMER_MAX          0xFFFFFFFF
#define TEST_TASK_STACKSIZE   256

#if ((ENABLE_INTRPT_RESPOND_WAKEUP) || (ENABLE_INTRPT_PREEMPT))
#define TRIGGER_HIGH_PRI_INTRTP(trig_time)  hal_rttest_trigger_highintrpt(trig_time)
#endif

#if ENABLE_INTRPT_PREEMPT
#define TRIGGER_LOW_PRI_INTRPT(trig_time)   hal_rttest_trigger_lowintrpt(trig_time);
#endif

/* test case ID define */
typedef enum
{
#if ENABLE_INTRPT_RESPOND_WAKEUP
    INTRPT_RESPOND_ID   = 0x1,
#endif

#if ENABLE_INTRPT_PREEMPT
    INTRPT_PREEMPT_ID   = 0x2,
#endif

#if ENABLE_INTRPT_RESPOND_WAKEUP
    INTRPT_WAKEUPTASK_ID = 0x3,
#endif

    SCHED_YIELD_ID    = 0x4,
    SCHED_PREEMPT_ID,
    SCHED_RESUME_ACTIVE_ID,
    SYNC_MUTEX_OPERATION_ID,
    SYNC_SEM_OPERATION_ID,
    SYNC_SEM_SHUFFLING_ID,
    SYNC_MUTEX_SHUFFLING_ID,
    SYNC_SEM_ACTIVE_TASK_ID,
    SYNC_MUTEX_ACTIVE_TASK_ID,
    SYNC_SEM_RRWP_TASK_ID,
    MESSAGE_QUEUE_OPERATION_ID,
    MESSAGE_QUEUE_RRWP_TASK_ID,
    MEM_BLK_INTRPT_CRITICAL_ID,
    MEM_TLF_INTRPT_CRITICAL_ID,
    MEM_BLK_MUTEX_CRITICAL_ID,
    MEM_TLF_MUTEX_CRITICAL_ID,
} rttest_case_id_t;

/* interrupt callback function type define */
typedef void (*intrpt_callback_t)(void);
extern intrpt_callback_t  highpri_intrpt_func;
extern intrpt_callback_t  lowpri_intrpt_func;

/* auxiliary function */
void rttest_aux_measure_waste(void);
void rttest_aux_show_result_header(void);
void rttest_aux_show_result(uint32_t test_id, char * test_name, uint32_t test_count, uint64_t time_sum,
                            hr_timer_t time_max, hr_timer_t time_min);
void rttest_aux_show_result_end(void);
void rttest_aux_intrpt_check_init(void);
bool rttest_aux_intrpt_occurred(void);
void rttest_aux_record_result(hr_timer_t time_current, uint64_t *time_sum,
                              hr_timer_t *time_max, hr_timer_t *time_min);
void rttest_aux_intrpt_test_init(intrpt_callback_t highp_func, intrpt_callback_t lowp_func);
void rttest_aux_show_sysconfig();

/* test case */
void rttest_intrpt_operation(void);
void rttest_intrpt_respond(void);
void rttest_intrpt_wakeuptask(void);
void rttest_intrpt_preempt(void);
void rttest_schedule_yield(void);
void rttest_schedule_preempt(void);
void rttest_schedule_resume_active(void);
void rttest_sync_mutex(void);
void rttest_sync_sem(void);
void rttest_shuffling_sem(void);
void rttest_shuffling_mutex(void);
void rttest_sem_active(void);
void rttest_mutex_active(void);
void rttest_sync_sem_rrwp_task(void);
void rttest_message_queue(void);
void rttest_message_rrwp_task(void);
void rttest_memory_blk_alloc(void);
void rttest_memory_tlf_alloc(void);
void rttest_intrpt_dev_respond(void);

/* benchmark API */
void rttest_cli_init(void);
void rttest_main(uint32_t id);  /* 0 for all test case*/
void rttest_max_disintrpt(void);

#ifdef __cplusplus
}
#endif

#endif
