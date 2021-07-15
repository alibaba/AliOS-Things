/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __TIMER_SERVICE_H__
#define __TIMER_SERVICE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_export.h"

// mv to app mk file
// #define ENABLE_COUNTDOWN_LIST
// #define ENABLE_LOCALTIMER
// #define ENABLE_PERIOD_TIMER
// #define ENABLE_RANDOM_TIMER

#define NUM_OF_LOCALTIMER 5

#define DAYS_OF_WEEK    7
#define HOURS_OF_DAY    24
#define MINUTES_OF_HOUR 60
#define SECONDS_OF_MINUTE   60
#define MINUTES_OF_WEEK     (60 * 24 * 7)
#define SUNDAY          1556956800       //2019/5/5 0:0:0 sunday
#define SECONDS_OF_DAY  86400


#define STRING_MAX_LEN  64
#define DEFAULT_TIMEZONEOFFSET    8  // BEIJING

#define NUM_OF_COUNTDOWN_TARGET 1
#define COUNTDOWN_TARGET_INIT() \
    const char *countdown_target_list[NUM_OF_COUNTDOWN_TARGET] = { "PowerSwitch"}

typedef enum timer_service_type {
    COUNT_DOWN,
#ifdef ENABLE_COUNTDOWN_LIST
    COUNT_DOWN_LIST,
#endif
#ifdef ENABLE_LOCALTIMER
    LOCAL_TIMER,
#endif
#ifdef ENABLE_PERIOD_TIMER
    PERIOD_TIMER,
#endif
#ifdef ENABLE_RANDOM_TIMER
    RANDOM_TIMER,
#endif
    TYPE_MAX
} timer_service_type_t;
const char *str_prop_name[]={
                            "CountDown",
                        #ifdef ENABLE_COUNTDOWN_LIST
                            "CountDownList",
                        #endif
                        #ifdef ENABLE_LOCALTIMER
                            "LocalTimer",
                        #endif
                        #ifdef ENABLE_PERIOD_TIMER
                            "PeriodTimer",
                        #endif
                        #ifdef ENABLE_RANDOM_TIMER
                            "RandomTimer",
                        #endif
                            ""};

typedef struct countdown {
    int value_list[NUM_OF_COUNTDOWN_TARGET];
    int time_left;
    int duration;
    int power_switch;
    int is_running;
    char timeStamp[20];
} countdown_t;

#ifdef ENABLE_COUNTDOWN_LIST
#define NUM_OF_COUNTDOWNLIST 10
#define NUM_OF_COUNTDOWN_LIST_TARGET 10
typedef struct countdown_list {
    // int value_list[NUM_OF_COUNTDOWN_LIST_TARGET];
    int time_left;
    int duration;
    int is_running;
    char timeStamp[20];
    int action;
} countdown_list_t;
#endif
#ifdef ENABLE_LOCALTIMER
#define NUM_OF_LOCAL_TIMER_TARGET 5
typedef struct local_timer {
    int value_list[NUM_OF_LOCAL_TIMER_TARGET];
    char cron_timer[STRING_MAX_LEN];
    char targets[STRING_MAX_LEN];
    int offset[DAYS_OF_WEEK];
    int enable;
    int action;
    int is_valid;
    int timezone_offset;
    int repeat;
} local_timer_t;
#endif 

#ifdef ENABLE_PERIOD_TIMER
#define NUM_OF_PERIOD_TIMER 1
typedef struct period_timer {
    // int value_list[NUM_OF_PERIOD_TIMER_TARGET];
    int offset_start[DAYS_OF_WEEK];
    char start[8];
    int offset_end[DAYS_OF_WEEK];
    char end[8];
    int timezoneOffset;
    int repeat;
    char repeat_raw[16];
    int enable;
    int run_time;
    int sleep_time;
} period_timer_t;
#endif

#ifdef ENABLE_RANDOM_TIMER
#define NUM_OF_RANDOM_TIMER 1
#define RANDOM_MINUTE_LIMIT 30
typedef struct random_timer {
    // int value_list[NUM_OF_RANDOM_TIMER_TARGET];
    int offset_start[DAYS_OF_WEEK];
    char start[8];
    int offset_end[DAYS_OF_WEEK];
    char end[8];
    int timezoneOffset;
    uint8_t repeat;
    char repeat_raw[16];
    int enable;
} random_timer_t;
#endif

#define EXAMPLE_TRACE(...)                               \
    do {                                                     \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\033[0m\r\n");                                   \
    } while (0)

#if defined(__cplusplus)
}								/* extern "C" */
#endif
#endif  /* __TIMER_SERVICE_H__ */