/** @file
 *  @brief Bluetooth subsystem logging helpers.
 */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_LOG_H
#define __BT_LOG_H

#include <ble_os.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/hci.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BT_DBG_ENABLED)
#define BT_DBG_ENABLED 1
#endif

#if defined(CONFIG_BT_DEBUG_LOG)

#if !defined(SYS_LOG_DOMAIN)
#define SYS_LOG_DOMAIN "bt"
#endif
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG

#define BT_DBG(fmt, ...) \
	if (BT_DBG_ENABLED) { \
		SYS_LOG_DBG("(%p) " fmt, k_current_get(), \
			    ##__VA_ARGS__); \
	}

#define BT_ERR(fmt, ...) SYS_LOG_ERR(fmt, ##__VA_ARGS__)
#define BT_WARN(fmt, ...) SYS_LOG_WRN(fmt, ##__VA_ARGS__)
#define BT_INFO(fmt, ...) SYS_LOG_INF(fmt, ##__VA_ARGS__)

/* Enabling debug increases stack size requirement considerably */
#define BT_STACK_DEBUG_EXTRA	300
#else
#include <ulog/ulog.h>

#define BT_DBG(fmt, ...)    \
    if (BT_DBG_ENABLED) { \
        LOGD("[DBG]",fmt"\n", ##__VA_ARGS__); \
    }
#define BT_ERR(fmt, ...)    LOGE("[ERR]",fmt"\n", ##__VA_ARGS__)
#define BT_WARN(fmt, ...)   LOGW("[WARN]",fmt"\n", ##__VA_ARGS__)
#define BT_INFO(fmt, ...)   LOGI("[INFO]",fmt"\n", ##__VA_ARGS__)

#define BT_STACK_DEBUG_EXTRA	0

#endif

#define BT_ASSERT(cond) if (!(cond)) { \
				BT_ERR("assert: '" #cond "' failed"); \
				k_oops(); \
			}
#define BT_ASSERT_MSG(cond, fmt, ...) if (!(cond)) { \
				BT_ERR(fmt, ##__VA_ARGS__); \
				BT_ERR("assert: '" #cond "' failed"); \
				k_oops(); \
			}

#define BT_HEXDUMP_DBG(_data, _length, _str) \
		aos_log_hexdump(_str, (char *)_data, _length)

/* NOTE: These helper functions always encodes into the same buffer storage.
 * It is the responsibility of the user of this function to copy the information
 * in this string if needed.
 *
 * NOTE: These functions are not thread-safe!
 */
const char *bt_hex_real(const void *buf, size_t len);
const char *bt_addr_str_real(const bt_addr_t *addr);
const char *bt_addr_le_str_real(const bt_addr_le_t *addr);
const char *bt_uuid_str_real(const struct bt_uuid *uuid);

/* NOTE: log_strdup does not guarantee a duplication of the string.
 * It is therefore still the responsibility of the user to handle the
 * restrictions in the underlying function call.
 */
#define bt_hex(buf, len) log_strdup(bt_hex_real(buf, len))
#define bt_addr_str(addr) log_strdup(bt_addr_str_real(addr))
#define bt_addr_le_str(addr) log_strdup(bt_addr_le_str_real(addr))
#define bt_uuid_str(uuid) log_strdup(bt_uuid_str_real(uuid))

#ifdef __cplusplus
}
#endif

#endif /* __BT_LOG_H */
