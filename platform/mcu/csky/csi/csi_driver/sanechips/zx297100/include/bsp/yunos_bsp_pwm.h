/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_PWM_H
#define YUNOS_BSP_PWM_H

#include <stdint.h>
#include <yunos_bsp_errno.h>

/**
 * error number related definitions: 0x0000AB00
 */
#define YUNOS_BSP_API_PWM_INIT        0x01
#define YUNOS_BSP_API_PWM_OPEN        0x02
#define YUNOS_BSP_API_PWM_CLOSE       0x03
#define YUNOS_BSP_API_PWM_CONFIG      0x04
#define YUNOS_BSP_API_PWM_START       0x05
#define YUNOS_BSP_API_PWM_STOP        0x06

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * Init the pwm, and provide the pwm device channels' number.
 * @param[in]   number  Output pointer which neet the device count.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_init(FAR uint32_t *number);

/**
 * Open the device with id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_open(uint32_t id);

/**
 * Close the device with id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_close(uint32_t id);

/**
 * Config PWM frequence and duty.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @param[in]   freq    PWM frequence(Hz).
 * @param[in]   duty    PWM duty(0-100).
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_config(uint32_t id, uint32_t freq, uint32_t duty);

/**
 * Start PWM.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_start(uint32_t id);

/**
 * Set the pwm time with the device id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_pwm_stop(uint32_t id);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_PWM_H */
