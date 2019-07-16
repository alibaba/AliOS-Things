/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_OPT_H
#define BREEZE_OPT_H

/*update to version 2.0.6 or higher if enable offline authentation.
 * otherwise should not use this version to avoid compatibility issue.*/
#ifdef EN_AUTH_OFFLINE
#define BZ_VERSION "2.0.6"
#else
#define BZ_VERSION "2.0.5"
#endif

#define BZ_AUTH_TIMEOUT 10000  // not allowed to be 0

#define BZ_TRANSPORT_TIMEOUT 10000

#define BZ_TRANSPORT_VER 0
#define BZ_MAX_MTU 247
#define GATT_MTU_SIZE_DEFAULT 23
#define ENCRYPT_DATA_SIZE 16
#define BZ_MAX_PAYLOAD_SIZE 256

#ifdef  EN_AUTH
#define BZ_ENABLE_AUTH 1
#else
#define BZ_ENABLE_AUTH 0
#endif

#ifdef  CONFIG_AIS_OTA
#define BZ_ENABLE_OTA 1
#else
#define BZ_ENABLE_OTA 0
#endif

#ifdef  EN_COMBO_NET
#define BZ_ENABLE_COMBO_NET 1
#else 
#define BZ_ENABLE_COMBO_NET 0
#endif

#ifndef BZ_AUTH_MODEL_SEC
#ifdef  BZ_ENABLE_AUTH
#define BZ_AUTH_MODEL_SEC 0
#else
#define BZ_AUTH_MODEL_SEC 1
#endif
#else
#error "No option if BZ_ENABLE_AUTH is not enabled."
#endif

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
#error "No valid target set."
#endif

#endif  // BREEZE_OPT_H
