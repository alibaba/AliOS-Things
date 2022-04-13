/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_H_
#define _DEVICE_VFS_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include <drivers/mutex.h>
#include <drivers/u_ld.h>
#include <drivers/platform/u_platform_bus.h>

#define __weak __attribute__((weak))

#ifndef USER_SPACE_DRIVER
static inline void *aos_ipc_copy(void *dst, void *src, size_t len) {

    memcpy(dst, src, len);
    return dst;
}
#endif

/**
 * subsys device type list
 */
enum SUBSYS_BUS_TYPE {
    BUS_TYPE_PLATFORM,
    BUS_TYPE_SPI,
    BUS_TYPE_I2C,
    BUS_TYPE_USB,
    BUS_TYPE_SDIO,
    BUS_TYPE_MAX
};

typedef struct file_ops subsys_file_ops_t;

/**
 * type and name are mandaotry, type is set to BUS_TYPE_PLATFORM if no bus is defined in "enum SUBSYS_BUS_TYPE"
 * delay_init should be set to 1 if the application scenario is time sensitive
 * if delay_init is set to true
 *  * subsys_drv's init/probe won't be called during driver init procedure;
 *  * init/probe will be called only when open request comes
 */
struct subsys_dev {
    int permission;
    bool delay_init;
    enum SUBSYS_BUS_TYPE type;
    void *user_data;
    char *node_name;
};

/**
 * drv_name - subsystem driver name
 *
 * here we provide 2 ind of driver ops:
 *  1. legacy driver API - for legacy rtos driver ops
 *  2. linux-like drivers - for drivers' written with concept of Linux
 * user can select either 1st or 2nd way, when both ops sets are provided, legacy driver will be used
 *
 * init - driver's initialization function callback, this API is called only once per device
 * deinit - driver's deinitialization function callback, this API is called only when device is removed when calling aos_dev_unreg
 * pm - for power managerment function
 *
 * probe - same function as init
 * remove - same function as deinit
 * shutdown/suspend/resume - power management related behavior's API
 */
struct subsys_drv {
    char *drv_name;

    /* for legacy drivers */
    int (*init)(struct u_platform_device *_dev);
    int (*deinit)(struct u_platform_device *_dev);
    int (*pm)(struct u_platform_device *dev, u_pm_ops_t state);

    /* for linux-like drivers */
    int (*probe)(struct u_platform_device *);
    int (*remove)(struct u_platform_device *);
    void (*shutdown)(struct u_platform_device *);
    int (*suspend)(struct u_platform_device *, int /*pm_message_t */state);
    int (*resume)(struct u_platform_device *);
};

/**
 *
 * register devices and corresponding driver into device/driver module
 *
 * @param sdev  - device identifier, name and type must be carefully set
 * @param sfops - file operations API
 * @param sdrv  - device driver operations callback
 *              - init/probe will be called when device exist
 *              - deinit/remove will be called when aos_dev_unreg is called
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_dev_reg(struct subsys_dev *sdev, subsys_file_ops_t *sfops, struct subsys_drv* sdrv);

/**
 * This function is not used for the moment
 * register remote devices/driver into device/driver module, only support rpc mode
 *
 * @param sdev  - device identifier, name and type must be carefully set
 * @param sfops - file operations API
 * @param sdrv  - device driver operations callback
 *              - probe will be called when device exist
 *              - remove will be called when aos_dev_unreg is called
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_remote_dev_reg(struct subsys_dev *sdev, subsys_file_ops_t *sfops, struct subsys_drv* sdrv);

/**
 *
 * register device array
 *
 * @param sdev pointer to devices array be register
 * @param size device array size
 * @param sfops file operations API
 * @param sdrv  - device driver operations callback
 *              - probe will be called when device exist
 *              - remove will be called when aos_dev_unreg is called
 * @return 0 for success; negative for failure
 */
int aos_devs_reg(struct subsys_dev *sdev[], int size, subsys_file_ops_t *sfops, struct subsys_drv* sdrv);

/**
 * unregister device driver
 *
 * @param sdev - device identifier, name and type must be carefully set
 *
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_dev_unreg(struct subsys_dev *sdev);

/**
 * unregister device array
 *
 * @param sdev - pointer to devices array be register
 * @param size - device array size
 *
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_devs_unreg(struct subsys_dev *sdev[], int size);

#endif //_DEVICE_VFS_H_
