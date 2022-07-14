/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef AOS_RTC_CORE_H
#define AOS_RTC_CORE_H

#include <aos/device_core.h>
#include <aos/rtc.h>

struct aos_rtc_ops;

typedef struct {
    aos_dev_t dev;

    /* must be initialized before registration */
    const struct aos_rtc_ops *ops;
} aos_rtc_t;

typedef struct aos_rtc_ops {
    void (*unregister)(aos_rtc_t *);
    aos_status_t (*startup)(aos_rtc_t *);
    void (*shutdown)(aos_rtc_t *);
    aos_status_t (*get_time)(aos_rtc_t *, aos_rtc_time_t *);
    aos_status_t (*set_time)(aos_rtc_t *, const aos_rtc_time_t *);
} aos_rtc_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_rtc_register(aos_rtc_t *rtc);
aos_status_t aos_rtc_register_argumented(aos_rtc_t *rtc, uint32_t id, const aos_rtc_ops_t *ops);
aos_status_t aos_rtc_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_RTC_CORE_H */
