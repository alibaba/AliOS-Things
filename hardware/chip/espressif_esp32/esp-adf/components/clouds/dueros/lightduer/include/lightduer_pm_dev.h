/**
 * Copyright (2017) Baidu Inc. All rights reserved.
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
/*
 * File: lightduer_pm_dev.h
 * Auth: Chang Li (changli@baidu.com)
 * Desc: PM Device Head file
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_DEV_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_DEV_H

#include <stdint.h>
#include "lightduer_mutex.h"

#define DEV_NAME_LEN 32

#ifdef __cplusplus
extern "C" {
#endif

typedef enum duer_pm_dev_state {
    PM_ST_NORMAL,
    PM_ST_IDLE,
    PM_ST_SUSPEND,
    PM_ST_SUSPENDING,
    PM_ST_RESUME,
    PM_ST_RESUMEING,
    PM_ST_POWER_OFF,
    MAX_PM_STATE,
} duer_pm_dev_state_t;

typedef enum duer_pm_dev_type {
    PM_DEV_CODEC,
    PM_DEV_LED,
    PM_DEV_PA,
    PM_DEV_WIFI,
    PM_DEV_DSP,
    MAX_PM_DEV_TYPE,
} duer_pm_dev_type_t;

typedef struct duer_pm_device {
    char name[DEV_NAME_LEN + 1];
    duer_mutex_t lock;
    duer_pm_dev_type_t dev_type;
    duer_pm_dev_state_t pm_state;
    uint16_t user_cnt;
    uint32_t suspend_target_time;
    uint32_t power_off_target_time;
    uint32_t normal_timestamp;
    uint32_t prepare_suspend_timestamp;
    uint32_t suspend_timestamp;
    uint32_t prepare_poweroff_timestamp;
    uint32_t poweroff_timestamp;
    uint32_t pm_state_duration;
    int (*enter_pm_state)(duer_pm_dev_state_t state);
    struct duer_pm_device *next;
} duer_pm_device_t;

typedef struct duer_pm_device_list {
    duer_mutex_t list_lock;
    duer_pm_device_t *list_head;
} duer_pm_device_list_t;

typedef struct duer_init_pm_ops {
    int (*register_pm_dev_func)(void);
    int (*start_policy)(void);
} duer_init_pm_ops_t;

typedef int (*treatment_dev_func)(duer_pm_device_t *dev);

/*
 * Initialize Power Managerment module
 *
 * @param pointer to duer_init_pm_ops_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_init_pm(const duer_init_pm_ops_t *init_ops);

/*
 * Initialize devices for power managerment
 *
 * @param none
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
int duer_pm_init_devices(void);

/**
 * Get device list
 *
 * @param none
 *
 * @return duer_pm_device_list_t*
 *
 */
const duer_pm_device_list_t *duer_pm_get_device_list(void);

/*
 * Alloc a device
 *
 * @param void
 *
 * @return pointer: Success: duer_pm_device_t *
 *                  Failed:  NULL
 */
extern duer_pm_device_t *duer_pm_alloc_dev(void);

/*
 * Destroy a device object
 *
 * @param pointer to device
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_destroy_dev(duer_pm_device_t *dev);

/*
 * Register a device to PM
 *
 * @param dev: duer_pm_device_t *
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_register_dev(duer_pm_device_t *dev);

/*
 * Get device Object by the name of device
 *
 * @param name: const char *
 *
 * @return pointer: Success: duer_pm_device_t *
 *                  Failed:  NULL
 */
extern duer_pm_device_t *duer_pm_get_dev_obj_by_name(const char *name);

/*
 * Get device Object by the device type
 *
 * @param name: const char *
 *
 * @return pointer: Success: duer_pm_device_t *
 *                  Failed:  NULL
 */
extern duer_pm_device_t *duer_pm_get_dev_obj_by_type(duer_pm_dev_type_t type);
/*
 * Unregister a device from PM and destroy it
 *
 * @param dev: duer_pm_device_t *
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_unregister_dev(duer_pm_device_t *dev);

/*
 * Increase the reference count of the device
 *
 * @param dev: duer_pm_device_t *
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_acquire_dev(duer_pm_device_t *dev);

/*
 * Reduce the reference count of the device
 *
 * @param dev: duer_pm_device_t *
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_release_dev(duer_pm_device_t *dev);

/*
 * Update the power state of the device
 *
 * @param dev: duer_pm_device_t *
 *      state: duer_pm_dev_state
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_update_pm_state(duer_pm_device_t *dev, duer_pm_dev_state_t state);

/*
 * Get the power state of the device
 *
 * @param dev: duer_pm_device_t *
 *
 * @return duer_pm_dev_state_t
 */
extern duer_pm_dev_state_t duer_pm_get_pm_state(duer_pm_device_t *dev);

/*
 * Update the suspend target time of the device
 *
 * @param dev: duer_pm_device_t *
 *       time: uint32_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_update_suspend_target_time(duer_pm_device_t *dev, uint32_t time_s);

/*
 * Update the poweroff target time of the device
 *
 * @param dev: duer_pm_device_t *
 *       time: uint32_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_update_poweroff_target_time(duer_pm_device_t *dev, uint32_t time_s);

/*
 * Traverse the device list
 *
 * @param traverse : Processing the device function
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_pm_traverses_dev_list(treatment_dev_func handler);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_DEV_H
