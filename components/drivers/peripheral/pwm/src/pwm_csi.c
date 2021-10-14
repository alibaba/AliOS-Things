/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdbool.h>
#include <aos/pwm_core.h>
#include <aos/pwm_csi.h>

static void pwm_csi_unregister(aos_pwm_t *pwm)
{
    aos_pwm_csi_t *aos_csi_pwm = aos_container_of(pwm, aos_pwm_csi_t, aos_pwm);

    csi_pwm_uninit(&(aos_csi_pwm->csi_pwm));
}

static aos_status_t pwm_csi_startup(aos_pwm_t *pwm)
{
    return 0;
}

static void pwm_csi_shutdown(aos_pwm_t *pwm)
{
    aos_pwm_csi_t *aos_csi_pwm = aos_container_of(pwm, aos_pwm_csi_t, aos_pwm);
    csi_pwm_out_stop(&(aos_csi_pwm->csi_pwm), aos_csi_pwm->aos_pwm.dev.id);
}

static aos_status_t pwm_csi_apply(aos_pwm_t *pwm, aos_pwm_attr_t const *attr)
{
    uint32_t period;
    uint32_t duty_cycle;

    aos_pwm_csi_t *aos_csi_pwm = aos_container_of(pwm, aos_pwm_csi_t, aos_pwm);
    period = attr->period / 1000;
    duty_cycle = attr->duty_cycle / 1000;
    csi_pwm_out_config(&(aos_csi_pwm->csi_pwm), aos_csi_pwm->aos_pwm.dev.id,
                    period, duty_cycle, (csi_pwm_polarity_t)attr->polarity);
    if (attr->enabled)
        csi_pwm_out_start(&(aos_csi_pwm->csi_pwm), aos_csi_pwm->aos_pwm.dev.id);
    else
        csi_pwm_out_stop(&(aos_csi_pwm->csi_pwm), aos_csi_pwm->aos_pwm.dev.id);
    return 0;
}

static const aos_pwm_ops_t pwm_csi_ops = {
    .unregister = pwm_csi_unregister,
    .apply      = pwm_csi_apply,
    .shutdown   = pwm_csi_shutdown,
    .startup = pwm_csi_startup
};

aos_status_t aos_pwm_csi_register(aos_pwm_csi_t *pwm_csi)
{
    if (!pwm_csi)
        return -EINVAL;

    pwm_csi->aos_pwm.ops = &pwm_csi_ops;

    return aos_pwm_register(&pwm_csi->aos_pwm);
}

aos_status_t aos_pwm_csi_unregister(uint32_t id)
{
    return aos_pwm_unregister(id);
}
