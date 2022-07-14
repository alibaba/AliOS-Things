/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef AOS_RTC_H
#define AOS_RTC_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup rtc_api RTC
 * @ingroup driver_api
 * @brief AOS API for RTC.
 * @{
 */

typedef struct {
    uint64_t year       : 22;
    uint64_t month      : 4;    /* 1 ~ 12 */
    uint64_t mday       : 5;
    uint64_t hour       : 5;
    uint64_t min        : 6;
    uint64_t sec        : 6;
    uint64_t reserved   : 16;   /* Reserved for millisecond, leap second flag, etc. */
} aos_rtc_time_t;

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

#define AOS_RTC_IOC_GET_TIME    0x5201
#define AOS_RTC_IOC_SET_TIME    0x5202

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_rtc_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get an RTC device.
 * @param   ref RTC ref to operate.
 * @param   id  RTC device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_rtc_get(aos_rtc_ref_t *ref, uint32_t id);
/**
 * @brief   Release an RTC device.
 * @param   ref RTC ref to operate.
 * @return  None.
 */
void aos_rtc_put(aos_rtc_ref_t *ref);
/**
 * @brief   Read time from an RTC device.
 * @param   ref     RTC ref to operate.
 * @param   time    Time buffer.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_rtc_get_time(aos_rtc_ref_t *ref, aos_rtc_time_t *time);

/**
 * @brief   Write time to an RTC device.
 * @param   ref     RTC ref to operate.
 * @param   time    Time buffer.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_rtc_set_time(aos_rtc_ref_t *ref, const aos_rtc_time_t *time);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_RTC_H */
