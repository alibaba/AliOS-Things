/**
 * @file pwm.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_HAL_PWM_INTERNAL_H
#define AOS_HAL_PWM_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_pwm PWM
 *  pwm hal API.
 *
 *  @{
 */

#include <stdint.h>

typedef struct {
    uint32_t duty_cycle; /**< the pwm duty_cycle */
    uint32_t freq;       /**< the pwm freq */
} aos_hal_pwm_config_t;

typedef struct {
    uint8_t       port;   /**< pwm port */
    uint8_t       channel;
    uint8_t       active;
    aos_hal_pwm_config_t  config; /**< spi config */
    void         *priv;   /**< priv data */
} aos_hal_pwm_dev_t;

/**
 * Initialises a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_pwm_init(aos_hal_pwm_dev_t *pwm);

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_pwm_start(aos_hal_pwm_dev_t *pwm);

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_pwm_stop(aos_hal_pwm_dev_t *pwm);

/**
 * change the para of pwm
 *
 * @param[in]  pwm   the PWM device
 * @param[in]  para  the para of pwm
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_pwm_para_chg(aos_hal_pwm_dev_t *pwm, aos_hal_pwm_config_t para);

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_pwm_finalize(aos_hal_pwm_dev_t *pwm);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_HAL_PWM_H */

