/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_TIME_H__
#define __GENIE_TIME_H__

#include <hal/hal.h>

#pragma pack(1)
typedef struct
{
    uint16_t type;
    uint8_t para;
} vendor_attr_data_t;
#pragma pack()

#define GENIE_TIME_HW_TEIMER_ID (0)
#define GENIE_TIME_HW_TEIMER_PERIOD (1000 * 1000)

#define DEF_SYNC_PERIOD 180
#define DEF_SYNC_DELAY 10
#define DEF_SYNC_DELAY_RETRY 10

//no update when init,because Genie will push UTC time when device bootup
//#define GENIE_TIME_UPDATE_WHEN_BOOTUP (1)

//#define GT_STORE

#define MINU 60
#define HOUR (60 * MINU)
#define DAY (24 * HOUR)

#define GENIE_TIME_MAX (40)

typedef int (*genie_time_event_func_t)(uint8_t event, uint8_t index, vendor_attr_data_t *data);

typedef enum
{
    TIMER_OFF = 0,
    TIMER_ON = 1,
    TIMER_INVAILD = 0xf,
} vt_state;

typedef struct _genie_time_s
{
    sys_snode_t next;
    uint8_t index;
    uint8_t state : 4;
    uint8_t periodic : 1;
    uint32_t periodic_time;
    uint8_t schedule;
    uint32_t unixtime_match;
    vendor_attr_data_t attr_data;
} genie_time_t;

typedef struct _unixtime_sync_s
{
    uint16_t period_time;
    uint8_t retry_delay;
    uint8_t retry_times;
} unixtime_sync_t;

typedef struct _genie_time_data_s
{
    uint16_t magic;
    int8_t timezone;
    unixtime_sync_t timing_sync_config;
    genie_time_t timer_data[GENIE_TIME_MAX];
} genie_time_data_t;

typedef struct _genie_time_timer_s
{
    uint8_t init : 1;
    uint8_t update : 1;
    timer_dev_t timer;
    sys_slist_t timer_list_active;
    sys_slist_t timer_list_idle;
    uint32_t unix_time;
    uint32_t unix_time_sync_match;
    uint8_t unix_time_sync_retry_times;
    genie_time_event_func_t genie_time_event_cb;
    struct k_work work;
    struct k_sem lock;
} genie_time_timer_t;

typedef struct _utc_time_s
{
    uint16_t year;   // 2019+
    uint8_t month;   // 0-11
    uint8_t day;     // 1-31
    uint8_t seconds; // 0-59
    uint8_t minutes; // 0-59
    uint8_t hour;    // 0-23
    uint8_t weekday; // 0 means sunday
} utc_time_t;

typedef enum
{
    GT_OK = 0,
    GT_E_INIT,
    GT_E_LOCALTIME_NOTSET,
    GT_E_INDEX,
    GT_E_PARAM,
    GT_E_NORESOURCE,
    GT_E_OTHER,
} genie_time_errno;

enum
{
    GT_TIMEOUT = 0,
    GT_TIMING_SYNC = 1,
};

int genie_time_init(void);
int genie_time_finalize(void);
int genie_time_handle_model_mesg(genie_transport_model_param_t *p_msg);

#endif
