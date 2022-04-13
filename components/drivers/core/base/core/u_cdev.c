/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>

#include "aos/list.h"

#include <drivers/char/u_cdev.h>
#include <drivers/char/u_device.>

int g_cdev_log_level = CDEV_LOG_DEBUG;

//TODO: need to add lock to protect operations on g_cdev_list_head
dlist_t g_cdev_list_head;

#ifdef ENABLE_U_CHAR_DEV_DUMP
static int u_cdev_dump(u_cdev_t *dev);
static int u_cdev_dump_all(void);

/**
 * 1. 怎样生成path name , 像ttyS0这种index怎样带进去？ device_add -> kobject_event(&dev->kobj, KOBJ_ADD)
 * 2. 当open操作到来的时候，回传一个handle object来记录thread, cdev, major no./minor no.等信息
 * 3. 后续read/write, 用handle找到对应的device
 * */
int u_cdev_dump(u_cdev_t *dev) {
	cdev_info("dev[%p] dev_id[%x] count[%d], ops[%p]\r\n", dev, dev->dev_id, dev->count, dev->ops);
	return 0;
}

int u_cdev_dump_all(void) {
	u_cdev_t *dev_p = NULL;

	/* add to global cdev list */
	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		u_cdev_dump(dev_p);
	}

	return 0;
}
#else
#define u_cdev_dump(x)
#define u_cdev_dump_all()
#endif

/**
 * u_cdev_init - u_cdev_t struct init and add file operation hook
 *
 * @dev: pointer to target char dev to be initialized
 * @fops: file operation struct hook to this char device
 * */
int u_cdev_init(u_cdev_t *dev, /*struct u_file_operations*/void *fops) {

	if (!dev || !fops) {
		cdev_err("invalid dev:%p or fops:%p\r\n", dev, fops);
		return -EINVAL;
	}
	/** check whether dev->ops is already assigned,
	 * if yes, it must be same with fops; return -EINVAL otherwise
	 * */
	if (dev->ops) {
		if (dev->ops != fops) {
			cdev_err("dev->ops:%p != fops:%p, ignore\r\n", dev->ops, fops);
			return -EINVAL;
		} else {
			cdev_warn("dev->ops is already assigned, ignore\r\n", dev->ops);
			return 0;
		}

	}
	dev->ops = fops;
	dev->thread_ctx = NULL;
	dlist_init(&dev->node);

	cdev_dbg("dev:%p init done, id:%d, fops:%p\r\n", dev, dev->dev_id, fops);
	return 0;
}

/**
 * u_cdev_add - add a new char dev into a global char device list
 * @dev: pointer to target char dev to be added
 * @devid: target char char device's device id
 * @count: minor device number
 * */
int u_cdev_add(u_cdev_t *dev, unsigned int devid, unsigned int count) {
	u_cdev_t *dev_p = NULL;

	if (!dev || !MAJOR(devid)) {
		cdev_err("invalid dev:%p, or devid:%x\r\n", dev, devid);
		return -EINVAL;
	}
	dev->dev_id = devid;
	dev->count = count;

	u_cdev_dump(dev);

	/* add to global cdev list with sorted sequence */
	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		if (devid > dev_p->dev_id)
			continue;

		if (devid == dev_p->dev_id) {
			if (dev_p->ops) {
				cdev_err("same devid[%x], different ops[%p, %p], fail to add cdev\r\n",
				         devid, dev_p->ops, dev->ops);
				return -EBUSY;
			} else {
				dev->thread_ctx = pthread_self();
				dlist_add(&dev->node, &dev_p->node); // add the new one
				dlist_del(&dev_p->node);      // delete the old one
				free(dev_p);
				cdev_dbg("same devid:%x found, replace it, thread_ctx:%p\r\n", devid, dev->thread_ctx);
				u_cdev_dump_all();
				return 0;
			}
		}

		dev->thread_ctx = pthread_self();
		dlist_add_tail(&dev->node, &dev_p->node);
		cdev_dbg("devid:%x added, thread_ctx:%p\r\n", devid, dev->thread_ctx);
		u_cdev_dump_all();

		return 0;
	}
	dev->thread_ctx = pthread_self();
	dlist_add_tail(&dev->node, &g_cdev_list_head);
	cdev_dbg("devid:%x added, thread_ctx:%p\r\n", devid, dev->thread_ctx);

	u_cdev_dump_all();

	return 0;
}

u_cdev_t * u_cdev_find_node_by_name(char *pathname) {
	u_cdev_t *dev_p = NULL;
	u_dev_node_t *dev_node = NULL;
	unsigned int dev_id = 0;

	if (!strncmp(pathname, "/dev/", sizeof("/dev/")))
		dev_node = u_device_node_find_by_pathname(pathname);
	else
		dev_node = u_device_node_find_by_nodename(pathname);

	if (!dev_node) {
		cdev_err("device node with name[%s] not found\r\n", pathname);
		return NULL;
	}

	dev_id = dev_node->dev_id;

	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		if (dev_id == dev_p->dev_id) {
			cdev_dbg("cdev found for %s, devid:%d\r\n", pathname, dev_id);
			return dev_p;
		}
	}
	cdev_dbg("cdev not found for %s\r\n", pathname);

	return NULL;
}


u_cdev_t * u_cdev_find_node_by_devid(unsigned int devid) {
	u_cdev_t *dev_p = NULL;

	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		if (devid == dev_p->dev_id) {
			cdev_dbg("cdev found for devid:%d\r\n", devid);
			return dev_p;
		}
		cdev_loud("u_cdev:%p, dev_id:0x%x\r\n", dev_p, devid);
	}
	cdev_warn("cdev not found for %d\r\n", devid);

	return NULL;
}

