/**
 * @file pwm.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_PWM_H
#define _AOS_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_pwm_device PWM
 *  aos pwm API.
 *
 *  @{
 */


// The data struct/defination exposed to application/component

typedef void* pwm_dev_handle_t;

// The data struct/defination used inside i2c subsystem
typedef struct pwm_config {
    uint16_t freq;          /**< PWM's frequence */
    float    duty_cycle;    /**< PWM's duty cycle */
} pwm_config_t;

/**
 * setup new PWM channel with specified PWM config
 *
 * @param[in]  id  host PWM controller's channel ID
 * @param[in]  config target parameter when using PWM controller
 *
 * @return  handle of the PWM device, used for later PWM, when open operation success;
            NULL when open operation fails
 */
pwm_dev_handle_t aos_pwm_open(uint32_t id, pwm_config_t *config);

/**
 * close target PWM channel
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_close(pwm_dev_handle_t dev);

/**
 * set frequence of PWM controller's output
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 * @param[in]  freq  target frequence to be set
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_set_freq(i2c_dev_handle_t dev,  uint32_t freq);

/**
 * set duty cycle of PWM controller's output
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 * @param[in]  duty_cycle  target duty cycle to be set
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_set_duty_cycle(i2c_dev_handle_t dev,  float duty_cycle);

/**
 * start target PWM controller's output
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_start(i2c_dev_handle_t dev);

/**
 * stop target PWM controller's output
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_stop(i2c_dev_handle_t dev);

/**
 * set special parameter/operation on current PWM
 *
 * @param[in]  dev  handle of the pwm device to be operated, must be the return value of aos_pwm_open
 * @param[in]  freq  target frequence to be set
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_pwm_ctrl(i2c_dev_handle_t dev,  uint32_t *arg);

/**
 * get the number of PWM controller
 *
 * @return  number of PWM controller
 * 
 */
int32_t aos_pwm_get_num(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _AOS_PWM_H */