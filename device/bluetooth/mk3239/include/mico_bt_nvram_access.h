
/** @file
 *
 * MiCO Bluetooth Low Energy (BLE) Functions
 *
 */
#pragma once

#ifndef OFFSETOF
#define OFFSETOF( type, member )  ( (uintptr_t)&((type *)0)->member )
#endif /* OFFSETOF */

#define MICO_BT_PARA_LOCAL_KEY_DATA  65  /* BTM_SECURITY_LOCAL_KEY_DATA_LEN */

#define MICO_BT_DCT_NAME            249
#define MICO_BT_DCT_MAX_KEYBLOBS    146   /* Maximum size of key blobs to be stored :=  size of BR-EDR link keys +  size of BLE keys*/
#define MICO_BT_DCT_ADDR_FIELD      6
#define MICO_BT_DCT_LENGTH_FIELD    2
#ifndef MICO_BT_DCT_MAX_DEVICES
#define MICO_BT_DCT_MAX_DEVICES     10    /* Maximum number of device records stored in nvram */
#endif
#define MICO_BT_DCT_ADDR_TYPE       1
#define MICO_BT_DCT_DEVICE_TYPE     1

/* Length of BD_ADDR + 2bytes length field */
#define MICO_BT_DCT_ENTRY_HDR_LENGTH  (MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD + MICO_BT_DCT_ADDR_TYPE + MICO_BT_DCT_DEVICE_TYPE)

#define MICO_BT_DCT_LOCAL_KEY_OFFSET  OFFSETOF( mico_bt_config_t, bluetooth_local_key )
#define MICO_BT_DCT_REMOTE_KEY_OFFSET OFFSETOF( mico_bt_config_t, bluetooth_remote_key )



#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
