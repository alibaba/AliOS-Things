/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/pwm.h>
#include <aos/pwm_core.h>
#include "device.h"
#include "objects.h"
#include "pinmap.h"

#define BIT_PWM_TIM_IDX_FLAG BIT(7)
#define BIT_PWM_TIM_IDX_SHIFT 7
#define PWM_TIMER 5

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
   uint8_t idx;
   void           *priv;
} rtl872xd_pwm_t;
/* Table elements express the pin to PWM channel number, they are:
 * {pinName, km0_pin2chan, km4_pin2chan}
 */
static const u32 pin2chan[18][2] = {
    { PA_12, 0 },
    { PA_13, 1 },
    { PA_23, 2 },
    { PA_24, 3 },
    { PA_25, 4 },
    { PA_26, 5 },
    { PA_28, 6 },
    { PA_30, 7 },
    { PB_4, 8 },
    { PB_5, 9 },
    { PB_18, 10 },
    { PB_19, 11 },
    { PB_20, 12 },
    { PB_21, 13 },
    { PB_22, 14 },
    { PB_23, 15 },
    { PB_24, 16 },
    { PB_7, 17 } // this channel also can be PB_7
};

static RTIM_TypeDef *RTL872xD_PWM_TIM[2] = { TIM5, TIMM05 };

static u8 km4_ch_start[18] = { 0 };

static void rtl872xd_pwm_uninit(rtl872xd_pwm_t *pwm_dev)
{
    uint32_t pwm_chan = pwm_dev->idx & (~BIT_PWM_TIM_IDX_FLAG);
    uint8_t pwm_tim_idx = pwm_dev->idx >> BIT_PWM_TIM_IDX_SHIFT;

    if (km4_ch_start[pwm_chan]) {
        km4_ch_start[pwm_chan] = 0;
        RTIM_CCxCmd(RTL872xD_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);

        /* stop timer5 if no pwm channels starts */
        for (pwm_chan = 0; pwm_chan < 18; pwm_chan++) {
            if (km4_ch_start[pwm_chan]) {
                return;
            }
        }
        RTIM_Cmd(RTL872xD_PWM_TIM[pwm_tim_idx], DISABLE);
    }

    if (pwm_dev && pwm_dev->priv) {
        free(pwm_dev->priv);
        pwm_dev->priv = NULL;
    }
}

static int rtl872xd_pwm_out_config(rtl872xd_pwm_t *pwm_dev, uint32_t channel, uint32_t period_ns,
                                   uint32_t pulse_width_ns, csi_pwm_polarity_t polarity)
{
    uint32_t arr, tmp, ccrx;
    uint32_t period;
    float pulse, value, dc;
    uint8_t pwm_tim_idx = pwm_dev->idx >> BIT_PWM_TIM_IDX_SHIFT;
    TIM_CCInitTypeDef *TIM_CCInitStruct = (TIM_CCInitTypeDef *)pwm_dev->priv;
    u32 prescaler = 0;

    if (channel > 18) {
        return 0;
    }
    RTIM_CCStructInit(TIM_CCInitStruct);
    RTIM_CCxInit(RTL872xD_PWM_TIM[pwm_tim_idx], TIM_CCInitStruct, channel);
    RTIM_CCxCmd(RTL872xD_PWM_TIM[pwm_tim_idx], channel, TIM_CCx_Enable);
    PinName pin = pin2chan[channel][0];
    if (km4_ch_start[channel] == 0) {
        if (pwm_tim_idx) {
            Pinmux_Config(pin, PINMUX_FUNCTION_PWM_LP);
        } else {
            Pinmux_Config(pin, PINMUX_FUNCTION_PWM_HS);
        }
    }
    km4_ch_start[channel] = 1;

    tmp = DIV_ROUND_CLOSEST(((uint64_t)period_ns * 40), 1000);
    tmp = tmp / (prescaler + 1);
    /*
     *  psr is 8bits
    */
    if (tmp > 0x10000) {
        prescaler = DIV_ROUND_CLOSEST(((uint64_t)period_ns * 40), 1000);
        prescaler = DIV_ROUND_CLOSEST(prescaler, 0x10000);
        if (prescaler > 0xff) {
            prescaler = 0xff;
        }
        RTIM_PrescalerConfig(RTL872xD_PWM_TIM[pwm_tim_idx], prescaler, TIM_PSCReloadMode_Update);
    }
    /*
     *     arr is 16bits
    */

    /*
     * 40M oscilator
    */
    arr = DIV_ROUND_CLOSEST(((uint64_t)period_ns * 40), 1000);
    arr = DIV_ROUND_CLOSEST(arr, (prescaler + 1)) - 1;
    if (arr > 0xffff) {
        arr = 0xffff;
    }
    RTIM_ChangePeriod(RTL872xD_PWM_TIM[pwm_tim_idx], arr);
    ccrx = DIV_ROUND_CLOSEST(((uint64_t)(period_ns - pulse_width_ns) * 40), 1000);
    ccrx = (ccrx / (prescaler + 1)) & 0x0000ffff;
    RTIM_CCRxSet(RTL872xD_PWM_TIM[pwm_tim_idx], ccrx, channel);

    if (0 == polarity)
        RTIM_CCxPolarityConfig(RTL872xD_PWM_TIM[pwm_tim_idx], TIM_CCPolarity_Low, channel);
    else
        RTIM_CCxPolarityConfig(RTL872xD_PWM_TIM[pwm_tim_idx], TIM_CCPolarity_High, channel);
    return 0;
}

