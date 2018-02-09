/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     zx29_rtc.c
 * @brief    CSI Source File for RTC Driver
 * @version  V1.0
 * @date     01. November 2017
 ******************************************************************************/

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <stdbool.h>
#include <string.h>
#include "zx29_rtc.h"
#include "csi_core.h"
#include "drv_rtc.h"
#include "drv_intc.h"
#include <io.h>
#include <soc.h>

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define THIS_MODULE 		MODULE_DEV_RTC
#define RTC_DEVICE_COUNT 	CONFIG_RTC_NUM
/* version of hardware */
#define RTC_HW_VERSION   	(0x0106)
/** rtc parameter,it must config this reg before write to anyother reg */
#define CONFIG_PARAMETER 	(0xc1cd)
/** It Must config parameter reg first,then config this reg,when get current time*/
#define CONFIG_GET_TIME 	(0x7fffffff)

#define ERR_RTC(errno) (CSI_DRV_ERRNO_RTC_BASE | errno)
#define RTC_NULL_PARAM_CHK(para)                    \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_RTC(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
typedef struct {
    uint8_t    inited;       /* 0: not inited, 1: inited */
    uint8_t    opened;       /* 0: closed, 1: opened */
    uint8_t    issettime;    /* 0: not set, 1: set */
    uint8_t    timer_active; /* 0: not active, 1: active */
    uint32_t   issetalarm;   /* 0: not set, 1: set */
    rtc_event_cb_t 	alarm_cb;     /* alarm callback */
    void       *alarm_arg;   /* alarm arg */
    rtc_event_cb_t	timer_cb;     /* timer callback */
    void       *timer_arg;   /* timer arg */
    uint32_t   addr;         /* the base-address of rtc */
} zx29_rtc_priv_t;

/**
 * @brief  stamp polar register
 *
 * @param :  stamp event polarity
 **/
enum rtc_enable {
        RTC_DISABLE = 0x0,
        RTC_ENABLE  = 0x1,
};

/**
* @brief rtc timer int mode
*
* @param :      Timer interrupt mode
**/
enum rtc_every {
    RTC_SEC_INT  = 0x0,    /* SEC INT */
    RTC_MIN_INT  = 0x1,    /* MIN INT */
    RTC_HOUR_INT = 0x2,    /* HOUR INT*/
    RTC_DAY_INT  = 0x3     /* DAY INT */
};

/**
 * @brief  stamp event
 *
 * @param :  Stamp sexternal event
 **/
enum stamp_event {
    EXTERNAL_EVENT_1 = 0x000,           /** external event 1 */
    EXTERNAL_EVENT_2 = 0x001,   /** external event 2 */
        EXTERNAL_EVENT_3 = 0x010,               /** external event 3 */
        EXTERNAL_EVENT_4 = 0x011,               /** external event 4 */
        EXTERNAL_EVENT_5 = 0x100,               /** external event 5 */
        EXTERNAL_EVENT_6 = 0x101,               /** external event 6 */
        EXTERNAL_EVENT_7 = 0x110,               /** external event 7 */
        EXTERNAL_EVENT_8 = 0x111,               /** external event 8 */
};

/**
 * @brief  stamp event polarity
 *
 * @param :  stamp event polarity
 **/
enum stamp_event_polarity {
        POSEDGE_EDGE = 0x00,
        NEGEDGE_EDGE = 0x01,
        DUAL_EDGE    = 0x10,
};


/**
 * @brief  stamp id
 *
 * @param :  stamp event polarity
 **/
enum stamp_num {
        TIMSTAMP0 = 0x0,
        TIMSTAMP1 = 0x1,
};

/**
 * @brief  stamp polar register
 *
 * @param :  stamp event polarity
 **/
enum stamp_reg {
        POS_REG = 0x0,
        NEG_REG = 0x1,
};

/*******************************************************************************
 *                        Local function declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                         Local variable definitions                          *
 ******************************************************************************/

/*******************************************************************************
 *                        Global variable definitions                          *
 ******************************************************************************/
