/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_GATT_DEFS_H_
#define _BLE_GATT_DEFS_H_

#ifndef BLE_GATT_DB_DEFINITIONS
#define BLE_GATT_DB_DEFINITIONS

/*
 * GATT attribute types
 */
#define GATT_UUID_PRI_SERVICE           0x2800
#define GATT_UUID_SEC_SERVICE           0x2801
#define GATT_UUID_INCLUDE_SERVICE       0x2802
#define GATT_UUID_CHAR_DECLARE          0x2803      /*  Characteristic Declaration*/

#define GATT_UUID_CHAR_EXT_PROP         0x2900      /*  Characteristic Extended Properties */
#define GATT_UUID_CHAR_DESCRIPTION      0x2901      /*  Characteristic User Description*/
#define GATT_UUID_CHAR_CLIENT_CONFIG    0x2902      /*  Client Characteristic Configuration */
#define GATT_UUID_CHAR_SRVR_CONFIG      0x2903      /*  Server Characteristic Configuration */
#define GATT_UUID_CHAR_PRESENT_FORMAT   0x2904      /*  Characteristic Presentation Format*/
#define GATT_UUID_CHAR_AGG_FORMAT       0x2905      /*  Characteristic Aggregate Format*/
#define GATT_UUID_CHAR_VALID_RANGE       0x2906     /*  Characteristic Valid Range */
#define GATT_UUID_EXT_RPT_REF_DESCR     0x2907
#define GATT_UUID_RPT_REF_DESCR         0x2908

/*
 *GAP Profile Attributes
 */
#define GATT_UUID_GAP_DEVICE_NAME       0x2A00
#define GATT_UUID_GAP_ICON              0x2A01
#define GATT_UUID_GAP_PREF_CONN_PARAM   0x2A04
#define GATT_UUID_GAP_CENTRAL_ADDR_RESOL 0x2AA6

/* Attribute Profile Attribute UUID */
#define GATT_UUID_GATT_SRV_CHGD         0x2A05
/* Attribute Protocol Test */


/****************************************************************************
 *  GATT Database Defintions
 *****************************************************************************/
/*  The permission bits (see Vol 3, Part F, 3.3.1.1) */
#define LEGATTDB_PERM_NONE                             (0x00)
#define LEGATTDB_PERM_VARIABLE_LENGTH                  (0x1 << 0)
#define LEGATTDB_PERM_READABLE                         (0x1 << 1)
#define LEGATTDB_PERM_WRITE_CMD                        (0x1 << 2)
#define LEGATTDB_PERM_WRITE_REQ                        (0x1 << 3)
#define LEGATTDB_PERM_AUTH_READABLE                    (0x1 << 4)
#define LEGATTDB_PERM_RELIABLE_WRITE                   (0x1 << 5)
#define LEGATTDB_PERM_AUTH_WRITABLE                    (0x1 << 6)

#define LEGATTDB_PERM_WRITABLE  (LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ| LEGATTDB_PERM_AUTH_WRITABLE)
#define LEGATTDB_PERM_MASK                             (0x7f)   /* All the permission bits. */
#define LEGATTDB_PERM_SERVICE_UUID_128                 (0x1 << 7)


/*  GATT Characteristic Properties */
#define LEGATTDB_CHAR_PROP_BROADCAST                  (0x1 << 0)
#define LEGATTDB_CHAR_PROP_READ                       (0x1 << 1)
#define LEGATTDB_CHAR_PROP_WRITE_NO_RESPONSE          (0x1 << 2)
#define LEGATTDB_CHAR_PROP_WRITE                      (0x1 << 3)
#define LEGATTDB_CHAR_PROP_NOTIFY                     (0x1 << 4)
#define LEGATTDB_CHAR_PROP_INDICATE                   (0x1 << 5)
#define LEGATTDB_CHAR_PROP_AUTHD_WRITES               (0x1 << 6)
#define LEGATTDB_CHAR_PROP_EXTENDED                   (0x1 << 7)

/* Conversion macros */
#define BIT16_TO_8( val ) \
    (uint8_t)(  (val)        & 0xff),/* LSB */ \
    (uint8_t)(( (val) >> 8 ) & 0xff) /* MSB */

/* UUID lengths */
#define LEGATTDB_UUID16_SIZE                 2
#define LEGATTDB_UUID128_SIZE                16

/* Service and Characteristic macros  */
#define ATTRIBUTE16(  handle, permission, datalen, uuid ) \
    BIT16_TO_8(handle), \
    (uint8_t)(permission), \
    (uint8_t)(datalen + 2), \
    BIT16_TO_8(uuid)

#define PRIMARY_SERVICE_UUID16(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    LEGATTDB_PERM_READABLE, \
    4, \
    BIT16_TO_8((GATT_UUID_PRI_SERVICE)), \
    BIT16_TO_8((service))

#define PRIMARY_SERVICE_UUID128(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    LEGATTDB_PERM_READABLE, \
    18, \
    BIT16_TO_8(GATT_UUID_PRI_SERVICE), \
    service

#define CHARACTERISTIC_UUID16(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    LEGATTDB_PERM_READABLE, \
    0x07, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    BIT16_TO_8(uuid), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission), \
    (uint8_t)(LEGATTDB_UUID16_SIZE), \
    BIT16_TO_8(uuid)

#define CHARACTERISTIC_UUID128(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    LEGATTDB_PERM_READABLE, \
    21, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    uuid, \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission | LEGATTDB_PERM_SERVICE_UUID_128), \
    (uint8_t)(LEGATTDB_UUID128_SIZE), \
    uuid

#define CHAR_DESCRIPTOR_UUID16(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission), \
    (uint8_t)(LEGATTDB_UUID16_SIZE), \
    BIT16_TO_8(uuid)

#define CHAR_DESCRIPTOR_UUID16_WRITABLE(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission), \
    (uint8_t)(LEGATTDB_UUID16_SIZE), \
    (uint8_t)(0), \
    BIT16_TO_8(uuid)

#endif
#endif
