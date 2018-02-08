/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 

#include "rtl8195a.h"

#ifdef CONFIG_PWM_EN
#include "hal_pwm.h"
#include "hal_timer.h"

const u8 PWMTimerIdx[MAX_PWM_CTRL_PIN]= {3,4,5,2};  // the G-timer ID used for PWM pin 0~3

/**
  * @brief  Initializes and enable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  * @param  sel: pin mux selection
  *
  * @retval HAL_Status
  */
HAL_Status 
HAL_Pwm_Init(
    HAL_PWM_ADAPTER *pPwmAdapt,
    u32 pwm_id,
    u32 sel
)
{
    u32 timer_id;

    if (NULL == pPwmAdapt) {
        DBG_PWM_ERR ("HAL_Pwm_Init: NULL adapter\n");
        return HAL_ERR_PARA;
    }

    if ((pwm_id >= MAX_PWM_CTRL_PIN) || (sel > 3)) {
        DBG_PWM_ERR ("HAL_Pwm_Init: Invalid PWM index(%d), sel(%d)\n", pwm_id, sel);
        return HAL_ERR_PARA;
    }
    
    pPwmAdapt->pwm_id = pwm_id;
    pPwmAdapt->sel = sel;
    timer_id = PWMTimerIdx[pwm_id];
    pPwmAdapt->gtimer_id = timer_id;

    if (_FALSE == FunctionChk((pPwmAdapt->pwm_id+PWM0), pPwmAdapt->sel)) {
        return HAL_ERR_HW;
    }

#ifndef CONFIG_CHIP_E_CUT
    return HAL_Pwm_Init_8195a(pPwmAdapt);
#else
    return HAL_Pwm_Init_8195a_V04(pPwmAdapt);
#endif
}


/**
  * @brief  Disable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  *
  * @retval None
  */
void 
HAL_Pwm_Enable(
    HAL_PWM_ADAPTER *pPwmAdapt
)
{
    if (NULL == pPwmAdapt) {
        DBG_PWM_ERR ("HAL_Pwm_Enable: NULL adapter\n");
        return;
    }
    
#ifndef CONFIG_CHIP_E_CUT
    HAL_Pwm_Enable_8195a(pPwmAdapt);
#else
    HAL_Pwm_Enable_8195a_V04(pPwmAdapt);
#endif
}


/**
  * @brief  Disable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  *
  * @retval None
  */
void 
HAL_Pwm_Disable(
    HAL_PWM_ADAPTER *pPwmAdapt
)
{
    if (NULL == pPwmAdapt) {
        DBG_PWM_ERR ("HAL_Pwm_Disable: NULL adapter\n");
        return;
    }
    
#ifndef CONFIG_CHIP_E_CUT
    HAL_Pwm_Disable_8195a(pPwmAdapt);
#else
    HAL_Pwm_Disable_8195a_V04(pPwmAdapt);
#endif
}

/**
  * @brief  Set the duty ratio of the PWM pin.
  *
  * @param  pwm_id: the PWM pin index
  * @param  period: the period time, in micro-second.
  * @param  pulse_width: the pulse width time, in micro-second.
  *
  * @retval None
  */
void
HAL_Pwm_SetDuty(
    HAL_PWM_ADAPTER *pPwmAdapt,
    u32 period,
    u32 pulse_width
)
{
    if (NULL == pPwmAdapt) {
        DBG_PWM_ERR ("HAL_Pwm_SetDuty: NULL adapter\n");
        return;
    }
    
#ifndef CONFIG_CHIP_E_CUT
    HAL_Pwm_SetDuty_8195a(pPwmAdapt, period, pulse_width);
#else
    HAL_Pwm_SetDuty_8195a_V04(pPwmAdapt, period, pulse_width);
#endif
}


#endif  // end of "#ifdef CONFIG_PWM_EN"