static uint32_t rtc_si_int_cnt_timer;
static uint32_t rtc_si_int_cnt_alarm;
static uint32_t rtc_si_int_cnt_stamp0;
static uint32_t rtc_si_int_cnt_stamp1;
static zx29_rtc_priv_t s_device_info;
volatile zx29_rtc_reg_t *rtc_si_reg_base = (volatile zx29_rtc_reg_t *)SI_RTC_BASE;
static const rtc_capabilities_t rtc_capabilities = {
    .interrupt_mode = 1,    /* supports Interrupt mode */
    .wrap_mode = 0          /* supports wrap mode */
};

/*******************************************************************************
 *                      Inline function implementations                        *
 ******************************************************************************/

/*******************************************************************************
 *                      Local function implementations                         *
 ******************************************************************************/
/**
 * Convert bcd2 to binary.
 *
 * @param[in]   value Value for convert
 * @return      Value of convert result.
 */
static uint32_t rtc_conv_bcd2_to_bin(uint32_t value)
{
    return(10 * (value >> 4) + (value & 0x0f));
}


/**
 * Convert binary to bcd2.
 *
 * @param[in]   value Value for convert
 * @return      Value of convert result.
 */
static uint32_t rtc_conv_bin_to_bcd2(uint32_t value)
{
    return((value % 10) | ((value / 10) << 4));
}


/**
 * Convert bcd4 to_binary.
 *
 * @param[in]   value Value for convert
 * @return      Value of convert result.
 */
static uint32_t rtc_conv_bcd4_to_bin(uint32_t value)
{
    uint32_t temp_val = 0;
    int8_t i;
    for (i = 3; i >= 0; i--) {
        temp_val = (temp_val * 10) + ((value >> (4 * i)) & 0x0f);
    }

    return (temp_val);
}


/**
 * Convert binary to bcd4.
 *
 * @param[in]   value Value for convert
 * @return      Value of convert result.
 */
static uint32_t rtc_conv_bin_to_bcd4(uint32_t value)
{
    uint32_t bcd4 = 0;
    uint8_t i;
    for (i = 0; i < 4; i++) {
        bcd4 = (bcd4) | (( value % 10) << 4 * i);
        value = value / 10;
    }

    return (bcd4);
}

/**
 * Wait not busy of rtc,when busy bit is 1,don't do anything.
 *
 * @return      none
 */
static void rtc_wait_not_busy(void)
{
    uint32_t timeout = 0;
    while (HAL_IS_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_BUSY_SHIFT)) {
        if (timeout++ > 2000) {
            break;
        }
    }
}


/**
 * Get timestamp0 date and time of positive.
 *
 * @param[in]  rtctime The point of time.
 * @return    none
 */
static void rtc_get_stamp0_datetime_pos(struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t rd_num;
    for (rd_num = 0; rd_num < 2; rd_num++) {
        rtc_wait_not_busy();
        sec_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_pos, TIMSTAP0_TIME_POS_SEC);

        rtc_wait_not_busy();
        min_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_pos, TIMSTAP0_TIME_POS_MIN);

        rtc_wait_not_busy();
        hour_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_pos, TIMSTAP0_TIME_POS_HOUR);

        rtc_wait_not_busy();
        day_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_pos, TIMSTAP0_DAY_POS_DAY);

        rtc_wait_not_busy();
        mon_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_pos, TIMSTAP0_DAY_POS_MONTH);

        rtc_wait_not_busy();
        year_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_pos, TIMSTAP0_DAY_POS_YEAR);

        rtc_wait_not_busy();
        wday_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_pos, TIMSTAP0_DAY_POS_WEEK);
    }

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);
    rtctime->tm_wday = rtc_conv_bcd2_to_bin(wday_temp);
}


/**
 * Get timestamp0 date and time of negative.
 *
 * @param[in]	rtctime The point of time.
 * @return		none
 */
