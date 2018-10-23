/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_OPT_H
#define BREEZE_OPT_H


#define BZ_VERSION "2.0.5"

#define BZ_ENABLE_AUTH 1
#define BZ_AUTH_TIMEOUT 10000  // not allowed to be 0

#define BZ_TRANSPORT_TIMEOUT 10000

#define BZ_TRANSPORT_VER 0
#define BZ_MAX_MTU 247
#define GATT_MTU_SIZE_DEFAULT 23
#define ENCRYPT_DATA_SIZE 16
#define BZ_MAX_PAYLOAD_SIZE 256

#define BZ_CONTEXT_SIZE 24

#ifndef BREEZE_LOG_DEBUG
#define BREEZE_LOG_DEBUG printf
#endif

#ifndef BREEZE_LOG_ERR
#define BREEZE_LOG_ERR printf
#endif

#ifndef BREEZE_LOG_INFO
#define BREEZE_LOG_INFO printf
#endif

#if defined(BLE_4_0)
#define BZ_BLUETOOTH_VER 0x00
#define BZ_MAX_SUPPORTED_MTU 23
#elif defined(BLE_4_2)
#define BZ_BLUETOOTH_VER 0x01
#define BZ_MAX_SUPPORTED_MTU 247
#elif defined(BLE_5_0)
#define BZ_BLUETOOTH_VER 0x10
#define BZ_MAX_SUPPORTED_MTU 247
#else
#error No valid target set.
#endif

#endif  // BREEZE_OPT_H
