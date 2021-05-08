/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "aos/list.h"

#include <drivers/u_ld.h>
#include <drivers/spinlock.h>
#include <drivers/char/u_device.h>
#include <drivers/char/u_driver.h>
#include <drivers/char/u_bus.h>

static int u_bus_dump(struct u_bus *bus);
static int u_device_unbind_driver(struct u_device *dev);
static int u_bus_detach_device(struct u_device *dev);

/**
 * all buses is connected via g_bus_list_head,
 * used to enumerate bus/device/driver,
 * either for info dump or for power management function
 *
 */
static dlist_t g_bus_list_head;
static spinlock_t g_bus_list_lock;

/**
 * dump all bus informations
 *
 * @return always returns 0
 */
int u_bus_dump_all(void) {
	u_bus_t *bus = NULL;
	dlist_t *next = NULL;
	u_bus_private_t *p = NULL;

	dlist_for_each_entry_safe(&g_bus_list_head, next, p, u_bus_private_t, bus_node) {
		ddkc_info("p:%p, bus_node:%p, next:%p, prev:%p\r\n", p, &p->bus_node, p->bus_node.next, p->bus_node.prev);
		bus = p->bus;
		u_bus_dump(bus);
	}

	return 0;
}

/**
 * dump single bus's information, all devices, drivers connected to this bus is included
 *
 * @param bus - pointer to specified bus
 * @return
 */
static int u_bus_dump(struct u_bus *bus) {
	u_device_t *dev = NULL;
	u_driver_t *drv = NULL;
	struct u_bus_private *p = NULL;
	u_device_private_t *dev_p = NULL;
	u_driver_private_t *drv_p = NULL;

	struct dlist_s *next = NULL;

	if (!bus)
		return -EINVAL;

	ddkc_info("bus:%p, name:%s, p:%p\r\n", bus, bus->name, bus->p);

	p = bus->p;

	/* dump information of all devices, who is connected into current bus */
	if (dlist_empty(&p->dev_list)) {
		ddkc_warn("bus[%s]'s device list is empty\r\n", bus->name);
	} else {
		dlist_for_each_entry_safe(&p->dev_list, next, dev_p, u_device_private_t, bus_node) {
			dev = dev_p->dev;
			u_device_dump(dev);
		}
	}

	/* dump information of all drivers, who is connected into current bus */
	if (dlist_empty(&p->drv_list)) {
		ddkc_warn("bus[%s]'s driver list is empty\r\n", bus->name);
	} else {
		dlist_for_each_entry_safe(&p->drv_list, next, drv_p, u_driver_private_t, bus_node) {
			drv = drv_p->drv;
			u_driver_dump(drv);
		}
	}

	return 0;
}

/**
 * connect dev into target bus(dev->bus)
 * low level operations, driver is not aware of this operation
 *
 * @param dev - pointer to target device
 * @return always returns 0
 */
int u_bus_add_device(struct u_device *dev) {
	unsigned long flags;
	u_bus_t *bus = dev->bus;

	/* add dev->p->bus_node into dev->bus's p->dev_list */
	spin_lock_irqsave(&bus->p->lock, flags);
	dlist_add_tail(&dev->p->bus_node, &bus->p->dev_list);
	spin_unlock_irqrestore(&bus->p->lock, flags);

	return 0;
}

/**
 * disconnect device with its driver and delete it from target bus(dev->bus)
 *
 * @param dev - pointer to target device
 * @return always return 0
 */
int u_bus_del_device(struct u_device *dev) {
	u_bus_t *bus = NULL;

	if (!dev) {
		ddkc_err("invalid dev:%p to delete\r\n", dev);
		return -EINVAL;
	}

	bus = dev->bus;

	/* if device is already attached with specified driver, detach it */
	if (dev && dev->drv) {
        u_bus_detach_device(dev);
        u_device_unbind_driver(dev);
    }

    /* delete dev from it's bus */
	if (dev && dev->bus && !dlist_empty(&dev->bus->p->dev_list)) {
		//TODO: should notify driver about this device is to be deleted?
		unsigned long flags;

		spin_lock_irqsave(&bus->p->lock, flags);
		dlist_del(&dev->p->bus_node);
		dlist_init(&dev->p->bus_node);
		spin_unlock_irqrestore(&bus->p->lock, flags);
	}

	return 0;
}

/**
 * bind device with target driver
 *
 * @param drv - pointer to the driver to be binded
 * @param dev - pointer to the device to be binded
 * @return always return 0
 */
