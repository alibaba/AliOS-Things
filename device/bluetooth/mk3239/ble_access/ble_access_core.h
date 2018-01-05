#ifndef __BLE_ACCESS_CORE_H__
#define __BLE_ACCESS_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Configurations & Constants
 *
 *-------------------------------------------------------------------------------------------------
 */

/* The prefix of device address of peripheral device. */
#define BLE_ACCESS_PREFIX_LEN               3
#define BLE_ACCESS_PREFIX_D0BAE4            "\xD0\xBA\xE4"
#define BLE_ACCESS_PREFIX_C89346            "\xC8\x93\x46"

/* The maximum length of a Bluetooth Device Name */
#define BLE_ACCESS_DEVICE_NAME_MAX_LEN      16

/* Advertisement PACKET type */
#define BLE_ACCESS_ADV_TYPE_INIT            0x01
#define BLE_ACCESS_ADV_TYPE_RECONN          0x02


/*
 *-------------------------------------------------------------------------------------------------
 * Host Request Command
 *-------------------------------------------------------------------------------------------------
 */

#define BLE_ACCESS_STATUS_SUCCESS           0x00
#define BLE_ACCESS_STATUS_FAILED            0x01
#define BLE_ACCESS_STATUS_AUTH_FAILED       0x02
#define BLE_ACCESS_STATUS_ADD_FAILED        0x03
#define BLE_ACCESS_STATUS_SLAVE_REJECT      0x04
#define BLE_ACCESS_STATUS_RECONN_FAILED     0x05
#define BLE_ACCESS_STATUS_NO_RESOURCES      0x06
#define BLE_ACCESS_STATUS_BUSY              0x07
#define BLE_ACCESS_STATUS_TIMEOUT           0x08
#define BLE_ACCESS_STATUS_SLAVE_REQ_DISC    0x09
#define BLE_ACCESS_STATUS_INVALID_PARMS     0x0A
#define BLE_ACCESS_STATUS_NO_CONNECTION     0x0B

/*
 * input:   none
 * output:  device_id,name,RSSI (BLE_ACCESS_EVENT_DEV_NEW)
 */
#define BLE_ACCESS_REQ_DEV_SCAN        0x01

/*
 * input:   device_id
 * output:  device_id, status (BLE_ACCESS_EVENT_DEV_ADD)
 */
#define BLE_ACCESS_REQ_DEV_ADD         0x02

/*
 * input:   device_id, timeout
 * output:  device_id, status (BLE_ACCESS_EVENT_DEV_DISC)
 */
#define BLE_ACCESS_REQ_DEV_DISC        0x03

/*
 * input:   device_id
 * output:  device_id, status (BLE_ACCESS_EVENT_DEV_REMOVE)
 */
#define BLE_ACCESS_REQ_DEV_REMOVE      0x04

/*
 * input: none
 * output: none
 */
#define BLE_ACCESS_REQ_DEV_START_AUTO  0x05    /* Request to start auto connection procedure */

/*
 * input: none
 * output: none
 */
#define BLE_ACCESS_REQ_DEV_STOP_AUTO   0x06    /* Request to stop auto connection procedure */

/*
 * Request command parameters
 */
typedef struct {
    uint32_t device_id;
    union {
        /* used by BLE_ACCESS_REQ_DEV_SCAN */
        mico_bool_t start;
        /* used by BLE_ACCESS_REQ_DEV_DISC */
        uint32_t timeout;
    } p;
} ble_access_cmd_parms_t;

/*
 * UUID type
 */
typedef struct {
    uint16_t        len;            /* UUID length */
    union {
        uint16_t    uuid16;         /* 16-bit UUID */
        uint32_t    uuid32;         /* 32-bit UUID */
        uint8_t     uuid128[16];    /* 128-bit UUID */
    } uu;
} ble_access_uuid_t;

#pragma pack(1)

/*
 * 3.1 Attribute Value of Service Definition
 */
typedef struct {
    uint16_t        start_handle; /* Starting handle */
    uint16_t        end_handle;   /* Ending handle   */
    ble_access_uuid_t  uuid;         /* UUID            */
} ble_access_attr_val_serv_t;

/*
 * 3.2 Attribute Value of Include Definition
 */