static void rtc_get_stamp0_datetime_neg(struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t rd_num;
    for (rd_num = 0; rd_num < 2; rd_num++) {
        rtc_wait_not_busy();
        sec_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_neg, TIMSTAP0_TIME_NEG_SEC);

        rtc_wait_not_busy();
        min_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_neg, TIMSTAP0_TIME_NEG_MIN);

        rtc_wait_not_busy();
        hour_temp = HAL_FEXT(rtc_si_reg_base->stap0_time_neg, TIMSTAP0_TIME_NEG_HOUR);

        rtc_wait_not_busy();
        day_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_neg, TIMSTAP0_DAY_NEG_DAY);

        rtc_wait_not_busy();
        mon_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_neg, TIMSTAP0_DAY_NEG_MONTH);

        rtc_wait_not_busy();
        year_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_neg, TIMSTAP0_DAY_NEG_YEAR);

        rtc_wait_not_busy();
        wday_temp = HAL_FEXT(rtc_si_reg_base->stap0_day_neg, TIMSTAP0_DAY_NEG_WEEK);
    }

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);
    rtctime->tm_wday = rtc_conv_bcd2_to_bin(wday_temp);
}


/**
 * Get timestamp1 date and time of positive.
 *
 * @param[in]	rtctime The point of time.
 * @return		none
 */
static void rtc_get_stamp1_datetime_pos(struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t rd_num;
    for (rd_num = 0; rd_num < 2; rd_num++) {
        rtc_wait_not_busy();
        sec_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_pos, TIMSTAP1_TIME_POS_SEC);

        rtc_wait_not_busy();
        min_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_pos, TIMSTAP1_TIME_POS_MIN);

        rtc_wait_not_busy();
        hour_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_pos, TIMSTAP1_TIME_POS_HOUR);

        rtc_wait_not_busy();
        day_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_pos, TIMSTAP1_DAY_POS_DAY);

        rtc_wait_not_busy();
        mon_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_pos, TIMSTAP1_DAY_POS_MONTH);

        rtc_wait_not_busy();
        year_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_pos, TIMSTAP1_DAY_POS_YEAR);

        rtc_wait_not_busy();
        wday_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_pos, TIMSTAP1_DAY_POS_WEEK);
    }

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);
    rtctime->tm_wday = rtc_conv_bcd2_to_bin(wday_temp);
}


/**
 * Get timestamp1 date and time of negative.
 *
 * @param[in]  rtctime The point of time.
 * @return    none
 */
static void rtc_get_stamp1_datetime_neg(struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t rd_num;
    for (rd_num = 0; rd_num < 2; rd_num++) {
        rtc_wait_not_busy();
        sec_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_neg, TIMSTAP1_TIME_NEG_SEC);

        rtc_wait_not_busy();
        min_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_neg, TIMSTAP1_TIME_NEG_MIN);

        rtc_wait_not_busy();
        hour_temp = HAL_FEXT(rtc_si_reg_base->stap1_time_neg, TIMSTAP1_TIME_NEG_HOUR);

        rtc_wait_not_busy();
        day_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_neg, TIMSTAP1_DAY_NEG_DAY);

        rtc_wait_not_busy();
        mon_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_neg, TIMSTAP1_DAY_NEG_MONTH);

        rtc_wait_not_busy();
        year_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_neg, TIMSTAP1_DAY_NEG_YEAR);

        rtc_wait_not_busy();
        wday_temp = HAL_FEXT(rtc_si_reg_base->stap1_day_neg, TIMSTAP1_DAY_NEG_WEEK);
    }

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);
    rtctime->tm_wday = rtc_conv_bcd2_to_bin(wday_temp);
}

/**
 * Enable or disable timestap1 interrupt.
 *
 * @param[in]	enable	FLAG of enable or disable.
 * @return    none
 */
static void rtc_stamp1_int_onoff(enum rtc_enable enable)
{
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_INSR(rtc_si_reg_base->rtc_int, RTC_INT_TIMSTAP1_SHIFT,enable);
}


/**
 * Enable or disable timestap0 interrupt.
 *
 * @param[in]  enable FLAG of enable or disable.
 * @return    none
 */
static void rtc_stamp0_int_onoff(enum rtc_enable enable)
{
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_INSR(rtc_si_reg_base->rtc_int, RTC_INT_TIMSTAP0_SHIFT,enable);
}

