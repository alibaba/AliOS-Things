/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "aos/list.h"

#include <drivers/char/u_device.h>
#include <drivers/char/u_driver.h>
#include <drivers/char/u_bus.h>
#include <drivers/spinlock.h>

#define G_ROOT_PATH "/dev"

int g_ddkc_log_level = DDKC_LOG_WARN;
static dlist_t g_dev_node_list;
static u_dev_node_t *g_root_dev_node = NULL;

char ddkc_level_tag[] = {
	'L',
	'D',
	'I',
	'W',
	'E'
};

/**
 * dump device's information, name, id, parent, bus, driver, drv_data included
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_dump(struct u_device *dev) {

	if (!dev) {
		ddkc_err("dev is NULL\r\n");
		return -1;
	}

	ddkc_info("dev:%p, dev->name:%s, id:%d, dev_id:%d, parent:%p, bus:%p, drv:%p, drv_data:%p\r\n",
	          dev, dev->name,
	          dev->id, dev->dev_id,
	          dev->parent, dev->bus, dev->drv, dev->driver_data);

	return 0;
}

/**
 * initialize u_device's private info
 *
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_initialize(struct u_device *dev) {
	u_device_private_t *p = NULL;

	/* dev is not NULL, guaranteed by caller */
	dev->drv = NULL;

	/* device's parent and bus should be setup before call u_device_initialize */
	//dev->parent = NULL;
	//dev->bus = NULL;
	p = (u_device_private_t *)malloc(sizeof(u_device_private_t));
	if (!p) {
		ddkc_err("malloc for u_device_private_t failed\r\n");
		return -ENOMEM;
	}

	p->name = NULL;
	spin_lock_init(&p->lock);
	dlist_init(&p->bus_node);
	dlist_init(&p->drv_node);
	dlist_init(&p->child_head);
	dlist_init(&p->parent_node);
	p->dev = dev;
	dev->p = p;

	return 0;
}

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
int u_device_add(struct u_device *dev) {
	u_device_private_t *p = NULL;
	int r = 0;
	int len = 0;
	/* dev is not NULL, guaranteed by caller */

	if (!dev || !dev->p || !dev->bus) {
		ddkc_err("invalid dev:%p or dev->p:%p or dev->bus:%p\r\n", dev, dev ? dev->p : NULL, dev ? dev->bus : NULL);
		return -EINVAL;
	}

	if (!dev->bus->name || (dev->id > MAX_DEV_ID_DIGS)){
		ddkc_err("invalid bus name:%p or dev->id:%d\r\n", dev->bus->name, dev->id);
		return -EINVAL;
	}

	p = dev->p;
	len = strlen(dev->bus->name) + MAX_DEV_ID_DIGS + 1; // 1 is for '\0'
	p->name = (char *)malloc(len);
	if (!p->name) {
		ddkc_err("malloc for dev->p->name failed, len:%d\r\n", len);
		return -ENOMEM;
	}

	/* name dev's name with <bus_name><dev_id> */
    memset(p->name, 0, len);
	snprintf(p->name, len, "%s%d", dev->bus->name, dev->id);

	/* add device into its parent's child_head */
	if (dev->parent) {
        unsigned long flags;

		spin_lock_irqsave(&dev->parent->p->lock, flags);
		dlist_add_tail(&p->parent_node, &dev->parent->p->child_head);
		spin_unlock_irqrestore(&dev->parent->p->lock, flags);
	}

	/* add this device into target bus */
	r = u_bus_add_device(dev);
	if (r)
		goto dev_add_error;

	/* it is not necessary to check u_bus_try_init_device's return value */
	u_bus_try_init_device(dev);

	return 0;

dev_add_error:
	if (p->name) {
		free(p->name);
		p->name = NULL;
	}

	return r;
}

/**
 * register a device into system
 *
 * @param dev - pointer to target device
 * @return 0 for success; netative for failure
 */
int u_device_register(struct u_device *dev) {

	if (!dev) {
		ddkc_err("invalid dev:%p\r\n", dev);
		return -EINVAL;
	}

	u_device_initialize(dev);
	return u_device_add(dev);
}

/**
 * check whether a device is registered or not
 * @param dev - pointer to the device to be checked
 *
 * @return 0 for success; netative for failure
 */
int u_device_is_registered(struct u_device *dev) {
	u_device_private_t *devp = dev->p;

	if (!devp || dlist_empty(&devp->bus_node))
		return 0;
	return 1;
}

unsigned int u_dev_node_get_pathname_length(u_dev_node_t *dev_node) {
	unsigned int length = 0;
#if 0
	u_dev_node_t *parent = dev_node;

	do {
		if(!strlen(parent->name))
			break;
		length += strlen(parent->name) + 1;

		parent = dev_node->parent;
	} while (parent);
#else
	/* 2 is for '/' and '\0' */
	length = strlen(dev_node->name) + strlen(dev_node->parent->path_name) + 2;
#endif
	return length;
}