static int u_driver_bind_device(struct u_driver *drv, struct u_device *dev) {
    unsigned long flags;
	u_driver_private_t *drv_p = drv->p;
	u_device_private_t *dev_p = dev->p;

	spin_lock_irqsave(&drv_p->lock, flags);
	dlist_add_tail(&dev_p->drv_node, &drv_p->dev_list);
	spin_unlock_irqrestore(&drv_p->lock, flags);

	return 0;
}

/**
 * unbind device with the binded driver
 *
 * @param dev - pointer to target device
 * @return always returns 0
 */
static int u_device_unbind_driver(struct u_device *dev) {
    unsigned long flags;
    u_device_private_t *dev_p = dev->p;

    spin_lock_irqsave(&dev_p->lock, flags);
    dlist_del(&dev_p->drv_node);
    spin_unlock_irqrestore(&dev_p->lock, flags);

    return 0;
}

/**
 * Try to init dev with drv's ops
 *
 * @param drv - target driver to be used for device initialization
 * @param dev - target device to be initialized
 * @return return 1 if init success; otherwise return 0
 */
static int u_driver_init_device(struct u_driver *drv, struct u_device *dev)
{
	int r = -1;

	/* attach to drv for the moment, will detach if init/probe fails */
	dev->drv = drv;
	ddkc_dbg("bus[%s] try to init dev[%s] with drv[%s]\r\n", dev->bus->name, dev->name, drv->name);

	ddkc_dbg("dev->bus->init:%p, drv->init:%p\r\n", dev->bus->init, drv->init);
	ddkc_dbg("dev->bus->probe:%p, drv->probe:%p\r\n", dev->bus->probe, drv->probe);
	do {
		/* try legacy ops, bus init cb is prioritized */
		if (dev->bus->init) {
			r = dev->bus->init(dev);
			break;
		} else if (drv->init) {
			r = drv->init(dev);
			break;
		}

		/* try probe ops - for linux-like drivers, bus probe cb is prioritized */
		if (dev->bus->probe) {
			r = dev->bus->probe(dev);
			break;
		} else if (drv->probe) {
			r = drv->probe(dev);
			break;
		}
	} while(0);

	if (!r) {
		ddkc_dbg("bus[%s] drv[%s] init dev[%s] success\r\n", dev->bus->name, drv->name, dev->p->name);
		u_driver_bind_device(drv, dev);
	} else {
		ddkc_warn("bus[%s] drv[%s] init dev[%s] fails\r\n", dev->bus->name, drv->name, dev->name);
		dev->drv = NULL;
	}

	return r;
}

/**
 * Try whether drv matches dev with bus match function
 *
 * @param drv - target driver to be matched with dev
 * @param dev - target device to be matched with drv
 * @return return 1 if match success; otherwise return 0
 */
static int u_driver_match_device(struct u_driver *drv, struct u_device *dev)
{
	if (!drv || !dev) {
		ddkc_err("invalid drv:%p or dev:%p\r\n", drv, dev);
		return 0;
	}

	ddkc_dbg("driver match device, drv:%p, drv->bus:%p, dev:%p\r\n", drv, drv->bus, dev);
	if (drv->bus) {
		ddkc_dbg("driver match device, drv->bus->match:%p\r\n", drv->bus->match);
		return drv->bus->match ? drv->bus->match(dev, drv) : 1;
	} else {
		ddkc_err("invalid drv->bus:%p\r\n", drv->bus);
		return 0;
	}
}

/**
 * enumerate driver list and try to init the device with the drivers
 *
 * @param dev - pointer to the deviice to be initialized
 *
 * @return 0 if device initialization success; negative if device initialization fails
 */
int u_bus_try_init_device(struct u_device *dev) {
	int r = 0;
	u_bus_t *bus = dev->bus;
	u_driver_t *drv = NULL;
	u_driver_private_t *drv_p = NULL;
	struct dlist_s *next = NULL;

	if (dev->drv) {
		ddkc_warn("dev[%s]'s is busy, is now driven by drv[%s]\r\n", dev->name, dev->drv->name);
		return 0;
	}
	// whenever device is created, try match and init it if match returns true
	if (dlist_empty(&bus->p->drv_list)) {
		ddkc_dbg("bus[%s]'s drv_list is empty\r\n", bus->name);
		return 0;
	}

	dlist_for_each_entry_safe(&bus->p->drv_list, next, drv_p, u_driver_private_t, bus_node) {
		drv = drv_p->drv;

		r = u_driver_match_device(drv, dev);
		if (!r) {
			ddkc_dbg("drv[%s] does not match dev[%s]\r\n", drv->name, dev->name);
			continue;
		}

		r = u_driver_init_device(drv, dev);
		if (r) {
			ddkc_err("drv[%s] init dev[%s] fails\r\n", drv->name, dev->name);
		} else {
			ddkc_info("drv[%s] init dev[%s] success\r\n", drv->name, dev->name);
		}
	}

	return 0;
}

