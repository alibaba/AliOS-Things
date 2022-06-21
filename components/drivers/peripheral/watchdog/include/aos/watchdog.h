/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_WATCHDOG_H
#define AOS_WATCHDOG_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup watchdog_api Watchdog
 * @ingroup driver_api
 * @brief AOS API for watchdog.
 * @{
 */

#define AOS_WATCHDOG_F_DISABLE_ON_PUT   ((uint32_t)0x1 << 0)

typedef struct {
    uint32_t flags;
    uint32_t timeout;
} aos_watchdog_info_t;

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

#define AOS_WATCHDOG_IOC_GET_INFO       0x5701
#define AOS_WATCHDOG_IOC_SET_TIMEOUT    0x5702
#define AOS_WATCHDOG_IOC_GET_TIMELEFT   0x5703
#define AOS_WATCHDOG_IOC_KICK           0x5704

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_watchdog_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a watchdog device.
 * @param   ref Watchdog ref to operate.
 * @param   id  Watchdog device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_watchdog_get(aos_watchdog_ref_t *ref, uint32_t id);
/**
 * @brief   Release a watchdog device.
 * @param   ref Watchdog ref to operate.
 * @return  None.
 */
void aos_watchdog_put(aos_watchdog_ref_t *ref);
/**
 * @brief   Get information about a watchdog device.
 * @param   ref     Watchdog ref to operate.
 * @param   info    Information structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_watchdog_get_info(aos_watchdog_ref_t *ref, aos_watchdog_info_t *info);
/**
 * @brief   Set the timeout of a watchdog device.
 * @param   ref     Watchdog ref to operate.
 * @param   timeout Timeout in milliseconds.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_watchdog_set_timeout(aos_watchdog_ref_t *ref, uint32_t timeout);
/**
 * @brief   Get the remaining time before timeout.
 * @param   ref         Watchdog ref to operate.
 * @param   timeleft    Remaining time in milliseconds.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_watchdog_get_timeleft(aos_watchdog_ref_t *ref, uint32_t *timeleft);
/**
 * @brief   Kick a watchdog device.
 * @param   ref Watchdog ref to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_watchdog_kick(aos_watchdog_ref_t *ref);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_WATCHDOG_H */
