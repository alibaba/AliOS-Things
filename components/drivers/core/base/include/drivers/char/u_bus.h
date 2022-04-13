/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_BUS_H_
#define _U_BUS_H_

#include "aos/list.h"

#include <drivers/spinlock.h>
#include <drivers/char/u_driver.h>

/**
 * struct u_bus_private - bus private information
 * should only be accessed by char_core
 * @dev_list: device list belongs to current bus
 * @drv_list: driver list belongs to current bus
 * @bus: pointer to the bus struct this u_bus_private_t belongs to
 *
 * */
typedef struct u_bus_private {
	spinlock_t lock;
	struct dlist_s dev_list;
	struct dlist_s drv_list;
	struct dlist_s bus_node;
	struct u_bus *bus;
} u_bus_private_t;

/**
 * struct u_bus - bus struct, virtual or hardware bus
 *
 * @name: bus name
 * @drv_list: driver list belongs to this bus
 * @match:
 * @probe:
 * @remove:
 * @shutdown:
 * @online:
 * @offline:
 * @suspend:
 * @resume:
 * @on_ping:
 * @pm:
 *
 * */
typedef struct u_bus {
	char *name;
	struct u_bus_private *p;

	// for legacy drivers
	int (*init)(struct u_device *_dev);
	int (*deinit)(struct u_device *_dev);
	int (*pm)(struct u_device *dev, u_pm_ops_t state);
	int (*match)(struct u_device *dev, struct u_driver *drv);

	// for linux-like drivers
	int (*probe)(struct u_device *dev);
	int (*remove)(struct u_device *dev);
	void (*shutdown)(struct u_device *dev);
	int (*online)(struct u_device *dev);
	int (*offline)(struct u_device *dev);
	int (*suspend)(struct u_device *dev, u_pm_ops_t state);
	int (*resume)(struct u_device *dev);

	// TODO: when receive IPC ping message call on_ping, shall move this to u_driver_private_t?
	int (*on_ping)(struct u_device *dev);

} u_bus_t;

/**
 * init global bus list
 * */
int u_bus_init(void);

/**
 * register bus to system
 * @drv: the bus to register
 *
 * */
int u_bus_register(struct u_bus *bus);

/**
 * unregister bus from system
 * @drv: the bus to be removed
 *
 * */
int u_bus_unregister(struct u_bus *bus);


/**
* connect dev into target bus(dev->bus)
* low level operations, driver is not aware of this operation
*
* @param dev - pointer to target device
* @return always returns 0
*/
int u_bus_add_device(struct u_device *dev);

/**
 * enumerate driver list and try to init the device with the drivers
 *
 * @param dev - pointer to the deviice to be initialized
 *
 * @return 0 if device initialization success; negative if device initialization fails
 */
int u_bus_try_init_device(struct u_device *dev);

/**
 * add a driver into bus's driver list
 *
 * @param drv - the driver to be added
 * @return always return 0
 */
int u_bus_add_driver(struct u_driver *drv);

/**
 * get the bus with specified name
 * @name: the name of target bus
 *
 * */
u_bus_t *u_bus_get_by_name(char *name);

/**
 * disconnect device with its driver and delete it from target bus(dev->bus)
 *
 * @param dev - pointer to target device
 * @return always return 0
 */
int u_bus_del_device(struct u_device *dev);

/**
 * delete a driver from bus's driver list
 *
 * @param drv - pointer to the driver to be deleted
 * @return always return 0
 */
int u_bus_del_driver(struct u_driver *drv);

/**
 * attach a driver into bus's driver list,
 * try to init free devices(not drived by any driver) if device is matchd
 *
 * @drv: the driver to be added
 * @return always return 0
 */
int u_bus_attach_driver(struct u_driver *drv);

/**
 * deattch a driver into bus's driver list
 *
 * @param drv - pointer to target driver to be deatched
 * @return always return 0
 */
int u_bus_detach_driver(struct u_driver *drv);

#if 0
struct u_device *u_device_find_by_devid(dev_t devid);
#endif

#endif //_U_BUS_H_
