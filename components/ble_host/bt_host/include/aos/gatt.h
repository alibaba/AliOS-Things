/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GATT_H__
#define __GATT_H__

#include <aos/uuid.h>
#include <ble_config.h>

#define ARRAY_SIZES(array) (sizeof(array)/sizeof(array[0]))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CCC_VALUE_NONE     = 0,
    CCC_VALUE_NOTIFY   = 1,
    CCC_VALUE_INDICATE = 2
} ccc_value_en;

/* GATT attribute permission bit field values */
typedef enum {
    /** No operations supported, e.g. for notify-only */
    GATT_PERM_NONE = 0,

    /** Attribute read permission. */
    GATT_PERM_READ = BLE_BIT(0),

    /** Attribute write permission. */
    GATT_PERM_WRITE = BLE_BIT(1),

    /** Attribute read permission with encryption.
     *
     *  If set, requires encryption for read access.
     */
    GATT_PERM_READ_ENCRYPT = BLE_BIT(2),

    /** Attribute write permission with encryption.
     *
     *  If set, requires encryption for write access.
     */
    GATT_PERM_WRITE_ENCRYPT = BLE_BIT(3),

    /** Attribute read permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for read
     *  access.
     */
    GATT_PERM_READ_AUTHEN = BLE_BIT(4),

    /** Attribute write permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for write
     *  access.
     */
    GATT_PERM_WRITE_AUTHEN = BLE_BIT(5),

    /** Attribute prepare write permission.
     *
     *  If set, allows prepare writes with use of GATT_WRITE_FLAG_PREPARE
     *  passed to write callback.
     */
    GATT_PERM_PREPARE_WRITE = BLE_BIT(6),
} gatt_perm_en;


/* Characteristic Properties Bit field values */
typedef enum {
    /** @def BT_GATT_CHRC_BROADCAST
     *  @brief Characteristic broadcast property.
     *
     *  If set, permits broadcasts of the Characteristic Value using Server
     *  Characteristic Configuration Descriptor.
     */
    GATT_CHRC_PROP_BROADCAST =  BLE_BIT(0),
    /** @def BT_GATT_CHRC_READ
     *  @brief Characteristic read property.
     *
     *  If set, permits reads of the Characteristic Value.
     */
    GATT_CHRC_PROP_READ =   BLE_BIT(1),
    /** @def BT_GATT_CHRC_WRITE_WITHOUT_RESP
     *  @brief Characteristic write without response property.
     *
     *  If set, permits write of the Characteristic Value without response.
     */
    GATT_CHRC_PROP_WRITE_WITHOUT_RESP = BLE_BIT(2),
    /** @def BT_GATT_CHRC_WRITE
     *  @brief Characteristic write with response property.
     *
     *  If set, permits write of the Characteristic Value with response.
     */
    GATT_CHRC_PROP_WRITE =  BLE_BIT(3),
    /** @def BT_GATT_CHRC_NOTIFY
     *  @brief Characteristic notify property.
     *
     *  If set, permits notifications of a Characteristic Value without
     *  acknowledgment.
     */
    GATT_CHRC_PROP_NOTIFY =     BLE_BIT(4),
    /** @def BT_GATT_CHRC_INDICATE
     *  @brief Characteristic indicate property.
     *
     * If set, permits indications of a Characteristic Value with acknowledgment.
     */
    GATT_CHRC_PROP_INDICATE =   BLE_BIT(5),
    /** @def BT_GATT_CHRC_AUTH
     *  @brief Characteristic Authenticated Signed Writes property.
     *
     *  If set, permits signed writes to the Characteristic Value.
     */
    GATT_CHRC_PROP_AUTH =   BLE_BIT(6),
    /** @def BT_GATT_CHRC_EXT_PROP
     *  @brief Characteristic Extended Properties property.
     *
     * If set, additional characteristic properties are defined in the
     * Characteristic Extended Properties Descriptor.
     */
    GATT_CHRC_PROP_EXT_PROP =   BLE_BIT(7),
} gatt_prop_en;

/* Error codes for Error response PDU */
#define ATT_ERR_INVALID_HANDLE          0x01
#define ATT_ERR_READ_NOT_PERMITTED      0x02
#define ATT_ERR_WRITE_NOT_PERMITTED     0x03
#define ATT_ERR_INVALID_PDU             0x04
#define ATT_ERR_AUTHENTICATION          0x05
#define ATT_ERR_NOT_SUPPORTED           0x06
#define ATT_ERR_INVALID_OFFSET          0x07
#define ATT_ERR_AUTHORIZATION           0x08
#define ATT_ERR_PREPARE_QUEUE_FULL      0x09
#define ATT_ERR_ATTRIBUTE_NOT_FOUND     0x0a
#define ATT_ERR_ATTRIBUTE_NOT_LONG      0x0b
#define ATT_ERR_ENCRYPTION_KEY_SIZE     0x0c
#define ATT_ERR_INVALID_ATTRIBUTE_LEN   0x0d
#define ATT_ERR_UNLIKELY                0x0e
#define ATT_ERR_INSUFFICIENT_ENCRYPTION 0x0f
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE  0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES  0x11

