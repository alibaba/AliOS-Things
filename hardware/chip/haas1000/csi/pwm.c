/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/pwm.h>
#include <aos/pwm_csi.h>
#include "hal_pwm.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_cmu.h"
#include "pmu.h"


#define _HAL_PWM_MAX_NUM 4
static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[_HAL_PWM_MAX_NUM] = {
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};

csi_error_t csi_pwm_init(csi_pwm_t *pwm, uint32_t idx)
{
    return CSI_OK;
}

void csi_pwm_uninit(csi_pwm_t *pwm)
{
    return;
}

csi_error_t csi_pwm_out_config(csi_pwm_t *pwm, uint32_t channel, uint32_t period_us,
                                uint32_t pulse_width_us, csi_pwm_polarity_t polarity)
{
    struct HAL_PWM_CFG_T cfg;
    hal_iomux_init(&pinmux_pwm[channel], 1);
    hal_gpio_pin_set_dir(pinmux_pwm[channel].pin, HAL_GPIO_DIR_OUT, 1);
    if (period_us == 0) {
        hal_pwm_disable(channel);
    } else {
        cfg.freq = 1000000UL / period_us;
        cfg.ratio = (((uint64_t)pulse_width_us) * 100 / period_us);
        cfg.inv = polarity;
        cfg.sleep_on = false;
        hal_pwm_enable(channel, &cfg);
    }
    return 0;
}

csi_error_t csi_pwm_out_start(csi_pwm_t *pwm, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    return CSI_OK;
}

void csi_pwm_out_stop(csi_pwm_t *pwm, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    hal_pwm_disable(pwm_chan);
}
static int pwm_csi_init(void)
{
    csi_error_t ret;
    static aos_pwm_csi_t pwm_csi_dev[CONFIG_PWM_NUM];
    int i;
    for (i = 0; i < CONFIG_PWM_NUM; i++) {
        pwm_csi_dev[i].csi_pwm.dev.idx |= (i);
        if (ret != CSI_OK) {
            return ret;
        }
        pwm_csi_dev[i].aos_pwm.dev.id = i;
        ret =  aos_pwm_csi_register(&(pwm_csi_dev[i]));
        if  (ret != CSI_OK) {
            return ret;
        }
    }
    return 0;
}

LEVEL1_DRIVER_ENTRY(pwm_csi_init)