typedef struct {
    uint16_t        included_service_handle; /* Included service handle */
    uint16_t        end_group_handle;        /* End group handle        */
    ble_access_uuid_t  uuid;                    /* UUID                    */
} ble_access_attr_val_inc_t;

/*
 * 3.3.1 Attribute Value of Characteristic Declaration
 */
typedef struct {
    uint8_t         properties;              /* Properties   */
    uint16_t        value_handle;            /* Value handle */
    ble_access_uuid_t  uuid;                    /* UUID         */

    uint16_t        descriptor_start_handle; /* Descriptor start handle. Additional field. Not in spec */
    uint16_t        descriptor_end_handle;   /* Descriptor end handle.   Additional field. Not in spec */
} ble_access_attr_val_char_t;

/*
 * 3.3.2 Attribute Value of Characteristic Value Declaration
 */
typedef struct {
    uint8_t         value[1]; /* Start of value */
} ble_access_attr_val_char_val_t;

/*
 * 3.3.3.1 Attribute Value of Characteristic Extended Properties
 */
typedef struct {
    uint8_t         properties; /* Properties */
} ble_access_attr_val_ext_prop_t;

/*
 * 3.3.3.2 Attribute Value of Characteristic User Description
 */
typedef struct {
    char            string[1]; /* User description string */
} ble_access_attr_val_usr_desc_t;

/*
 * 3.3.3.3 Attribute Value of Client Characteristic Configuration
 */
typedef struct {
    uint16_t        config_bits; /* Configuration bits */
} ble_access_attr_val_cli_cfg_t;

/*
 * 3.3.3.4 Attribute Value of Server Characteristic Configuration
 */
typedef struct {
    uint16_t        config_bits; /* Configuration bits */
} ble_access_attr_val_srv_cfg_t;

/*
 * 3.3.3.5 Attribute Value of Characteristic Presentation Format
 */
typedef struct {
    uint8_t         format;      /* Format      */
    uint8_t         exponent;    /* Exponent    */
    uint16_t        unit;        /* Unit        */
    uint8_t         name_space;  /* Namespace   */
    uint16_t        description; /* Description */
} ble_access_attr_val_pres_format_t;

/*
 * 3.3.3.6 Attribute Value of Characteristic Aggregate Format
 */
typedef struct {
    uint16_t        handle_list[1]; /* Handle list */
} ble_access_attr_val_aggre_format_t;

/*
 * Vol 3 Part C 12.1 Attribute Value of Device Name Characteristic
 */
typedef struct {
    char            device_name[1]; /* Maximum length is 248 bytes */
} ble_access_attr_val_dev_name_t;

/*
 * Vol 3 Part C 12.2 Attribute Value of Appearance Characteristic
 */
typedef struct {
    uint16_t        appearance; /* Enumerated value defined in "Assigned Numbers" */
} ble_access_attr_val_appear_t;

/*
 * Vol 3 Part C 12.3 Attribute Value of Peripheral Privacy Flag Characteristic
 */
typedef struct {
    uint8_t         periph_privacy_flag; /* Peripheral privacy flag: 0 if disabled; 1 if enabled */
} ble_access_attr_val_privacy_flag_t;

/*
 * Vol 3 Part C 12.4 Attribute Value of Reconnection Address Characteristic
 */
typedef struct {
    uint8_t         reconn_address[6]; /* Network-order reconnection address */
} ble_access_attr_val_reconn_addr_t;

/*
 * Vol 3 Part C 12.5 Attribute Value of Peripheral Preferred Connection Parameters Characteristic
 */
typedef struct {
    uint16_t        min_conn_interval;                   /* Minimum connection interval               */
    uint16_t        max_conn_interval;                   /* Maximum connection interval               */
    uint16_t        slave_latency;                       /* Slave latency                             */
    uint16_t        conn_supervision_timeout_multiplier; /* Connection supervision timeout multiplier */
} ble_access_attr_val_conn_parms_t;

/*
 * Attribute Structure
 */
