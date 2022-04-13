/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_PLATFORM_BUS_H_
#define _U_PLATFORM_BUS_H_
#include <stdbool.h>

#include "aos/list.h"

#include <drivers/char/u_device.h>
#include <drivers/char/u_driver.h>
#include <drivers/char/u_bus.h>

typedef struct u_platform_device {
	const char	*name;
	int		id;
	struct u_device	dev;

#if 0 //TODO: how to originize hardware resources
	unsigned		num_resources;
	struct u_resource	*resource;
#endif
} u_platform_device_t;

typedef struct u_platform_driver {
	/* user can select either legacy driver API or linux-like drivers */
	/* for legacy drivers */
	int (*init)(struct u_platform_device *_dev);
	int (*deinit)(struct u_platform_device *_dev);
	int (*pm)(struct u_platform_device *dev, u_pm_ops_t state);

	/* for linux-like drivers */
	int (*probe)(struct u_platform_device *);
	int (*remove)(struct u_platform_device *);
	void (*shutdown)(struct u_platform_device *);
	int (*suspend)(struct u_platform_device *, /*u_pm_ops_t*/int state);
	int (*resume)(struct u_platform_device *);

	u_driver_t driver;
	u_bus_t *bus;
} u_platform_driver_t;

#define to_u_platform_device(x) aos_container_of((x), struct u_platform_device, dev)

#define to_u_platform_driver(x) aos_container_of((x), struct u_platform_driver, driver)

static inline void *u_platform_get_user_data(const struct u_platform_device *pdev)
{
	return pdev->dev.user_data;
}

static inline void u_platform_set_user_data(struct u_platform_device *pdev,
                                        void *data)
{
	pdev->dev.user_data = data;
}

int u_platform_device_init(void);

u_platform_device_t* u_platform_device_alloc(void);
int u_platform_device_register(u_platform_device_t *dev);
void u_platform_device_del(u_platform_device_t *pdev);
int u_platform_device_unregister(u_platform_device_t *dev);

int u_platform_driver_register(u_platform_driver_t *drv);
int u_platform_driver_unregister(u_platform_driver_t *drv);

#endif //_U_PLATFORM_BUS_H_
