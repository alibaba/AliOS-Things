/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_DRIVER_H_
#define _U_DRIVER_H_

#include <drivers/spinlock.h>

enum U_PM_EVENT {
	E_PM_ENTER,
	E_PM_L0,
	E_PM_L1,
	E_PM_EXIT,
};

// TODO: should move this to power manager header file
typedef struct u_pm_ops {
	int event;
} u_pm_ops_t;

/**
 * struct u_driver_private - driver private information
 * should only be accessed by char_core
 * @dev_list: device list driven by this driver
 * @bus_node: node in bus driver list
 *
 * */
typedef struct u_driver_private {
	spinlock_t lock;
	struct dlist_s dev_list;
	struct dlist_s bus_node;
	struct u_driver *drv;
} u_driver_private_t;


/**
 * struct u_bus - bus struct, virtual or hardware bus
 *
 * @name: driver name
 * @next: driver list
 * @bus: bus pointer to which the driver belongs
 * */
typedef struct u_driver {
	char *name;
	struct u_bus *bus;
	u_driver_private_t *p;

	// for legacy drivers
	int (*init)(struct u_device *dev);
	int (*deinit)(struct u_device *dev);
	int (*pm)(struct u_device *dev, u_pm_ops_t state);

	// for linux-like drivers
	int (*probe)(struct u_device *dev);
	int (*remove)(struct u_device *dev);
	void (*shutdown)(struct u_device *dev);
	int (*suspend)(struct u_device *dev, u_pm_ops_t state);
	int (*resume)(struct u_device *dev);

	// TODO: when receive IPC ping message call on_ping
	int (*on_ping)(struct u_device *dev);

} u_driver_t;

/**
 * register a driver into system, init it's private struct before add it into system
 *
 * @param drv - pointer to target driver to be registered
 * @return 0 for success; negative for failure
 */
int u_driver_register(struct u_driver *drv);

/**
 * unregister a driver from system
 * delete it and then free its private struct
 *
 * @param drv - pointer to target driver to be registered
 * @return 0 for success; negative for failure
 */
int u_driver_unregister(struct u_driver *drv);

/**
 * dump driver information
 *
 * @param drv - pointer to target driver to be dumped
 * @return 0 for success; -1 for failure
 *
 */
int u_driver_dump(struct u_driver *drv);

#endif //_U_DRIVER_H_
