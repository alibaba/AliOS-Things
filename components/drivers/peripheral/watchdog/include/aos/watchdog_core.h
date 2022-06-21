/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_WATCHDOG_CORE_H
#define AOS_WATCHDOG_CORE_H

#include <aos/device_core.h>
#include <aos/watchdog.h>

struct aos_watchdog_ops;

typedef struct {
    aos_dev_t dev;
    uint32_t timeout;

    /* must be initialized before registration */
    const struct aos_watchdog_ops *ops;
    uint32_t flags;
} aos_watchdog_t;

typedef struct aos_watchdog_ops {
    void (*unregister)(aos_watchdog_t *);
    aos_status_t (*set_timeout)(aos_watchdog_t *);
    uint32_t (*get_timeleft)(aos_watchdog_t *);
    void (*kick)(aos_watchdog_t *);
} aos_watchdog_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_watchdog_register(aos_watchdog_t *watchdog);
aos_status_t aos_watchdog_register_argumented(aos_watchdog_t *watchdog, uint32_t id,
                                              const aos_watchdog_ops_t *ops, uint32_t flags);
aos_status_t aos_watchdog_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_WATCHDOG_CORE_H */
