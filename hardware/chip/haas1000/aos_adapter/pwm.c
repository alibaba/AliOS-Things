/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/pwm.h>
#include <aos/pwm_core.h>
#include "hal_pwm.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_cmu.h"
#include "pmu.h"

#define NS_PER_SEC (1000000000UL)
#define DIV_ROUND_CLOSEST(x, divisor)(          \
{                                               \
    typeof(x) __x = x;                          \
    typeof(divisor) __d = divisor;              \
    (((typeof(x))-1) > 0 ||                     \
     ((typeof(divisor))-1) > 0 ||               \
     (((__x) > 0) == ((__d) > 0))) ?            \
        (((__x) + ((__d) / 2)) / (__d)) :       \
        (((__x) - ((__d) / 2)) / (__d));        \
}                                               \
)
typedef struct {
    aos_pwm_t aos_pwm;
    void *priv;
} haas1000_pwm_t;

#define _HAL_PWM_MAX_NUM 4
static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[_HAL_PWM_MAX_NUM] = {
    { HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE },
    { HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE },
    { HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE },
    { HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE },
};

static uint8_t pwm_chan[_HAL_PWM_MAX_NUM] = {0};
static void haas1000_pwm_uninit(haas1000_pwm_t *pwm)
{
    return;
}

int haas1000_pwm_out_config(haas1000_pwm_t *pwm, uint32_t channel, uint32_t period_ns,
                            uint32_t pulse_width_ns, csi_pwm_polarity_t polarity)
{
    struct HAL_PWM_CFG_T cfg;

    if (channel >= _HAL_PWM_MAX_NUM)
            return 0;
    if (pwm_chan[channel] == 0) {
        hal_iomux_init(&pinmux_pwm[channel], 1);
        hal_gpio_pin_set_dir(pinmux_pwm[channel].pin, HAL_GPIO_DIR_OUT, 1);
    }
    pwm_chan[channel] = 1;
    if (period_ns == 0) {
        hal_pwm_disable(channel);
    } else {
        cfg.freq = DIV_ROUND_CLOSEST(NS_PER_SEC, period_ns);
        cfg.ratio = DIV_ROUND_CLOSEST(((uint64_t)pulse_width_ns) * 100, period_ns);
        cfg.inv = polarity;
        cfg.sleep_on = false;
        hal_pwm_enable(channel, &cfg);
    }
    return 0;
}

static int haas1000_pwm_out_start(haas1000_pwm_t *pwm_dev, uint32_t channel)
{
    return 0;
}

static int haas1000_pwm_out_stop(haas1000_pwm_t *pwm_dev, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    hal_pwm_disable(pwm_chan);
    return 0;
}

static void haas1000_pwm_unregister(aos_pwm_t *pwm)
{
    haas1000_pwm_t *pwm_dev = aos_container_of(pwm, haas1000_pwm_t, aos_pwm);

    haas1000_pwm_uninit(pwm_dev);
}

static int haas1000_pwm_startup(aos_pwm_t *pwm)
{
    return 0;
}

static void haas1000_pwm_shutdown(aos_pwm_t *pwm)
{
    haas1000_pwm_t *pwm_dev = aos_container_of(pwm, haas1000_pwm_t, aos_pwm);
    haas1000_pwm_out_stop(pwm_dev, pwm_dev->aos_pwm.dev.id);
}

static int haas1000_pwm_apply(aos_pwm_t *pwm, aos_pwm_attr_t const *attr)
{
    uint32_t period;
    uint32_t duty_cycle;

    haas1000_pwm_t *pwm_dev = aos_container_of(pwm, haas1000_pwm_t, aos_pwm);
    period = attr->period;
    duty_cycle = attr->duty_cycle;
    haas1000_pwm_out_config(pwm_dev, pwm_dev->aos_pwm.dev.id,
                            period, duty_cycle, attr->polarity);
    if (attr->enabled)
        haas1000_pwm_out_start(pwm_dev, pwm_dev->aos_pwm.dev.id);
    else
        haas1000_pwm_out_stop(pwm_dev, pwm_dev->aos_pwm.dev.id);
    return 0;
}

static const aos_pwm_ops_t haas1000_pwm_ops = {
    .unregister = haas1000_pwm_unregister,
    .apply      = haas1000_pwm_apply,
    .shutdown   = haas1000_pwm_shutdown,
    .startup    = haas1000_pwm_startup
};
static int haas1000_pwm_init(void)
{
    int ret;
    static haas1000_pwm_t pwm_dev[CONFIG_PWM_NUM];
    int i;
    for (i = 0; i < CONFIG_PWM_NUM; i++) {
        pwm_dev[i].aos_pwm.dev.id = i;
        pwm_dev[i].aos_pwm.ops = &haas1000_pwm_ops;
        ret = aos_pwm_register(&(pwm_dev[i].aos_pwm));
        if  (ret != 0) {
            return ret;
        }
    }
    return 0;
}

LEVEL1_DRIVER_ENTRY(haas1000_pwm_init)