/**
 * add a driver into bus's driver list
 *
 * @param drv - the driver to be added
 * @return always return 0
 */
int u_bus_add_driver(struct u_driver *drv) {
	u_bus_t *bus = drv->bus;
	unsigned long flags;

	spin_lock_irqsave(&bus->p->lock, flags);
	dlist_add_tail(&drv->p->bus_node, &bus->p->drv_list);
	spin_unlock_irqrestore(&bus->p->lock, flags);

	return 0;
}

/**
 * attach a driver into bus's driver list,
 * try to init free devices(not drived by any driver) if device is matchd
 *
 * @drv: the driver to be added
 * @return always return 0
 */
int u_bus_attach_driver(struct u_driver *drv) {
	int r = 0;
	u_bus_t *bus = drv->bus;
	u_device_t *dev = NULL;
	u_device_private_t *dev_p = NULL;
	dlist_t *next = NULL;

	// whenever device is created, try init it if match returns success
	if (dlist_empty(&bus->p->dev_list)) {
		ddkc_dbg("bus[%s]'s dev_list is empty\r\n", bus->name);
		return 0;
	}

	/* try to match the device in bus device list with driver */

	dlist_for_each_entry_safe(&bus->p->dev_list, next, dev_p, u_device_private_t, bus_node) {
		dev = dev_p->dev;
		/* exclude the devices who is already attached by driver */
		if (dev->drv) {
			ddkc_dbg("bus[%s] dev[%s] is already probed by drv[%s]\r\n",
			         bus->name, dev->p->name, dev->drv->name);
			continue;
		}

		r = u_driver_match_device(drv, dev);
		if (!r) {
			ddkc_dbg("drv[%s] does not match dev[%s]\r\n", drv->name, dev->name);
			continue;
		}

		r = u_driver_init_device(drv, dev);
		ddkc_dbg("drv[%s] init dev[%s] %s, r:%d\r\n", drv->name, dev->name, r ? "fail" : "success", r);
	}

	return 0;
}

u_bus_t * u_bus_get_by_name(char *name) {
	dlist_t *next = NULL;
	u_bus_private_t *p = NULL;
	u_bus_t *bus = NULL;

	if (!name)
		return NULL;


	dlist_for_each_entry_safe(&g_bus_list_head, next, p, u_bus_private_t, bus_node) {
		u_bus_dump(p->bus);
		bus = p->bus;
		ddkc_dbg("bus->name:%s, name_len:%d\r\n", bus->name, strlen(bus->name));
		if (!strcmp(bus->name, name)) {
			ddkc_dbg("name:%s, bus:%p\r\n", name, bus);
			break;
		}
		bus = NULL;
	}

	return bus;
}

/**
 * init global bus list, this is module entry API of u_bus
 * @return always return 0
 */
int u_bus_list_init(void) {
	ddkc_dbg("bus list init\r\n");
	dlist_init(&g_bus_list_head);
	spin_lock_init(&g_bus_list_lock);
	return 0;
}

/**
 * add new bus into global bus list
 *
 * @param bus - pointer to target bus to be added
 * @return always return 0
 */
int u_bus_add(struct u_bus *bus) {
	unsigned long flags;

	ddkc_dbg("add bus[%s] into global bus list\r\n", bus->name);
	spin_lock_irqsave(&g_bus_list_lock, flags);
	dlist_add_tail(&bus->p->bus_node, &g_bus_list_head);
	spin_unlock_irqrestore(&g_bus_list_lock, flags);
	return 0;
}

/**
 * init bus node struct
 *
 * @param bus - target bus to be initialized
 * @return 0 for success; negative for failure
 */
