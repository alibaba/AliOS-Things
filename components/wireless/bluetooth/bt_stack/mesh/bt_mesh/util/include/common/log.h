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

#include <zephyr.h>

typedef struct {
    u8_t  val[6];
} bt_addr_t;

typedef struct {
    u8_t      type;
    bt_addr_t a;
} bt_addr_le_t;

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#include <ulog/ulog.h>
#define TAG "[Mesh]"

#define BT_DBG(fmt, ...)                    \
    if (BT_DBG_ENABLED) {                   \
        LOGD(TAG,"[%s]" fmt, __func__,   \
                ##__VA_ARGS__);             \
    }

#define BT_ERR(fmt, ...) LOGE(TAG, fmt, ##__VA_ARGS__)
#define BT_WARN(fmt, ...) LOGW(TAG, fmt, ##__VA_ARGS__)
#define BT_INFO(fmt, ...) LOGI(TAG, fmt, ##__VA_ARGS__)
#else
#define BT_DBG(fmt, ...)   
#define BT_ERR(fmt, ...)
#define BT_WARN(fmt, ...)
#define BT_INFO(fmt, ...)
#endif

#define MESH_MSG_TAG "[MESH] "
#define MESH_MSG_RX(fmt, ...)
#define MESH_MSG_RX_BUFF(fmt, ...)
#define MESH_MSG_TX(fmt, ...)
#define MESH_MSG_TX_BUFF(fmt, ...)

#ifndef BT_PRT
#define BT_PRT(fmt, ...)
#endif



/* This helper is only available when BT_DEBUG is enabled */
const char *bt_hex(const void *buf, size_t len);

u8_t stringtohex(char *str, u8_t *out, u8_t count);
void hextostring(const uint8_t *source, char *dest, int len);

/* These helpers are only safe to be called from internal threads as they're
 * not multi-threading safe
 */
const char *bt_addr_str(const bt_addr_t *addr);
const char *bt_addr_le_str(const bt_addr_le_t *addr);


#ifdef __cplusplus
}
#endif

#endif /* __BT_LOG_H */