char *u_dev_node_get_pathname(u_dev_node_t *dev_node) {
	char *pathname = NULL;
	unsigned int len = 0;
	unsigned int length = u_dev_node_get_pathname_length(dev_node);

	pathname = malloc(length);
	if (!pathname) {
		ddkc_err("malloc for pathname failed, len[%d]\r\n", length);
		return NULL;
	}
	memset(pathname, 0, length);
#if 0
	u_dev_node_t *parent = dev_node;

	length -= 2;

	do {
		len = strlen(parent->name);
		if(!len)
			break;
		strncpy(pathname[length - len], parent->name, len);
		length -= len;
		pathname[length--] = '/';
		parent = dev_node->parent;
	} while (parent);
#else
	ddkc_dbg("pathname length:%d\r\n", length);
	ddkc_dbg("parent pathname:%s\r\n", dev_node->parent->path_name);
	ddkc_dbg("node pathname:%p - %s\r\n", dev_node->name, dev_node->name);
	len = strlen(dev_node->parent->path_name);
	if (len) {
		strncpy(pathname, dev_node->parent->path_name, len);
		pathname[len++] = '/';
	}
	strncpy(pathname + len, dev_node->name, strlen(dev_node->name));
#endif
	ddkc_info("pathname:%s\r\n", pathname);

	return pathname;
}

struct u_dev_node * u_device_node_create(struct u_dev_node *parent, unsigned int dev_id, void *drv_data, char *name) {
	u_dev_node_t *dev_node = NULL;

	if (!name) {
		ddkc_err("name should not be NULL\r\n");
		return NULL;
	}

	/**
	 * //TODO: need to do sanity check
	 * 1. whether dev_id is already binded with exist device node?
	 * 2. whether name is already binded to parent?
	 */

	// manage all device, registered with u_device_node_create, into one list,
	// <pathname, dev_id> is in this list
	dev_node = malloc(sizeof(u_dev_node_t) + strlen(name) + 1);
	memset(dev_node, 0, sizeof(u_dev_node_t) + strlen(name) + 1);
	dlist_init(&dev_node->node);

	dev_node->dev_id = dev_id;
	if (parent)
		dev_node->parent = parent;
	else
		dev_node->parent = g_root_dev_node;

	if (drv_data)
		dev_node->drv_data = drv_data;

	strncpy(dev_node->name, name, strlen(name));
	dev_node->name[strlen(name)] = '\0';

	// compose device pathname
	dev_node->path_name = u_dev_node_get_pathname(dev_node);

	ddkc_info("path_name:%s added\r\n", dev_node->path_name);

	// enqueue to global device node
	dlist_add(&dev_node->node, &g_dev_node_list);

	/* TODO: send IPC message to notify VFS new device node is created */

	return dev_node;
}

int u_device_node_delete(unsigned int dev_id) {
	u_dev_node_t *dev_node = NULL;
	struct dlist_s *node = NULL;

	dlist_for_each_entry_safe(&g_dev_node_list, node, dev_node, u_dev_node_t, node) {
		if (dev_node->dev_id == dev_id) {
			dlist_del(&dev_node->node);
			// free full path name memory
			if (dev_node->path_name)
				free(dev_node->path_name);
			// free device node struct
			free(dev_node);
		}
	}
	return 0;
}

#if 0
struct u_dev_node * u_device_node_find_by_devid(dev_t dev_id) {
	struct u_dev_node *parent;
	u_dev_node_t *dev_node = NULL;
	struct dlist_s *node = NULL;

	dlist_for_each_entry_safe(&g_dev_node_list, node, dev_node, u_dev_node_t, node) {
		if (dev_node->dev_id == dev_id) {
			return dev_node;
		}
	}
	return NULL;
}
#endif

struct u_dev_node * u_device_node_find_by_pathname(char *path_name) {
	dlist_t *root = NULL;
	u_dev_node_t *dev_node = NULL;

	if (!path_name || !g_root_dev_node) {
		ddkc_warn("invalid pathname[%p] or root dev node[%p]\r\n", path_name, g_root_dev_node);
		return NULL;
	}
	root = &g_root_dev_node->node;
	dlist_for_each_entry(root, dev_node, u_dev_node_t, node) {
		if (!strcmp(path_name, dev_node->path_name)) {
			ddkc_dbg("dev_node with pathname[%s] found\r\n", dev_node->path_name);
			return dev_node;
		}
	}
	ddkc_dbg("dev_node with pathname[%s] not found\r\n", path_name);
	return NULL;
}

struct u_dev_node * u_device_node_find_by_nodename(char *node_name) {
	dlist_t *root = NULL;
	u_dev_node_t *dev_node = NULL;

	if (!node_name || !g_root_dev_node) {
		ddkc_warn("invalid node_name[%p] or root dev node[%p]\r\n", node_name, g_root_dev_node);
		return NULL;
	}
	root = &g_root_dev_node->node;
	dlist_for_each_entry(root, dev_node, u_dev_node_t, node) {
		// exclude "/dev/" from dev_node->path_name
		if (!strcmp(node_name, dev_node->path_name + sizeof(G_ROOT_PATH) + 1)) {
			ddkc_dbg("dev_node with node_name[%s] found\r\n", dev_node->path_name);
			return dev_node;
		}
	}
	ddkc_dbg("dev_node with node_name[%s] not found\r\n", node_name);
	return NULL;
}

