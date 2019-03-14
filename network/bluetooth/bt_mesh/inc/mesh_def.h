#ifndef _GLUE_H_
#define _GLUE_H_

#include <stdint.h>

typedef uint8_t esp_ble_mesh_dev_add_flag_t;
#define ADD_DEV_RM_AFTER_PROV_FLAG  BIT(0)
#define ADD_DEV_START_PROV_NOW_FLAG BIT(1)
#define ADD_DEV_FLUSHABLE_DEV_FLAG  BIT(2)

/* Error codes for Error response PDU */
#define BT_MESH_ATT_ERR_INVALID_HANDLE               0x01
#define BT_MESH_ATT_ERR_READ_NOT_PERMITTED           0x02
#define BT_MESH_ATT_ERR_WRITE_NOT_PERMITTED          0x03
#define BT_MESH_ATT_ERR_INVALID_PDU                  0x04
#define BT_MESH_ATT_ERR_AUTHENTICATION               0x05
#define BT_MESH_ATT_ERR_NOT_SUPPORTED                0x06
#define BT_MESH_ATT_ERR_INVALID_OFFSET               0x07
#define BT_MESH_ATT_ERR_AUTHORIZATION                0x08
#define BT_MESH_ATT_ERR_PREPARE_QUEUE_FULL           0x09
#define BT_MESH_ATT_ERR_ATTRIBUTE_NOT_FOUND          0x0a
#define BT_MESH_ATT_ERR_ATTRIBUTE_NOT_LONG           0x0b
#define BT_MESH_ATT_ERR_ENCRYPTION_KEY_SIZE          0x0c
#define BT_MESH_ATT_ERR_INVALID_ATTRIBUTE_LEN        0x0d
#define BT_MESH_ATT_ERR_UNLIKELY                     0x0e
#define BT_MESH_ATT_ERR_INSUFFICIENT_ENCRYPTION      0x0f
#define BT_MESH_ATT_ERR_UNSUPPORTED_GROUP_TYPE       0x10
#define BT_MESH_ATT_ERR_INSUFFICIENT_RESOURCES       0x11

/* Common Profile Error Codes (from CSS) */
#define BT_MESH_ATT_ERR_WRITE_REQ_REJECTED           0xfc
#define BT_MESH_ATT_ERR_CCC_IMPROPER_CONF            0xfd
#define BT_MESH_ATT_ERR_PROCEDURE_IN_PROGRESS        0xfe
#define BT_MESH_ATT_ERR_OUT_OF_RANGE                 0xff

#define BT_MESH_GATT_ERR(_att_err)                  (-(_att_err))

/* Client Characteristic Configuration Values */

/** @def BT_MESH_GATT_CCC_NOTIFY
 *  @brief Client Characteristic Configuration Notification.
 *
 *  If set, changes to Characteristic Value shall be notified.
 */
#define BT_MESH_GATT_CCC_NOTIFY                      0x0001
/** @def BT_MESH_GATT_CCC_INDICATE
 *  @brief Client Characteristic Configuration Indication.
 *
 *  If set, changes to Characteristic Value shall be indicated.
 */
#define BT_MESH_GATT_CCC_INDICATE                    0x0002

typedef void * bt_mesh_conn_t;

struct bt_mesh_conn_cb {
        /** @brief A new connection has been established.
         *
         *  This callback notifies the application of a new connection.
         *  In case the err parameter is non-zero it means that the
         *  connection establishment failed.
         *
         *  @param conn New connection object.
         *  @param err HCI error. Zero for success, non-zero otherwise.
         */
        void (*connected)(bt_mesh_conn_t conn, uint8_t err);

        /** @brief A connection has been disconnected.
         *
         *  This callback notifies the application that a connection
         *  has been disconnected.
         *
         *  @param conn Connection object.
         *  @param reason HCI reason for the disconnection.
         */
        void (*disconnected)(bt_mesh_conn_t conn, uint8_t reason);
};

/** @brief Bluetooth UUID types */
enum {
        BT_MESH_UUID_TYPE_16,
        BT_MESH_UUID_TYPE_32,
        BT_MESH_UUID_TYPE_128,
};

/** @brief This is a 'tentative' type and should be used as a pointer only */
struct bt_mesh_uuid {
        uint8_t type;
};

