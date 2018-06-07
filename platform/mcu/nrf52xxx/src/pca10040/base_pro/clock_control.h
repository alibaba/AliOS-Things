/* clock_control.h - public clock controller driver API */

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CLOCK_CONTROL_H__
#define __CLOCK_CONTROL_H__

#include <zephyr/types.h>
#include <stddef.h>
#include <device.h>
#include <misc/__assert.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef CONFIG_KERNEL_INIT_PRIORITY_DEVICE
#define CONFIG_KERNEL_INIT_PRIORITY_DEVICE 0
#endif


#ifndef CONFIG_CLOCK_CONTROL_NRF5_IRQ_PRIORITY
#define CONFIG_CLOCK_CONTROL_NRF5_IRQ_PRIORITY 5
#endif


/* Define 32KHz clock source */
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_RC
#define CLOCK_CONTROL_NRF5_K32SRC 0
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_XTAL
#define CLOCK_CONTROL_NRF5_K32SRC 1
#endif

/* Define 32KHz clock accuracy */
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_500PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 0
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_250PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 1
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_150PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 2
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_100PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 3
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_75PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 4
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_50PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 5
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_30PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 6
#endif
#ifdef CONFIG_CLOCK_CONTROL_NRF5_K32SRC_20PPM
#define CLOCK_CONTROL_NRF5_K32SRC_ACCURACY 7
#endif


/* Clock control API */

/* Used to select all subsystem of a clock controller */
#define CLOCK_CONTROL_SUBSYS_ALL	NULL

/**
 * clock_control_subsys_t is a type to identify a clock controller sub-system.
 * Such data pointed is opaque and relevant only to the clock controller
 * driver instance being used.
 */
typedef void *clock_control_subsys_t;

typedef int (*clock_control)(struct device *dev, clock_control_subsys_t sys);

typedef int (*clock_control_get)(struct device *dev,
				 clock_control_subsys_t sys,
				 u32_t *rate);

struct clock_control_driver_api {
	clock_control		on;
	clock_control		off;
	clock_control_get	get_rate;
};

/**
 * @brief Enable the clock of a sub-system controlled by the device
 * @param dev Pointer to the device structure for the clock controller driver
 * 	instance
 * @param sys A pointer to an opaque data representing the sub-system
 */
static inline int clock_control_on(struct device *dev,
				   clock_control_subsys_t sys)
{
	const struct clock_control_driver_api *api = dev->driver_api;

	return api->on(dev, sys);
}

/**
 * @brief Disable the clock of a sub-system controlled by the device
 * @param dev Pointer to the device structure for the clock controller driver
 * 	instance
 * @param sys A pointer to an opaque data representing the sub-system
 */
static inline int clock_control_off(struct device *dev,
				    clock_control_subsys_t sys)
{
	const struct clock_control_driver_api *api = dev->driver_api;

	return api->off(dev, sys);
}

/**
 * @brief Obtain the clock rate of given sub-system
 * @param dev Pointer to the device structure for the clock controller driver
 *        instance
 * @param sys A pointer to an opaque data representing the sub-system
 * @param[out] rate Subsystem clock rate
 */
static inline int clock_control_get_rate(struct device *dev,
					 clock_control_subsys_t sys,
					 u32_t *rate)
{
	const struct clock_control_driver_api *api = dev->driver_api;

	__ASSERT(api->get_rate, "%s not implemented for device %s",
		__func__, dev->config->name);

	return api->get_rate(dev, sys, rate);
}

#ifdef __cplusplus
}
#endif

#endif /* __CLOCK_CONTROL_H__ */
