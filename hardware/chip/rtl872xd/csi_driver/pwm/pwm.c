/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/pwm.h>
#include <aos/pwm_csi.h>
#include "device.h"
#include "objects.h"
#include "pinmap.h"

#define BIT_PWM_TIM_IDX_FLAG BIT(7)
#define BIT_PWM_TIM_IDX_SHIFT 7
#define PWM_TIMER 5

/* Table elements express the pin to PWM channel number, they are:
 * {pinName, km0_pin2chan, km4_pin2chan}
 */
const u32 csi_pin2chan[18][2] = {
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
    { PB_25, 17 } // this channel also can be PB_7
};

RTIM_TypeDef *CSI_PWM_TIM[2] = { TIM5, TIMM05 };

u8 csi_km4_ch_start[18] = { 0 };

csi_error_t csi_pwm_init(csi_pwm_t *pwm, uint32_t idx)
{
    if (!pwm)
        return CSI_ERROR;

    if (idx != 0) {
        return CSI_ERROR;
    }
    pwm->priv = (TIM_CCInitTypeDef *)malloc(sizeof(TIM_CCInitTypeDef));
    pwm->dev.idx = idx << BIT_PWM_TIM_IDX_SHIFT;

    return CSI_OK;
}

void csi_pwm_uninit(csi_pwm_t *pwm)
{
    uint32_t pwm_chan = pwm->dev.idx & (~BIT_PWM_TIM_IDX_FLAG);
    uint8_t pwm_tim_idx = pwm->dev.idx >> BIT_PWM_TIM_IDX_SHIFT;

    if (csi_km4_ch_start[pwm_chan]) {
        csi_km4_ch_start[pwm_chan] = 0;
        RTIM_CCxCmd(CSI_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);

        /* stop timer5 if no pwm channels starts */
        for (pwm_chan = 0; pwm_chan < 18; pwm_chan++) {
            if (csi_km4_ch_start[pwm_chan]) {
                return;
            }
        }
        RTIM_Cmd(CSI_PWM_TIM[pwm_tim_idx], DISABLE);
    }

    if (pwm && pwm->priv) {
        free(pwm->priv);
        pwm->priv = NULL;
    }
}

csi_error_t csi_pwm_out_config(csi_pwm_t *pwm, uint32_t channel, uint32_t period_us,
                                uint32_t pulse_width_us, csi_pwm_polarity_t polarity)
{
    uint32_t arr, tmp, ccrx;
    uint32_t period;
    float pulse, value, dc;
    uint8_t pwm_tim_idx = pwm->dev.idx >> BIT_PWM_TIM_IDX_SHIFT;
    TIM_CCInitTypeDef *TIM_CCInitStruct = (TIM_CCInitTypeDef *)pwm->priv;
    u32 csi_prescaler = 0;

    if (channel > 18) {
        return CSI_ERROR;
    }

    RTIM_CCStructInit(TIM_CCInitStruct);
    RTIM_CCxInit(CSI_PWM_TIM[pwm_tim_idx], TIM_CCInitStruct, channel);
    RTIM_CCxCmd(CSI_PWM_TIM[pwm_tim_idx], channel, TIM_CCx_Enable);

    PinName pin = csi_pin2chan[channel][0];

    if (pwm_tim_idx) {
        Pinmux_Config(pin, PINMUX_FUNCTION_PWM_LP);
    } else {
        Pinmux_Config(pin, PINMUX_FUNCTION_PWM_HS);
    }
    pwm->dev.idx |= channel;
    csi_km4_ch_start[channel] = 1;

    tmp = period_us * 40 / (csi_prescaler + 1);
    /*
     *  psr is 8bits
    */
    if (tmp > 0x10000) {
        csi_prescaler = period_us * 40 / 0x10000;
        if (csi_prescaler > 0xff) {
            csi_prescaler = 0xff;
        }
        RTIM_PrescalerConfig(CSI_PWM_TIM[pwm_tim_idx], csi_prescaler, TIM_PSCReloadMode_Update);
    }
    /*
     *     arr is 16bits
    */

    /*
     * 40M oscilator  range:2HZ-2KHZ?
    */
    arr = period_us * 40 / (csi_prescaler + 1) - 1;
    if (arr > 0xffff) {
        arr = 0xffff;
    }
    RTIM_ChangePeriod(CSI_PWM_TIM[pwm_tim_idx], arr);

    ccrx = (u32)((period_us - pulse_width_us) * 40 / (csi_prescaler + 1)) & 0x0000ffff;
    RTIM_CCRxSet(CSI_PWM_TIM[pwm_tim_idx], ccrx, channel);

    if (0 == polarity)
        RTIM_CCxPolarityConfig(CSI_PWM_TIM[pwm_tim_idx], TIM_CCPolarity_Low, channel);
    else
        RTIM_CCxPolarityConfig(CSI_PWM_TIM[pwm_tim_idx], TIM_CCPolarity_High, channel);
    return CSI_OK;
}

csi_error_t csi_pwm_out_start(csi_pwm_t *pwm, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    uint8_t pwm_tim_idx = pwm->dev.idx >> BIT_PWM_TIM_IDX_SHIFT;

    RTIM_CCxCmd(CSI_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Enable);
    return CSI_OK;
}

void csi_pwm_out_stop(csi_pwm_t *pwm, uint32_t channel)
{
    uint32_t pwm_chan = channel;
    uint8_t pwm_tim_idx = pwm->dev.idx >> BIT_PWM_TIM_IDX_SHIFT;

    RTIM_CCxCmd(CSI_PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);
}
static int pwm_csi_init(void)
{
    csi_error_t ret;
    static aos_pwm_csi_t pwm_csi_dev[CONFIG_PWM_NUM];
    int idx = 0, i;
    RTIM_TimeBaseInitTypeDef TIM_InitStruct;
    RTIM_TimeBaseStructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Idx = PWM_TIMER;
    RTIM_TimeBaseInit(CSI_PWM_TIM[idx], &TIM_InitStruct, TIMER5_IRQ, NULL, (u32)&TIM_InitStruct);
    RTIM_Cmd(CSI_PWM_TIM[idx], ENABLE);
    for (i = 0; i < CONFIG_PWM_NUM; i++) {
        ret = csi_pwm_init(&(pwm_csi_dev[i].csi_pwm), idx);
        pwm_csi_dev[i].csi_pwm.dev.idx |= (i) & (~BIT_PWM_TIM_IDX_FLAG);
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