int u_bus_node_initialize(struct u_bus *bus) {
	u_bus_private_t *p = NULL;

	p = (u_bus_private_t *)malloc(sizeof(u_bus_private_t));
	if (!p) {
		ddkc_err("malloc for u_bus_private_t fails\r\n");
		return -ENOMEM;
	}

	spin_lock_init(&p->lock);
	dlist_init(&p->dev_list);
	dlist_init(&p->drv_list);
	dlist_init(&p->bus_node);
	p->bus = bus;
	bus->p = p;

	return 0;
}

/**
 * register bus to system
 *
 * @param bus - pointer to the target bus to be registered
 * @return 0 for success; negative for failure
 */
int u_bus_register(struct u_bus *bus) {
	int r = 0;

	if(!bus) {
		ddkc_err("invalid bus:%p\r\n", bus);
		return -EINVAL;
	}

	if (bus->p || !bus->name) {
		ddkc_err("invalid bus priv data:%p or bus name:%p, ignore\r\n", bus->p, bus->name);
		return -EINVAL;
	}

	r = u_bus_node_initialize(bus);
	if (r) {
		ddkc_err("bus node init fails, error:%d\r\n", r);
		return -1;
	}

	return u_bus_add(bus);
}

/**
 * unregister bus from system
 *
 * @param bus  - pointer to the target bus to be unregistered
 * @return
 */
int u_bus_unregister(struct u_bus *bus) {
	unsigned long flags;

	// TODO: all bus related driver/device list should be clean up before calling u_bus_unregister
	ddkc_info("add bus[%s] into global bus list\r\n", bus->name);
	spin_lock_irqsave(&g_bus_list_lock, flags);
	dlist_del(&bus->p->bus_node);
	spin_unlock_irqrestore(&g_bus_list_lock, flags);
	return 0;
}

#if 0
struct u_device *u_bus_find_dev_by_id(struct u_bus *bus, dev_t devid) {
	u_device_t *dev = NULL;
	struct u_driver_t *drv = NULL;
	struct u_bus_private *p = NULL;
	u_device_private_t *dev_p = NULL;

	struct dlist_s *next = NULL;

	if (!bus)
		return -EINVAL;
	ddkc_info("bus:%p, name:%s, p:%p, devid:0x%x\r\n", bus, bus->name, bus->p, devid);

	p = bus->p;

	if (dlist_empty(&p->dev_list)) {
		ddkc_dbg("bus[%s]'s device list is empty\r\n", bus->name);
	}
	dlist_for_each_entry_safe(&p->dev_list, next, dev_p, u_device_private_t, bus_node) {
		dev = dev_p->dev;
		if (dev->dev_id == devid) {
			ddkc_loud("dev:%p found with devid:0x%x\r\n", dev, devid);
			break;
		}
        ddkc_loud("dev->dev_id:%d\r\n", dev->dev_id);
		dev = NULL;
	}

	return dev;
}

struct u_device *u_device_find_by_devid(dev_t devid) {
	u_bus_t *bus = NULL;
	dlist_t *pos = NULL;
	dlist_t *next = NULL;
	u_bus_private_t *p = NULL;
	struct u_device *dev = NULL;

    ddkc_dbg ("%s\r\n", __func__);
	dlist_for_each_entry_safe(&g_bus_list_head, next, p, u_bus_private_t, bus_node) {
		bus = p->bus;
		dev = u_bus_find_dev_by_id(bus, devid);
		if (dev)
			break;
	}

	ddkc_err("u_device[%p] found by devid[0x%x]\r\n", dev, devid);
	return dev;
}

#endif


/**
 * delete a driver from bus's driver list
 *
 * @param drv - pointer to the driver to be deleted
 * @return always return 0
 */
int u_bus_del_driver(struct u_driver *drv) {
	u_bus_t *bus = NULL;
	unsigned long flags;

	if (!drv) {
		ddkc_err("invalid drv:%p to delete\r\n", drv);
		return -EINVAL;
	}

	bus = drv->bus;
	spin_lock_irqsave(&bus->p->lock, flags);
	dlist_del(&drv->p->bus_node);
	spin_unlock_irqrestore(&bus->p->lock, flags);

	return 0;
}

/**
 * Try to deinit a device
 *
 * @param drv - pointer to target device's driver
 * @param dev - pointer to target device
 * @return 0 if deinit success; otherwise return negative
 */
static int u_driver_deinit_device(struct u_driver *drv, struct u_device *dev)
{
    int r = -1;

    /* attach to drv for the moment, will detach if init fails */
    ddkc_dbg("bus[%s] drv[%s] removing dev[%s]\r\n", dev->bus->name, drv->name, dev->name);

	ddkc_dbg("dev->bus->deinit:%p, drv->deinit:%p\r\n", dev->bus->deinit, drv->deinit);
    ddkc_dbg("dev->bus->remove:%p, drv->remove:%p\r\n", dev->bus->remove, drv->remove);

    /* try legacy ops */
	if (dev->bus->deinit) {
		r = dev->bus->deinit(dev);
		if (r)
			goto remove_fail;
	} else if (drv->deinit) {
		r = drv->deinit(dev);
		if (r)
			goto remove_fail;
	}

    if (dev->bus->remove) {
        r = dev->bus->remove(dev);
        if (r)
            goto remove_fail;
    } else if (drv->remove) {
        r = drv->remove(dev);
        if (r)
            goto remove_fail;
    }

    dev->drv = NULL;
    ddkc_dbg("bus[%s] drv[%s] remove dev[%s] success\r\n", dev->bus->name, drv->name, dev->p->name);

    return 0;

remove_fail:
    dev->drv = NULL;
    ddkc_dbg("bus[%s] drv[%s] remove dev[%s] fails\r\n", dev->bus->name, drv->name, dev->name);
    return -1;
}

/**
 * deattch a driver into bus's driver list
 *
 * @param drv - pointer to target driver to be deatched
 * @return always return 0
 */
int u_bus_detach_driver(struct u_driver *drv) {
	int r = 0;
	u_device_t *dev = NULL;
	u_device_private_t *dev_p = NULL;
	dlist_t *next = NULL;

    if (dlist_empty(&drv->p->dev_list)) {
        ddkc_dbg("drv[%s]'s dev_list is empty\r\n", drv->name);
        return 0;
    }
    ddkc_dbg("drv:%p, drv->p:%p, &drv->p->dev_list:%p\r\n", drv, drv->p, &drv->p->dev_list);

	/* try to remove the devices which is already attached with the driver */
	dlist_for_each_entry_safe(&drv->p->dev_list, next, dev_p, u_device_private_t, drv_node) {
		ddkc_dbg("dev:%p, dev_p:%p\r\n", dev, dev_p);
		dev = dev_p->dev;
        ddkc_dbg("dev:%p, dev_p:%p\r\n", dev, dev_p);
        r = u_driver_deinit_device(drv, dev);
		ddkc_dbg("drv[%s] remove dev[%s] %s, r:%d\r\n", drv->name, dev->name, r ? "fail" : "success", r);
	}

	return 0;
}

/**
 * detach a device into bus's driver list
 *
 * @param dev - target device to be detached
 * @return always return 0
 */
int u_bus_detach_device(struct u_device *dev) {
    int r = 0;
    u_driver_t *drv = dev->drv;

    if (!drv) {
        ddkc_dbg("dev[%s] is not attached to driver\r\n", dev->name);
        return 0;
    }

    r = u_driver_deinit_device(drv, dev);
    if (r) {
        ddkc_err("dev[%s] deatch with driver[%s] failed\r\n", dev->name, drv->name);
        return 0;
    }
    ddkc_dbg("dev[%s] deatch with driver[%s] succeed\r\n", dev->name, drv->name);

    return 0;
}

#if AOS_COMP_CLI
/**
 * command service handler - dump information of all bus/device/driver
 *
 */
static void u_bus_dump_cmd(char *wbuf, int len, int argc, char **argv) {

	ddkc_info("dumping bus/device/driver information\r\n");
	u_bus_dump_all();
	ddkc_info("dump bus/device/driver information done\r\n");

	return;
}

struct cli_command u_bus_test_cmds[] = {
	{"ubusdump",         "dump system's bus/device/driver information", u_bus_dump_cmd},
};

static int u_bus_test_cmd_init(void) {
	ddkc_dbg("registering udevice commands\r\n");
	return aos_cli_register_commands(&u_bus_test_cmds[0],
	                                 sizeof(u_bus_test_cmds)/sizeof(u_bus_test_cmds[0]));
}
#endif /* AOS_COMP_CLI */

/**
 * u_bus module init entry
 * declared with CORE_DRIVER_ENTRY, which is 1st level driver init entry
 * @return 0
 */
int u_bus_init(void) {
	#if AOS_COMP_CLI
	u_bus_test_cmd_init();
	#endif
	return u_bus_list_init();
}

CORE_DRIVER_ENTRY(u_bus_init)
