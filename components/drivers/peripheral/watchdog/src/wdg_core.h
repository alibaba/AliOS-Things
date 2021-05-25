/**
 * @file wdg_core.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _WDG_CORE_H
#define _WDG_CORE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "aos/driver/wdg.h"
#include <aos/list.h>
#include <aos/kernel.h>

#define AOS_WDG_MAGIC 0x3D63D63D

/* This struct define watchdog main handle */
typedef struct {
    uint8_t             id;                /**< watchdog controller's id */
    uint32_t            timeout;            /**< watchdog's timeout value, in unit of ms */
    aos_mutex_t         lock;             /**< lock for watchdog device's operations */
    uint32_t            wdg_dev_handle;    /**< use &wdg_dev_handle as watchdog device's handle */
    dlist_t             node;              /**<  node in watchdog controller's device list */
    void                *controller;       /**<  pointer to watchdog controller's struct */
    bool                started;           /**< whether watchdog user is started or not >*/
} wdg_dev_t;

/**
 * watchdog controller's settings
 * all watdhdog users connected to the same watchdog controller are connected to the list named with user
*/
typedef struct wdg_controller_dev {
    uint32_t    id;          /**< watchdog controller's id */
    uint32_t    tmo;         /**< current watdhdog's timeout value setting, in unit of ms */
    aos_mutex_t lock;        /**< used to lock all operations on current watchdog controller */
    dlist_t     user;       /**< node in watchdog controller's user list */
    csi_wdt_t   csi_dev;     /**< CSI watchdog device */
    bool        init;        /**< whether watchdog controller is initialized or not >*/
    bool        started;     /**< whether watchdog controller is started or not >*/
} wdg_controller_t;


#ifdef __cplusplus
}
#endif

#endif /* _WDG_CORE_H */