/**
 * Set rtc time.
 *
 * @param[in]	sec    Second of time.
 * @param[in]	min    Minutes of time.
 * @param[in]	hour   Hour of time.
 * @param[in]	mode12 12 or 24 mode of time.
 * @param[in]	ampm   AM or PM of time.
 * @return    none
 */
static void rtc_set_time(uint32_t sec, uint32_t min, uint32_t hour, uint32_t mode12, uint32_t ampm)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;

    sec_temp = rtc_conv_bin_to_bcd2(sec);
    min_temp = rtc_conv_bin_to_bcd2(min);

     /* config time mod */
    if (hour > 12 || mode12 == MODEL_24) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_CLR(rtc_si_reg_base->rtc_control, RTC_CONTROL_MODEL12_24_SHIFT);
        hour_temp = rtc_conv_bin_to_bcd2(hour);
    } else {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_SET(rtc_si_reg_base->rtc_control, RTC_CONTROL_MODEL12_24_SHIFT);
        hour_temp = rtc_conv_bin_to_bcd2(hour) | (ampm << RTC_HOUR_AM_PM_SHIFT);
    }

    /* set time hour */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->hour = hour_temp;

    /* set time minute */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->min = min_temp;

    /* set time second */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->sec = sec_temp;
}

/**
 * Enable or disable rtc.
 *
 * @param[in]  enable FLAG of enable or disable.
 * @return    none
 */
static void rtc_onoff(enum rtc_enable enable)
{
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_INSR(rtc_si_reg_base->rtc_control, RTC_CONTROL_STOP_SHIFT, enable);
}


/**
 * Select the type of timer interrupt.
 *
 * @param[in]  rtc_every 0->second  1->minute  2->hour 3->day
 * @return    none
 */
static void rtc_nt_sel(enum rtc_every rtc_every)
{
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_int, RTC_INT_EVERY, rtc_every);
}


/**
 * Isr of stamp interrupt
 *
 * @return    none
 */
void zx29_rtc_stamp_handler(void)
{
    /*Free stamp interrupt status*/
    if (HAL_IS_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_TIMSTAP0_SHIFT)) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_TIMSTAP0_SHIFT);
        rtc_si_int_cnt_stamp0++;
    } else if (HAL_IS_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_TIMSTAP1_SHIFT)) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_TIMSTAP1_SHIFT);
        rtc_si_int_cnt_stamp1++;
    }

    /* clear pcu */
    csi_intc_ack_irq(PIC_IRQ_RTC_TIMESTAMP);
}

/**
 * Isr of timer interrupt
 *
 * @return    none
 */
void zx29_rtc_timer_handler(void)
{
    rtc_si_int_cnt_timer++;
    /* Clear interrupt flag */
    /*free int of timer*/
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_TIMER_SHIFT);
    rtc_wait_not_busy();

    /*clear pcu*/
    csi_intc_ack_irq(PIC_IRQ_RTC_TIMER);

    /* The isr of rtc */
    if (s_device_info.timer_cb) {
//        s_device_info.timer_cb(s_device_info.timer_arg);
        s_device_info.timer_cb(NULL, NULL);
    }
}

/**
 * Isr of alarm interrupt
 *
 * @return    none
 */
void zx29_rtc_alarm_handler(void)
{
    rtc_si_int_cnt_alarm++;
    s_device_info.issetalarm = 0;
    /* Clear interrupt flag */
    /*free int of alarm*/
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_ALARM_SHIFT);
    rtc_wait_not_busy();

    /*clear pcu*/
    csi_intc_ack_irq(PIC_IRQ_RTC_ALARM);

    /* The isr of rtc */
    if (s_device_info.alarm_cb) {
//        s_device_info.alarm_cb(s_device_info.alarm_arg);
        s_device_info.alarm_cb(NULL, NULL);
    }
}

int32_t __attribute__((weak)) target_get_rtc_count(void)
{
    return 0;
}

int32_t __attribute__((weak)) target_get_rtc(uint32_t idx, uint32_t *base, uint32_t *irq)
{
    return NULL;
}

