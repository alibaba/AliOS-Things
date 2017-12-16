/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* @file
 *
 * AOS Bluetooth Low Energy (BLE) Functions
 *
 */

#ifndef _AOS_BT_NVRAM_ACCESS_H_
#define _AOS_BT_NVRAM_ACCESS_H_

#ifndef OFFSETOF
#define OFFSETOF( type, member )  ( (uintptr_t)&((type *)0)->member )
#endif /* OFFSETOF */

#define AOS_BT_PARA_LOCAL_KEY_DATA  65  /* BTM_SECURITY_LOCAL_KEY_DATA_LEN */

#define AOS_BT_DCT_NAME            249
#define AOS_BT_DCT_MAX_KEYBLOBS    146   /* Maximum size of key blobs to be stored :=  size of BR-EDR link keys +  size of BLE keys*/
#define AOS_BT_DCT_ADDR_FIELD      6
#define AOS_BT_DCT_LENGTH_FIELD    2
#ifndef AOS_BT_DCT_MAX_DEVICES
#define AOS_BT_DCT_MAX_DEVICES     10    /* Maximum number of device records stored in nvram */
#endif
#define AOS_BT_DCT_ADDR_TYPE       1
#define AOS_BT_DCT_DEVICE_TYPE     1

/* Length of BD_ADDR + 2bytes length field */
#define AOS_BT_DCT_ENTRY_HDR_LENGTH  (AOS_BT_DCT_ADDR_FIELD + AOS_BT_DCT_LENGTH_FIELD + AOS_BT_DCT_ADDR_TYPE + AOS_BT_DCT_DEVICE_TYPE)

#define AOS_BT_DCT_LOCAL_KEY_OFFSET  OFFSETOF( aos_bt_config_t, bluetooth_local_key )
#define AOS_BT_DCT_REMOTE_KEY_OFFSET OFFSETOF( aos_bt_config_t, bluetooth_remote_key )

#endif
