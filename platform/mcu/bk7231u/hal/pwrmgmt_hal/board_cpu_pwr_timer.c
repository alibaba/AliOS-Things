/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
 * C1: sleep mode.
 * C2: low power sleep mode.
 */

#include <k_api.h>

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)

#include "mcu_ps_pub.h"
#include "cpu_tickless.h"


#define TIMER_32BIT_MAX_COUNT 0xffffffff

static kspinlock_t mcu_spin;
static uint32_t init_flag = 0;


static pwr_status_t mcu_timer_init(void);
static uint32_t     mcu_one_shot_max_seconds(void);
static pwr_status_t mcu_one_shot_start(uint64_t planUs);
static pwr_status_t mcu_one_shot_stop(uint64_t *pPassedUs);

one_shot_timer_t mcu_one_shot =
{
    mcu_timer_init,
    mcu_one_shot_max_seconds,
    mcu_one_shot_start,
    mcu_one_shot_stop,
};

pwr_status_t mcu_timer_init(void)
{
    uint32_t           uwAPB1Prescaler = 0U;
    uint32_t           pFLatency;

    if (init_flag == 1)
    {
        return PWR_OK;
    }

    krhino_spin_lock_init (&mcu_spin);

    mcu_ps_init();

    /* Return function status */
    init_flag = 1;

    return PWR_OK;
}


/* return the max period(in second) that could trigger interrupt */
uint32_t mcu_one_shot_max_seconds(void)
{
    /* the max 32 bit value / count frequency */
    return (0xffffffff / 32000);
}

/**
 * mcu_one_shot_start - enable the timer in oneshot mode
 *
 * This function enables the timer in oneshot mode, the interrupt will be fired
 * after servral microseconds which is indicated by planUs.
 *
 * @return  PWR_OK or PWR_ERR if timer is not enabled
 */
pwr_status_t mcu_one_shot_start
(
    uint64_t  planUs      /* IN */
)
{
    uint64_t cnt;
    pwr_status_t ret;

    krhino_spin_lock_irq_save(&mcu_spin);

    ret = aos_mcu_ps_timer_start(planUs);

    krhino_spin_unlock_irq_restore (&mcu_spin);

    return ret;

}

/**
 * mcu_one_shot_stop - cancel the timer which was in oneshot mode
 *
 * This function is called to cancel the timer which was in oneshot mode.
 * the time has passed(microseconds) will be return in pPassedUs.
 * @param[out]  pPassedUs
 * @return  PWR_OK or PWR_ERR if timer is not disabled
 */
pwr_status_t mcu_one_shot_stop(uint64_t *pPassedUs)
{

    uint32_t count;
    uint32_t passedCount32;
    uint64_t passedCount64;

    /* validate the parameters */
    *pPassedUs = 0;

    krhino_spin_lock_irq_save(&mcu_spin);

    aos_mcu_ps_timer_stop(pPassedUs);

    krhino_spin_unlock_irq_restore (&mcu_spin);

    return PWR_OK;
}


#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */

