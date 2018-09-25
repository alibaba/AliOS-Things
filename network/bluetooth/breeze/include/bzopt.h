/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_OPT_H
#define BREEZE_OPT_H

#define BZ_ENABLE_AUTH 1
#define BZ_AUTH_TIMEOUT 10000

#define BZ_TRANSPORT_TIMEOUT 10000

#define BZ_MAX_MTU 247

#ifndef BREEZE_LOG_DEBUG
#define BREEZE_LOG_DEBUG printf
#endif

#if defined(BLE_4_0)
#define BZ_BLUETOOTH_VER 0x00
#define BZ_MAX_SUPPORTED_MTU 23
#define BZ_CONTEXT_SIZE 612
#elif defined(BLE_4_2)
#define BZ_BLUETOOTH_VER 0x01
#define BZ_MAX_SUPPORTED_MTU 247
#define BZ_CONTEXT_SIZE 612
#elif defined(BLE_5_0)
#define BZ_BLUETOOTH_VER 0x10
#define BZ_MAX_SUPPORTED_MTU 247
#define BZ_CONTEXT_SIZE 612
#else
#error No valid target set.
#endif

#endif  // BREEZE_OPT_H