static int rtl872xd_pwm_out_start(rtl872xd_pwm_t *pwm_dev, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    uint8_t pwm_tim_idx = pwm_dev->idx >> BIT_PWM_TIM_IDX_SHIFT;

    RTIM_CCxCmd(RTL872xD_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Enable);
    return 0;
}

static int rtl872xd_pwm_out_stop(rtl872xd_pwm_t *pwm_dev, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    uint8_t pwm_tim_idx = pwm_dev->idx >> BIT_PWM_TIM_IDX_SHIFT;

    RTIM_CCxCmd(RTL872xD_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);
    return 0;
}

static void rtl872xd_pwm_unregister(aos_pwm_t *pwm)
{
    rtl872xd_pwm_t *pwm_dev = aos_container_of(pwm, rtl872xd_pwm_t, aos_pwm);

    rtl872xd_pwm_uninit(pwm_dev);
}

static int rtl872xd_pwm_startup(aos_pwm_t *pwm)
{
    return 0;
}

static void rtl872xd_pwm_shutdown(aos_pwm_t *pwm)
{
    rtl872xd_pwm_t *pwm_dev = aos_container_of(pwm, rtl872xd_pwm_t, aos_pwm);
    rtl872xd_pwm_out_stop(pwm_dev, pwm_dev->aos_pwm.dev.id);
}

static int rtl872xd_pwm_apply(aos_pwm_t *pwm, aos_pwm_attr_t const *attr)
{
    uint32_t period;
    uint32_t duty_cycle;

    rtl872xd_pwm_t *pwm_dev = aos_container_of(pwm, rtl872xd_pwm_t, aos_pwm);
    period = attr->period;
    duty_cycle = attr->duty_cycle;
    rtl872xd_pwm_out_config(pwm_dev, pwm_dev->aos_pwm.dev.id,
                            period, duty_cycle, attr->polarity);
    if (attr->enabled)
        rtl872xd_pwm_out_start(pwm_dev, pwm_dev->aos_pwm.dev.id);
    else
        rtl872xd_pwm_out_stop(pwm_dev, pwm_dev->aos_pwm.dev.id);
    return 0;
}

static const aos_pwm_ops_t rtl872xd_pwm_ops = {
    .unregister = rtl872xd_pwm_unregister,
    .apply      = rtl872xd_pwm_apply,
    .shutdown   = rtl872xd_pwm_shutdown,
    .startup    = rtl872xd_pwm_startup
};

static int rtl872xd_pwm_init(void)
{
    int ret;
    static rtl872xd_pwm_t pwm_dev[CONFIG_PWM_NUM];
    int idx = 0, i;
    RTIM_TimeBaseInitTypeDef TIM_InitStruct;
    RTIM_TimeBaseStructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Idx = PWM_TIMER;
    RTIM_TimeBaseInit(RTL872xD_PWM_TIM[idx], &TIM_InitStruct, TIMER5_IRQ, NULL, (u32)&TIM_InitStruct);
    RTIM_Cmd(RTL872xD_PWM_TIM[idx], ENABLE);
    for (i = 0; i < CONFIG_PWM_NUM; i++) {
        pwm_dev[i].priv = (TIM_CCInitTypeDef *)malloc(sizeof(TIM_CCInitTypeDef));
        pwm_dev[i].idx = idx << BIT_PWM_TIM_IDX_SHIFT;
        pwm_dev[i].idx |= (i) & (~BIT_PWM_TIM_IDX_FLAG);
        if (ret != 0) {
            return ret;
        }
        pwm_dev[i].aos_pwm.dev.id = i;
        pwm_dev[i].aos_pwm.ops = &rtl872xd_pwm_ops;
        ret = aos_pwm_register(&(pwm_dev[i].aos_pwm));
        if  (ret != 0) {
            return ret;
        }
    }
    return 0;
}

LEVEL1_DRIVER_ENTRY(rtl872xd_pwm_init)
