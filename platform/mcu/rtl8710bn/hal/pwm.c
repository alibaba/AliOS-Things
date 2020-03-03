#include <stdint.h>
#include "pwmout_api.h"
#include "aos/hal/pwm.h"
#include "board.h"

static pwmout_t PWM[PORT_PWM_MAX];

u32 port2pin[PORT_PWM_MAX][2]={
    {0, PA_23},
    {1, PA_22},
    {2, PA_0},
    {3, PA_12},
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