/*******************************************************************************
 *                      Global function impleme                         	   *
 ******************************************************************************/
/**
 * Get timestamp date and time.
 *
 * @param[in]	stamp_num	The stamp num to read.
 * @param[in]	stamp_reg	The polarity of register to read.
 * @param[in]	rtctime		The point of time.
 * @return		none
 */
int zx29_rtc_read_stamp(enum stamp_num stamp_num, enum stamp_reg stamp_reg, struct tm *rtctime)
{
    if (stamp_num == TIMSTAMP0) {
        if (stamp_reg == POS_REG) {
            rtc_get_stamp0_datetime_pos(rtctime);
        } else if (stamp_reg == NEG_REG) {
            rtc_get_stamp0_datetime_neg(rtctime);
        }
    } else if (stamp_num == TIMSTAMP1) {
        if (stamp_reg == POS_REG) {
            rtc_get_stamp1_datetime_pos(rtctime);
        } else if (stamp_reg == NEG_REG) {
            rtc_get_stamp1_datetime_neg(rtctime);
        }
    }

    return 0;
}
/**
 * Config the stamp.
 *
 * @param[in]	stamp_num	The number stamp to set.
 * @param[in]	stamp_pol	Varible of stamp polarity.
 * @param[in]	stamp_event	Varible of stamp event.
 * @return		Zero on success; a negated errno value on failure.
 */
int zx29_rtc_set_stamp(enum stamp_num stamp_num, enum stamp_event_polarity stamp_pol, enum stamp_event stamp_event)
{
    if (stamp_num == TIMSTAMP0) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_FINS(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP0_POL, stamp_pol);

        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_FINS(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP0_SEL, stamp_event);
    } else if (stamp_num == TIMSTAMP1) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_FINS(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP1_POL, stamp_pol);

        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_FINS(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP1_SEL, stamp_event);
    }

    return 0;
}


/**
 * Enable or disable timestap.
 *
 * @param[in]	stamp_num	The number stamp to set.
 * @param[in]	enable	FLAG of enable or disable.
 * @return		Zero on success; a negated errno value on failure.
 */
int zx29_rtc_set_stamp_enable(enum stamp_num stamp_num, enum rtc_enable enable)
{
    if (stamp_num == TIMSTAMP0) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_INSR(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP0_EN_SHIFT, enable);
        rtc_stamp0_int_onoff(enable);
    } else if (stamp_num == TIMSTAMP1) {
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_INSR(rtc_si_reg_base->stap_control, STAMP_CONTROL_TIMSTAP1_EN_SHIFT, enable);
        rtc_stamp1_int_onoff(enable);
    }

    return 0;
}

/**
 * Stop the rtc timer intterupt.
 *
 * @param[in]   id         The device index. (Should be less than the device count)
 * @param[in]   timer_id   The timer id.
 * @return      Zero on success; a negated errno value on failure.
 */
static int zx29_rtc_stoptimer(void)
{
    /*disable timer interrupt*/;
    while (! HAL_IS_BIT_CLR(rtc_si_reg_base->rtc_int, RTC_INT_ITTIMER_SHIFT)) {
        rtc_wait_not_busy();
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_CLR(rtc_si_reg_base->rtc_int, RTC_INT_ITTIMER_SHIFT);
    }

    s_device_info.timer_cb = NULL;
    s_device_info.timer_arg = NULL;
    s_device_info.timer_active = 0;

    return 0;
}

/**
 * Set rtc date.
 *
 * @param[in]   day    day of date.
 * @param[in]   mon    Month of date.
 * @param[in]   year   Year of date.
 * @param[in]   week   weekday of date.
 * @return      none
 */
static void rtc_set_date(uint32_t day, uint32_t mon, uint32_t year, uint32_t week)
{
    day = rtc_conv_bin_to_bcd2(day);
    mon = rtc_conv_bin_to_bcd2(mon);
    year = rtc_conv_bin_to_bcd4(year);
    week = rtc_conv_bin_to_bcd2(week);

    /* set time day */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->day = day;

    /* set time month */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->month = mon;

    /* set time year */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->year = year;

    /* set time week */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->week = week;
}

