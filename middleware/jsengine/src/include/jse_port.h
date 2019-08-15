/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_PORT_H
#define JSE_PORT_H

#if defined(__cplusplus)
extern "C" {
#endif

/* JSEngine OS & System Adapt Layer */
#include "aos/jse_osal.h"
#include "aos/jse_system.h"

#ifdef JSE_CLI_DEBUG
#include "aos/jse_cli.h"
#endif

/* AliOS Things HAL */
#include "aos/hal/adc.h"
#include "aos/hal/dac.h"
#include "aos/hal/flash.h"
#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "aos/hal/pwm.h"
#include "aos/hal/rtc.h"
#include "aos/hal/spi.h"
#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"

/* JSEngine HAL */
#include "hal/gpio_i2c.h"
#include "hal/lcd.h"

#if defined(__cplusplus)
}
#endif
#endif /* JSE_PORT_H */
