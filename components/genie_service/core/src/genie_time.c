/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <api/mesh.h>
#include <misc/slist.h>
#include "genie_service.h"

#ifdef GENIE_TIME_UPDATE_WHEN_BOOTUP
#include "mesh.h"
#endif

static inline uint8_t is_leap_year(uint16_t year);
static genie_time_data_t genie_time_data;
static genie_time_timer_t genie_time_timer;
static utc_time_t local_time = {0};

#define GENIE_TIME_LOCK k_sem_take(&genie_time_timer.lock, -1)
#define GENIE_TIME_UNLOCK k_sem_give(&genie_time_timer.lock)

utc_time_t genie_time_local_time_get()
{
    return local_time;
}

uint32_t genie_time_local_unixtime_get()
{
    return genie_time_timer.unix_time;
}

static int genie_time_time_sync_get(uint16_t *period_time, uint8_t *retry_delay, uint8_t *retry_times)
{
    *period_time = genie_time_data.timing_sync_config.period_time;
    *retry_delay = genie_time_data.timing_sync_config.retry_delay;
    *retry_times = genie_time_data.timing_sync_config.retry_times;

    return 0;
}

static int genie_time_save()
{
#ifdef GT_STORE
    int ret;

    ret = genie_storage_write_userdata(GFI_MESH_VND_TIMER, &genie_time_data, sizeof(genie_time_data));

    if (ret)
    {
        GENIE_LOG_DBG("vendor timers save fail %d\n", ret);
    }

    return ret;
#else
    return -1;
#endif
}

static inline uint32_t UTC2unix(utc_time_t *utc_time)
{
    uint32_t days;
    uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    days = (utc_time->year - 1970) * 365;
    days += (utc_time->year - 1969) >> 2;

    days += g_noleap_daysbeforemonth[utc_time->month];

    if (utc_time->month >= 2 && is_leap_year(utc_time->year))
    {
        days++;
    }

    days += utc_time->day - 1;

    return ((days * 24 + utc_time->hour) * 60 + utc_time->minutes) * 60 + utc_time->seconds;
}

static inline uint8_t next_weekday_diff_get(uint8_t weekday_now, uint8_t schedule)
{
    uint16_t schedule_tmp = 0;

    if (weekday_now == 0)
    {
        weekday_now = 7;
    }

    schedule_tmp = ((schedule | ((uint16_t)schedule << 7)) >> (weekday_now - 1)) & 0x7f;

    uint8_t day_diff = 0;

    while (day_diff < 7)
    {
        if ((schedule_tmp >> day_diff) & 0x0001)
        {
            break;
        }

        day_diff++;
    }

    return day_diff;
}

static genie_time_t *genie_time_find(uint8_t index)
{
    if (index >= GENIE_TIME_MAX)
    {
        return NULL;
    }

    GENIE_TIME_LOCK;
    genie_time_t *tmp, *node;
    sys_slist_t *list;
    list = &genie_time_timer.timer_list_active;

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, node, tmp, next)
    {
        if (node->index == index)
        {
            GENIE_TIME_UNLOCK;
            return node;
        }
    }
    GENIE_TIME_UNLOCK;

    return NULL;
}

static genie_time_t *genie_time_new()
{
    genie_time_t *free_timer;
    GENIE_TIME_LOCK;
    free_timer = (genie_time_t *)sys_slist_get(&genie_time_timer.timer_list_idle);
    GENIE_TIME_UNLOCK;
    GENIE_LOG_DBG("timer new %p\n", free_timer);
    return free_timer;
}

static int genie_time_periodic_start(uint8_t index, uint32_t periodic_time, uint8_t schedule, vendor_attr_data_t *attr_data)
{
    genie_time_t *vendor_timer;

    GENIE_LOG_DBG("periodic timer start index %d periodic_time %d schedule %d on_off %d\n",
                  index, periodic_time, schedule, attr_data->para);

    if (!genie_time_timer.init)
    {
        return -GT_E_INIT;
    }

    if (!genie_time_timer.update)
    {
        return -GT_E_LOCALTIME_NOTSET;
    }

    if (index >= GENIE_TIME_MAX)
    {
        //return -GT_E_INDEX;
    }

    if (schedule == 0)
    {
        return -GT_E_PARAM;
    }

    vendor_timer = genie_time_find(index);

    if (vendor_timer == NULL)
    {
        vendor_timer = genie_time_new();

        if (vendor_timer == NULL)
        {
            return -GT_E_NORESOURCE;
        }
    }
    else
    {
        GENIE_TIME_LOCK;
        sys_slist_find_and_remove(&genie_time_timer.timer_list_active, &vendor_timer->next);
        GENIE_TIME_UNLOCK;
    }

    vendor_timer->index = index;
    vendor_timer->periodic = 1;
    vendor_timer->periodic_time = periodic_time;
    vendor_timer->schedule = schedule;
    vendor_timer->state = TIMER_ON;
    vendor_timer->attr_data.para = attr_data->para;
    vendor_timer->attr_data.type = attr_data->type;

    utc_time_t utc = local_time;
    utc.hour = 0;
    utc.minutes = 0;
    utc.seconds = 0;
    utc.day = utc.day + next_weekday_diff_get(local_time.weekday, schedule);

    vendor_timer->unixtime_match = UTC2unix(&utc) + periodic_time - genie_time_data.timezone * HOUR;

    GENIE_LOG_DBG("periodic timer unixtime_match %d\n", vendor_timer->unixtime_match);

    GENIE_TIME_LOCK;
    sys_slist_append(&genie_time_timer.timer_list_active, &vendor_timer->next);
    GENIE_TIME_UNLOCK;

    genie_time_save();

    return 0;
}