int u_cdev_del(u_cdev_t *dev) {
	int r = -EBUSY;
	u_cdev_t *dev_p = NULL;

	if (!dev) {
		cdev_err("invalid dev:%p\r\n", dev);
		return -EINVAL;
	}

	/* remove cdev from global cdev list */
	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		// TODO: check whether device is in use or not
		if (dev_p == dev) {
			dlist_del(&dev->node);
			r = 0;
			break;
		}
	}

	cdev_dbg("dev[%p] with dev_id[%d] is %s\r\n", dev, dev->dev_id, !r ? "removed" : "not found");

	return r;
}

/**
 *
 * @param major: major number of the device to be allocated
 * @param minor: start minor number of the device to bea located
 * @param count: subdevice number
 * @return major number of the device if success; negative number if fails
 */

int __register_u_cdev_region(unsigned int major,
                                    unsigned int minor,
                                    int count)
{
	// TODO: should check return value of u_cdev_add
	int last_maj = 0;
	int last_min = 0;
	int last_count = 0;
	u_cdev_t *dev_p = NULL;
	u_cdev_t *new = NULL;

	new = malloc(sizeof(*new));
	if (!new) {
		cdev_err("malloc for u_cdev_t failed\r\n");
		return -ENOMEM;
	}
	memset(new, 0, sizeof(*new));
	dlist_init(&new->node);


	/* dev_id in g_cdev_list_head is ordered */
	if(dlist_empty(&g_cdev_list_head)) {
		/* major starts from 1 */
		// TODO: define DYN_MAJOR_NO_START 256 ?
		major = major ? major : 1;
		u_cdev_add(new, MKDEV(major, minor), count);
		cdev_dbg("empty, add 1st dev, dev:%p, devid:0x%x, count:%d, ops:%p\r\n",
		         new, new->dev_id, count, new->ops);
		return major;
	}

	dlist_for_each_entry_reverse(dev_p, &g_cdev_list_head, node, u_cdev_t) {
		last_maj = MAJOR(dev_p->dev_id);
		last_min = MINOR(dev_p->dev_id);
		last_count = dev_p->count;
		cdev_dbg("last_maj:%d, last_min:%d, last_count:%d\r\n", last_maj, last_min, last_count);
		if (!major) {
			major = last_maj + 1;
			u_cdev_add(new, MKDEV(major, minor), count);
			cdev_dbg("add dev, dev:%p, devid:0x%x, count:%d, ops:%p\r\n",
			         new, new->dev_id, count, new->ops);
			return major;
		}

		if (major > last_maj) {
			u_cdev_add(new, MKDEV(major, minor), count);
			cdev_dbg("add dev:%p, devid:0x%x, count:%d, ops:%p\r\n",
			         new, new->dev_id, count, new->ops);
			return major;
		}

		if (major == last_maj) {
#if 1
			if (MKDEV(major, minor) >= dev_p->dev_id + last_count)
				continue; // check whether there's multiple cdev with the same major number
			else if (MKDEV(major, minor) + count <= dev_p->dev_id)
				continue; // check whether there's multiple cdev with the same major number
			else
				return -EINVAL;
#else
			cdev_err("major id:%d equls with already existed device, not allowed\r\n", major);
			free(new);
			new = NULL;
			return -EINVAL; // not allowed
#endif
		}
	}
	u_cdev_add(new, MKDEV(major, minor), count);
	cdev_dbg("empty, add 1st dev, dev:%p, devid:0x%x, count:%d, ops:%p\r\n",
	         new, new->dev_id, count, new->ops);
	return major;
}

/**
 *
 * @param dev_id: device id of the device if the region is allocated successfully
 * @param minor: start minor number
 * @param count: subdevice number
 * @return major number if success, otherwise return negative number
 */
int alloc_u_cdev_region(unsigned int *dev_id, unsigned int minor, unsigned int count)
{
	int major = -1;

	if (!count || !dev_id) {
		cdev_err("invalid count[%d]\r\n", count);
		return -EINVAL;
	}

	major = __register_u_cdev_region(0, minor, count);
	if (major <= 0) {
		cdev_err("fail to alloc major number, major:%d\r\n", major);
		return -1;
	}
	*dev_id = MKDEV(major, minor);
	cdev_loud("major:%d, minor:%d, dev_id:%d, sizeof(*dev_id):%d\r\n", major, minor, *dev_id, sizeof(*dev_id));

	return major;
}

int register_u_cdev_region(unsigned dev_id, unsigned count) {
	int r = __register_u_cdev_region(MAJOR(dev_id), MINOR(dev_id), count);

	if (r == MAJOR(dev_id))
		return 0;
	else {
		cdev_err("register cdev region fails, r:%d\r\n", r);
		return -1;
	}
}

int unregister_u_cdev_region(unsigned dev_id, unsigned count) {
	u_cdev_t *dev_p = NULL;

	dlist_for_each_entry(&g_cdev_list_head, dev_p, u_cdev_t, node) {
		/* only check major number, ignore minor number */
		if (/* MAJOR(dev_id) == MAJOR(dev_p->dev_id)*/dev_id == dev_p->dev_id) {
			cdev_dbg("%p with devid:0x%x is removed from global char dev list\r\n",
			         dev_p, dev_p->dev_id);
			dlist_del(&dev_p->node);      // remove it from the list
			free(dev_p);
			break;
		}
	}

	dev_p = NULL;
	u_cdev_dump_all();

	return 0;
}


int u_cdev_module_init(void) {

	dlist_init(&g_cdev_list_head);
	return 0;
}

#include "drivers/u_ld.h"
CORE_DRIVER_ENTRY(u_cdev_module_init)