struct u_dev_node * u_device_node_find_by_dev(struct u_device *u_dev) {
	dlist_t *root = NULL;
	u_dev_node_t *dev_node = NULL;

	if (!u_dev || !g_root_dev_node) {
		ddkc_warn("invalid dev[%p] or root dev node[%p]\r\n", u_dev, g_root_dev_node);
		return NULL;
	}

	root = &g_dev_node_list;
	dlist_for_each_entry(root, dev_node, u_dev_node_t, node) {
		if (u_dev == dev_node->dev) {
			ddkc_dbg("dev_node with u_dev[%p] found\r\n", u_dev);
			return dev_node;
		}
	}
	ddkc_dbg("dev_node with u_dev[%p] not found\r\n", u_dev);
	return NULL;
}

int u_device_root_node_init(void) {
	char *root_path = G_ROOT_PATH;
	unsigned int len = sizeof(u_dev_node_t) + strlen(root_path) + 1;

	dlist_init(&g_dev_node_list);

	g_root_dev_node = malloc(len);
	if (!g_root_dev_node) {
		ddkc_err("fail to malloc for root device node\r\n");
		return -ENOMEM;
	}

	memset(g_root_dev_node, 0, len);
	dlist_init(&g_root_dev_node->node);

	strncpy(g_root_dev_node->name, root_path, strlen(root_path) + 1);
	g_root_dev_node->name[strlen(root_path)] = '\0';

	g_root_dev_node->path_name = g_root_dev_node->name;

	dlist_add(&g_root_dev_node->node, &g_dev_node_list);

	ddkc_dbg("root device node initialized\r\n");

	return 0;
}

/**
 * delete device from system
 *
 * @param dev - pointer to device to be deleted
 * @return 0 for success; negative for failure
 */
int u_device_del(struct u_device *dev) {
	if (!dev)
		return -EINVAL;

	if (dev->p) {
		unsigned long flags;
		u_driver_t *drv = NULL;
		u_device_private_t *p = dev->p;

		//dlist_del(p->bus_node); /* already done in u_bus_del_device */
		drv = dev->drv;
		if (drv && drv->p) {
			// remove drv_node from driver's device list
			u_driver_private_t *drv_p = drv->p;

			spin_lock_irqsave(&drv_p->lock, flags);
			dlist_del(&p->drv_node);
			spin_unlock_irqrestore(&drv_p->lock, flags);
		}

		spin_lock_irqsave(&dev->parent->p->lock, flags);
		dlist_del(&p->parent_node);
		// TODO: should detach all child in child_head
		//dlist_del(p->child_head);
		spin_unlock_irqrestore(&dev->parent->p->lock, flags);

		if (p->name) {
			free(p->name);
			p->name = NULL;
		}

		free(p);
		dev->p = NULL;
	}

	return 0;
}

/**
 * unregister device from system
 * @param dev - pointer to target device to be unregistered
 * @return 0 for success; negative for failure
 */
int u_device_unregister(struct u_device *dev) {
	return u_device_del(dev);
}

#if AOS_COMP_CLI
/**
 * service handler - change log level of udevice
 *
 */
static void u_device_loglevel_cmd(char *wbuf, int len, int argc, char **argv) {

    if (argc < 1) {
	    ddkc_err("arg too short, current loglevel:%d\r\n", g_ddkc_log_level);
	    return;
    }

    switch (*argv[1]) {
        case 'l':
        case 'L':
            g_ddkc_log_level = DDKC_LOG_LOUD;
            break;
        case 'd':
        case 'D':
            g_ddkc_log_level = DDKC_LOG_DEBUG;
            break;
        case 'i':
        case 'I':
            g_ddkc_log_level = DDKC_LOG_INFO;
            break;
        case 'w':
        case 'W':
            g_ddkc_log_level = DDKC_LOG_WARN;
            break;
        case 'e':
        case 'E':
	        g_ddkc_log_level = DDKC_LOG_ERROR;
            break;
        default:
            break;
    }
    ddkc_err("loglevel set to %d\r\n", g_ddkc_log_level);

    return;
}

struct cli_command u_device_test_cmds[] = {
    {"udev_loglevel",         "adjust udevice log level", u_device_loglevel_cmd},
};

static int u_devicd_test_cmd_init(void) {
	ddkc_dbg("registering udevice commands\r\n");
    return aos_cli_register_commands(&u_device_test_cmds[0],
                                     sizeof(u_device_test_cmds)/sizeof(u_device_test_cmds[0]));
}
#endif /* AOS_COMP_CLI */

int u_device_init(void) {
	#if AOS_COMP_CLI
    u_devicd_test_cmd_init();
	#endif
    return u_device_root_node_init();
}

#include "drivers/u_ld.h"
CORE_DRIVER_ENTRY(u_device_init)
