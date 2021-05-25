#ifndef _GATT_H_
#define _GATT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
*                         Macros
*============================================================================*/

/**

 * \addtogroup  GATT_SERVER_API GATT Server API
 *
 * \defgroup    GATT_ATTRIBUTE  GATT Attribute
 *
 * \brief   GATT Attribute Definitions
 *
 * \ingroup     GATT_SERVER_API
 */

/**
 * gatt.h
 *
 * \name  GATT_DECLARATIONS_UUID
 * \brief GATT declarations uuid.
 * \anchor  GATT_DECLARATIONS_UUID
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_UUID_PRIMARY_SERVICE       0x2800  /**<  GATT Primary Service Declaration. */
#define GATT_UUID_SECONDARY_SERVICE     0x2801  /**<  GATT Secondary Service Declaration. */
#define GATT_UUID_INCLUDE               0x2802  /**<  GATT Include Declaration. */
#define GATT_UUID_CHARACTERISTIC        0x2803  /**<  GATT Characteristic Declaration. */

/**
 * gatt.h
 *
 * \name  GATT_DESCRIPTORS_UUID
 * \brief GATT descriptors uuid.
 * \anchor  GATT_DESCRIPTORS_UUID
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_UUID_CHAR_EXTENDED_PROP                    0x2900  /**<  Characteristic Extended Properties. */
#define GATT_UUID_CHAR_USER_DESCR                       0x2901  /**<  Characteristic User Description. */
#define GATT_UUID_CHAR_CLIENT_CONFIG                    0x2902  /**<  Client Characteristic Configuration. */
#define GATT_UUID_CHAR_SERVER_CONFIG                    0x2903  /**<  Server Characteristic Configuration. */
#define GATT_UUID_CHAR_FORMAT                           0x2904  /**<  Characteristic Presentation Format. */
#define GATT_UUID_CHAR_AGGR_FORMAT                      0x2905  /**<  Characteristic Aggregate Format. */
#define GATT_UUID_CHAR_VALID_RANGE                      0x2906  /**<  Valid Range. */
#define GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE        0x2907  /**<  External Report Reference. */
#define GATT_UUID_CHAR_REPORT_REFERENCE                 0x2908  /**<  Report Reference. */
#define GATT_UUID_CHAR_DESCRIPTOR_NUM_OF_DIGITALS       0x2909  /**<  Number of Digitals. */
#define GATT_UUID_CHAR_DESCRIPTOR_VALUE_TRIGGER_SETTING 0x290A  /**<  Value Trigger Setting. */
#define GATT_UUID_CHAR_SENSING_CONFIGURATION            0x290B  /**<  Environmental Sensing Configuration. */
#define GATT_UUID_CHAR_SENSING_MEASUREMENT              0x290C  /**<  Environmental Sensing Measurement. */
#define GATT_UUID_CHAR_SENSING_TRIGGER_SETTING          0x290D  /**<  Environmental Sensing Trigger Setting. */
#define GATT_UUID_CHAR_DESCRIPTOR_TIME_TRIGGER_SETTING  0x290E  /**<  Time Trigger Setting. */

/**
 * gatt.h
 *
 * \name  GATT_CHARACTERISTICS_UUID
 * \brief GATT characteristics uuid.
 * \anchor  GATT_CHARACTERISTICS_UUID
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_UUID_CHAR_DEVICE_NAME                     0x2A00  /**<  Device Name. */
#define GATT_UUID_CHAR_APPEARANCE                      0x2A01  /**<  Appearance. */
#define GATT_UUID_CHAR_PER_PRIV_FLAG                   0x2A02  /**<  Peripheral Privacy Flag. */
#define GATT_UUID_CHAR_RECONN_ADDRESS                  0x2A03  /**<  Reconnection Address. */
#define GATT_UUID_CHAR_PER_PREF_CONN_PARAM             0x2A04  /**<  Peripheral Preferred Connection Parameters. */
#define GATT_UUID_CHAR_SERVICE_CHANGED                 0x2A05  /**<  Service Changed. */
#define GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION      0x2AA6  /**<  Central Address Resolution. */
#define GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY 0x2AC9  /**<  Resolvable Private Address Only. */

/**
 * gatt.h
 *
 * \name  GATT_SERVICE_UUID
 * \brief GATT services uuid.
 * \anchor  GATT_SERVICE_UUID
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_UUID_GAP                       0x1800  /**<  Generic Access. */
#define GATT_UUID_GATT                      0x1801  /**<  Generic Attribute. */