struct bt_mesh_uuid_16 {
        struct bt_mesh_uuid uuid;
        uint16_t val;
};

struct bt_mesh_uuid_32 {
        struct bt_mesh_uuid uuid;
        uint32_t val;
};

struct bt_mesh_uuid_128 {
        struct bt_mesh_uuid uuid;
        uint8_t val[16];
};

#define BT_MESH_UUID_INIT_16(value)             \
{                                          \
        .uuid.type = BT_MESH_UUID_TYPE_16, \
        .val = (value),                    \
}

#define BT_MESH_UUID_INIT_32(value)             \
{                                          \
        .uuid.type = BT_MESH_UUID_TYPE_32, \
        .val = (value),                    \
}

#define BT_MESH_UUID_INIT_128(value...)         \
{                                          \
        .uuid.type = BT_MESH_UUID_TYPE_128,\
        .val = { value },                  \
}

#define BT_MESH_UUID_DECLARE_16(value) \
        ((struct bt_mesh_uuid *) (&(struct bt_mesh_uuid_16) BT_MESH_UUID_INIT_16(value)))
#define BT_MESH_UUID_DECLARE_32(value) \
        ((struct bt_mesh_uuid *) (&(struct bt_mesh_uuid_32) BT_MESH_UUID_INIT_32(value)))
#define BT_MESH_UUID_DECLARE_128(value...) \
        ((struct bt_mesh_uuid *) (&(struct bt_mesh_uuid_128) BT_MESH_UUID_INIT_128(value)))

/** @def BT_MESH_UUID_MESH_PROV
 *  @brief Mesh Provisioning Service
 */
#define BT_MESH_UUID_MESH_PROV                 BT_MESH_UUID_DECLARE_16(0x1827)
#define BT_MESH_UUID_MESH_PROV_VAL             0x1827
/** @def BT_MESH_UUID_MESH_PROXY
 *  @brief Mesh Proxy Service
 */
#define BT_MESH_UUID_MESH_PROXY                BT_MESH_UUID_DECLARE_16(0x1828)
#define BT_MESH_UUID_MESH_PROXY_VAL            0x1828
/** @def BT_MESH_UUID_GATT_PRIMARY
 *  @brief GATT Primary Service
 */
#define BT_MESH_UUID_GATT_PRIMARY              BT_MESH_UUID_DECLARE_16(0x2800)
#define BT_MESH_UUID_GATT_PRIMARY_VAL          0x2800
/** @def BT_UUID_GATT_CHRC
 *  @brief GATT Characteristic
 */
#define BT_MESH_UUID_GATT_CHRC                 BT_MESH_UUID_DECLARE_16(0x2803)
#define BT_MESH_UUID_GATT_CHRC_VAL             0x2803
/** @def BT_MESH_UUID_GATT_CCC
 *  @brief GATT Client Characteristic Configuration
 */
#define BT_MESH_UUID_GATT_CCC                  BT_MESH_UUID_DECLARE_16(0x2902)
#define BT_MESH_UUID_GATT_CCC_VAL              0x2902
/** @def BT_MESH_UUID_MESH_PROV_DATA_IN
 *  @brief Mesh Provisioning Data In
 */
#define BT_MESH_UUID_MESH_PROV_DATA_IN         BT_MESH_UUID_DECLARE_16(0x2adb)
#define BT_MESH_UUID_MESH_PROV_DATA_IN_VAL     0x2adb
/** @def BT_MESH_UUID_MESH_PROV_DATA_OUT
 *  @brief Mesh Provisioning Data Out
 */
#define BT_MESH_UUID_MESH_PROV_DATA_OUT        BT_MESH_UUID_DECLARE_16(0x2adc)
#define BT_MESH_UUID_MESH_PROV_DATA_OUT_VAL    0x2adc
/** @def BT_MESH_UUID_MESH_PROXY_DATA_IN
 *  @brief Mesh Proxy Data In
 */
#define BT_MESH_UUID_MESH_PROXY_DATA_IN        BT_MESH_UUID_DECLARE_16(0x2add)
#define BT_MESH_UUID_MESH_PROXY_DATA_IN_VAL    0x2add
/** @def BT_MESH_UUID_MESH_PROXY_DATA_OUT
 *  @brief Mesh Proxy Data Out
 */