typedef int (*char_read_func_t)(uint16_t conn_handle, uint16_t char_handle, void *buf, uint16_t len, uint16_t offset);
typedef int (*char_write_func_t)(uint16_t conn_handle, uint16_t char_handle, void *buf, uint16_t len, uint16_t offset);
typedef void (*char_ccc_change_func_t)(ccc_value_en value);

struct gatt_char_t {

    /** Characteristic UUID. */
    const uuid_t    *uuid;
    /** Characteristic Value handle. */
	uint16_t			value_handle;
    /** Characteristic properties. */
    uint8_t         properties;
};

typedef struct _gatt_cep_t {
    uint16_t  ext_props;
} gatt_cep_t;

typedef struct _gatt_cud_t {
    char *user_des; /* User Description */
} gatt_cud_t;

typedef struct _gatt_cpf_t {
    /** Format of the value of the characteristic */
    uint8_t format;
    /** Exponent field to determine how the value of this characteristic is further formatted */
    int8_t exponent;
    /** Unit of the characteristic */
    uint16_t unit;
    /** Name space of the description */
    uint8_t name_space;
    /** Description of the characteristic as defined in a higher layer profile */
    uint16_t description;
} gatt_cpf_t;

#if 0
typedef struct _gatt_att_t {
    uuid_t *uuid;

    uint8_t perm;  /* for Characteristic Value Declaration*/

    union {
        void *value;
        gatt_char_t *char_def;
        gatt_cep_t *cep;
        gatt_cud_t *cud;
        gatt_cpf_t *cpf;
    };

    char_read_func_t read;

    char_write_func_t write;

} gatt_att_t;
#endif


typedef struct _gatt_attr_t {
    /** Attribute UUID */
    uuid_t *uuid;

    char_read_func_t read;

    char_write_func_t write;

    /** Attribute user data */
    void            *user_data;
    /** Attribute handle */
    uint16_t            handle;
    /** Attribute permissions */
    uint8_t         perm;
} gatt_attr_t;

typedef struct {
    uint8_t  val[6];
} bt_addr;

typedef struct {
    uint8_t      type;
    bt_addr a;
} bt_addr_le;

struct bt_gatt_ccc_cfg_t {
    uint8_t                    id;
    bt_addr_le      peer;
    uint16_t            value;
};

#define BT_RTL
struct bt_gatt_ccc_t {
#ifndef BT_RTL
    struct bt_gatt_ccc_cfg_t    cfg[CONFIG_BT_MAX_PAIRED + CONFIG_BT_MAX_CONN];
#else
    struct bt_gatt_ccc_cfg_t    cfg[2];
#endif
    uint16_t            value;
    void (*cfg_changed)(const gatt_attr_t *attr,  uint16_t value);
    int (*cfg_write)(void *conn, gatt_attr_t *attr, uint16_t value);
	int (*cfg_match)(void *conn, gatt_attr_t *attr);
};

#define GATT_ATT_DEFINE(_uuid, _perm, _read, _write,_value)     \
    {                                   \
        .uuid = _uuid,                          \
                .perm = _perm,                          \
                        .read = _read,                          \
                                .write = _write,                        \
        .user_data =(void *){ _value},                      \
    }
#define GATT_PRIMARY_SERVICE_DEFINE(_uuid) \
    GATT_ATT_DEFINE(UUID_GATT_PRIMARY, GATT_PERM_READ, NULL, NULL,_uuid)

#define GATT_SECONDARY_SERVICE_DEFINE(_uuid) \
    GATT_ATT_DEFINE(UUID_GATT_SECONDARY, GATT_PERM_READ, NULL, NULL, _uuid)

#define GATT_CHAR_DEFINE(_uuid, _props) \
    GATT_ATT_DEFINE(UUID_GATT_CHRC, GATT_PERM_READ,  \
    NULL, NULL,(&(struct gatt_char_t){.uuid= _uuid,.value_handle = 0,.properties = _props,}))

#define GATT_CHAR_VAL_DEFINE(_uuid, _perm) \
    GATT_ATT_DEFINE(_uuid, _perm, NULL, NULL, NULL)