/**
 * Set alarm and register callback function.
 *
 * @param[in]   rtctime    The time of alarm.
 * @param[in]   callback   Callback function for register.
 * @param[in]   arg        Argument of Callback.
 * @return      Zero on success; a negated errno value on failure.
 */
int zx29_rtc_startalarm(struct tm *rtctime, rtc_event_cb_t callback, void *arg)
{
    /*set alarm time*/
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;

    sec_temp = rtc_conv_bin_to_bcd2(rtctime->tm_sec);
    min_temp = rtc_conv_bin_to_bcd2(rtctime->tm_min);

    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_INSR(rtc_si_reg_base->rtc_control, RTC_CONTROL_MODEL12_24_SHIFT, MODEL_24);  /*0:24h model, 1:12h model*/

    hour_temp = rtc_conv_bin_to_bcd2(rtctime->tm_hour) | (MODEL_AM << RTC_HOUR_AM_PM_SHIFT);
    day_temp  = rtc_conv_bin_to_bcd2(rtctime->tm_mday);
    mon_temp  = rtc_conv_bin_to_bcd2(rtctime->tm_mon);
    year_temp = rtc_conv_bin_to_bcd4(rtctime->tm_year);

    /*before update alarm,must set bit of RTC_CONTROL_SETALARM to 0 first*/
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_CLR(rtc_si_reg_base->rtc_control, RTC_CONTROL_SETALARM_SHIFT);

    /* set alarm day */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_day = day_temp;

    /* set alarm month */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_month = mon_temp;

    /* set alarm year */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_year = year_temp;

    /* set alarm hour */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_hour = hour_temp;

    /* set alarm min */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_min = min_temp;

    /* set alarm second */
    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    rtc_si_reg_base->alarm_sec = sec_temp;

    /*at last of update alarm,must set bit of RTC_CONTROL_SETALARM to 1*/
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_SET(rtc_si_reg_base->rtc_control, RTC_CONTROL_SETALARM_SHIFT);

    /*enable alarm interrupt*/;
    while (! HAL_IS_BIT_SET(rtc_si_reg_base->rtc_int, RTC_INT_ITALARM_SHIFT)) {
        rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_SET(rtc_si_reg_base->rtc_int, RTC_INT_ITALARM_SHIFT);
    }

    s_device_info.alarm_cb  = callback;
    s_device_info.alarm_arg  = arg;
    s_device_info.issetalarm = 1;
    return 0;
}
#if 0

/**
 * Stop alarm.
 *
 * @return      Zero on success; a negated errno value on failure.
 */
static int zx29_rtc_stopalarm(void)
{
    /*disable alarm interrupt*/;
    while (! HAL_IS_BIT_CLR(rtc_si_reg_base->rtc_int, RTC_INT_ITALARM_SHIFT)) {
        rtc_wait_not_busy();
        HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
        HAL_BIT_CLR(rtc_si_reg_base->rtc_int, RTC_INT_ITALARM_SHIFT);
    }

    s_device_info.alarm_cb = NULL;
    s_device_info.alarm_arg = NULL;
    s_device_info.issetalarm = 0;

    return 0;
}
#endif

/**
 * Read alarm time.
 * @param[in]   rtctime Point to result of time.
 * @return Standard errno
 */
int zx29_rtc_get_alarm(struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t hourreg;

    if (!s_device_info.issetalarm) {
        return -1;
    }

    rtc_wait_not_busy();
    year_temp = rtc_si_reg_base->alarm_year;

    rtc_wait_not_busy();
    mon_temp  = rtc_si_reg_base->alarm_month;

    rtc_wait_not_busy();
    day_temp  = rtc_si_reg_base->alarm_day;

    rtc_wait_not_busy();
    hourreg = rtc_si_reg_base->alarm_hour;
    hour_temp = hourreg & (~(MODEL_PM << RTC_HOUR_AM_PM_SHIFT));

    rtc_wait_not_busy();
    min_temp = rtc_si_reg_base->alarm_min;

    rtc_wait_not_busy();
    sec_temp = rtc_si_reg_base->alarm_sec;

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);

    return 0;
}

