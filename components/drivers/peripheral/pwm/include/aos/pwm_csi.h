/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_PWM_CSI_H
#define _AOS_PWM_CSI_H

#include <aos/pwm_core.h>
#include <drv/pwm.h>

typedef struct {
    aos_pwm_t aos_pwm;  /* aos pwm device */
    csi_pwm_t csi_pwm; /* csi pwm device */
} aos_pwm_csi_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register an CSI PWM device
 *
 * @param[in]   pwm      PWM ref to operate. The caller should allocate memory,
 *                       init it and set id for the CSI PWM device.
 *
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_pwm_csi_register(aos_pwm_csi_t *pwm_csi);

/**
 * Unregister an CSI PWM device by id
 *
 * @param[in]   id      CSI PWM device id
 *
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_pwm_csi_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_PWM_CSI_H */
