
#include "peripheralnames.h"
#include "pwmout_api.h"

#define PWM_NUM 8
static pwmout_t PWM_OBJ[PWM_NUM];
static PinName PWM_MAP[PWM_NUM] = {
    [0] = PD_0,
    [1] = PC_1,
    [2] = PB_0,
    [3] = PD_3,
    [4] = PB_3,
    [5] = PB_5,
    [6] = PB_6,
    [7] = PB_1,
};

int32_t hal_pwm_init(pwm_dev_t *pwm){
	if(pwm->port >= PWM_NUM){
		return -1;
	}

#ifndef DELETE_HFILOP_CODE
	if(pwm->config.freq > 1000000 || pwm->config.freq < 200)
		return -1;
	if(pwm->config.duty_cycle > 1 || pwm->config.duty_cycle < 0)
		return -1;
#endif

	pwm->priv = &PWM_OBJ[pwm->port];
	pwmout_init(pwm->priv, PWM_MAP[pwm->port]);

#ifndef DELETE_HFILOP_CODE
	static int init_clk = 0;
	if(!init_clk)
	{
		pwmout_clk_set(pwm->priv, 1, 0);
		init_clk = 1;
	}
#endif

	return 0;
}

int32_t hal_pwm_clk(pwm_dev_t *pwm, int src, int div){
    if(1 == src){
		pwm->config.freq = 10000000;
	}else{
	    pwm->config.freq = 16384;
	}
	pwmout_clk_set(pwm->priv, src, div);
	return 0;
}

int32_t hal_pwm_period_us(pwm_dev_t *pwm, int us){
	pwmout_period_us(pwm->priv, us);
	return 0;
}

int32_t hal_pwm_write(pwm_dev_t *pwm, float percent){
	pwm->config.duty_cycle = percent;
	pwmout_write(pwm->priv, percent);
	return 0;
}

int32_t hal_pwm_diff_write(pwm_dev_t *pwm, float percent){
	pwm->config.duty_cycle = percent;
	pwmout_diff_write(pwm->priv, percent);
	return 0;
}

int32_t hal_pwm_sync_write(pwm_dev_t *pwm, float percent, uint8_t duty_sel){
	pwm->config.duty_cycle = percent;
	pwmout_sync_write(pwm->priv, percent, duty_sel);
	return 0;
}

int32_t hal_pwm_start(pwm_dev_t *pwm){
#ifndef DELETE_HFILOP_CODE
	int us = 1000000/pwm->config.freq;
	pwmout_period_us(pwm->priv, us);
	pwmout_write(pwm->priv, pwm->config.duty_cycle);
#else
	pwmout_start(pwm->priv);
#endif
	return 0;
}

int32_t hal_pwm_stop(pwm_dev_t *pwm){
	pwmout_stop(pwm->priv);
	return 0;
}

int32_t hal_pwm_finalize(pwm_dev_t *pwm){
	pwmout_free(pwm->priv);
    return 0;
}