int zx29_rtc_resume(void)
{
	return 0;
}

int yunos_bsp_rtc_suspend(void)
{
	return 0;
}

/**
  \brief       Initialize RTC Interface. 1. Initializes the resources needed for the RTC interface 2.registers event callback function
  \param[in]   idx  rtc index
  \param[in]   cb_event  Pointer to \ref rtc_event_cb_t
  \return      pointer to rtc instance
*/
rtc_handle_t csi_rtc_initialize(int32_t idx, rtc_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_RTC_NUM) {
        return NULL;
    }

    int32_t real_idx;
    uint32_t base = 0u;
    uint32_t irq;

    real_idx = target_get_rtc(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    zx29_rtc_priv_t *rtc_priv;

    s_device_info.timer_cb = cb_event;
//    s_device_info.timer_arg = arg;
    rtc_priv = &s_device_info;
    rtc_priv->addr = base;

   // zx29_rtc_reg_t *addr = (zx29_rtc_reg_t *)(rtc_priv->addr);

    /*open rtc workclk*/
    HAL_BIT_SET(*(unsigned int *)SI_CLK_REG_RTC, SI_CLK_BIT_EN_SHIFT);

    if (s_device_info.inited) {
        return 0;
    }

    rtc_onoff(RTC_ENABLE);
    /*set alarm and timer pcu trigger level to edge*/
    csi_intc_set_attribute(PIC_IRQ_RTC_TIMER,1,INT_MODE_RISING_EDGE);
    csi_intc_set_attribute(PIC_IRQ_RTC_ALARM,1,INT_MODE_RISING_EDGE);
    csi_intc_set_attribute(PIC_IRQ_RTC_TIMESTAMP,1,INT_MODE_RISING_EDGE);

//    csi_intc_enable_irq(PIC_IRQ_RTC_TIMER);
    csi_intc_enable_irq(PIC_IRQ_RTC_ALARM);
    csi_intc_enable_irq(PIC_IRQ_RTC_TIMESTAMP);

    s_device_info.inited = 1;
    return (rtc_handle_t)rtc_priv;
}

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_uninitialize(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    zx29_rtc_priv_t *rtc_priv = handle;

    rtc_priv->alarm_cb = NULL;
    rtc_priv->timer_cb = NULL;
    rtc_priv->timer_active = 0;

    csi_intc_disable_irq(PIC_IRQ_RTC_TIMER);
    csi_intc_disable_irq(PIC_IRQ_RTC_ALARM);
    csi_intc_disable_irq(PIC_IRQ_RTC_TIMESTAMP);
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx  rtc index
  \return      \ref rtc_capabilities_t
*/
rtc_capabilities_t csi_rtc_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPI_NUM) {
        rtc_capabilities_t ret;
        memset(&ret, 0, sizeof(rtc_capabilities_t));
        return ret;
    }

    return rtc_capabilities;
}

/**
  \brief       Set RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
*/
int32_t csi_rtc_set_time(rtc_handle_t handle, const struct tm *rtctime)
{
    RTC_NULL_PARAM_CHK(handle);
    RTC_NULL_PARAM_CHK(rtctime);

    /*Initiate DateTime*/
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t mode12;
    uint32_t ampm;

    sec_temp = rtctime->tm_sec;
    min_temp = rtctime->tm_min;
    hour_temp = rtctime->tm_hour;
    mode12 = MODEL_24;/*24 hour mod*/
    ampm = MODEL_AM;

    day_temp  = rtctime->tm_mday;
    mon_temp  = rtctime->tm_mon;
    year_temp = rtctime->tm_year;
    wday_temp = rtctime->tm_wday;

    rtc_set_time(sec_temp, min_temp, hour_temp, mode12, ampm);
    rtc_set_date(day_temp, mon_temp, year_temp, wday_temp);

    /*start rtc*/
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_BIT_SET(rtc_si_reg_base->rtc_control, RTC_CONTROL_STOP_SHIFT);
    s_device_info.issettime = 1;

    return 0;
}