typedef struct {
    void               *next;               // Not used.
    uint16_t            handle;             // attribute handle
    ble_access_uuid_t   type;               // attribute type
    uint8_t             permission;         // attribute permissions
    uint32_t            value_length;       // length of the attribute value. If no value, this equals 0
    uint32_t            value_struct_size;  // size of the value structure

    /* Union of attribute values. Use the right format based on Attribute Type */
    union {
        /* Not support Long Value                                                             */
        uint8_t                               value[512];
        /* Attribute Value for Service                                                         */
        ble_access_attr_val_serv_t            service;
        /* Attribute Value for Include                                                         */
        ble_access_attr_val_inc_t             include;
        /* Attribute Value for Characteristic                                                  */
        ble_access_attr_val_char_t            characteristic;
        /* Attribute Value for Characteristic Value                                            */
        ble_access_attr_val_char_val_t        characteristic_value;
        /* Attribute Value for Descriptor: Characteristic Extended Properties                  */
        ble_access_attr_val_ext_prop_t        extended_properties;
        /* Attribute Value for Descriptor: Characteristic User_Description                     */
        ble_access_attr_val_usr_desc_t        user_description;
        /* Attribute Value for Descriptor: Client Characteristic Configuration                 */
        ble_access_attr_val_cli_cfg_t         client_config;
        /* Attribute Value for Descriptor: Server Characteristic Configuration                 */
        ble_access_attr_val_srv_cfg_t         server_config;
        /* Attribute Value for Descriptor: Characteristic Presentation Format                  */
        ble_access_attr_val_pres_format_t     presentation_format;
        /* Attribute Value for Descriptor: Characteristic Aggregate Format                     */
        ble_access_attr_val_aggre_format_t    aggregate_format;
        /* Attribute Value for Characteristic Type: Device Name                                */
        ble_access_attr_val_dev_name_t        device_name;
        /* Attribute Value for Characteristic Type: Appearance                                 */
        ble_access_attr_val_appear_t          appearance;
        /* Attribute Value for Characteristic Type: Peripheral Privacy Flag                    */
        ble_access_attr_val_privacy_flag_t    periph_privacy_flag;
        /* Attribute Value for Characteristic Type: Reconnection Address                       */
        ble_access_attr_val_reconn_addr_t     reconn_address;
        /* Attribute Value for Characteristic Type: Peripheral Preferred Connection Parameters */
        ble_access_attr_val_conn_parms_t      periph_preferred_conn_params;
    } value;
} ble_access_attribute_t;

#pragma pack()

/*
 *-------------------------------------------------------------------------------------------------
 * Controller Command Response
 *-------------------------------------------------------------------------------------------------
 */

#define BLE_ACCESS_EVENT_DEV_NEW        0x01    /* A new device is scanned. */
#define BLE_ACCESS_EVENT_DEV_NEW_CMPL   0x02    /* The new device discovery procedure has completed. */
#define BLE_ACCESS_EVENT_DEV_AUTHING    0x03    /* A new device is authing. */
#define BLE_ACCESS_EVENT_DEV_ADD        0x04    /* A new device is or isn't added. */
#define BLE_ACCESS_EVENT_DEV_CONN       0x05    /* A old device is re-connection */
#define BLE_ACCESS_EVENT_DEV_DISC       0x06    /* A device request to disconnect */
#define BLE_ACCESS_EVENT_GATT_CHAR      0x07    /* A characteristic event for ble_access_get_characteristics */
#define BLE_ACCESS_EVENT_GATT_CHAR_CMPL 0x08    /* All characteristics event finished. */
#define BLE_ACCESS_EVENT_GATT_NOTIFY    0x09    /* A notification from peer server */

/*
 *  Used to descript a GATT Service Handle range and its type.
 */
typedef struct {
    uint16_t            start_handle;
    uint16_t            end_handle;
    ble_access_uuid_t   type;
} ble_access_serv_t;

/*
 * Event Handle Parameters
 */
typedef struct {
    /* Always valid */
    uint32_t device_id;
    uint8_t  status;

    union {
        /* BLE_ACCESS_BT_EVENT_DEV_ADD and BLE_ACCESS_BT_EVENT_DEV_CONN */
        struct {
            uint8_t            serv_count;
            ble_access_serv_t *serv;
        } add;
        /* BLE_ACCESS_EVENT_CHAR and BLE_ACCESS_EVENT_GATT_NOTIFY */
        struct {
            ble_access_attribute_t *attr;
        } gatt;
        /* BLE_ACCESS_EVENT_DEV_NEW */
        struct {
            char   *name;
            int8_t  RSSI;
        } scan;
        /* BLE_ACCESS_EVENT_DEV_STATE */
        uint8_t state;
    } p;
} ble_access_evt_parms_t;