#define BT_MESH_UUID_MESH_PROXY_DATA_OUT       BT_MESH_UUID_DECLARE_16(0x2ade)
#define BT_MESH_UUID_MESH_PROXY_DATA_OUT_VAL   0x2ade

/** @brief GATT Attribute structure. */
struct bt_mesh_gatt_attr {
	/** Attribute UUID */
	const struct bt_mesh_uuid	*uuid;

	/** Attribute read callback
	 *
	 *  @param conn   The connection that is requesting to read
	 *  @param attr   The attribute that's being read
	 *  @param buf    Buffer to place the read result in
	 *  @param len    Length of data to read
	 *  @param offset Offset to start reading from
	 *
	 *  @return Number fo bytes read, or in case of an error
	 *          BT_MESH_GATT_ERR() with a specific ATT error code.
	 */
	int			(*read)(bt_mesh_conn_t conn,
					const struct bt_mesh_gatt_attr *attr,
					void *buf, uint16_t len,
					uint16_t offset);

	/** Attribute write callback
	 *
	 *  @param conn   The connection that is requesting to write
	 *  @param attr   The attribute that's being written
	 *  @param buf    Buffer with the data to write
	 *  @param len    Number of bytes in the buffer
	 *  @param offset Offset to start writing from
	 *  @param flags  Flags (BT_MESH_GATT_WRITE_*)
	 *
	 *  @return Number of bytes written, or in case of an error
	 *          BT_MESH_GATT_ERR() with a specific ATT error code.
	 */
	int			(*write)(bt_mesh_conn_t conn,
					 const struct bt_mesh_gatt_attr *attr,
					 const void *buf, uint16_t len,
					 uint16_t offset, uint8_t flags);

	/** Attribute user data */
	void			*user_data;
	/** Attribute handle */
	uint16_t		handle;
	/** Attribute permissions */
	uint8_t			perm;
};

/** @brief GATT Service structure */
struct bt_mesh_gatt_service {
	/** Service Attributes */
	struct bt_mesh_gatt_attr	*attrs;
	/** Service Attribute count */
	uint16_t			attr_count;
};

typedef struct {
        uint8_t  val[6];
} bt_mesh_addr_t;

/** LE Advertising Parameters. */
struct bt_mesh_le_adv_param {
        /** Bit-field of advertising options */
        uint8_t  options;

        /** Minimum Advertising Interval (N * 0.625) */
        uint16_t interval_min;

        /** Maximum Advertising Interval (N * 0.625) */
        uint16_t interval_max;

        /** Optional predefined (random) own address. Currently
         *  the only permitted use of this is for NRPA with
         *  non-connectable advertising.
         */
        const bt_mesh_addr_t *own_addr;
};

/** LE scan parameters */
struct bt_mesh_le_scan_param {
        /** Scan type (BT_MESH_HCI_LE_SCAN_ACTIVE or BT_MESH_HCI_LE_SCAN_PASSIVE) */
        uint8_t  type;

        /** Duplicate filtering (BT_MESH_HCI_LE_SCAN_FILTER_DUP_ENABLE or
         *  BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE)
         */
        uint8_t  filter_dup;

        /** Scan interval (N * 0.625 ms) */
        uint16_t interval;

        /** Scan window (N * 0.625 ms) */
        uint16_t window;
};

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_mesh_le_adv_start() function.
  */
struct bt_mesh_data {
        uint8_t type;
        uint8_t data_len;
        const uint8_t *data;
};

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _data Pointer to the data field payload
 *  @param _data_len Number of bytes behind the _data pointer
 */
#define BT_MESH_DATA(_type, _data, _data_len) \
        { \
                .type = (_type), \
                .data_len = (_data_len), \
                .data = (const u8_t *)(_data), \
        }

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _bytes Variable number of single-byte parameters
 */
#define BT_MESH_DATA_BYTES(_type, _bytes...) \
        BT_MESH_DATA(_type, ((u8_t []) { _bytes }), \
                sizeof((u8_t []) { _bytes }))

