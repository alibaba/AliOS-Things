#include "pwm/hal_pwm.h"
#include "gpio/hal_gpio.h"
#include <hal/soc/pwm.h>
#include "errno.h"


int32_t hal_pwm_init(pwm_dev_t *pwm) {
    if ((pwm->port >= PWM_0) && (pwm->port <= PWM_4)) {
        _hal_pwm_init(pwm->port);
        hal_pwm_config(pwm->port, pwm->config.freq, (int32_t)pwm->config.duty_cycle, 0);
        return 0;
    }
    return EIO;
}

int32_t hal_pwm_start(pwm_dev_t *pwm) {
    if ((pwm->port >= PWM_0) && (pwm->port <= PWM_4)) {
        hal_pwm_config(pwm->port, pwm->config.freq, (int32_t)(pwm->config.duty_cycle*4096), 0);
        hal_pwm_enable(pwm->port);
        return 0;
    }
    return EIO;
}

int32_t hal_pwm_stop(pwm_dev_t *pwm) {
    if ((pwm->port >= PWM_0) && (pwm->port <= PWM_4)) {
        hal_pwm_disable(pwm->port);
        return 0;
    }
    return EIO;
}

int32_t hal_pwm_finalize(pwm_dev_t *pwm) {
    if ((pwm->port >= PWM_0) && (pwm->port <= PWM_4)) {
        hal_pwm_disable(pwm->port);
        return 0;
    }
    return EIO;
}

