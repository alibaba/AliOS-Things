/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_SOC_H
#define HAL_SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "aos/hal/adc.h"
#include "aos/hal/flash.h"
#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "aos/hal/nand.h"
#include "aos/hal/nor.h"
#include "aos/hal/pwm.h"
#include "aos/hal/qspi.h"
#include "aos/hal/rng.h"
#include "aos/hal/rtc.h"
#include "aos/hal/sd.h"
#include "aos/hal/spi.h"
#include "aos/hal/timer.h"
#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"
#include "aos/hal/interpt.h"
#include "aos/hal/dac.h"

#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_SOC_H */