#define GATT_CHAR_DESCRIPTOR_DEFINE(_uuid, _perm) \
    GATT_ATT_DEFINE(_uuid, _perm, NULL, NULL, NULL)

#define GATT_CHAR_CCC_DEFINE() \
    GATT_ATT_DEFINE(UUID_GATT_CCC, GATT_PERM_READ | GATT_PERM_WRITE,  \
    NULL, NULL,(&(struct bt_gatt_ccc_t){.cfg= {{0}}}))

#define GATT_CHAR_CUD_DEFINE(_value, _perm) \
    GATT_ATT_DEFINE(UUID_GATT_CUD, _perm, NULL, NULL,(&(gatt_cud_t) {.user_des = _value}))

#define GATT_CHAR_CPF_DEFINE(_value, _perm) \
    GATT_ATT_DEFINE(UUID_GATT_CPF, _perm, NULL, NULL,(&(gatt_cep_t) {.ext_props = _value}))


typedef enum {
    GATT_SERVER_TYPE_PRIMARY = 0,
    GATT_SERVER_TYPE_SECONDARY,
    GATT_SERVER_TYPE_INCLUDE,
} gatt_service_type_en;

typedef struct _gatt_service_param_t {
    uuid_t *uuid;
    gatt_service_type_en type;
} gatt_service_param;

#if 0
typedef struct _gatt_char_param_t {
    uuid_t *uuid;
    void *value;
    char_read_func_t read;
    char_write_func_t write;
    uint8_t props;
    uint8_t perm;
} gatt_char_param;

typedef struct _gatt_char_des_param_t {
    uuid_t *uuid;
    void *value;
    char_read_func_t read;
    char_write_func_t write;
    uint8_t perm;
} gatt_char_des_param_t;

#endif

typedef enum {
    GATT_FIND_PRIMARY_SERVICE,
    GATT_FIND_INC_SERVICE,
    GATT_FIND_CHAR,
    GATT_FIND_CHAR_DESCRIPTOR,
} gatt_discovery_type_en;

struct _snode_ {
    struct _snode_ *next;
};

typedef struct _snode_ sys_snode;

/** @brief GATT Service structure */
typedef struct _gatt_service_ {
    /** Service Attributes */
    struct bt_gatt_attr *attrs;
    /** Service Attribute count */
    uint32_t            attr_count;
    sys_snode       node;
} gatt_service;


int ble_stack_gatt_registe_service(gatt_service *s, gatt_attr_t attrs[], uint16_t attr_num);

int ble_stack_gatt_notificate(int16_t conn_handle, uint16_t char_handle, const uint8_t *data, uint16_t len);
int ble_stack_gatt_indicate(int16_t conn_handle, int16_t char_handle, const uint8_t *data, uint16_t len);
int ble_stack_gatt_mtu_get(int16_t conn_handle);
int ble_stack_gatt_mtu_exchange(int16_t conn_handle);

int ble_stack_gatt_service_discovery(int16_t conn_handle);
int ble_stack_gatt_discovery(int16_t conn_handle, gatt_discovery_type_en type, uuid_t *uuid, uint16_t start_handle, uint16_t end_handle);
#define ble_stack_gatt_discovery_all(conn_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_PRIMARY_SERVICE, 0, 0x0001, 0xffff)

#define ble_stack_gatt_discovery_primary(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_PRIMARY_SERVICE, uuid, start_handle, end_handle)

#define ble_stack_gatt_discovery_include(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_INC_SERVICE, uuid, start_handle, end_handle)

#define ble_stack_gatt_discovery_char_all(conn_handle, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR, 0, start_handle, end_handle)

#define ble_stack_gatt_discovery_char(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR, uuid, start_handle, end_handle)

#define ble_stack_gatt_discovery_descriptor(conn_handle, uuid,start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR_DESCRIPTOR, uuid, start_handle, end_handle)

#define ble_stack_gatt_discovery_descriptor_all(conn_handle, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR_DESCRIPTOR, 0, start_handle, end_handle)

typedef enum {
    GATT_WRITE,
    GATT_WRITE_WITHOUT_RESPONSE,
    GATT_WRITE_SINGED,
} gatt_write_en;

int ble_stack_gatt_write(int16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len, uint16_t offset, gatt_write_en type);

#define ble_stack_gatt_write_response(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE)

#define ble_stack_gatt_write_no_response(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE_WITHOUT_RESPONSE)

#define ble_stack_gatt_write_signed(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE_SINGED)

int ble_stack_gatt_read(int16_t conn_handle, uint16_t attr_handle, uint16_t offset);
int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[]);

#ifdef __cplusplus
}
#endif

#endif  /* __BLE_H__ */

