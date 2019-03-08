/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_BLE_PORT_H_
#define _AIS_BLE_PORT_H_

#include <stdint.h>
#include <stddef.h>

/* AIS - Alibaba IoT Service*/

/* UUIDs */
#define BLE_UUID_AIS_SERVICE 0xFEB3 /* The UUID of the Alibaba IOT Service. */
#define BLE_UUID_AIS_RC \
    0xFED4 /* The UUID of the "Read Characteristics" Characteristic. */
#define BLE_UUID_AIS_WC \
    0xFED5 /* The UUID of the "Write Characteristics" Characteristic. */
#define BLE_UUID_AIS_IC \
    0xFED6 /* The UUID of the "Indicate Characteristics" Characteristic. */
#define BLE_UUID_AIS_WWNRC                                      \
    0xFED7 /* The UUID of the "Write WithNoRsp Characteristics" \
              Characteristic. */
#define BLE_UUID_AIS_NC \
    0xFED8 /* The UUID of the "Notify Characteristics" Characteristic. */

#define AIS_BT_MAC_LEN 6

#define ABIT(n) (1 << n)

/* Characteristic Properties Bit field values */
typedef enum
{
    /** @def AIS_GATT_CHRC_BROADCAST
     *  @brief Characteristic broadcast property.
     *
     *  If set, permits broadcasts of the Characteristic Value using Server
     *  Characteristic Configuration Descriptor.
     */
    AIS_GATT_CHRC_BROADCAST = ABIT(0),
    /** @def AIS_GATT_CHRC_READ
     *  @brief Characteristic read property.
     *
     *  If set, permits reads of the Characteristic Value.
     */
    AIS_GATT_CHRC_READ = ABIT(1),
    /** @def AIS_GATT_CHRC_WRITE_WITHOUT_RESP
     *  @brief Characteristic write without response property.
     *
     *  If set, permits write of the Characteristic Value without response.
     */
    AIS_GATT_CHRC_WRITE_WITHOUT_RESP = ABIT(2),
    /** @def AIS_GATT_CHRC_WRITE
     *  @brief Characteristic write with response property.
     *
     *  If set, permits write of the Characteristic Value with response.
     */
    AIS_GATT_CHRC_WRITE = ABIT(3),
    /** @def AIS_GATT_CHRC_NOTIFY
     *  @brief Characteristic notify property.
     *
     *  If set, permits notifications of a Characteristic Value without
     *  acknowledgment.
     */
    AIS_GATT_CHRC_NOTIFY = ABIT(4),
    /** @def AIS_GATT_CHRC_INDICATE
     *  @brief Characteristic indicate property.
     *
     * If set, permits indications of a Characteristic Value with
     * acknowledgment.
     */
    AIS_GATT_CHRC_INDICATE = ABIT(5),
    /** @def AIS_GATT_CHRC_AUTH
     *  @brief Characteristic Authenticated Signed Writes property.
     *
     *  If set, permits signed writes to the Characteristic Value.
     */
    AIS_GATT_CHRC_AUTH = ABIT(6),
    /** @def AIS_GATT_CHRC_EXT_PROP
     *  @brief Characteristic Extended Properties property.
     *
     * If set, additional characteristic properties are defined in the
     * Characteristic Extended Properties Descriptor.
     */
    AIS_GATT_CHRC_EXT_PROP = ABIT(7)
} ais_char_prop_t;

/* GATT attribute permission bit field values */
typedef enum
{
    /** No operations supported, e.g. for notify-only */
    AIS_GATT_PERM_NONE = 0,
    /** Attribute read permission. */
    AIS_GATT_PERM_READ = ABIT(0),
    /** Attribute write permission. */
    AIS_GATT_PERM_WRITE = ABIT(1),
    /* Attribute read permission with encryption. */
    AIS_GATT_PERM_READ_ENCRYPT = ABIT(2),
    /* Attribute write permission with encryption. */
    AIS_GATT_PERM_WRITE_ENCRYPT = ABIT(3),
    /* Attribute read permission with authentication. */
    AIS_GATT_PERM_READ_AUTHEN = ABIT(4),
    /* Attribute write permission with authentication. */
    AIS_GATT_PERM_WRITE_AUTHEN = ABIT(5),
    /* Attribute prepare write permission. */
    AIS_GATT_PERM_PREPARE_WRITE = ABIT(6)
} ais_attr_perm_t;

typedef enum
{
    AIS_ERR_SUCCESS = 0,
    AIS_ERR_STACK_FAIL,
    AIS_ERR_MEM_FAIL,
    AIS_ERR_INVALID_ADV_DATA,
    AIS_ERR_ADV_FAIL,
    AIS_ERR_STOP_ADV_FAIL,
    AIS_ERR_GATT_INDICATE_FAIL,
    AIS_ERR_GATT_NOTIFY_FAIL,
    /* Add more AIS error code hereafter */
} ais_err_t;

typedef enum
{
    AIS_UUID_TYPE_16,
    AIS_UUID_TYPE_32,
    AIS_UUID_TYPE_128,
} ais_uuid_type_t;

typedef struct
{
    uint8_t type;
} ais_uuid_t;

typedef struct
{
    ais_uuid_t uuid;
    uint16_t   val;
} ais_uuid_16_t;

typedef struct
{
    ais_uuid_t uuid;
    uint32_t   val;
} ais_uuid_32_t;

typedef struct
{
    ais_uuid_t uuid;
    uint8_t    val[16];
} ais_uuid_128_t;