/**
  \brief       Get RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
*/
int32_t csi_rtc_get_time(rtc_handle_t handle, struct tm *rtctime)
{
    uint32_t sec_temp;
    uint32_t min_temp;
    uint32_t hour_temp;
    uint32_t day_temp;
    uint32_t mon_temp;
    uint32_t year_temp;
    uint32_t wday_temp;
    uint32_t hourreg;

    if (!s_device_info.issettime) {
        return -1;
    }

    if (rtctime == NULL) {
        return -1;
    }

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    year_temp = rtc_si_reg_base->year;

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    mon_temp = rtc_si_reg_base->month;

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    day_temp = rtc_si_reg_base->day;

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    wday_temp = rtc_si_reg_base->week;

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    hourreg = rtc_si_reg_base->hour;
    hour_temp = hourreg & (~(MODEL_PM << RTC_HOUR_AM_PM_SHIFT));

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    min_temp = rtc_si_reg_base->min;

    rtc_wait_not_busy();
    HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
    HAL_FINS(rtc_si_reg_base->rtc_get_time, RTC_GET_TIME, CONFIG_GET_TIME);
    sec_temp = rtc_si_reg_base->sec;

    rtctime->tm_sec  = rtc_conv_bcd2_to_bin(sec_temp);
    rtctime->tm_min  = rtc_conv_bcd2_to_bin(min_temp);
    rtctime->tm_hour = rtc_conv_bcd2_to_bin(hour_temp);
    rtctime->tm_mday = rtc_conv_bcd2_to_bin(day_temp);
    rtctime->tm_mon  = rtc_conv_bcd2_to_bin(mon_temp);
    rtctime->tm_year = rtc_conv_bcd4_to_bin(year_temp);
    rtctime->tm_wday = rtc_conv_bcd2_to_bin(wday_temp);

    return 0;
}

/**
  \brief       Start RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_start(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    rtc_onoff(RTC_ENABLE);
    return 0;
}

/**
  \brief       Stop RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_stop(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    rtc_onoff(RTC_DISABLE);
    return 0;
}

/**
  \brief       Get RTC status.
  \param[in]   handle rtc handle to operate.
  \return      RTC status \ref rtc_status_t
*/
rtc_status_t csi_rtc_get_status(rtc_handle_t handle)
{
    rtc_status_t rtc_status = {0};

    if (handle == NULL) {
        return rtc_status;
    }

    if (HAL_IS_BIT_SET(rtc_si_reg_base->rtc_status, RTC_STATUS_BUSY_SHIFT)) {
        rtc_status.active = 1;
    }

    return rtc_status;
}

/**
  \brief       config RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime time to wake up
  \return      error code
*/
int32_t csi_rtc_timer_config(rtc_handle_t handle, const struct tm *rtctime)
{
    RTC_NULL_PARAM_CHK(handle);
    return 0;
}

/**
  \brief       disable or enable RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   en set 1 enable for rtc timer
  \return      error code
*/
int32_t csi_rtc_timer_enable(rtc_handle_t handle, uint8_t en)
{
    RTC_NULL_PARAM_CHK(handle);

    if (en == 1) {
        if (s_device_info.timer_active) {
            return -1;
        }

        if (!s_device_info.issettime) {
            return -1;
        }

        rtc_nt_sel(RTC_SEC_INT);

        /*enable timer interrupts*/
        while (! HAL_IS_BIT_SET(rtc_si_reg_base->rtc_int, RTC_INT_ITTIMER_SHIFT)) {
            rtc_wait_not_busy();
            HAL_FINS(rtc_si_reg_base->conf_id, RTC_CONFID, CONFIG_PARAMETER);
            HAL_BIT_SET(rtc_si_reg_base->rtc_int, RTC_INT_ITTIMER_SHIFT);
        }

        s_device_info.timer_active = 1;
        csi_intc_enable_irq(PIC_IRQ_RTC_TIMER);

        return 0;
    } else if (en == 0) {
        zx29_rtc_stoptimer();
        return 0;

    } else {
        return ERR_RTC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

