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

#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "i2c.h"
#include "nand.h"
#include "nor.h"
#include "pwm.h"
#include "qspi.h"
#include "rng.h"
#include "rtc.h"
#include "sd.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "wdg.h"
#include "interpt.h"
#include "dac.h"

#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_SOC_H */

