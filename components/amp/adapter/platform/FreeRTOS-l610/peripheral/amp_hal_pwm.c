/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "amp_hal_pwm.h"
#include "drv_pwm.h"
#include "drv_names.h"
#include "osi_log.h"
#include "fibo_common.h"
#include "fibo_gpio.h"


static drvPwm_t *pwm_drv= NULL;


int32_t amp_hal_pwm_init(pwm_dev_t *pwm)
{
	
	OSI_LOGI(0, "pwm iomux set");
	#ifdef CONFIG_FIBOCOM_LC610
	fibo_hal_gpioModeSet(53,1);  //lpg
	fibo_hal_gpioModeSet(54,3);		//pwl0
	fibo_hal_gpioModeSet(17,2);   // pwT
	fibo_hal_gpioModeSet(55,3);  //pwL1
	#else
	//fibo_hal_gpioModeSet(5,1);  //lpg
	//fibo_hal_gpioModeSet(13,3);		//pwl0
	fibo_hal_gpioModeSet(135,2);   // pwT
	//fibo_hal_gpioModeSet(133,3);  //pwL1
	#endif
	pwm_drv = drvPwmAcquire(DRV_NAME_PWM1);
	if (pwm_drv == NULL)
	{
		OSI_LOGI(0, "pwm test init fail");
		return false;
	}
	uint duty_cycle = pwm->config.duty_cycle*100;
	OSI_LOGI(0, "pwm iomux set pwm->config.freq = %d, duty_cycle = %d\n", pwm->config.freq, duty_cycle);	
	drvPwtSetPeriodDuty(pwm_drv, pwm->config.freq, 0, duty_cycle);	
    return 0;
}

int32_t amp_hal_pwm_start(pwm_dev_t *pwm)
{
    OSI_LOGI(0, "pwm hal_pwm_start \n");	
    drvPwtStart(pwm_drv);
    return 0;
}

int32_t amp_hal_pwm_stop(pwm_dev_t *pwm)
{
    drvPwtStop(pwm_drv);
    return 0;
}

int32_t amp_hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    drvPwtSetPeriodDuty(pwm_drv, para.freq, 0, para.duty_cycle*100);
    return 0;
}

int32_t amp_hal_pwm_finalize(pwm_dev_t *pwm)
{
	if (pwm_drv != NULL)
	{
		drvPwmRelease(pwm_drv);
	}
    return 0;
}