/* EIR/AD data type definitions */
#define BT_MESH_DATA_FLAGS                   0x01 /* AD flags */
#define BT_MESH_DATA_UUID16_SOME             0x02 /* 16-bit UUID, more available */
#define BT_MESH_DATA_UUID16_ALL              0x03 /* 16-bit UUID, all listed */
#define BT_MESH_DATA_UUID32_SOME             0x04 /* 32-bit UUID, more available */
#define BT_MESH_DATA_UUID32_ALL              0x05 /* 32-bit UUID, all listed */
#define BT_MESH_DATA_UUID128_SOME            0x06 /* 128-bit UUID, more available */
#define BT_MESH_DATA_UUID128_ALL             0x07 /* 128-bit UUID, all listed */
#define BT_MESH_DATA_NAME_SHORTENED          0x08 /* Shortened name */
#define BT_MESH_DATA_NAME_COMPLETE           0x09 /* Complete name */
#define BT_MESH_DATA_TX_POWER                0x0a /* Tx Power */
#define BT_MESH_DATA_SOLICIT16               0x14 /* Solicit UUIDs, 16-bit */
#define BT_MESH_DATA_SOLICIT128              0x15 /* Solicit UUIDs, 128-bit */
#define BT_MESH_DATA_SVC_DATA16              0x16 /* Service data, 16-bit UUID */
#define BT_MESH_DATA_GAP_APPEARANCE          0x19 /* GAP appearance */
#define BT_MESH_DATA_SOLICIT32               0x1f /* Solicit UUIDs, 32-bit */
#define BT_MESH_DATA_SVC_DATA32              0x20 /* Service data, 32-bit UUID */
#define BT_MESH_DATA_SVC_DATA128             0x21 /* Service data, 128-bit UUID */
#define BT_MESH_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define BT_MESH_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define BT_MESH_DATA_MESH_BEACON             0x2b /* Mesh Beacon */

#define BT_MESH_DATA_MANUFACTURER_DATA       0xff /* Manufacturer Specific Data */

#define BT_MESH_LE_AD_LIMITED                0x01 /* Limited Discoverable */
#define BT_MESH_LE_AD_GENERAL                0x02 /* General Discoverable */
#define BT_MESH_LE_AD_NO_BREDR               0x04 /* BR/EDR not supported */

/* GATT attribute permission bit field values */
enum {
	/** No operations supported, e.g. for notify-only */
	BT_MESH_GATT_PERM_NONE = 0,

	/** Attribute read permission. */
	BT_MESH_GATT_PERM_READ = 1 << 0,

	/** Attribute write permission. */
	BT_MESH_GATT_PERM_WRITE = 1 << 1,

	/** Attribute read permission with encryption.
	 *
	 *  If set, requires encryption for read access.
	 */
	BT_MESH_GATT_PERM_READ_ENCRYPT = 1 << 2,

	/** Attribute write permission with encryption.
	 *
	 *  If set, requires encryption for write access.
	 */
	BT_MESH_GATT_PERM_WRITE_ENCRYPT = 1 << 3,

	/** Attribute read permission with authentication.
	 *
	 *  If set, requires encryption using authenticated link-key for read
	 *  access.
	 */
	BT_MESH_GATT_PERM_READ_AUTHEN = 1 << 4,

	/** Attribute write permission with authentication.
	 *
	 *  If set, requires encryption using authenticated link-key for write
	 *  access.
	 */
	BT_MESH_GATT_PERM_WRITE_AUTHEN = 1 << 5,

	/** Attribute prepare write permission.
	 *
	 *  If set, allows prepare writes with use of BT_MESH_GATT_WRITE_FLAG_PREPARE
	 *  passed to write callback.
	 */
	BT_MESH_GATT_PERM_PREPARE_WRITE = 1 << 6,
};

/* Characteristic Properties Bit field values */

/** @def BT_MESH_GATT_CHRC_BROADCAST
 *  @brief Characteristic broadcast property.
 *
 *  If set, permits broadcasts of the Characteristic Value using Server
 *  Characteristic Configuration Descriptor.
 */
#define BT_MESH_GATT_CHRC_BROADCAST			0x01
/** @def BT_MESH_GATT_CHRC_READ
 *  @brief Characteristic read property.
 *
 *  If set, permits reads of the Characteristic Value.
 */
