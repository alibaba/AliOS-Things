/**
 ******************************************************************************
 * @file    BkDriverPwm.h
 * @brief   This file provides all the headers of PWM operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */
#include "include.h"
#include "rtos_pub.h"
#include "BkDriverPwm.h"
#include "drv_model_pub.h"
#include "error.h"
#include "pwm_pub.h"

OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, float duty_cycle)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.en     = PWM_DISABLE;
    param.cfg.bits.int_en = PWM_INT_EN;
    param.cfg.bits.mode   = PMODE_PWM;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = duty_cycle;
    param.end_value       = frequency;  // ?????

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_start(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_ENABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_stop(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UINT_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

// eof