static int8_t genie_time_timezone_get()
{
    return genie_time_data.timezone;
}

static int genie_time_operate_status(uint8_t tid, u16_t attr_type)
{
    u16_t payload_len = 0;
    uint8_t payload[10] = {0};
    genie_transport_payload_param_t transport_payload_param;

    switch (attr_type)
    {
    case UNIX_TIME_T:
    {
        uint32_t unix_time = genie_time_local_unixtime_get();

        payload[payload_len++] = UNIX_TIME_T & 0xff;
        payload[payload_len++] = (UNIX_TIME_T >> 8) & 0xff;
        payload[payload_len++] = (uint8_t)(unix_time & 0xFF);
        payload[payload_len++] = (uint8_t)((unix_time >> 8) & 0xFF);
        payload[payload_len++] = (uint8_t)((unix_time >> 16) & 0xFF);
        payload[payload_len++] = (uint8_t)((unix_time >> 24) & 0xFF);
    }
    break;
    case TIMEZONE_SETTING_T:
    {
        int8_t timezone = genie_time_timezone_get();

        payload[payload_len++] = TIMEZONE_SETTING_T & 0xff;
        payload[payload_len++] = (TIMEZONE_SETTING_T >> 8) & 0xff;
        payload[payload_len++] = timezone;
    }
    break;
    case TIMING_SYNC_T:
    {
        u16_t period_time = 0;
        u8_t retry_delay = 0;
        u8_t retry_times = 0;
        genie_time_time_sync_get(&period_time, &retry_delay, &retry_times);

        payload[payload_len++] = TIMING_SYNC_T & 0xff;
        payload[payload_len++] = (TIMING_SYNC_T >> 8) & 0xff;
        payload[payload_len++] = period_time & 0xff;

        payload[payload_len++] = (period_time >> 8) & 0xff;
        payload[payload_len++] = retry_delay;
        payload[payload_len++] = retry_times;
    }
    break;

    default:
        return -1;
    }

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_STATUS;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = payload_len;
    transport_payload_param.retry_cnt = 1;

    return genie_transport_send_payload(&transport_payload_param);
}

static int _genie_time_errcode_status(u16_t attr_type, u8_t err_code, u8_t index, uint8_t tid)
{
    uint8_t payload[6] = {0};
    genie_transport_payload_param_t transport_payload_param;

    payload[0] = ERROR_CODE_T & 0xff;
    payload[1] = (ERROR_CODE_T >> 8) & 0xff;
    payload[2] = attr_type & 0xff;
    payload[3] = (attr_type >> 8) & 0xff;
    payload[4] = err_code;
    payload[5] = index;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_STATUS;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = 1;

    return genie_transport_send_payload(&transport_payload_param);
}

static int genie_time_start(uint8_t index, uint32_t unix_time, vendor_attr_data_t *attr_data)
{
    genie_time_t *vendor_timer;

    if (!attr_data)
    {
        return -GT_E_PARAM;
    }

    GENIE_LOG_DBG("timer start index %d unix_time %d on_off %d\n",
                  index, unix_time, attr_data->type);

    if (!genie_time_timer.init)
    {
        return -GT_E_INIT;
    }

    if (!genie_time_timer.update)
    {
        return -GT_E_LOCALTIME_NOTSET;
    }

    if (index >= GENIE_TIME_MAX)
    {
        //return -GT_E_INDEX;
    }

    if (unix_time <= genie_time_timer.unix_time)
    {
        return -GT_E_PARAM;
    }

    vendor_timer = genie_time_find(index);

    if (vendor_timer == NULL)
    {
        vendor_timer = genie_time_new();

        if (vendor_timer == NULL)
        {
            return -GT_E_NORESOURCE;
        }
    }
    else
    {
        GENIE_TIME_LOCK;
        sys_slist_find_and_remove(&genie_time_timer.timer_list_active, &vendor_timer->next);
        GENIE_TIME_UNLOCK;
    }

    vendor_timer->index = index;
    vendor_timer->unixtime_match = unix_time; // + genie_time_timer.timezone * HOUR;
    vendor_timer->state = TIMER_ON;
    vendor_timer->attr_data.type = attr_data->type;
    vendor_timer->attr_data.para = attr_data->para;

    GENIE_TIME_LOCK;
    sys_slist_append(&genie_time_timer.timer_list_active, &vendor_timer->next);
    GENIE_TIME_UNLOCK;

    genie_time_save();

    return 0;
}