#define BT_MESH_GATT_CHRC_READ			0x02
/** @def BT_MESH_GATT_CHRC_WRITE_WITHOUT_RESP
 *  @brief Characteristic write without response property.
 *
 *  If set, permits write of the Characteristic Value without response.
 */
#define BT_MESH_GATT_CHRC_WRITE_WITHOUT_RESP		0x04
/** @def BT_MESH_GATT_CHRC_WRITE
 *  @brief Characteristic write with response property.
 *
 *  If set, permits write of the Characteristic Value with response.
 */
#define BT_MESH_GATT_CHRC_WRITE			0x08
/** @def BT_MESH_GATT_CHRC_NOTIFY
 *  @brief Characteristic notify property.
 *
 *  If set, permits notifications of a Characteristic Value without
 *  acknowledgment.
 */
#define BT_MESH_GATT_CHRC_NOTIFY			0x10
/** @def BT_MESH_GATT_CHRC_INDICATE
 *  @brief Characteristic indicate property.
 *
 * If set, permits indications of a Characteristic Value with acknowledgment.
 */
#define BT_MESH_GATT_CHRC_INDICATE			0x20
/** @def BT_MESH_GATT_CHRC_AUTH
 *  @brief Characteristic Authenticated Signed Writes property.
 *
 *  If set, permits signed writes to the Characteristic Value.
 */
#define BT_MESH_GATT_CHRC_AUTH			0x40
/** @def BT_MESH_GATT_CHRC_EXT_PROP
 *  @brief Characteristic Extended Properties property.
 *
 * If set, additional characteristic properties are defined in the
 * Characteristic Extended Properties Descriptor.
 */
#define BT_MESH_GATT_CHRC_EXT_PROP			0x80

/** @brief Characteristic Attribute Value. */
struct bt_mesh_gatt_chrc {
        /** Characteristic UUID. */
        const struct bt_mesh_uuid    *uuid;
        /** Characteristic properties. */
        uint8_t                    properties;
};

/** @def BT_MESH_GATT_SERVICE
 *  @brief Service Structure Declaration Macro.
 *
 *  Helper macro to declare a service structure.
 *
 *  @param _attrs Service attributes.
 */
#define BT_MESH_GATT_SERVICE(_attrs)                                    \
{                                                                       \
        .attrs = _attrs,                                                \
        .attr_count = sizeof(_attrs) / sizeof((_attrs)[0]),             \
}

/** @def BT_MESH_GATT_PRIMARY_SERVICE
 *  @brief Primary Service Declaration Macro.
 *
 *  Helper macro to declare a primary service attribute.
 *
 *  @param _service Service attribute value.
 */

#define BT_MESH_GATT_PRIMARY_SERVICE(_service)                          \
{                                                                       \
        .uuid = BT_MESH_UUID_GATT_PRIMARY,                                   \
        .perm = BT_MESH_GATT_PERM_READ,                                      \
        .read = bt_mesh_gatt_attr_read_service,                              \
        .user_data = _service,                                          \
}

/** @def BT_MESH_GATT_CHARACTERISTIC
 *  @brief Characteristic Declaration Macro.
 *
 *  Helper macro to declare a characteristic attribute.
 *
 *  @param _uuid Characteristic attribute uuid.
 *  @param _props Characteristic attribute properties.
 */
#define BT_MESH_GATT_CHARACTERISTIC(_uuid, _props)                      \
{                                                                       \
        .uuid = BT_MESH_UUID_GATT_CHRC,                                      \
        .perm = BT_MESH_GATT_PERM_READ,                                      \
        .read = bt_mesh_gatt_attr_read_chrc,                                 \
        .user_data = (&(struct bt_mesh_gatt_chrc) { .uuid = _uuid,           \
                                                    .properties = _props, }),\
}

/** @def BT_MESH_GATT_DESCRIPTOR
 *  @brief Descriptor Declaration Macro.
 *
 *  Helper macro to declare a descriptor attribute.
 *
 *  @param _uuid Descriptor attribute uuid.
 *  @param _perm Descriptor attribute access permissions.
 *  @param _read Descriptor attribute read callback.
 *  @param _write Descriptor attribute write callback.
 *  @param _value Descriptor attribute value.
 */