/*
 * Event handle
 */
typedef void (*ble_access_event_callback_t)(uint8_t event, const ble_access_evt_parms_t *params);

/* Initialize Bluetooth Interface */
extern OSStatus ble_access_bluetooth_init(void);

/* Start BLE Auto Connection Procedure */
extern OSStatus ble_access_bluetooth_start(ble_access_event_callback_t callback);

/* Stop BLE Auto Connection Procedure */
extern OSStatus ble_access_bluetooth_stop(void);

/* Send a command to Bluetooth Interface. */
extern OSStatus ble_access_bluetooth_request(uint8_t request,
                                             const ble_access_cmd_parms_t *parms);

/*
 * Get an attribute of Characteristic with the UUID provided from the local attribute database.
 *
 * @param dev_id[in]    The Peer Device ID.
 * @param serv[in]      The GATT Service Handle Structure. (used start_handle & end_handle)
 * @param uuid[in]      The Characteristic UUID.
 * @param attr[inout]    This function calling result in attr->characteristic.
 */
extern OSStatus ble_access_get_characteritic_by_uuid(uint32_t dev_id,
                                                     const ble_access_serv_t *serv,
                                                     const ble_access_uuid_t *uuid,
                                                     ble_access_attribute_t *attr);

/*
 * Find all characteristics for 'serv' from the local attribute database and generate
 * several EVENT 'BLE_ACCESS_EVENT_CHAR' and 'BLE_ACCESS_EVENT_CHAR_CMPL' to user application.
 *
 * @param dev_id[in]    The Peer Device ID.
 * @param serv[in]      The GATT Service Handle Structure. (used start_handle & end_handle)
 */
extern OSStatus ble_access_get_characteristics(uint32_t dev_id,
                                               const ble_access_serv_t *serv);

/*
 * Find and read attribute with the Handle provided from the Attribute Cache
 *
 * @param dev_id[in]    The Peer Device ID.
 * @param handle[in]    The Attribute Handle Value
 * @param attr[out]     This function calling result. And user should make sure its is a valid pointer.
 * @param size[in]      The 'attr' buffer size in bytes.
 */
extern OSStatus ble_access_get_attribute_by_handle(uint32_t dev_id,
                                                   uint16_t handle,
                                                   ble_access_attribute_t *attr);

/*
 * Update Characteristic Value in the Attribute To the server
 *
 * @param dev_id[in]    The Peer device ID
 * @param handle[in]    The characteristic value handle.
 * @param length[in]    The lenght of Data to update characteristic value.
 * @param data[in]      Point to the data to update characteristic value.
 */
extern OSStatus ble_access_update_characteristic_value(uint32_t dev_id, uint16_t handle, uint8_t length, uint8_t *data);

/*
 * Enable Characteristic Client Configuration Indication or Notification with the
 * attribute provided from the Attribute Cache
 *
 * @param dev_id[in]    The Peer Device ID.
 * @param attr[int]     This function calling result. And user should make sure its is a valid pointer.
 * @param notify[in]    Notification(YES) or Indication(FALSE)
 */
extern OSStatus ble_access_enable_notification(uint32_t dev_id,
                                               const ble_access_attribute_t *attr,
                                               mico_bool_t notify);

/*
 * Compare two UUID type.
 *
 * @param uuid1[in]
 * @param uuid2[in]
 *
 * @return 0 if they are equal, otherwise non-zero.
 */
extern mico_bool_t ble_access_uuid_compare(const ble_access_uuid_t *uuid1, const ble_access_uuid_t *uuid2);

/*
 * Calculate device ID by Device Address
 *
 * @param addr[in] device address
 *
 * @return device ID or zero
 */
extern uint32_t ble_access_calculate_device_id(const mico_bt_device_address_t addr);

/*
 * Generate a device address by device ID
 *
 * @param addr[out]     device address
 * @param device_id[in] device ID
 *
 * @return kNoErr if successful.
 */
extern OSStatus ble_access_generate_device_address(mico_bt_device_address_t addr, uint32_t device_id);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_ACCESS_CORE_H__ */
