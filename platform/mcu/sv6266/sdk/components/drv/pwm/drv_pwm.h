#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include "hal_pwm.h"

/**
 * Initilize PWM to ready operations.
 * 
 * @param   pwm_id
 *              0x00 - PWM_0.
 *              0x01 - PWM_1.
 *              0x02 - PWM_2.
 *              0x03 - PWM_3.
 *              0x04 - PWM_4. 
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_pwm_init(uint8_t pwm_id);



/**
 * Deinitialize all PWM to disable operations
 * 
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_pwm_deinit(void);



/**
 * Saves PWM setting configuration.
 * 
 * @param   pwm_id
 *              0x00 - PWM_0.
 *              0x01 - PWM_1.
 *              0x02 - PWM_2.
 *              0x03 - PWM_3.
 *              0x04 - PWM_4.
 * @param   freq_hz         freq_hz range (5 ~ 2000000 - AHB bus 40M)(5 ~ 4000000 - AHB bus 80M).
 * @param   duty            range 0 ~ 4096
 * @param   is_invert       invert the waveform
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_pwm_config(uint8_t pwm_id, uint32_t freq_hz, uint32_t duty, uint8_t is_invert);


/**
 * Enables the PWM to start operation
 *
 * @param       pwm_id
 *              0x00 - PWM_0.
 *              0x01 - PWM_1.
 *              0x02 - PWM_2.
 *              0x03 - PWM_3.
 *              0x04 - PWM_4. 
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_pwm_enable(uint8_t pwm_id);



/**
 * Disables the PWM to stop operation
 *
 * @param   pwm_id
 *              0x00 - PWM_0.
 *              0x01 - PWM_1.
 *              0x02 - PWM_2.
 *              0x03 - PWM_3.
 *              0x04 - PWM_4. 
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_pwm_disable(uint8_t pwm_id);

#endif  // __DRV_PINMUX_H__

