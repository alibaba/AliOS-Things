/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/list.h"

#include <drivers/spinlock.h>
#include <drivers/char/u_device.h>
#include <drivers/char/u_driver.h>
#include <drivers/char/u_bus.h>

/**
 * dump driver information
 *
 * @param drv - pointer to target driver to be dumped
 * @return 0 for success; -1 for failure
 *
 */
int u_driver_dump(struct u_driver *drv) {

	if (!drv) {
		ddkc_err("drv is NULL\r\n");
		return -1;
	}

	ddkc_info("drv->name:%s, drv:%p, bus:%p, p:%p\r\n",
	          drv->name ? drv->name : "NULL",
	          drv, drv->bus, drv->p);
	ddkc_info("drv->init:%p, drv->deinit:%p, drv->pm:%p, drv->on_ping:%p\r\n",
	          drv->init, drv->deinit, drv->pm, drv->on_ping);
	ddkc_info("drv->probe:%p, drv->remove:%p, drv->shutdown:%p, drv->suspend:%p, drv->resume:%p\r\n",
	          drv->probe, drv->remove, drv->shutdown, drv->suspend, drv->resume);

	return 0;
}

/**
 * check whether driver with the same name is already loaded or not
 * check whether driver with same name is already registered or not
 *
 * @param drv - pointer to target driver to be found
 * @return 0 for success; -1 for failure
 */
int _bus_find_driver(struct u_driver *drv) {
	u_bus_t *bus = drv->bus;
	u_driver_private_t *drv_p = NULL;
	struct dlist_s *next = NULL;
	u_driver_t *drv_b = NULL;

	dlist_for_each_entry_safe(&bus->p->drv_list, next, drv_p, u_driver_private_t, bus_node) {
		drv_b = drv_p->drv;

		if (!strcmp(drv->name, drv_b->name))
			return 1;
	}

	return 0;
}

/**
 * driver private struct initialization
 * @param drv - pointer to target driver
 * @return 0 for success; negative for failure
 */
int u_driver_initialize(struct u_driver *drv) {
	u_driver_private_t *p = NULL;

	p = (u_driver_private_t *)malloc(sizeof(u_driver_private_t));
	if (!p) {
		ddkc_err("malloc for u_driver_private_t fails\r\n");
		return -ENOMEM;
	}

	ddkc_dbg("malloc for u_driver_private_t done\r\n");

	spin_lock_init(&p->lock);
	dlist_init(&p->dev_list);
	dlist_init(&p->bus_node);
	p->drv = drv;
	drv->p = p;

	return 0;
}

/**
 * free driver's private struct
 *
 * @param drv - pointer to target driver
 * @return always return 0
 */
int u_driver_deinitialize(struct u_driver *drv) {

	free(drv->p);
    drv->p = NULL;

	return 0;
}

/**
 * add drvier into system
 *
 * @param drv - pointer to target driver
 * @return 0 for success; negative for failure
 */
int u_driver_add(struct u_driver *drv) {
	int r = 0;
	// TODO: need to assign drv->bus to platform_bus if it is not assigned

	if (!drv->p || !drv->bus || !drv->name) {
		ddkc_err("invalid drv->p:%p, drv->bus:%p or drv->name:%p\r\n", drv->p, drv->bus, drv->name);
		return -EINVAL;
	}

	r = _bus_find_driver(drv);
	if (r) {
		ddkc_err("drv[%s] is already added\r\n", drv->name);
		return -EBUSY;
	}

	r = u_bus_add_driver(drv);
	if (r)
		goto drv_add_error;

	u_bus_attach_driver(drv);

	return 0;

drv_add_error:
	free(drv->p);
	drv->p = NULL;

	return -1;
}

/**
 * delete driver from system
 * detach it from bus and ehen delete the driver from bus
 *
 * @param drv  - pointer to target driver
 * @return 0 for success; negative for failure
 */
int u_driver_del(struct u_driver *drv) {
	int r = 0;

	/* deinit devices attached to this driver before delete it from bus */
	r = u_bus_detach_driver(drv);
	if (r)
		ddkc_err("detach drv[%s] failed, r:%d\r\n", drv->name, r);

	r = u_bus_del_driver(drv);
	if (r)
		ddkc_err("delete drv[%s] failed, r:%d\r\n", drv->name, r);

	return r;
}

/**
 * register a driver into system, init it's private struct before add it into system
 *
 * @param drv - pointer to target driver to be registered
 * @return 0 for success; negative for failure
 */
int u_driver_register(struct u_driver *drv) {
	if(!drv)
		return -EINVAL;

	u_driver_initialize(drv);
	return u_driver_add(drv);
}

/**
 * unregister a driver from system
 * delete it and then free its private struct
 *
 * @param drv - pointer to target driver to be registered
 * @return 0 for success; negative for failure
 */
int u_driver_unregister(struct u_driver *drv) {
	if(!drv)
		return -EINVAL;

	u_driver_del(drv);
	return u_driver_deinitialize(drv);
}
