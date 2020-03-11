#include <stdio.h>
#include <stdint.h>
#include "lega_cm4.h"
#include "lega_pwm.h"
#include "aos/hal/pwm.h"

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
    return lega_pwm_init((lega_pwm_dev_t *)pwm);
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
    return lega_pwm_start((lega_pwm_dev_t *)pwm);
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
    return lega_pwm_stop((lega_pwm_dev_t *)pwm);
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
    lega_pwm_config_t lega_para;
    lega_para.duty_cycle = para.duty_cycle;
    lega_para.freq = para.freq;
    return lega_pwm_para_chg((lega_pwm_dev_t *)pwm, lega_para);
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
    return lega_pwm_finalize((lega_pwm_dev_t *)pwm);
}

