/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/list.h"

#include <drivers/u_ld.h>
#include <drivers/char/u_device.h>
#include <drivers/char/u_driver.h>
#include <drivers/char/u_bus.h>
#include <drivers/platform/u_platform_bus.h>
#include <drivers/u_device_info.h>
#include <drivers/u_module.h>

static int u_platform_bus_match(struct u_device *dev, struct u_driver *drv);
static int u_platform_bus_init(struct u_device *_dev);
static int u_platform_bus_deinit(struct u_device *_dev);
static int u_platform_bus_pm(struct u_device *_dev, u_pm_ops_t state);

u_bus_t u_platform_bus = {
	.name = "u_platform",
	.p = NULL,
	// legay driver ops
	.match = u_platform_bus_match,
	.init = u_platform_bus_init,
	.deinit = u_platform_bus_deinit,
	.pm = u_platform_bus_pm,

	// linux-like driver ops
	.probe = NULL,
	.remove = NULL,
	.shutdown = NULL,
	.resume =  NULL,

	// heartbeat ops
	.on_ping = NULL,
};

static u_platform_device_t u_platform_root_dev = {
	.name = "u_platform",

};

/**
 * try init u_platform device
 *
 * @param _dev - pointer to low level device struct
 * @return 0 for success; negatiive for failure
 */
static int u_platform_bus_init(struct u_device *_dev) {
	int ret = 0;
	struct u_platform_driver *drv = NULL;
	struct u_platform_device *dev = to_u_platform_device(_dev);

	drv = to_u_platform_driver(_dev->drv);

	ddkc_dbg("dev:%p, _dev->driver:%p, drv:%p\r\n", dev, _dev->drv, drv);

	/* driver must provide either init or probe functions, otherwise return fail
	 * try init cb prior to probe cb
	 * */
	if (drv->init) {
		ret = drv->init(dev);
	} else if (drv->probe) {
		ret = drv->probe(dev);
	} else
		ret = -1;

	/* reset _dev->drv to NULL if init procedure fails */
	if (ret)
		_dev->drv = NULL;

	ddkc_dbg("ret:%d\r\n", ret);

	return ret;
}

/**
 * deinit u_platform device
 *
 * @param _dev - pointer to low level device struct
 * @return 0 for success; negatiive for failure
 */
static int u_platform_bus_deinit(struct u_device *_dev) {
	int ret = 0;
	struct u_platform_driver *drv = NULL;
	struct u_platform_device *dev = to_u_platform_device(_dev);

    drv = to_u_platform_driver(_dev->drv);
	ddkc_dbg("dev:%p, _dev->drv:%p, drv:%p\r\n", dev, _dev->drv, drv);

	if (drv->deinit) {
		ret = drv->deinit(dev);
	} else if (drv->remove)
		ret = drv->remove(dev);

	ddkc_dbg("ret:%d\r\n", ret);

	return 0;
}

/**
 * do power managerment of target device
 *
 * @param _dev - pointer to low level device struct
 * @param state
 * @return 0 for success; negatiive for failure
 */
static int u_platform_bus_pm(struct u_device *_dev, u_pm_ops_t state) {
	int ret = 0;
	struct u_platform_driver *drv = NULL;
	struct u_platform_device *dev = to_u_platform_device(_dev);

    drv = to_u_platform_driver(_dev->drv);
	ddkc_dbg("dev:%p, _dev->drv:%p, drv:%p, state:%d\r\n", dev, _dev->drv, drv, state.event);

	/* try legacy pm prior to linux-like driver suspend/resume callback */
	if (drv->pm) {
		ret = drv->pm(dev, state);
	} else {
		if (drv->resume && (state.event == E_PM_EXIT)) {
			ret = drv->resume(dev);
		} else if (drv->suspend && (state.event != E_PM_EXIT)) {
			ret = drv->suspend(dev, state.event);
		} else
			ret = -EINVAL;
	}

	ddkc_dbg("ret:%d\r\n", ret);
	return ret;
}

/**
 * try to match device with driver
 *
 * @param dev - pointer to low level device struct
 * @param drv - pointer to low level driiver struct
 * @return 1 for success; 0 for failure
 */
static int u_platform_bus_match(struct u_device *dev, struct u_driver *drv) {
	struct u_platform_device *pdev = to_u_platform_device(dev);
	struct u_platform_driver *pdrv = to_u_platform_driver(drv);

	ddkc_dbg("dev:%p, drv:%p, pdev:%p, pdrv:%p, pdev->name:%p, drv->name:%p\r\n", dev, drv, pdev, pdrv, pdev->name, drv->name);
    if (!pdev->name || !drv->name) {
        return 0;
    }
	ddkc_dbg("pdev->name:%s, drv->name:%s\r\n", pdev->name, drv->name);

    /* check whether device's name match with driver's name or not */
	return (strcmp(pdev->name, drv->name) == 0);
}

/**
 * register device into system
 * @param dev - pointer to low level device struct
 * @return 0 for success; negatiive for failure
 */
