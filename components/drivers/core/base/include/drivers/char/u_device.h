/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_DEVICE_H_
#define _U_DEVICE_H_

#include <stdio.h>

#include "aos/list.h"
#include "aos/vfs.h"
#include <drivers/spinlock.h>
#include <drivers/ddkc_log.h>

#define LOG_TAG "UDEV"
#include <ulog/ulog.h>

struct u_device;
struct u_device_private;


/* valid device id: [0, 9999), this should be very enough */
#define MAX_DEV_ID_DIGS 4

/**
 * struct u_device_private - device private information
 * should only be accessed by char_core
 * @bus_node: node in bus list
 * @driver_node: node in driver list
 *
 * */
typedef struct u_device_private {
	spinlock_t lock;
	struct dlist_s bus_node;
	struct dlist_s drv_node;
	struct dlist_s parent_node; /* node in parent's child list with head of child_head*/
	struct dlist_s child_head;  /* child list's head */
	struct u_device *dev;
	char *name;
} u_device_private_t;

/**
 * struct u_device - device structure
 * @parent: device's parent
 * @drv: the driver by which this device is driven
 * @bus: the bus to which this device is on
 * @p: driver private information, device driver and bus list information
 * */
typedef struct u_device {
	struct u_device* parent;
	struct u_driver *drv;
	struct u_bus *bus;
	char *name;
	unsigned int id;
	void *user_data;
	void *driver_data;
	unsigned int dev_id;
	u_device_private_t *p;
} u_device_t;

/*
 * all device node must be registered with u_device_node_create
 * */
typedef struct u_dev_node {
	//TODO: should add lock for device node list operation
	struct dlist_s node;
	struct u_dev_node *parent;
	int dev_id;
	//TODO: shall dev be added for device node?
	struct u_device *dev;
	char *path_name;
	void *drv_data;
	struct file_operations *fops;
	struct file_ops legacy_fops;
	char name[0];
} u_dev_node_t;


typedef struct u_device_info_ipc {
	unsigned int id;
	char name[32];
} u_device_info_ipc_t;

/**
 * add device into bus device list
 * conditions to check in u_device_add:
 * 1. dev is not NULL
 * 2. dev->bus is assigned correctly
 * 3. dev->p is allocated and initialized
 *
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_add(struct u_device *dev);

/**
 * initialize u_device's private info
 *
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_initialize(struct u_device *dev);

/**
 * register a device into system
 *
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_register(struct u_device *dev);

/**
 * check whether a device is registered or not
 * @param dev - pointer to the device to be checked
 *
 * @return 0 for success; netative for failure
 */
int u_device_is_registered(struct u_device *dev);

/**
 * unregister device from system
 * @param dev - pointer to target device to be unregistered
 * @return 0 for success; negative for failure
 */
int u_device_unregister(struct u_device *dev);

/**
 * delete device from system
 *
 * @param dev - pointer to device to be deleted
 * @return 0 for success; negative for failure
 */
int u_device_del(struct u_device *dev);

/**
 * dump device's information, name, id, parent, bus, driver, drv_data included
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_dump(struct u_device *dev);

int u_device_root_node_init(void);
struct u_dev_node * u_device_node_create(struct u_dev_node *parent, unsigned int dev_id, void *drv_data, char *name);
#if 0
struct u_dev_node * u_device_node_find_by_devid(dev_t devid);
#endif
struct u_dev_node * u_device_node_find_by_pathname(char *pathname);
struct u_dev_node * u_device_node_find_by_dev(struct u_device *u_dev);
struct u_dev_node * u_device_node_find_by_nodename(char *node_name);
int u_device_node_delete(unsigned int dev_id);

#endif //_U_DEVICE_H_