typedef enum
{
    AIS_CCC_VALUE_NONE     = 0,
    AIS_CCC_VALUE_NOTIFY   = 1,
    AIS_CCC_VALUE_INDICATE = 2
} ais_ccc_value_t;

#define AIS_UUID_INIT_16(value)                        \
    {                                                  \
        .uuid.type = AIS_UUID_TYPE_16, .val = (value), \
    }

#define AIS_UUID_INIT_32(value)                        \
    {                                                  \
        .uuid.type = AIS_UUID_TYPE_32, .val = (value), \
    }

#define AIS_UUID_INIT_128(value...)                       \
    {                                                     \
        .uuid.type = AIS_UUID_TYPE_128, .val = { value }, \
    }

#define AIS_UUID_DECLARE_16(value) \
    ((ais_uuid_t *)(&(ais_uuid_16_t)AIS_UUID_INIT_16(value)))
#define AIS_UUID_DECLARE_32(value) \
    ((ais_uuid_t *)(&(bt_uuid_32_t)AIS_UUID_INIT_32(value)))
#define AIS_UUID_DECLARE_128(value...) \
    ((ais_uuid_t *)(&(bt_uuid_128_t)AIS_UUID_INIT_128(value)))

typedef void (*connected_callback_t)();
typedef void (*disconnected_callback_t)();
typedef size_t (*on_char_read_t)(void *buf, uint16_t len);
typedef size_t (*on_char_write_t)(const void *buf, uint16_t len);
typedef void (*on_char_ccc_change_t)(ais_ccc_value_t value);

typedef struct
{
    /* Characteristics UUID */
    ais_uuid_t *uuid;
    /* Characteristics property */
    uint8_t prop;
    /* Characteristics value attribute permission */
    uint8_t perm;
    /* Characteristics value read handler, NULL if not used */
    on_char_read_t on_read;
    /* Characteristics value write handler, NULL if not used */
    on_char_write_t on_write;
    /**
     * Characteristics value ccc changed handler.
     * Only applied to NOFITY and INDICATE type Characteristics,
     * NULL if not applied.
     */
    on_char_ccc_change_t on_ccc_change;
} ais_char_init_t;

typedef struct
{
    /* AIS primamry service */
    ais_uuid_t *uuid_svc;
    /* AIS's Read Characteristics */
    ais_char_init_t rc;
    /* AIS's Write Characteristics */
    ais_char_init_t wc;
    /* AIS's Indicate Characteristics */
    ais_char_init_t ic;
    /* AIS's Write WithNoRsp Characteristics */
    ais_char_init_t wwnrc;
    /* AIS's Notify Characteristics */
    ais_char_init_t nc;
    /* Callback function when bluetooth connected */
    connected_callback_t on_connected;
    /* Callback function when bluetooth disconnected */
    disconnected_callback_t on_disconnected;
} ais_bt_init_t;

typedef enum
{
    AIS_ADV_NAME_SHORT,
    AIS_ADV_NAME_FULL
} ais_adv_name_type_t;

typedef struct
{
    ais_adv_name_type_t ntype;
    char *              name;
} ais_adv_name_t;

/* DISCOVERABILITY MODES, spec v4.2, in 4.1 section */
typedef enum
{
    AIS_AD_LIMITED  = ABIT(0), /* Limited Discoverable */
    AIS_AD_GENERAL  = ABIT(1), /* General Discoverable */
    AIS_AD_NO_BREDR = ABIT(2)  /* BR/EDR not supported */
} ais_adv_flag_t;

#define MAX_VENDOR_DATA_LEN 24
typedef struct
{
    uint8_t  data[MAX_VENDOR_DATA_LEN];
    uint16_t len;
} ais_adv_vendor_data_t;

typedef struct
{
    ais_adv_flag_t        flag;
    ais_adv_name_t        name;
    ais_adv_vendor_data_t vdata;
    /* Subject to add more hereafter in the future */
} ais_adv_init_t;

enum
{
    AIS_BT_REASON_REMOTE_USER_TERM_CONN = 0,
    /* Add more supported reasons here. */
    AIS_BT_REASON_UNSPECIFIED = 0x0f
};

/**
 * API to initialize ble stack.
 * @parma[in]  ais_init  Bluetooth stack init parmaters.
 * @return     0 on success, error code if failure.
 */
typedef void (* stack_init_done_t)(uint8_t res);
ais_err_t ble_stack_init(ais_bt_init_t *ais_init, stack_init_done_t init_done);

/**
 * API to de-initialize ble stack.
 * @return     0 on success, error code if failure.
 */
ais_err_t ble_stack_deinit();

/**
 * API to send data via AIS's Notify Characteristics.
 * @parma[in]  p_data  data buffer.
 * @parma[in]  length  data length.
 * @return     0 on success, error code if failure.
 */
ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length);

/**
 * API to send data via AIS's Indicate Characteristics.
 * @parma[in]  p_data  data buffer.
 * @parma[in]  length  data length.
 * @parma[in]  txdone  txdone callback.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length, void (*txdone)(uint8_t res));

/**
 * API to disconnect BLE connection.
 * @param[in]  reason  the reason to disconnect the connection.
 */
void ble_disconnect(uint8_t reason);

/**
 * API to start bluetooth advertising.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_advertising_start(ais_adv_init_t *adv);

/**
 * API to stop bluetooth advertising.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_advertising_stop();

/**
 * API to start bluetooth advertising.
 * @parma[out]  mac  the uint8_t[BD_ADDR_LEN] space the save the mac address.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_get_mac(uint8_t *mac);

#endif
