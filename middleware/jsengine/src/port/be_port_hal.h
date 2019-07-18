/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*文件功能说明:
  本文件定义了硬件接口抽象层Hardware Abstract Layer,
  使用统一的头文件定义，并基于不同的平台实现这些接口函数
 */

#ifndef BE_PORT_HAL_H
#define BE_PORT_HAL_H

#if defined(__cplusplus)
extern "C" {
#endif

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

#include "hal/gpio_i2c.h"
#include "hal/lcd.h"
#include "hal/system.h"

#if defined(__cplusplus)
}
#endif
#endif /* BE_PORT_HAL_H */
