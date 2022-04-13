/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_PWM_H_
#define _IO_PWM_H_

#include <stdbool.h>

#define IOC_PWM_BASE 'T'
#define IOC_PWM_ON IOC_PWM_BASE + 1 // PWM on
#define IOC_PWM_OFF IOC_PWM_BASE + 2 // PWM off
#define IOC_PWM_FREQ IOC_PWM_BASE + 3 // pwm's frequency
#define IOC_PWM_DUTY_CYCLE IOC_PWM_BASE + 4 // pwm's duty cycle

int vfs_pwm_drv_init (void);

#endif //_IO_PWM_H_
