/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_KEY_H__
#define __APP_KEY_H__

#include "hal_key.h"

#define APP_KEY_SET_MESSAGE(appevt, code, evt) (appevt = (((uint32_t)code&0xffffff)<<8)|(evt&0xff))
#define APP_KEY_GET_CODE(appevt, code) (code = (appevt>>8)&0xffffff)
#define APP_KEY_GET_EVENT(appevt, evt) (evt = appevt&0xff)

#define APP_KEY_CODE_GOOGLE                 APP_KEY_CODE_FN15
#define APP_KEY_CODE_VOICEPATH              APP_KEY_CODE_FN15

enum APP_KEY_CODE_T {
    APP_KEY_CODE_NONE = HAL_KEY_CODE_NONE,
    APP_KEY_CODE_PWR =  HAL_KEY_CODE_PWR,
    APP_KEY_CODE_FN1 =  HAL_KEY_CODE_FN1,
    APP_KEY_CODE_FN2 =  HAL_KEY_CODE_FN2,
    APP_KEY_CODE_FN3 =  HAL_KEY_CODE_FN3,
    APP_KEY_CODE_FN4 =  HAL_KEY_CODE_FN4,
    APP_KEY_CODE_FN5 =  HAL_KEY_CODE_FN5,
    APP_KEY_CODE_FN6 =  HAL_KEY_CODE_FN6,
    APP_KEY_CODE_FN7 =  HAL_KEY_CODE_FN7,
    APP_KEY_CODE_FN8 =  HAL_KEY_CODE_FN8,
    APP_KEY_CODE_FN9 =  HAL_KEY_CODE_FN9,
    APP_KEY_CODE_FN10 = HAL_KEY_CODE_FN10,
    APP_KEY_CODE_FN11 = HAL_KEY_CODE_FN11,
    APP_KEY_CODE_FN12 = HAL_KEY_CODE_FN12,
    APP_KEY_CODE_FN13 = HAL_KEY_CODE_FN13,
    APP_KEY_CODE_FN14 = HAL_KEY_CODE_FN14,
    APP_KEY_CODE_FN15 = HAL_KEY_CODE_FN15,
};

enum APP_KEY_EVENT_T {
    APP_KEY_EVENT_NONE               = HAL_KEY_EVENT_NONE,
    APP_KEY_EVENT_DOWN               = HAL_KEY_EVENT_DOWN,
    APP_KEY_EVENT_FIRST_DOWN         = HAL_KEY_EVENT_FIRST_DOWN,
    APP_KEY_EVENT_CONTINUED_DOWN     = HAL_KEY_EVENT_CONTINUED_DOWN,
    APP_KEY_EVENT_UP                 = HAL_KEY_EVENT_UP,
    APP_KEY_EVENT_UP_AFTER_LONGPRESS = HAL_KEY_EVENT_UP_AFTER_LONGPRESS,
    APP_KEY_EVENT_LONGPRESS          = HAL_KEY_EVENT_LONGPRESS,
    APP_KEY_EVENT_LONGLONGPRESS      = HAL_KEY_EVENT_LONGLONGPRESS,
    APP_KEY_EVENT_CLICK              = HAL_KEY_EVENT_CLICK,
    APP_KEY_EVENT_DOUBLECLICK        = HAL_KEY_EVENT_DOUBLECLICK,
    APP_KEY_EVENT_TRIPLECLICK        = HAL_KEY_EVENT_TRIPLECLICK,
    APP_KEY_EVENT_ULTRACLICK         = HAL_KEY_EVENT_ULTRACLICK,
    APP_KEY_EVENT_RAMPAGECLICK       = HAL_KEY_EVENT_RAMPAGECLICK,
    APP_KEY_EVENT_REPEAT             = HAL_KEY_EVENT_REPEAT,
    APP_KEY_EVENT_GROUPKEY_DOWN      = HAL_KEY_EVENT_GROUPKEY_DOWN,
    APP_KEY_EVENT_GROUPKEY_REPEAT    = HAL_KEY_EVENT_GROUPKEY_REPEAT,
    APP_KEY_EVENT_INITDOWN           = HAL_KEY_EVENT_INITDOWN,
    APP_KEY_EVENT_INITUP             = HAL_KEY_EVENT_INITUP,
    APP_KEY_EVENT_INITLONGPRESS      = HAL_KEY_EVENT_INITLONGPRESS,
    APP_KEY_EVENT_INITLONGLONGPRESS  = HAL_KEY_EVENT_INITLONGLONGPRESS,
    APP_KEY_EVENT_INITFINISHED       = HAL_KEY_EVENT_INITFINISHED,

    APP_KEY_EVENT_NUM = HAL_KEY_EVENT_NUM,
};

typedef struct {
    uint32_t code;
    uint8_t event;
}APP_KEY_STATUS;

typedef void (*APP_KEY_HANDLE_CB_T)(APP_KEY_STATUS*, void *param);

typedef struct {
    APP_KEY_STATUS key_status;
    const char* string;
    APP_KEY_HANDLE_CB_T function;
    void *param;
} APP_KEY_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

int app_key_open(int checkPwrKey);

int app_key_close(void);

uint32_t app_key_read_status(uint32_t code);

int app_key_handle_registration(const APP_KEY_HANDLE *key_handle);

void app_key_handle_clear(void);

void app_key_simulate_key_event(uint32_t key_code, uint8_t key_event);

int simul_key_event_process(uint32_t key_code, uint8_t key_event);

#ifdef __cplusplus
}
#endif
#endif//__FMDEC_H__
