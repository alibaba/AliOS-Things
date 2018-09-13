/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __XRADIO_PM_H
#define __XRADIO_PM_H

#include <stddef.h>
#include <stdint.h>
#include "sys/list.h"

#ifdef __CONFIG_BOOTLOADER
#else
#define CONFIG_PM
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defined all supported low power state.
 * @note PM_MODE_HIBERNATION is not used.
 */
enum suspend_state_t {
	PM_MODE_ON              = 0,
	PM_MODE_SLEEP           = 1,
	PM_MODE_STANDBY         = 2,
	PM_MODE_HIBERNATION     = 3,
	PM_MODE_POWEROFF        = 4,
	PM_MODE_MAX             = 5,
};

/** @brief Platform pm mode support. */
#define PM_SUPPORT_SLEEP        (1<<PM_MODE_SLEEP)
#define PM_SUPPORT_STANDBY      (1<<PM_MODE_STANDBY)
#define PM_SUPPORT_HIBERNATION  (1<<PM_MODE_HIBERNATION)
#define PM_SUPPORT_POWEROFF     (1<<PM_MODE_POWEROFF)

/** @brief Suspend test levels. */
enum suspend_test_level_t {
	TEST_NONE,              /* keep first */
	TEST_CORE,
	TEST_PLATFORM,
	TEST_DEVICES,
	__TEST_AFTER_LAST       /* keep last */
};

/** @brief Wlan power on/off callback. */
typedef int (*pm_wlan_power_onoff)(unsigned int enable);

enum pm_op_t {
	PM_OP_NORMAL = 0,
	PM_OP_NOIRQ,
	PM_OP_NUM,
};

struct soc_device;

/**
 * @brief The basic device driver structure.
 * @suspend:    Called to put the device to sleep mode. Usually to a
 *              low power state.
 * @resume:     Called to bring a device from sleep mode.
 * @suspend_noirq:
 *  1. The device will be in a low-power state after suspend_noirq() has
 *      returned successfully.
 *  2. If the device can generate system wakeup signals and is enabled to wake
 *      up the system, it should be configured to do so at that time.
 * @resume_noirq:
 *  1. Handle device wakeup signal if device can generate system wakeup signals.
 *  2. Resume device to work mode.
 */
struct soc_device_driver {
	const char *name;                       /* name of the device driver. */
	//const struct soc_device *dev;

	int (*suspend)(struct soc_device *dev, enum suspend_state_t state);
	int (*resume)(struct soc_device *dev, enum suspend_state_t state);

	int (*suspend_noirq)(struct soc_device *dev, enum suspend_state_t state);
	int (*resume_noirq)(struct soc_device *dev, enum suspend_state_t state);
};

/**
 * @brief The basic device structure.
 * @note For devices on custom boards, as typical of embedded and SOC based
 *   hardware, You uses platform_data to point to board-specific structures
 *   describing devices and how they are wired.
 */
struct soc_device {
	struct list_head node[PM_OP_NUM];
	unsigned int ref;

	const char *name;                       /* initial name of the device */

	struct soc_device_driver *driver;       /* which driver has allocated this device */
	void *platform_data;                    /* Platform specific data, device core doesn't touch it */
};

#ifdef CONFIG_PM
/**
 * @brief Register a set of system core operations.
 * @note Not use printf for this func maybe called very earlier.
 * @param dev:
 *        @arg dev->Device will be registered.
 * @retval  0 if success or other if failed.
 */
extern int pm_register_ops(struct soc_device *dev);

/**
 * @brief Unregister a set of system core operations.
 * @param dev:
 *        @arg dev->Device will be unregistered.
 * @retval  0 if success or other if failed.
 */
extern int pm_unregister_ops(struct soc_device *dev);

/**
 * @brief Set a magin to synchronize with net.
 */
extern void pm_set_sync_magic(void);

/**
 * @brief Set system to a lowpower mode.
 * @param state:
 *        @arg state->The lowpower mode will enter.
 * @retval  0 if success or other if failed.
 */
extern int pm_enter_mode(enum suspend_state_t state);

/**
 * @brief Initialize the PM-related part of a device object.
 * @note not use printf for this fun is called very earlier.
 * @retval  0 if success or other if failed.
 */
extern int pm_init(void);

/**
 * @brief Set suspend test level.
 * @param level:
 *        @arg level->Suspend will exit when run up to setted level.
 */
extern void pm_set_test_level(enum suspend_test_level_t level);

/**
 * @brief Set delay ms in debug mode.
 * @note To prevent mutual interference between devices.
 * @param ms:
 *        @arg ms->The delayed ms between two devices.
 */
extern void pm_set_debug_delay_ms(unsigned int ms);

/** @brief Show suspend statistic info. */
extern void pm_stats_show(void);

/**
 * @brief Select pm modes used on this platform.
 * @note Select modes at init for some modes are not used on some platforms.
 *        This will prevent enter unselected modes.
 * @param select:
 *        @arg select->The selected modes set.
 */
extern void pm_mode_platform_select(unsigned int select);

/**
 * @brief register wlan power on/off callback.
 * @note Wlan power on/off calback will be called when pm enter select modes.
 * @param wlan_power_cb:
 *        @arg wlan_power_cb->Wlan power on/off calback.
 * @param select:
 *        @arg select->The selected modes set.
 * retval  0 if success or other if failed.
 */
extern int pm_register_wlan_power_onoff(pm_wlan_power_onoff wlan_power_cb,
                                        unsigned int select);

/** @brief unregister wlan power on/off callback. */
extern void pm_unregister_wlan_power_onoff(void);

extern int pm_test(void);
#else /* CONFIG_PM */
static inline int pm_register_ops(struct soc_device *dev) { return 0; }
static inline int pm_unregister_ops(struct soc_device *dev) { return 0; }
static inline void pm_set_sync_magic(void) { ; }
static inline int pm_enter_mode(enum suspend_state_t state) { return 0; }
static inline int pm_init(void) { return 0;}
static inline void pm_set_test_level(enum suspend_test_level_t level) {;}
static inline void pm_set_debug_delay_ms(unsigned int ms) { ; }
static inline void pm_stats_show(void) {;}
static inline void pm_mode_platform_select(unsigned int select) {;}
static inline int pm_register_wlan_power_onoff(pm_wlan_power_onoff wlan_power_cb,
                                               unsigned int select) { return 0; }
static inline int pm_test(void) { return 0; }
#endif /* CONFIG_PM */

#ifdef __cplusplus
}
#endif

#endif