/**
 * gatt.h
 *
 * \name  GATT_ATTRIBUTE_PERMISSIONS
 * \brief Attribute read/write permissions, encryption key size.
 * \anchor  GATT_ATTRIBUTE_PERMISSIONS
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_PERM_NONE                   0x00
#define GATT_PERM_ALL                    0x01  /**< bits 0..1 (rd), 4..5 (wr), 8..9 (notif/ind) */
#define GATT_PERM_AUTHEN_REQ             0x02
#define GATT_PERM_AUTHEN_MITM_REQ        0x03
#define GATT_PERM_AUTHOR_REQ             0x04  /**< bits 2 (rd), 6 (wr), 10 (notif/ind) */
#define GATT_PERM_ENCRYPTED_REQ          0x08  /**< bits 3 (rd), 7 (wr), 11 (notif/ind) */
#define GATT_PERM_AUTHEN_SC_REQ          0x00010000

/** @brief read (bits 0..3) */
#define GATT_PERM_READ                   GATT_PERM_ALL
#define GATT_PERM_READ_AUTHEN_REQ        GATT_PERM_AUTHEN_REQ
#define GATT_PERM_READ_AUTHEN_MITM_REQ   GATT_PERM_AUTHEN_MITM_REQ
#define GATT_PERM_READ_AUTHOR_REQ        GATT_PERM_AUTHOR_REQ
#define GATT_PERM_READ_ENCRYPTED_REQ     GATT_PERM_ENCRYPTED_REQ
#define GATT_PERM_READ_AUTHEN_SC_REQ     GATT_PERM_AUTHEN_SC_REQ

#define GATT_PERM_READ_AUTHEN_GET(x)     (x & 0x03)
#define GATT_PERM_READ_AUTHOR_GET(x)     (x & 0x04)
#define GATT_PERM_READ_ENCRYPT_GET(x)    (x & 0x08)
#define GATT_PERM_READ_AUTHEN_SC_GET(x)  (x & 0x00010000)

/** @brief write (bits 4..7) */
#define GATT_PERM_WRITE                  (GATT_PERM_ALL << 4)
#define GATT_PERM_WRITE_AUTHEN_REQ       (GATT_PERM_AUTHEN_REQ << 4)
#define GATT_PERM_WRITE_AUTHEN_MITM_REQ  (GATT_PERM_AUTHEN_MITM_REQ << 4)
#define GATT_PERM_WRITE_AUTHOR_REQ       (GATT_PERM_AUTHOR_REQ << 4)
#define GATT_PERM_WRITE_ENCRYPTED_REQ    (GATT_PERM_ENCRYPTED_REQ << 4)
#define GATT_PERM_WRITE_AUTHEN_SC_REQ    (GATT_PERM_AUTHEN_SC_REQ<<4)

#define GATT_PERM_WRITE_AUTHEN_GET(x)    ((x >> 4) & 0x03)
#define GATT_PERM_WRITE_AUTHOR_GET(x)    ((x >> 4) & 0x04)
#define GATT_PERM_WRITE_ENCRYPT_GET(x)   ((x >> 4) & 0x08)
#define GATT_PERM_WRITE_AUTHEN_SC_GET(x) ((x>>4) & 0x00010000)

/** @brief notification/indication (bits 8..11) */
#define GATT_PERM_NOTIF_IND                  (GATT_PERM_ALL << 8)
#define GATT_PERM_NOTIF_IND_AUTHEN_REQ       (GATT_PERM_AUTHEN_REQ << 8)
#define GATT_PERM_NOTIF_IND_AUTHEN_MITM_REQ  (GATT_PERM_AUTHEN_MITM_REQ << 8)
#define GATT_PERM_NOTIF_IND_AUTHOR_REQ       (GATT_PERM_AUTHOR_REQ << 8)
#define GATT_PERM_NOTIF_IND_ENCRYPTED_REQ    (GATT_PERM_ENCRYPTED_REQ << 8)
#define GATT_PERM_NOTIF_IND_AUTHEN_SC_REQ    (GATT_PERM_AUTHEN_SC_REQ<<8)

#define GATT_PERM_NOTIF_IND_AUTHEN_GET(x)    ((x >> 8) & 0x03)
#define GATT_PERM_NOTIF_IND_AUTHOR_GET(x)    ((x >> 8) & 0x04)
#define GATT_PERM_NOTIF_IND_ENCRYPT_GET(x)   ((x >> 8) & 0x08)
#define GATT_PERM_NOTIF_IND_AUTHEN_SC_GET(x) ((x>>8) & 0x00010000)

/** @brief key size - 1 (bits 12..15) */
#define GATT_PERM_KEYSIZE(size)          ((size-1) << 12)
#define GATT_PERM_KEYSIZE_GET(x, size)   {                             \
        size = ((x >> 12) & 0x0F);  \
        if ( size > 0 )             \
            size++;                   \
    }

