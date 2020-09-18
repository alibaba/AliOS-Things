/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_KEY_H__
#define __HAL_KEY_H__

#include "hal_gpio.h"
#include "hal_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_KEY_CODE_T {
    HAL_KEY_CODE_NONE = 0,
    HAL_KEY_CODE_PWR = (1 << 0),
    HAL_KEY_CODE_FN1 = (1 << 1),
    HAL_KEY_CODE_FN2 = (1 << 2),
    HAL_KEY_CODE_FN3 = (1 << 3),
    HAL_KEY_CODE_FN4 = (1 << 4),
    HAL_KEY_CODE_FN5 = (1 << 5),
    HAL_KEY_CODE_FN6 = (1 << 6),
    HAL_KEY_CODE_FN7 = (1 << 7),
    HAL_KEY_CODE_FN8 = (1 << 8),
    HAL_KEY_CODE_FN9 = (1 << 9),
    HAL_KEY_CODE_FN10 = (1 << 10),
    HAL_KEY_CODE_FN11 = (1 << 11),
    HAL_KEY_CODE_FN12 = (1 << 12),
	HAL_KEY_CODE_FN13 = (1 << 13),
	HAL_KEY_CODE_FN14 = (1 << 14),
	HAL_KEY_CODE_FN15 = (1 << 15),
};

enum HAL_KEY_EVENT_T {
    HAL_KEY_EVENT_NONE = 0,
    HAL_KEY_EVENT_DOWN,
    HAL_KEY_EVENT_FIRST_DOWN,
    HAL_KEY_EVENT_CONTINUED_DOWN,
    HAL_KEY_EVENT_UP,
    HAL_KEY_EVENT_UP_AFTER_LONGPRESS,
    HAL_KEY_EVENT_LONGPRESS,
    HAL_KEY_EVENT_LONGLONGPRESS,
    HAL_KEY_EVENT_CLICK,
    HAL_KEY_EVENT_DOUBLECLICK,
    HAL_KEY_EVENT_TRIPLECLICK,
    HAL_KEY_EVENT_ULTRACLICK,
    HAL_KEY_EVENT_RAMPAGECLICK,
    HAL_KEY_EVENT_REPEAT,
    HAL_KEY_EVENT_GROUPKEY_DOWN,
    HAL_KEY_EVENT_GROUPKEY_REPEAT,
    HAL_KEY_EVENT_INITDOWN,
    HAL_KEY_EVENT_INITUP,
    HAL_KEY_EVENT_INITLONGPRESS,
    HAL_KEY_EVENT_INITLONGLONGPRESS,
    HAL_KEY_EVENT_INITFINISHED,

    HAL_KEY_EVENT_NUM,
};

#define KEY_EVENT_SET(a)                (1 << HAL_KEY_EVENT_ ## a)
#define KEY_EVENT_SET2(a, b)            (KEY_EVENT_SET(a) | KEY_EVENT_SET(b))
#define KEY_EVENT_SET3(a, b, c)         (KEY_EVENT_SET2(a, b) | KEY_EVENT_SET(c))
#define KEY_EVENT_SET4(a, b, c, d)      (KEY_EVENT_SET3(a, b, c) | KEY_EVENT_SET(d))

enum HAL_KEY_GPIOKEY_VAL_T {
    HAL_KEY_GPIOKEY_VAL_LOW = 0,
    HAL_KEY_GPIOKEY_VAL_HIGH,
};

struct HAL_KEY_GPIOKEY_CFG_T {
    enum HAL_KEY_CODE_T key_code;
    struct HAL_IOMUX_PIN_FUNCTION_MAP key_config;
    enum HAL_KEY_GPIOKEY_VAL_T key_down;
};

int hal_key_open(int checkPwrKey, int (* cb)(uint32_t, uint8_t));

enum HAL_KEY_EVENT_T hal_key_read_status(enum HAL_KEY_CODE_T code);

int hal_key_close(void);

int get_recovery_mode(void);
#ifdef __cplusplus
    }
#endif

#endif//__FMDEC_H__
