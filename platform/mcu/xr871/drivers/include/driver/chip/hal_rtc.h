/**
  * @file  hal_rtc.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_RTC_H_
#define _DRIVER_CHIP_HAL_RTC_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RTC register block structure
 */
typedef struct {
    __IO uint32_t CTRL;                 /* offset: 0x00, RTC control register */
         uint32_t RESERVED0[3];
    __IO uint32_t YYMMDD;               /* offset: 0x10, RTC YYMMDD Register */
    __IO uint32_t DDHHMMSS;             /* offset: 0x14, RTC HHMMSS register */
         uint32_t RESERVED1[2];
    __IO uint32_t SEC_ALARM_LOAD_VAL;   /* offset: 0x20, RTC second alarm load/interval value register */
    __IO uint32_t SEC_ALARM_CUR_VAL;    /* offset: 0x24, RTC second alarm current value register */
    __IO uint32_t SEC_ALARM_EN;         /* offset: 0x28, RTC second alarm enable register */
    __IO uint32_t SEC_ALARM_IRQ_EN;     /* offset: 0x2C, RTC second alarm IRQ enable register */
    __IO uint32_t SEC_ALARM_IRQ_STATUS; /* offset: 0x30, RTC second alarm IRQ status register */
         uint32_t RESERVED2[3];
    __IO uint32_t WDAY_ALARM_HHMMSS;    /* offset: 0x40, RTC week day alarm HHMMSS register */
    __IO uint32_t WDAY_ALARM_WDAY_EN;   /* offset: 0x44, RTC week day alarm enable register */
    __IO uint32_t WDAY_ALARM_IRQ_EN;    /* offset: 0x48, RTC week day alarm IRQ enable register */
    __IO uint32_t WDAY_ALARM_IRQ_STATUS;/* offset: 0x4C, RTC week day alarm IRQ status register */
         uint32_t RESERVED3[4];
    __IO uint32_t FREERUN_CNT_L;        /* offset: 0x60, Free running counter low register */
    __IO uint32_t FREERUN_CNT_H;        /* offset: 0x64, Free running counter high register */
} RTC_T;

#define RTC ((RTC_T *)RTC_BASE)         /* address: 0x40041800*/

/*
 * Bit field definition of RTC->CTRL
 */
#define RTC_TEST_MODE_BIT                   HAL_BIT(31)
#define RTC_SIMULATION_BIT                  HAL_BIT(30)
#define RTC_WDAY_ALARM_HHMMSS_ACCESS_BIT    HAL_BIT(2)
#define RTC_DDHHMMSS_ACCESS_BIT             HAL_BIT(1)
#define RTC_YYMMDD_ACCESS_BIT               HAL_BIT(0)

/*
 * Bit field definition of RTC->YYMMDD
 */
#define RTC_LEAP_YEAR_BIT   HAL_BIT(24)

#define RTC_YEAR_SHIFT      16  /* R/W, [0, 255] */
#define RTC_YEAR_VMASK      0xFF
#define RTC_YEAR_MIN        0
#define RTC_YEAR_MAX        255

#define RTC_MONTH_SHIFT     8   /* R/W, [1, 12] */
#define RTC_MONTH_VMASK     0xF
#define RTC_MONTH_MIN       1
#define RTC_MONTH_MAX       12

#define RTC_MDAY_SHIFT      0   /* R/W, [1, 31] */
#define RTC_MDAY_VMASK      0x1F
#define RTC_MDAY_MIN        1
#define RTC_MDAY_MAX        31

/*
 * Bit field definition of RTC->DDHHMMSS
 */
#define RTC_WDAY_SHIFT      29  /* R/W */
#define RTC_WDAY_VMASK      0x7
typedef enum {
    RTC_WDAY_MONDAY         = 0U,
    RTC_WDAY_TUESDAY        = 1U,
    RTC_WDAY_WEDNESDAY      = 2U,
    RTC_WDAY_THURSDAY       = 3U,
    RTC_WDAY_FRIDAY         = 4U,
    RTC_WDAY_SATURDAY       = 5U,
    RTC_WDAY_SUNDAY         = 6U
} RTC_WeekDay;

#define RTC_HOUR_SHIFT      16  /* R/W, [0, 23] */
#define RTC_HOUR_VMASK      0x1F
#define RTC_HOUR_MIN        0
#define RTC_HOUR_MAX        23