/**
 * gatt.h
 *
 * \name  GATT_CHARACTERISTIC_PROPERTIES
 * \brief GATT characteristic properties.
 * \anchor  GATT_CHARACTERISTIC_PROPERTIES
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define GATT_CHAR_PROP_BROADCAST           0x01  /**< If set, permits broadcasts of the Characteristic Value using
Server Characteristic Configuration Descriptor. */
#define GATT_CHAR_PROP_READ                0x02  /**< If set, permits reads of the Characteristic Value */
#define GATT_CHAR_PROP_WRITE_NO_RSP        0x04  /**< If set, permit writes of the Characteristic Value without response */
#define GATT_CHAR_PROP_WRITE               0x08  /**< If set, permits writes of the Characteristic Value with response */
#define GATT_CHAR_PROP_NOTIFY              0x10  /**< If set, permits notifications of a Characteristic Value without acknowledgment */
#define GATT_CHAR_PROP_INDICATE            0x20  /**< If set, permits indications of a Characteristic Value with acknowledgment */
#define GATT_CHAR_PROP_WRITE_AUTHEN_SIGNED 0x40  /**< If set, permits signed writes to the Characteristic Value */
#define GATT_CHAR_PROP_EXT_PROP            0x80  /**< If set, additional characteristic properties are defined in the Characteristic
Extended Properties Descriptor */

/** @brief GATT client characteristic configuration bit field */
#define GATT_CLIENT_CHAR_CONFIG_DEFAULT         0x0000 /**< The Characteristic Value shall be neither indicated nor notified. */
#define GATT_CLIENT_CHAR_CONFIG_NOTIFY          0x0001 /**< The Characteristic Value shall be notified. */
#define GATT_CLIENT_CHAR_CONFIG_INDICATE        0x0002 /**< The Characteristic Value shall be indicated. */
#define GATT_CLIENT_CHAR_CONFIG_NOTIFY_INDICATE 0x0003 /**< The Characteristic Value shall be both indicated and notified. */

/** @brief GATT server characteristic configuration bit field */
#define GATT_SERVER_CHAR_CONFIG_BROADCAST       0x0001 /**< The Characteristic Value shall be broadcast. */


/**
 * gatt.h
 *
 * \name  GATT_UUID_SIZE
 * \brief GATT uuid size.
 * \anchor  GATT_UUID_SIZE
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define UUID_16BIT_SIZE                 2   /**< 16 bits UUID size. */
#define UUID_128BIT_SIZE                16  /**< 128 bits UUID size. */

#define ATT_ATTRIBUTE_MAX_LENGTH        512 /**< Attribute Max length. */

#define HI_WORD(x)  ((uint8_t)((x & 0xFF00) >> 8))
#define LO_WORD(x)  ((uint8_t)(x))


/*---------------------------------------------------------------------------
 * GATT server attribute descriptor
 *--------------------------------------------------------------------------*/

/**
 * gatt.h
 *
 * \name  GATT_ATTRIBUTE_FLAG
 * \brief GATT attribute flag.
 * \anchor  GATT_ATTRIBUTE_FLAG
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
#define ATTRIB_FLAG_VOID           0x0000 /**< Attribute value neither supplied by application
nor included following 16bit UUID. Attribute value is pointed by p_value_context
and value_len shall be set to the length of attribute value. */
#define ATTRIB_FLAG_UUID_128BIT    0x0001 /**< Attribute uses 128 bit UUID */
#define ATTRIB_FLAG_VALUE_INCL     0x0002 /**< Attribute value is included following 16 bit UUID */
#define ATTRIB_FLAG_VALUE_APPL     0x0004 /**< Application has to supply write value */
#define ATTRIB_FLAG_ASCII_Z        0x0008 /**< Attribute value is ASCII_Z string */
#define ATTRIB_FLAG_CCCD_APPL      0x0010 /**< Application will be informed about CCCD value is changed */
#define ATTRIB_FLAG_CCCD_NO_FILTER 0x0020 /**< Application will be informed about CCCD value
when CCCD is write by client, no matter it is changed or not */
#define ATTRIB_FLAG_LE             0x0800 /**< Used only for primary service declaration attributes if GATT over BLE is supported */

/**
 * gatt.h
 *
 * \name  GATT_ATTRIBUTE_APPL
 * \brief GATT attribute definition.
 * \anchor  GATT_ATTRIBUTE_APPL
 */
/**
 * \ingroup     GATT_ATTRIBUTE
 */
typedef struct
{
    uint16_t    flags;              /**< Attribute flags @ref GATT_ATTRIBUTE_FLAG */
    uint8_t     type_value[2 + 14]; /**< 16 bit UUID + included value or 128 bit UUID */
    uint16_t    value_len;          /**< Length of value */
    void        *p_value_context;   /**< Pointer to value if @ref ATTRIB_FLAG_VALUE_INCL
    and @ref ATTRIB_FLAG_VALUE_APPL not set */
    uint32_t    permissions;        /**< Attribute permission @ref GATT_ATTRIBUTE_PERMISSIONS */
} T_ATTRIB_APPL;

#ifdef __cplusplus
}
#endif

#endif /* _GATT_H_ */