int u_platform_device_register(u_platform_device_t *dev) {
	int r = -1;

	ddkc_loud("u_platform bus device register start\r\n");
	if (dev->dev.bus && dev->dev.bus != &u_platform_bus) {
		ddkc_err("bus->bus[%p] with name[%s] does not equal to u_platform_bus:%p\r\n",
		             dev->dev.bus, dev->dev.bus->name, &u_platform_bus);
		return -EINVAL;
	} else
		dev->dev.bus = &u_platform_bus;

	if (!dev->dev.parent && dev != &u_platform_root_dev)
		dev->dev.parent = &u_platform_root_dev.dev;

	dev->dev.name = (char *)dev->name;
	r = u_device_register(&dev->dev);

	if (!r) {
		// TODO: need to register to DM
		ddkc_dbg("ALERT!!! this bypass device manager process, need to register to DM!!\r\n");
        ddkc_dbg("dev[%s] register with bus[%s] succeed\r\n", dev->name, u_platform_bus.name);
    } else {
        ddkc_err("dev[%s] register with bus[%s] failed, ret:%d\r\n", dev->name, u_platform_bus.name, r);
	}
	return r;
}

/**
 * register driver into system
 * @param drv - pointer to low level driver struct
 * @return 0 for success; negatiive for failure
 */
int u_platform_driver_register(u_platform_driver_t *drv) {
	int r = -1;

	if (!drv) {
		ddkc_err("invalid drv\r\n");
		return -EINVAL;
	}

	if (drv->bus && drv->bus != &u_platform_bus) {
		ddkc_err("drv->bus[%p] with name[%s] does not equal to u_platform_bus:%p\r\n",
		             drv->bus, drv->bus->name, &u_platform_bus);
		return -EINVAL;
	} else
		drv->bus = &u_platform_bus;

	/* assign u_platform bus's init/deinit/pm callback to lowlevel driver's ops */
	drv->driver.bus = &u_platform_bus;
	drv->driver.init = u_platform_bus_init;
	drv->driver.deinit = u_platform_bus_deinit;
	drv->driver.pm = u_platform_bus_pm;

	/* assign driver probe/remove, etc. to lower level driver's probe/remove, etc. */
	drv->driver.probe = u_platform_bus_init;
	drv->driver.remove = u_platform_bus_deinit;
	drv->driver.on_ping = NULL;

	/* register driver to system's device-driver model */
	r = u_driver_register(&drv->driver);
	ddkc_dbg("drv[%s] register with bus[%s] %s\r\n", drv->driver.name, u_platform_bus.name, !r ? "success" : "fail");

	return r;
}

/**
 * unregister device from system
 * @param dev - pointer to low level device struct
 * @return 0 for success; negatiive for failure
 */
int u_platform_device_unregister(u_platform_device_t *dev) {
	if (!dev)
		return -EINVAL;
	u_bus_del_device(&dev->dev);
	return u_device_unregister(&dev->dev);
}

/**
 * unregister device from system
 * @param drv - pointer to low level driver struct
 * @return 0 for success; negatiive for failure
 */
int u_platform_driver_unregister(u_platform_driver_t *drv) {
	if (!drv)
		return -EINVAL;

	return u_driver_unregister(&drv->driver);
}

/**
 * delete u_platform_device from system
 * @param pdev - pointer to target u_platform_device
 *
 */
void u_platform_device_del(u_platform_device_t *pdev) {
	if (pdev) {
		u_device_unregister(&pdev->dev);
	}

	return;
}

/**
 * allocate memory for u_platform_device - for device-tree function in the future
 *
 * @return pointer to the u_platform_device
 */
u_platform_device_t* u_platform_device_alloc(void) {
	u_platform_device_t* new = (u_platform_device_t *)malloc(sizeof(u_platform_device_t));

	if (!new) {
		ddkc_err("malloc for u_platform_device_t failed\r\n");
		return NULL;
	}
	memset(new, 0, sizeof(u_platform_device_t));

	u_device_initialize(&new->dev);

	return new;

}

/**
 * module init entry of u_platform_bus
 * declared with BUS_DRIVER_ENTRY, which is 2nd level driver init entry
 * @return 0 for success; negatiive for failure
 */
static int u_platform_init(void)
{
	int r = 0;

	ddkc_dbg("u_platform_bus init\r\n");
	r = u_bus_register(&u_platform_bus);
	if (r) {
		ddkc_err("u_platform_bus init failed, ret:%d\r\n", r);
		return r;
	}

	r = u_platform_device_register(&u_platform_root_dev);
	if (r) {
		ddkc_err("u_platform_device_register failed, ret:%d\r\n", r);
		u_bus_unregister(&u_platform_bus);
	}

	return r;
}

#if 0
u_bus_t *u_get_platform_bus(void) {
	return &u_platform_bus;
}

u_driver_bin_info_t* u_aos_get_driver_info (void) {
	static u_driver_bin_info_t g_drv_info;

	ddkc_info("u_aos_get_driver_info\r\n");
	g_drv_info.name = u_platform_bus.name;
	g_drv_info.type = DRV_TYPE_BUS;
	g_drv_info.bus_type = BUS_TYPE_PLATFORM;
	return &g_drv_info;
}

static u_bus_t *get_current_bus(void) {
	return &u_platform_bus;
}
#endif

BUS_DRIVER_ENTRY(u_platform_init)