#define RTC_MINUTE_SHIFT    8   /* R/W, [0, 59] */
#define RTC_MINUTE_VMASK    0x3F
#define RTC_MINUTE_MIN      0
#define RTC_MINUTE_MAX      59

#define RTC_SECOND_SHIFT    0   /* R/W, [0, 59] */
#define RTC_SECOND_VMASK    0x3F
#define RTC_SECOND_MIN      0
#define RTC_SECOND_MAX      59

/* RTC->SEC_ALARM_LOAD_VAL */

/* RTC->SEC_ALARM_CUR_VAL */

/* RTC->SEC_ALARM_EN */
#define RTC_SEC_ALARM_EN_BIT            HAL_BIT(0)

/* RTC->SEC_ALARM_IRQ_EN */
#define RTC_SEC_ALARM_IRQ_EN_BIT        HAL_BIT(0)

/* RTC->SEC_ALARM_IRQ_STATUS */
#define RTC_SEC_ALARM_IRQ_PENDING_BIT   HAL_BIT(0)

/* RTC->WDAY_ALARM_DDHHMMSS */
#define RTC_WDAY_ALARM_HOUR_SHIFT       16  /* R/W, [0, 23] */
#define RTC_WDAY_ALARM_HOUR_VMASK       0x1F

#define RTC_WDAY_ALARM_MINUTE_SHIFT     8   /* R/W, [0, 59] */
#define RTC_WDAY_ALARM_MINUTE_VMASK     0x3F

#define RTC_WDAY_ALARM_SECOND_SHIFT     0   /* R/W, [0, 59] */
#define RTC_WDAY_ALARM_SECOND_VMASK     0x3F

/* RTC->WDAY_ALARM_EN */
#define RTC_WDAY_ALARM_EN_BIT(wday)     HAL_BIT(wday)   /* day is RTC_WeekDay */
#define RTC_WDAY_ALARM_EN_MASK          0x7F

/* RTC->WDAY_ALARM_IRQ_EN */
#define RTC_WDAY_ALARM_IRQ_EN_BIT       HAL_BIT(0)

/* RTC->WDAY_ALARM_IRQ_STATUS */
#define RTC_WDAY_ALARM_IRQ_PENDING_BIT  HAL_BIT(0)

/******************************************************************************/

/** @brief Type define of RTC alarm IRQ callback function */
typedef void (*RTC_AlarmIRQCallback) (void *arg);

/**
 * @brief RTC second alarm starting parameters
 */
typedef struct {
    uint32_t                alarmSeconds;   /* RTC second alarm's count down value */
    RTC_AlarmIRQCallback    callback;       /* RTC second alarm IRQ callback function */
    void                   *arg;            /* Argument of RTC second alarm IRQ callback function */
} RTC_SecAlarmStartParam;

/**
 * @brief RTC weekday alarm starting parameters
 */
typedef struct {
    uint8_t                 alarmHour;      /* RTC weekday alarm's hour, [0, 23] */
    uint8_t                 alarmMinute;    /* RTC weekday alarm's minute, [0, 59] */
    uint8_t                 alarmSecond;    /* RTC weekday alarm's second, [0, 59] */
    uint8_t                 alarmWDayMask;  /* RTC weekday alarm's weekday, bit mask of RTC_WDAY_ALARM_EN_BIT(RTC_WeekDay) */
    RTC_AlarmIRQCallback    callback;       /* RTC weekday alarm IRQ callback function */
    void                   *arg;            /* Argument of RTC weekday alarm IRQ callback function */
} RTC_WDayAlarmStartParam;

void HAL_RTC_SetYYMMDD(uint8_t isLeapYear, uint8_t year, uint8_t month, uint8_t mday);
void HAL_RTC_SetDDHHMMSS(RTC_WeekDay wday, uint8_t hour, uint8_t minute, uint8_t second);

void HAL_RTC_GetYYMMDD(uint8_t *isLeapYear, uint8_t *year, uint8_t *month, uint8_t *mday);
void HAL_RTC_GetDDHHMMSS(RTC_WeekDay *wday, uint8_t *hour, uint8_t *minute, uint8_t *second) ;

void HAL_RTC_StartSecAlarm(const RTC_SecAlarmStartParam *param);
void HAL_RTC_StopSecAlarm(void);

void HAL_RTC_StartWDayAlarm(const RTC_WDayAlarmStartParam *param);
void HAL_RTC_StopWDayAlarm(void);

uint64_t HAL_RTC_GetFreeRunTime(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_RTC_H_ */