#define BT_MESH_GATT_DESCRIPTOR(_uuid, _perm, _read, _write, _value)    \
{                                                                       \
        .uuid = _uuid,                                                  \
        .perm = _perm,                                                  \
        .read = _read,                                                  \
        .write = _write,                                                \
        .user_data = _value,                                            \
}

/* Advertising types */
#define BT_MESH_LE_ADV_IND                           0x00
#define BT_MESH_LE_ADV_DIRECT_IND                    0x01
#define BT_MESH_LE_ADV_SCAN_IND                      0x02
#define BT_MESH_LE_ADV_NONCONN_IND                   0x03
#define BT_MESH_LE_ADV_DIRECT_IND_LOW_DUTY           0x04

/** Advertising options */
enum {
        /** Convenience value when no options are specified. */
        BT_MESH_LE_ADV_OPT_NONE = 0,

        /** Advertise as connectable. Type of advertising is determined by
         * providing SCAN_RSP data and/or enabling local privacy support.
         */
        BT_MESH_LE_ADV_OPT_CONNECTABLE = 1 << 0,

        /** Don't try to resume connectable advertising after a connection.
         *  This option is only meaningful when used together with
         *  BT_LE_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
         *  when bt_le_adv_stop() is called or when an incoming (slave)
         *  connection happens. If this option is not set the stack will
         *  take care of keeping advertising enabled even as connections
         *  occur.
         */
        BT_MESH_LE_ADV_OPT_ONE_TIME = 1 << 1,
};

/* Defined GAP timers */
#define BT_MESH_GAP_ADV_FAST_INT_MIN_1               0x0030  /* 30 ms    */
#define BT_MESH_GAP_ADV_FAST_INT_MAX_1               0x0060  /* 60 ms    */
#define BT_MESH_GAP_ADV_FAST_INT_MIN_2               0x00a0  /* 100 ms   */
#define BT_MESH_GAP_ADV_FAST_INT_MAX_2               0x00f0  /* 150 ms   */
#define BT_MESH_GAP_ADV_SLOW_INT_MIN                 0x0640  /* 1 s      */
#define BT_MESH_GAP_ADV_SLOW_INT_MAX                 0x0780  /* 1.2 s    */

/* Scan types */
#define BT_MESH_HCI_LE_SCAN_PASSIVE                  0x00
#define BT_MESH_HCI_LE_SCAN_ACTIVE                   0x01

#define BT_MESH_HCI_LE_SCAN_DISABLE                  0x00
#define BT_MESH_HCI_LE_SCAN_ENABLE                   0x01

#define BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE       0x00
#define BT_MESH_HCI_LE_SCAN_FILTER_DUP_ENABLE        0x01

/* HCI Error Codes */
#define BT_MESH_HCI_ERR_SUCCESS                      0x00
#define BT_MESH_HCI_ERR_UNKNOWN_CMD                  0x01
#define BT_MESH_HCI_ERR_UNKNOWN_CONN_ID              0x02
#define BT_MESH_HCI_ERR_AUTHENTICATION_FAIL          0x05
#define BT_MESH_HCI_ERR_PIN_OR_KEY_MISSING           0x06
#define BT_MESH_HCI_ERR_MEM_CAPACITY_EXCEEDED        0x07
#define BT_MESH_HCI_ERR_CMD_DISALLOWED               0x0c
#define BT_MESH_HCI_ERR_INSUFFICIENT_RESOURCES       0x0d
#define BT_MESH_HCI_ERR_UNSUPP_FEATURE_PARAM_VAL     0x11
#define BT_MESH_HCI_ERR_INVALID_PARAM                0x12
#define BT_MESH_HCI_ERR_REMOTE_USER_TERM_CONN        0x13
#define BT_MESH_HCI_ERR_PAIRING_NOT_ALLOWED          0x18
#define BT_MESH_HCI_ERR_UNSUPP_REMOTE_FEATURE        0x1a
#define BT_MESH_HCI_ERR_INVALID_LL_PARAM             0x1e
#define BT_MESH_HCI_ERR_UNSPECIFIED                  0x1f
#define BT_MESH_HCI_ERR_PAIRING_NOT_SUPPORTED        0x29
#define BT_MESH_HCI_ERR_UNACCEPT_CONN_PARAM          0x3b
#define BT_MESH_HCI_ERR_ADV_TIMEOUT                  0x3c

#endif