int genie_time_timing_settting_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    struct
    {
        uint32_t unix_time;
        uint8_t index;
        vendor_attr_data_t attr_data;
    } timing_setting_attr;

    u8_t *pmsg = msg;
    u16_t attr_type = *pmsg++;
    uint16_t msg_len = msg_length;
    genie_transport_payload_param_t transport_payload_param;

    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_SETTING_T)
    {
        return -1;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK && op != VENDOR_OP_ATTR_GET_STATUS)
    {
        return -1;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK)
    {
        while (msg_len > 0)
        {
            if (msg_len < sizeof(timing_setting_attr))
            {
                break;
            }

            timing_setting_attr.index = *pmsg++;
            uint32_t unixtime_tmp = (pmsg[0]) | (pmsg[1] << 8) | (pmsg[2] << 16) | (pmsg[3] << 24);
            pmsg += 4;

            timing_setting_attr.unix_time = unixtime_tmp / 60 * 60;

            if (unixtime_tmp % 60 != 1)
            {
                //return _genie_time_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (timing_setting_attr.attr_data.type != ATTR_TYPE_GENERIC_ONOFF)
            {
                return _genie_time_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                                                  timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.para = *pmsg++;

            if (timing_setting_attr.attr_data.para > 1)
            {
                return _genie_time_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                  timing_setting_attr.index, tid);
            }

            int ret = genie_time_start(timing_setting_attr.index,
                                       timing_setting_attr.unix_time,
                                       &timing_setting_attr.attr_data);

            if (ret)
            {
                uint8_t errcode;

                if (ret == -GT_E_INIT || ret == GT_E_LOCALTIME_NOTSET)
                {
                    errcode = ERR_CODE_UNIXTIME;
                }
                else if (ret == -GT_E_INDEX)
                {
                    errcode = ERR_CODE_TIMER_INDEX;
                }
                else if (ret == -GT_E_NORESOURCE)
                {
                    errcode = ERR_CODE_TIMER_FULL;
                }
                else if (ret == -GT_E_PARAM)
                {
                    errcode = ERR_CODE_TIMER_SETTING;
                }
                else
                {
                    errcode = ERR_CODE_UNIXTIME;
                }

                return _genie_time_errcode_status(TIMING_SETTING_T, errcode,
                                                  timing_setting_attr.index, tid);
            }

            msg_len -= sizeof(timing_setting_attr);
        }
    }

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_STATUS;
    transport_payload_param.p_payload = msg;
    transport_payload_param.payload_len = msg_length;
    transport_payload_param.retry_cnt = 1;

    return genie_transport_send_payload(&transport_payload_param);
}

