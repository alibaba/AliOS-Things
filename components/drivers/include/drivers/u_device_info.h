/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_DEVICE_INFO_H_
#define _U_DEVICE_INFO_H_

#include "aos/list.h"

#include <drivers/u_io.h>

struct u_bus_info;
struct u_device_info;
struct u_driver_info;

typedef unsigned int u_dev_t;

typedef struct u_bus_info {
	bus_type_e type;
	/*TODO: const*/ char *name;
	struct dlist_s bus_node;
	struct slist_s bus_dev_head;
	struct u_driver_info *drv;
} u_bus_info_t;

typedef enum device_state {
	DEV_STATE_IDLE = 0x71, /* idle, not bind with any driver */
	DEV_STATE_BINDED,      /* binded with the driver */
	DEV_STATE_SHARED,      /* shared state */
	DEV_STATE__MAX,
} device_state_e;

/**
 * struct u_device_info - the base device struct
 * @parent: The device's parent, usually it points to device's bus
 *          NULL is not allowed for non-bus type device, if the device is not
 *          attached to any bus, attach to virtual bus named platform
 * @dev_name: device's name
 * @id: device id
 * @dev_res: device resource list
 * @devt:
 * @state: device state, idle/binded
 * @bus: bus of the device
 * @drv: driver for this device
 * @dev_node： device list
 * @bus_dev_node: device list of the same bus
 * @drv_dev_node: device list of the same driver
 *
 * */
typedef struct u_device_info {
	struct u_device_info *parent;

	/*TODO: const*/ char *dev_name;
	unsigned int id;

	struct dev_res *res;
	u_dev_t devt;

	unsigned int state;

	struct u_bus_info *bus;
	struct u_driver_info *drv;

	struct dlist_s dev_node;
	struct slist_s bus_dev_node;
	struct slist_s drv_dev_node;
} u_device_info_t;

/**
 * struct u_driver - device driver information
 * @name: driver's name, should be compatible with dev_name (in struct u_device_info)
 *        of the device it drives
 * @path: driver path in file system
 * @pid: id of the process in which the driver is loaded, used for IPC
 * @tid: id of the thread belongs to the driver, used for IPC
 * @drv_dev_node: device list which this driver drives
 * @
 * */
typedef struct u_driver_info {
	/*TODO: const*/ char name[128]; /* TODO: use char* and malloc memory dynamic later */
	/*TODO: const*/ char *path;     /* driver binary file path */
	int type;                       /* bus, device or subsystem */
	int bus_type;                   /* bus driver's type, platform, sdio, usb and etc. */
	unsigned int pid;
	unsigned int tid;

	struct slist_s drv_dev_head;
	struct dlist_s drv_node;

} u_driver_info_t;


typedef enum drv_type {
	DRV_TYPE_MIN,
	DRV_TYPE_BUS,
	DRV_TYPE_SUBSYS,
	DRV_TYPE_CHAR_DEV,
	DRV_TYPE_BLOCK_DEV,
	DRV_TYPE_NET_DEV,
	DRV_TYPE_MAX
} drv_type_e;

typedef struct u_driver_bin_info {
	int type;     /* bus, device or subsystem */
	int bus_type; /* bus_type_e */
	char *name;   /* driver name got from binary */
} u_driver_bin_info_t;

#endif //_U_DEVICE_INFO_H_
