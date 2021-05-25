/**
 * @file wdg.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_WDG_H
#define _AOS_WDG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aos/kernel.h>

/** @addtogroup aos_wdg_device I2C
 *  aos watchdog API.
 *
 *  @{
 */

typedef uint32_t* wdg_dev_handle_t;

/**
 * start a new watchdog
 *
 * @param[in]  id  watchdog controller's id
 * @param[in]  ms  watchdog's timeout value, in unit of ms, 0xFFFFFFFF is not allowed to use
 *
 * @return  handle of the watchdog device, used for later watchdog operations, when open operation success;
            NULL when open operation fails
 */
extern wdg_dev_handle_t aos_wdg_open (uint32_t id, uint32_t ms);

/**
 * close target watchdog handle
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
extern aos_status_t aos_wdg_close (wdg_dev_handle_t dev_h);


/**
 * start target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
extern aos_status_t aos_wdg_start (wdg_dev_handle_t dev_h);

/**
 * stop target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
extern aos_status_t aos_wdg_stop (wdg_dev_handle_t dev_h);

/**
 * set timeout value of target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 * @param[in]  clk  watchdog's new timeout value, in unit of ms
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
extern aos_status_t aos_wdg_timeout_set (wdg_dev_handle_t dev,  uint32_t ms);

/**
 * feed target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
extern aos_status_t aos_wdg_feed (wdg_dev_handle_t dev_h);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _AOS_WDG_H */