int genie_time_priordic_timing_settting_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    struct
    {
        uint8_t index;
        uint16_t prioridc_time;
        uint8_t schedule;
        vendor_attr_data_t attr_data;
    } priordic_timing_attr;

    u8_t *pmsg = msg;
    u16_t attr_type = *pmsg++;
    uint16_t msg_len = msg_length;
    genie_transport_payload_param_t transport_payload_param;

    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_PERIODIC_SETTING_T)
    {
        return -1;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK && op != VENDOR_OP_ATTR_GET_STATUS)
    {
        return -1;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK)
    {
        while (msg_len > 0)
        {
            if (msg_len < 7)
            { // sizeof(priordic_timing_attr) by bytes
                break;
            }

            priordic_timing_attr.index = *pmsg++;
            uint32_t priordic_tmp = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            priordic_timing_attr.prioridc_time = priordic_tmp & 0x0FFF;

            if (((priordic_tmp >> 12) & 0x0F) != 1)
            {
                //return _genie_time_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.schedule = *pmsg++;

            if (priordic_timing_attr.schedule > 0x7F)
            {
                return _genie_time_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_TIMER_PRIORDIC_PARAM,
                                                  priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (priordic_timing_attr.attr_data.type != ATTR_TYPE_GENERIC_ONOFF)
            {
                return _genie_time_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                                                  priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.para = *pmsg++;

            if (priordic_timing_attr.attr_data.para > 1)
            {
                return _genie_time_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                  priordic_timing_attr.index, tid);
            }

            int ret = genie_time_periodic_start(priordic_timing_attr.index,
                                                priordic_timing_attr.prioridc_time * 60,
                                                priordic_timing_attr.schedule,
                                                &priordic_timing_attr.attr_data);

            if (ret)
            {
                uint8_t errcode;

                if (ret == -GT_E_INIT || ret == GT_E_LOCALTIME_NOTSET)
                {
                    errcode = ERR_CODE_UNIXTIME;
                }
                else if (ret == -GT_E_INDEX)
                {
                    errcode = ERR_CODE_TIMER_INDEX;
                }
                else if (ret == -GT_E_NORESOURCE)
                {
                    errcode = ERR_CODE_TIMER_FULL;
                }
                else if (ret == -GT_E_PARAM)
                {
                    errcode = ERR_CODE_TIMER_SETTING;
                }
                else
                {
                    errcode = ERR_CODE_TIMER_PRIORDIC_PARAM;
                }

                return _genie_time_errcode_status(TIMING_PERIODIC_SETTING_T, errcode,
                                                  priordic_timing_attr.index, tid);
            }

            msg_len -= 7; // sizeof(priordic_timing_attr) by bytes
        }
    }

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_STATUS;
    transport_payload_param.p_payload = msg;
    transport_payload_param.payload_len = msg_length;
    transport_payload_param.retry_cnt = 1;

    return genie_transport_send_payload(&transport_payload_param);
}

static int genie_time_stop(int8_t index)
{
    if (!genie_time_timer.init)
    {
        return -GT_E_INIT;
    }

    if (!genie_time_timer.update)
    {
        return -GT_E_LOCALTIME_NOTSET;
    }

    if (index >= GENIE_TIME_MAX)
    {
        return -GT_E_INDEX;
    }

    genie_time_t *vendor_timer = genie_time_find(index);

    if (vendor_timer == NULL)
    {
        return -GT_E_INDEX;
    }
    else
    {
        GENIE_TIME_LOCK;
        GENIE_LOG_DBG("timer stop %d\n", index);
        vendor_timer->index = 0xFF;
        vendor_timer->state = TIMER_INVAILD;
        vendor_timer->unixtime_match = 0;
        sys_slist_find_and_remove(&genie_time_timer.timer_list_active, &vendor_timer->next);
        sys_slist_append(&genie_time_timer.timer_list_idle, &vendor_timer->next);
        GENIE_TIME_UNLOCK;
    }

    return 0;
}

static int genie_time_remove(int8_t index)
{
    int i;
    int ret = 0;

    GENIE_LOG_DBG("timer remove %d\n", index);

    /* remove alll timers */
    if (index == 0xFF)
    {
        for (i = 0; i < GENIE_TIME_MAX; i++)
        {
            genie_time_stop(i);
        }
    }
    else
    {
        ret = genie_time_stop(index);
    }

    genie_time_save();

    return ret;
}

int genie_time_timing_remove_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    u8_t *pmsg = msg;
    u16_t attr_type = *pmsg++;
    uint16_t msg_len = msg_length;
    genie_transport_payload_param_t transport_payload_param;

    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_DELETE_T)
    {
        return -1;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK)
    {
        return -1;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK)
    {
        while (msg_len > 0)
        {
            uint8_t index = *pmsg++;
            msg_len--;

            int ret = genie_time_remove(index);

            if (ret)
            {
                //return _genie_time_errcode_status(TIMING_DELETE_T, ERR_CODE_TIMER_INDEX, index, tid);
            }
        }
    }

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_STATUS;
    transport_payload_param.p_payload = msg;
    transport_payload_param.payload_len = msg_length;
    transport_payload_param.retry_cnt = 1;

    return genie_transport_send_payload(&transport_payload_param);
}

static void _genie_time_timeout_indicate(u8_t index)
{
    uint8_t payload[4] = {0};
    genie_transport_payload_param_t transport_payload_param;

    payload[0] = TIMING_TIMEOUT_T & 0xff;
    payload[1] = (TIMING_TIMEOUT_T >> 8) & 0xff;
    payload[2] = EL_TIMING_TIMEOUT_T;
    payload[3] = index;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    genie_transport_send_payload(&transport_payload_param);
}

