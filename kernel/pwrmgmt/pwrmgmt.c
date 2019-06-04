/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <hal/wifi.h>

#include "pwrmgmt.h"
#include "pwrmgmt_debug.h"
#include "cpu_pwr_lib.h"

#define SET_BIT(off) ((uint32_t)1 << (off))

static kmutex_t pwrmgmt_mutex;

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
static uint32_t cpu_suspend_lock = 0;
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
static uint32_t wifi_suspend_lock = 0;
#endif

#if ((PWRMGMT_CONFIG_CPU_LOWPOWER > 0) && (PWRMGMT_CONFIG_CPU_ACTIVE > 0))
#define PWRMGMT_TASK_STACK_SIZE 200
#define PWRMGMT_TASK_PRI        (RHINO_IDLE_PRI - 1)

static cpu_stack_t pwrmgmt_task_stack[PWRMGMT_TASK_STACK_SIZE];
static ktimer_t    pwrmgmt_timer;
static ktask_t     pwrmgmt_task;
static ksem_t      pwrmgmt_sem;

static void pwrmgmt_task_entry(void *arg);
static void pwrmgmt_timer_callback(void *timer, void *arg);
#endif

int pwrmgmt_init() {
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt init\r\n");
    krhino_mutex_create(&pwrmgmt_mutex, "pwrmgmt_mutex");

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    cpu_pwrmgmt_init();
#endif

#if ((PWRMGMT_CONFIG_CPU_LOWPOWER > 0) && (PWRMGMT_CONFIG_CPU_ACTIVE > 0))
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt active function init\r\n");

    krhino_sem_create(&pwrmgmt_sem, "pwrmgmt_sem", 0);
    krhino_timer_create(&pwrmgmt_timer, "pwrmgmt_timer", pwrmgmt_timer_callback,
                        100, 0, NULL, 0);
    krhino_task_create(&pwrmgmt_task, "pwrmgmt_task", NULL, PWRMGMT_TASK_PRI, 5,
                       pwrmgmt_task_stack, PWRMGMT_TASK_STACK_SIZE, pwrmgmt_task_entry, 1);
#endif
    return 0;
}

int pwrmgmt_lowpower_suspend(uint32_t suspend_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "suspend lowpower\r\n");

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if ((cpu_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "suspend low power error suspend_lock=%x, "
                    "suspend_module=%x\r\n", cpu_suspend_lock, SET_BIT(suspend_module));
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if ((wifi_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "suspend low power error suspend_lock=%x, "
                    "suspend_module=%x\r\n", wifi_suspend_lock, SET_BIT(suspend_module));
    }
#endif

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if (cpu_suspend_lock == 0) {
        cpu_pwr_suspend();
    }

    cpu_suspend_lock |= SET_BIT(suspend_module);
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_exit_powersave(module);
    }

    wifi_suspend_lock |= SET_BIT(suspend_module);
#endif

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

int pwrmgmt_lowpower_resume(uint32_t resume_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "resume lowpower\r\n");

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if ((cpu_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "resume low power error suspend_lock=%x, "
                    "reusme_module=%x\r\n", cpu_suspend_lock, SET_BIT(resume_module));
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if ((wifi_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "resume low power error suspend_lock=%x, "
                    "reusme_module=%x\r\n", wifi_suspend_lock, SET_BIT(resume_module));
    }
#endif

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    cpu_suspend_lock &= ~SET_BIT(resume_module);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_resume();
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    wifi_suspend_lock &= ~SET_BIT(resume_module);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
    }
#endif

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "suspend cpu low power\r\n");

    if ((cpu_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "suspend low power error suspend_lock=%x, "
                    "suspend_module=%x\r\n", cpu_suspend_lock, SET_BIT(suspend_module));
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_suspend();
    }

    cpu_suspend_lock |= SET_BIT(suspend_module);
    krhino_mutex_unlock(&pwrmgmt_mutex);
}

int pwrmgmt_cpu_lowpower_resume(uint32_t resume_module)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "resume cpu low power\r\n");

    if ((cpu_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "resume low power error suspend_lock=%x, "
                    "reusme_module=%x\r\n", cpu_suspend_lock, SET_BIT(resume_module));
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    cpu_suspend_lock &= ~SET_BIT(resume_module);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_resume();
    }
    krhino_mutex_unlock(&pwrmgmt_mutex);
}
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int pwrmgmt_wifi_powersave_resume(uint32_t resume_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "resume wifi power save\r\n");

    if ((wifi_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "resume low power error suspend_lock=%x, "
                    "reusme_module=%x\r\n", wifi_suspend_lock, SET_BIT(resume_module));
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    wifi_suspend_lock &= ~SET_BIT(resume_module);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
    }

    krhino_mutex_unlock(&pwrmgmt_mutex);
}

int pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "suspend wifi power save\r\n");

    if ((wifi_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "suspend low power error suspend_lock=%x, "
                    "suspend_module=%x\r\n", wifi_suspend_lock, SET_BIT(suspend_module));
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_exit_powersave(module);
    }

    wifi_suspend_lock |= SET_BIT(suspend_module);

    krhino_mutex_unlock(&pwrmgmt_mutex);

    return 0;
}
#endif

#if ((PWRMGMT_CONFIG_CPU_LOWPOWER > 0) && (PWRMGMT_CONFIG_CPU_ACTIVE > 0))
static void pwrmgmt_task_entry(void *arg)
{
    while(1){
        krhino_sem_take(&pwrmgmt_sem, RHINO_WAIT_FOREVER);
        PWRMGMT_LOG(PWRMGMT_LOG_INFO, "stop the active state\r\n");
        krhino_timer_stop(&pwrmgmt_timer);
        pwrmgmt_cpu_lowpower_resume(PWRMGMT_PWRMGMT);
    }
}

static void pwrmgmt_timer_callback(void *timer, void *arg)
{
    krhino_sem_give(&pwrmgmt_sem);
    krhino_timer_stop(&pwrmgmt_timer);
}

int pwrmgmt_cpu_active_ms_set(uint32_t active_time)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "start to keep cpu be active for %d ms\r\n",
                active_time);
    pwrmgmt_cpu_lowpower_suspend(PWRMGMT_PWRMGMT);
    krhino_timer_change(&pwrmgmt_timer, krhino_ms_to_ticks(active_time), 0);
    krhino_timer_start(&pwrmgmt_timer);
}
#endif
