#include <stdint.h>
#include "pwmout_api.h"
#include "aos/hal/pwm.h"


static pwmout_t PWM[18];

u32 port2pin[18][2]={
	{0, PA_12},
	{1, PA_13},
	{2, PA_23},
	{3, PA_24},
	{4, PA_25},
	{5, PA_26},
	{6, PA_28},
	{7, PA_30},
	{8, PB_4},
	{9, PB_5},
	{10, PB_18},
	{11, PB_19},
	{12, PB_20},
	{13, PB_21},
	{14, PB_22},
	{15, PB_23},
	{16, PB_24},
	{17, PB_25},    //this channel also can be PB_7
};

/**
 * Initialises a PWM pin
 *
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{
	int period_us;
	period_us = 1000000/pwm->config.freq;

	pwm->priv = &(PWM[pwm->port]);

	
	pwmout_init( pwm->priv , port2pin[pwm->port][1]);

	pwmout_period_us(pwm->priv , period_us);

	pwmout_write(pwm->priv , pwm->config.duty_cycle);

	return 0;

}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
	pwmout_start(pwm->priv);
	return 0;
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
	pwmout_stop(pwm->priv);
	
	return 0;
}

/**
 * change the para of pwm
 *
 * @param[in]  pwm  the PWM device
 * @param[in]  para the para of pwm
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
	int period_us;
	period_us = 1000000/para.freq;

	pwmout_period_us(pwm->priv, period_us);

	pwmout_write(pwm->priv, para.duty_cycle);
	return 0;
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
	pwmout_free(pwm->priv);
	return 0;

}
