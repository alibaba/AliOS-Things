#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#include "soc_defs.h"

#define M_PWM_DUTY_IN_FREQ_LOW_PRECISE      (80)
#define M_PWM_DUTY_IN_FREQ_HIGH_PRECISE     (20)
#define M_PWM_DUTY_STEP					(1 << 12)

#if defined(CONFIG_OS_RHINO)
int8_t _hal_pwm_init(uint8_t pwm_id);
#else
int8_t hal_pwm_init(uint8_t pwm_id);
#endif
int8_t hal_pwm_deinit(void);
int8_t hal_pwm_config(uint8_t pwm_id, uint32_t freq_hz, uint32_t duty, uint8_t is_invert);
int8_t hal_pwm_enable(uint8_t pwm_id);
int8_t hal_pwm_disable(uint8_t pwm_id);
int8_t hal_pwm_enable_all(void);

#endif