static inline utc_time_t unix2UTC(uint32_t unix_time)
{
    utc_time_t utc;

    uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
    uint32_t epoch;
    uint32_t jdn;
    int year;
    int month;
    int day;
    int hour;
    int minutes;
    int sec;
    int weekday;

    epoch = unix_time;

    jdn = epoch / DAY;
    epoch -= DAY * jdn;

    weekday = (jdn + 4) % 7; // 1970/1/1 is thursday

    hour = epoch / HOUR;
    epoch -= HOUR * hour;

    minutes = epoch / MINU;
    epoch -= MINU * minutes;

    sec = epoch;

    year = jdn / (4 * 365 + 1);  /* Number of 4-years periods since the epoch */
    jdn -= year * (4 * 365 + 1); /* Remaining days */
    year <<= 2;                  /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    bool leapyear;
    int tmp;

    for (;;)
    {
        /* Is this year a leap year (we'll need this later too) */

        leapyear = is_leap_year(year + 1970);

        /* Get the number of days in the year */

        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */

        if (jdn >= tmp)
        {
            /* Yes.. bump up the year */

            year++;
            jdn -= tmp;
        }
        else
        {
            /* Nope... then go handle months */

            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */

    year += 1970;

    /* Handle the month (zero based) */
    int min = 0;
    int max = 11;
    int value = 0;

    do
    {
        /* Get the midpoint */

        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */

        tmp = g_noleap_daysbeforemonth[value + 1];

        if (value + 1 >= 2 && leapyear)
        {
            tmp++;
        }

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */

        if (tmp > jdn)
        {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */

            tmp = g_noleap_daysbeforemonth[value];

            if (value >= 2 && leapyear)
            {
                tmp++;
            }

            if (tmp > jdn)
            {
                /* No... The one we want is somewhere between min and value-1 */

                max = value - 1;
            }
            else
            {
                /* Yes.. 'value' contains the month that we want */

                break;
            }
        }
        else
        {
            /* No... The one we want is somwhere between value+1 and max */

            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */

        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */

    tmp = g_noleap_daysbeforemonth[value];

    if (value >= 2 && leapyear)
    {
        tmp++;
    }

    jdn -= tmp;

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */

    month = value; // zero based
    day = jdn + 1; // one based

    utc.year = year;
    utc.month = month;
    utc.day = day;
    utc.weekday = weekday;
    utc.hour = hour;
    utc.minutes = minutes;
    utc.seconds = sec;

    return utc;
}

static inline uint8_t is_leap_year(uint16_t year)
{
    if (((year % 4) == 0) && ((year % 100) != 0))
    {
        return 1;
    }
    else if ((year % 400) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline void month_update()
{
    local_time.month++;

    if (local_time.month >= 12)
    {
        local_time.month = 0;
        local_time.year++;
    }
}

static inline void days_update()
{
    uint8_t month_days_list[12] = {
        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31,
    };

    local_time.day++;

    if (is_leap_year(local_time.year))
    {
        month_days_list[1] = 29;
    }

    uint8_t month_day = month_days_list[local_time.month];

    if (local_time.day > month_day)
    {
        local_time.day = 0;
        month_update();
    }

    local_time.weekday = (local_time.weekday + 1) % 7;
}

static inline void hours_update()
{
    local_time.hour++;

    if (local_time.hour == 24)
    {
        local_time.hour = 0;
        days_update();
    }
}

static inline void minutes_update()
{
    local_time.minutes++;

    if (local_time.minutes == 60)
    {
        local_time.minutes = 0;
        hours_update();
    }
}

static inline void seconds_update()
{
    local_time.seconds++;

    if (local_time.seconds == 60)
    {
        local_time.seconds = 0;
        minutes_update();
    }
}

static inline uint8_t is_weekday_match(uint8_t weekday_now, uint8_t schedule)
{
    uint8_t weekday_mask = weekday_now ? (uint8_t)(1 << (weekday_now - 1)) : (uint8_t)(1 << 6);
    return (weekday_mask == (schedule & weekday_mask));
}

static inline uint8_t next_weekday(uint8_t weekday_now)
{
    return (weekday_now + 1) % 7;
}

static int genie_time_erase()
{
#ifdef GT_STORE
    int ret;
    ret = genie_storage_delete_userdata();
    if (ret)
    {
        GENIE_LOG_DBG("vendor timers flash erase fail %d\n", ret);
    }
    return ret;
#else
    return 0;
#endif
}

static int genie_time_restore()
{
#ifdef GT_STORE
    int ret;

    uint16_t len = sizeof(genie_time_data);

    memset(&genie_time_data, 0, sizeof(genie_time_data));

    ret = genie_storage_read_userdata(GFI_MESH_VND_TIMER, &genie_time_data, &len);

    if (ret)
    {
        GENIE_LOG_DBG("vendor timers restore fail %d\n", ret);
        return ret;
    }

    if (genie_time_data.magic != 0xABCD || genie_time_data.timezone < -12 || genie_time_data.timezone > 12)
    {
        GENIE_LOG_DBG("vendor timers restore missmatch\n");
        return -1;
    }

    int i = 0;

    for (i = 0; i < GENIE_TIME_MAX; i++)
    {
        if (genie_time_data.timer_data[i].state != TIMER_INVAILD)
        {
            sys_slist_append(&genie_time_timer.timer_list_active, &genie_time_data.timer_data[i].next);
        }
        else
        {
            sys_slist_append(&genie_time_timer.timer_list_idle, &genie_time_data.timer_data[i].next);
        }

        GENIE_LOG_DBG("restore vendor timer index %d state %d periodic %d periodic_time %d schedule %d unixtime_match %d\n",
                      genie_time_data.timer_data[i].index, genie_time_data.timer_data[i].state,
                      genie_time_data.timer_data[i].periodic, genie_time_data.timer_data[i].periodic_time,
                      genie_time_data.timer_data[i].schedule, genie_time_data.timer_data[i].unixtime_match);
    }

    return 0;
#else
    return -1;
#endif
}

static uint8_t is_genie_time_timeout(genie_time_t *vendor_timer)
{
    if (vendor_timer->state == TIMER_INVAILD)
    {
        return 0;
    }

    if (vendor_timer->periodic)
    {
        if (is_weekday_match(local_time.weekday, vendor_timer->schedule) && vendor_timer->unixtime_match < genie_time_timer.unix_time)
        {
            vendor_timer->unixtime_match += (1 + next_weekday_diff_get(next_weekday(local_time.weekday), vendor_timer->schedule)) * DAY;
        }
    }

    return vendor_timer->unixtime_match == genie_time_timer.unix_time;
}

static void genie_time_do_work(struct k_work *work)
{
    genie_time_t *tmp = NULL, *node = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&genie_time_timer.timer_list_active, node, tmp, next)
    {
        if (is_genie_time_timeout(node))
        {
            if (genie_time_timer.genie_time_event_cb)
            {
                genie_time_timer.genie_time_event_cb(GT_TIMEOUT, node->index, &node->attr_data);
            }

            GENIE_TIME_LOCK;

            if (!node->periodic)
            {
                node->unixtime_match = 0xffffffff;
                node->state = TIMER_INVAILD;
                sys_slist_find_and_remove(&genie_time_timer.timer_list_active, &node->next);
                sys_slist_append(&genie_time_timer.timer_list_idle, &node->next);
            }
            else
            {
                node->unixtime_match += 24 * HOUR;
            }

            GENIE_TIME_UNLOCK;
            genie_time_save();
        }
    }

    if (genie_time_timer.unix_time_sync_match && genie_time_timer.unix_time_sync_match <= genie_time_timer.unix_time)
    {
        if (genie_time_timer.genie_time_event_cb)
        {
            int ret = genie_time_timer.genie_time_event_cb(GT_TIMING_SYNC, 0, NULL);

            if (ret)
            {
                if (genie_time_timer.unix_time_sync_retry_times > 0)
                {
                    genie_time_timer.unix_time_sync_match += genie_time_data.timing_sync_config.retry_delay * MINU;
                    genie_time_timer.unix_time_sync_retry_times--;
                    return;
                }
            }
        }

        genie_time_timer.unix_time_sync_retry_times = genie_time_data.timing_sync_config.retry_times;
        genie_time_timer.unix_time_sync_match = genie_time_timer.unix_time + genie_time_data.timing_sync_config.period_time * MINU;
    }
}

static void timer_update(void *args)
{
    if (!genie_time_timer.update)
    {
        return;
    }

    genie_time_timer.unix_time += 1;
    seconds_update();

    k_work_submit(&genie_time_timer.work);

    if (genie_time_timer.unix_time % 60 == 0)
    {
        GENIE_LOG_DBG("timer_update %d\n", genie_time_timer.unix_time);
    }
}

int genie_time_utc_start(uint8_t index, utc_time_t utc_time, vendor_attr_data_t *attr_data)
{
    GENIE_LOG_DBG("utc timer start index %d on_off %d %4d/%2d/%2d %2d:%2d:%d weekday %2d %04d\n",
                  index, attr_data->para,
                  utc_time.year, utc_time.month + 1, utc_time.day,
                  utc_time.hour, utc_time.minutes, utc_time.seconds,
                  utc_time.weekday, genie_time_data.timezone);

    return genie_time_start(index, UTC2unix(&utc_time), attr_data);
}

void genie_time_local_time_show()
{
    GENIE_LOG_DBG("%4d/%2d/%2d %2d:%2d:%d weekday %2d %04d\n",
                  local_time.year, local_time.month + 1, local_time.day,
                  local_time.hour, local_time.minutes, local_time.seconds,
                  local_time.weekday, genie_time_data.timezone);
}

static int genie_time_timezone_update(int8_t timezone)
{
    GENIE_LOG_DBG("timezone update %d\n", timezone);

    if (timezone < -12 || timezone > 12)
    {
        return -GT_E_PARAM;
    }

    if (!genie_time_timer.init)
    {
        return -GT_E_INIT;
    }

    genie_time_data.timezone = timezone;

    return 0;
}

static int genie_time_time_sync_set(uint16_t period_time, uint8_t retry_delay, uint8_t retry_times)
{
    GENIE_LOG_DBG("timing sync set period_time %d retry_delay %d retry_times %d\n",
                  period_time, retry_delay, retry_times);

    if (!period_time || !retry_delay || !retry_times)
    {
        return -GT_E_PARAM;
    }

    genie_time_data.timing_sync_config.period_time = period_time;
    genie_time_data.timing_sync_config.retry_delay = retry_delay;
    genie_time_data.timing_sync_config.retry_times = retry_times;

    genie_time_timer.unix_time_sync_match = genie_time_timer.unix_time + genie_time_data.timing_sync_config.period_time * MINU;
    genie_time_timer.unix_time_sync_retry_times = retry_times;

    return 0;
}

static int genie_time_local_time_update(uint32_t unix_time)
{
    if (!genie_time_timer.init)
    {
        return -GT_E_INIT;
    }

    genie_time_timer.update = 1;
    genie_time_timer.unix_time = unix_time;

    local_time = unix2UTC(unix_time + genie_time_data.timezone * HOUR);

    GENIE_LOG_DBG("unix_time %d\n", unix_time);
    GENIE_LOG_DBG("localtime update %4d/%2d/%2d %2d:%2d:%d weekday %2d\n",
                  local_time.year, local_time.month + 1, local_time.day,
                  local_time.hour, local_time.minutes, local_time.seconds,
                  local_time.weekday);
    GENIE_LOG_DBG("unix_time revert %d\n", UTC2unix(&local_time));

    genie_time_time_sync_set(DEF_SYNC_PERIOD, DEF_SYNC_DELAY, DEF_SYNC_DELAY_RETRY);

    return 0;
}

#ifdef GENIE_TIME_UPDATE_WHEN_BOOTUP
static int genie_time_update(void)
{
    if (bt_mesh_is_provisioned())
    {
        genie_time_timer.genie_time_event_cb(GT_TIMING_SYNC, 0, NULL);
        return 0;
    }

    return -1;
}
#endif

static void genie_time_request_utc_time()
{
    uint8_t payload[2] = {0};
    genie_transport_payload_param_t transport_payload_param;

    payload[0] = UNIX_TIME_T & 0xff;
    payload[1] = (UNIX_TIME_T >> 8) & 0xff;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE_TG;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = 1;

    genie_transport_send_payload(&transport_payload_param);
}

int genie_time_handle_model_mesg(genie_transport_model_param_t *p_msg)
{
    u16_t attr_type = 0;
    uint8_t *p_data = NULL;
    uint16_t remain_len;
    uint8_t is_time_mesg = 1;

    if (!p_msg || !p_msg->data || p_msg->len == 0)
    {
        return -1;
    }

    p_data = p_msg->data;
    remain_len = p_msg->len;

    switch (p_msg->opid)
    {
    case VENDOR_OP_ATTR_GET_STATUS:
    {
        attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        remain_len -= 2;

        if (attr_type == UNIX_TIME_T ||
            attr_type == TIMEZONE_SETTING_T ||
            attr_type == TIMING_SYNC_T)
        {
            genie_time_operate_status(p_msg->tid, attr_type);
        }
        else if (attr_type == TIMING_SETTING_T)
        {
            genie_time_timing_settting_event(VENDOR_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
        }
        else if (attr_type == TIMING_PERIODIC_SETTING_T)
        {
            genie_time_priordic_timing_settting_event(VENDOR_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
        }
        else
        {
            is_time_mesg = 0;
        }
    }
    break;

    case VENDOR_OP_ATTR_SET_ACK:
    {
        attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        remain_len -= 2;

        if (attr_type == UNIX_TIME_T)
        {
            if (remain_len < 4) {
                return -1;
            }
            uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
            p_data += 4;
            remain_len -= 4;
            genie_time_local_time_update(unix_time);
            genie_time_operate_status(p_msg->tid, attr_type);
        }
        else if (attr_type == TIMEZONE_SETTING_T)
        {
            if (remain_len < 1) {
                return -1;
            }
            int8_t timezone = *p_data++;
            remain_len -= 1;
            genie_time_timezone_update(timezone);
            genie_time_operate_status(p_msg->tid, attr_type);
        }
        else if (attr_type == TIMING_SYNC_T)
        {
            if (remain_len < 4) {
                return -1;
            }
            u16_t period_time = (p_data[0]) | (p_data[1] << 8);
            p_data += 2;
            u8_t retry_delay = *p_data++;
            u8_t retry_times = *p_data++;
            remain_len -= 4;
            genie_time_time_sync_set(period_time, retry_delay, retry_times);
            genie_time_operate_status(p_msg->tid, attr_type);
        }
        else if (attr_type == TIMING_SETTING_T)
        {
            genie_time_timing_settting_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        }
        else if (attr_type == TIMING_PERIODIC_SETTING_T)
        {
            genie_time_priordic_timing_settting_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        }
        else if (attr_type == TIMING_DELETE_T)
        {
            genie_time_timing_remove_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        }
        else
        {
            is_time_mesg = 0;
        }
    }
    break;

    case VENDOR_OP_ATTR_CONFIME_TG:
    {
        attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        remain_len -= 2;

        if (attr_type == UNIX_TIME_T)
        {
            if (remain_len < 4) {
                return -1;
            }
            uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
            genie_time_local_time_update(unix_time);
        }
        else
        {
            is_time_mesg = 0;
        }
    }
    break;

    default:
    {
        is_time_mesg = 0;
    }
    break;
    }

    return is_time_mesg;
}

static int genie_time_event_callback(uint8_t event, uint8_t index, vendor_attr_data_t *data)
{
    if (event == GT_TIMEOUT)
    {
        genie_time_local_time_show();
        printf("timer index %d timeout\n", index);
        genie_event(GENIE_EVT_TIMEOUT, data);
        _genie_time_timeout_indicate(index);
    }
    else if (event == GT_TIMING_SYNC)
    {
        genie_time_request_utc_time();
    }

    return 0;
}

int genie_time_init(void)
{
    int i;

    if (genie_time_timer.init)
    {
        return 0;
    }

    memset(&genie_time_timer, 0, sizeof(genie_time_timer));
    memset(&local_time, 0, sizeof(local_time));

    genie_time_timer.genie_time_event_cb = genie_time_event_callback;

    sys_slist_init(&genie_time_timer.timer_list_active);
    sys_slist_init(&genie_time_timer.timer_list_idle);

    k_sem_init(&genie_time_timer.lock, 1, 1);

    k_work_init(&genie_time_timer.work, genie_time_do_work);
    genie_time_timer.timer.port = GENIE_TIME_HW_TEIMER_ID;
    genie_time_timer.timer.config.period = GENIE_TIME_HW_TEIMER_PERIOD;
    genie_time_timer.timer.config.reload_mode = TIMER_RELOAD_AUTO;
    genie_time_timer.timer.config.cb = timer_update;
    genie_time_timer.timer.config.arg = NULL;
    hal_timer_init(&genie_time_timer.timer);
    hal_timer_start(&genie_time_timer.timer);

    genie_time_timer.init = 1;

    if (genie_time_restore())
    {
        memset(&genie_time_data, 0, sizeof(genie_time_data));
        genie_time_data.timezone = 8;
        genie_time_data.magic = 0xABCD;

        for (i = 0; i < GENIE_TIME_MAX; i++)
        {
            genie_time_data.timer_data[i].unixtime_match = 0xffffffff;
            genie_time_data.timer_data[i].index = 0xFF;
            genie_time_data.timer_data[i].state = TIMER_INVAILD;
            sys_slist_append(&genie_time_timer.timer_list_idle, &genie_time_data.timer_data[i].next);
        }
    }

#ifdef GENIE_TIME_UPDATE_WHEN_BOOTUP
    genie_time_update(); //update UTC time from cloud
#endif

    //GENIE_LOG_INFO("genie time init");

    return 0;
}

int genie_time_finalize(void)
{
    int ret;
    int i = 0;

    if (!genie_time_timer.init)
    {
        return -1;
    }

    for (i = 0; i < GENIE_TIME_MAX; i++)
    {
        genie_time_stop(i);
    }

    hal_timer_stop(&genie_time_timer.timer);
    /*
    ret = hal_timer_finalize(&genie_time_timer.timer);

    if (ret)
    {
        printf("timer finalize faild\r\n");
        return ret;
    }*/

    k_sem_delete(&genie_time_timer.lock);
    (void)memset(&genie_time_timer, 0, offsetof(genie_time_timer_t, work));
    memset(&local_time, 0, sizeof(local_time));

    ret = genie_time_erase();

    if (ret)
    {
        printf("timer erase faild\r\n");
    }

    return ret;
}
