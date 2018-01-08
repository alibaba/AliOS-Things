
/** @file
 *  Defines common constants and types for MiCO support for Bluetooth Smart
 */

#pragma once

#include "mico_bt_smartbridge_constants.h"
#include "mico_bt_smart_attribute.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/** @cond !ADDTHIS*/
/* Limits and Default Value for Scan Interval */
#define DEFAULT_SCAN_INTERVAL  0x0010
#define MIN_SCAN_INTERVAL      0x0004
#define MAX_SCAN_INTERVAL      0x4000

/* Limits and Default Value for Scan Window */
#define DEFAULT_SCAN_WINDOW    0x0010
#define MIN_SCAN_WINDOW        0x0004
#define MAX_SCAN_WINDOW        0x4000

#define BONDING_DISABLED          0x00
#define BONDING_ENABLED           0x01
#define PASSKEY_ENTRY_ENABLED     0x04
#define SECURE_CONNECTION_ENABLE  0x08

#define DISTRIBUTE_ENCRYPTION_KEY 0x01
#define DISTRIBUTE_ID_KEY         0x02
#define DISTRIBUTE_SIGN_KEY       0x04
/** @endcond */

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 *  Bluetooth Smart filter policy
 */
typedef enum {
    FILTER_POLICY_NONE        = 0x00, /**< No filter policy         */
    FILTER_POLICY_WHITE_LIST  = 0x01, /**< White list filter policy */
} mico_bt_smart_filter_policy_t;

/**
 *  Bluetooth Smart scan duplicates filter
 */
typedef enum {
    DUPLICATES_FILTER_DISABLED = 0x00, /**< Duplicates filter is disabled */
    DUPLICATES_FILTER_ENABLED  = 0x01, /**< Duplicates filter is enabled  */
} mico_bt_smart_filter_duplicated_t;

/**
 *  Bluetooth Smart address type
 */
typedef enum {
    BT_SMART_ADDR_TYPE_PUBLIC  = 0x00, /**< Public address */
    BT_SMART_ADDR_TYPE_RANDOM  = 0x01  /**< Random address */
} mico_bt_smart_address_type_t;

/**
 *  Bluetooth Smart link role
 */
typedef enum {
    BT_SMART_LINK_ROLE_MASTER  = 0x00, /**< Public address */
    BT_SMART_LINK_ROLE_SLAVE  = 0x01  /**< Random address */
} mico_bt_smart_link_role_t;

/**
 *  Bluetooth Smart scan type
 */
typedef enum {
    BT_SMART_PASSIVE_SCAN  = 0x00, /**< Passive scan. Controller does not send SCAN_REQ and listens for Advertising from remote devices */
    BT_SMART_ACTIVE_SCAN   = 0x01, /**< Active scan. Controller sends SCAN_REQ. Controller listens for Advertising from remote devices and may receive SCAN_RSP from remote devices */
} mico_bt_smart_scan_type_t;

/**
 *  Bluetooth Smart advertising event
 */
typedef enum {
    BT_SMART_CONNECTABLE_UNDIRECTED_ADVERTISING_EVENT     = 0x00, /**< ADV_IND         : Connectable undirected advertising event     */
    BT_SMART_CONNECTABLE_DIRECTED_ADVERTISING_EVENT       = 0x01, /**< ADV_DIRECT_IND  : Connectable directed advertising event       */
    BT_SMART_SCANNABLE_UNDIRECTED_ADVERTISING_EVENT       = 0x02, /**< ADV_SCAN_IND    : Scannable undirected advertising event       */
    BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING_EVENT = 0x03, /**< ADV_NONCONN_IND : Non-connectable undirected advertising event */
    BT_SMART_SCAN_RESPONSE_EVENT                          = 0x04, /**< SCAN_RSP        : Scan response event                          */
} mico_bt_smart_advertising_event_t;

/**
 *  Bluetooth Smart advertising event
 */
typedef enum {
    BT_SMART_UNDIRECTED_ADVERTISING                     = 0x00, /**< General undirected advertising         */
    BT_SMART_DISCOVERABLE_ADVERTISING                   = 0x01, /**< Discoverable advertising               */
    BT_SMART_DIRECTED_ADVERTISING                       = 0x02, /**< Directed advertising                   */
    BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING     = 0x03, /**< Non-connectable undirected advertising */
} mico_bt_smart_advertising_type_t;

/**
 * Bluetooth Smart device input/output (IO) capabilities
 */
typedef enum {
    BT_SMART_IO_DISPLAY_ONLY           = 0x00,
    BT_SMART_IO_DISPLAY_YES_NO_BUTTONS = 0x01,
    BT_SMART_IO_KEYBOARD_ONLY          = 0x02,
    BT_SMART_IO_NO_INPUT_NO_OUTPUT     = 0x03,
    BT_SMART_IO_KEYBOARD_DISPLAY       = 0x04,
} mico_bt_io_capabilities_t;

/**
 * Bluetooth Smart device Authentication Requirements (AuthReq)
 */
typedef enum {
    BT_SMART_AUTH_REQ_NONE                      = 0,
    BT_SMART_AUTH_REQ_BONDING                   = ( BONDING_ENABLED ),
    BT_SMART_AUTH_REQ_PASSKEY_ENTRY             = ( PASSKEY_ENTRY_ENABLED ),
    BT_SMART_AUTH_REQ_BONDING_AND_PASSKEY_ENTRY = ( BONDING_ENABLED | PASSKEY_ENTRY_ENABLED ),
} mico_bt_smart_auth_req_t;

/**
 * Bluetooth Smart device bonding request type
 */
typedef enum {
    /* Bonding event -- user should reply it with #mico_bt_smart_bond_reply() */
    MICO_BT_SMART_BOND_PASS_KEY_REQ     = 0x01,
    /* Bonding event -- user should reply it with #mico_bt_smart_bond_reply() */
    MICO_BT_SMART_BOND_USR_CONFIRM_REQ  = 0x02,
    /* Bonding event -- user should reply it with #mico_bt_smart_bond_reply() */
    MICO_BT_SMART_BOND_OOB_DATA_REQ     = 0x03,
    /* Bonding event -- user should not reply it */
    MICO_BT_SMART_BOND_PASS_KEY_NOTIFY  = 0x04,
} mico_bt_smart_bond_request_type_t;

/**
 * Bluetooth Smart device key distribution types
 */
typedef enum {
    BT_SMART_DISTRIBUTE_NONE                     = 0,
    BT_SMART_DISTRIBUTE_ENCRYPTION_KEY           = ( DISTRIBUTE_ENCRYPTION_KEY ),
    BT_SMART_DISTRIBUTE_ID_KEY                   = ( DISTRIBUTE_ID_KEY ),
    BT_SMART_DISTRIBUTE_ENCRYPTION_AND_ID_KEYS   = ( DISTRIBUTE_ID_KEY | DISTRIBUTE_ENCRYPTION_KEY ),
    BT_SMART_DISTRIBUTE_SIGN_KEY                 = ( DISTRIBUTE_SIGN_KEY ),
    BT_SMART_DISTRIBUTE_ENCRYPTION_AND_SIGN_KEYS = ( DISTRIBUTE_SIGN_KEY | DISTRIBUTE_ENCRYPTION_KEY ),
    BT_SMART_DISTRIBUTE_ID_AND_SIGN_KEYS         = ( DISTRIBUTE_SIGN_KEY | DISTRIBUTE_ID_KEY ),
    BT_SMART_DISTRIBUTE_ALL_KEYS                 = ( DISTRIBUTE_SIGN_KEY | DISTRIBUTE_ID_KEY | DISTRIBUTE_ENCRYPTION_KEY ),
} mico_bt_smart_key_distribution_t;

/**
 *  Bluetooth Smart Out-of-Band (OOB) authentication data
 */
typedef enum {
    BT_SMART_OOB_AUTH_NONE,      /**< OOB authentication data is not available  */
    BT_SMART_OOB_AUTH_AVAILABLE, /**< OOB authentication data is available      */
} mico_bt_smart_oob_auth_t;

/******************************************************
 *                    Structures
 ******************************************************/

#pragma pack(1)

/**
 *  Bluetooth Smart device
 */
typedef struct {
    mico_bt_device_address_t            address;      /**< Bluetooth device address */
    mico_bt_smart_address_type_t        address_type; /**< Address Type             */
    char                                name[31];     /**< User-friendly name       */
} mico_bt_smart_device_t;

/**
 *  Bluetooth Smart scan settings
 */
typedef struct {
    mico_bt_smart_scan_type_t
    type;                 /**< Scan type                                                                                     */
    mico_bt_smart_filter_policy_t
    filter_policy;        /**< Scan filter policy                                                                            */
    mico_bt_smart_filter_duplicated_t
    filter_duplicates;    /**< Scan duplicates filter                                                                        */
    uint16_t
    interval;            /**< Interval between scans.                Unit: 0.625ms. Range: 0x0004 - 0x4000 (2.5ms - 10.24s) */
    uint16_t
    window;              /**< Scan window. Must be <= scan interval. Unit: 0.625ms. Range: 0x0004 - 0x4000 (2.5ms - 10.24s) */
    uint16_t
    duration_second;     /**< Scan duration in seconds                                                                      */
} mico_bt_smart_scan_settings_t;

/**
 *  Bluetooth Smart advertising report
 */
typedef struct {
    mico_bt_smart_device_t              remote_device;   /**< Remote device                                      */
    int8_t                              signal_strength; /**< RSSI in dBm                                        */
    mico_bt_smart_advertising_event_t   event;           /**< Advertising event received                         */
    uint8_t                             eir_data_length; /**< Length of EIR data received with advertising event */
    uint8_t                             eir_data[31];    /**< EIR data of advertising event                      */
} mico_bt_smart_advertising_report_t;

/**
 *  Bluetooth Smart scan result
 */
typedef struct mico_bt_smart_scan_result {
    mico_bt_smart_device_t              remote_device;                   /**< Remote device                       */
    int8_t                              signal_strength;                 /**< RSSI in dBm                         */
    mico_bt_smart_advertising_report_t  last_scan_response_received;     /**< Last scan response event received   */
    mico_bt_smart_advertising_report_t  last_advertising_event_received; /**< Last advertising event received     */
    struct mico_bt_smart_scan_result   *next;                            /**< Pointer to the next scan result     */

    /* Additional flag to help application filter scan results */
    mico_bool_t                         filter_display;                  /**< Set to MICO_TRUE if filter display */

} mico_bt_smart_scan_result_t;

/**
 *  Bluetooth Smart advertise settings
 */
typedef struct {
    mico_bt_smart_advertising_type_t
    type;                               /**< Advertising type                                               */
    mico_bool_t
    use_high_duty;                      /**< Start advertising use high duty cycle interval                 */
    uint16_t
    high_duty_interval;                 /**< High duty advertising interval                                 */
    uint16_t
    high_duty_duration;                 /**< High duty advertising duration in seconds (0 for infinite)     */
    uint16_t
    low_duty_interval;                  /**< Low duty advertising interval                                  */
    uint16_t
    low_duty_duration;                  /**< Low duty advertising duration in seconds (0 for infinite)      */
    mico_bt_smart_address_type_t
    directed_advertisement_addr_type;   /**< Target device address type for directed advertising            */
    mico_bt_device_address_t
    directed_advertisement_addr;        /**< Target device address for directed advertising                 */
} mico_bt_smart_advertising_settings_t;

/**
 *  Bluetooth Smart connection settings
 */
typedef struct {
    uint16_t
    timeout_second;                /**< Connection timeout in seconds                                                              */
    mico_bt_smart_filter_policy_t
    filter_policy;                 /**< Connection initiator filter policy: No filter or using white list                          */
    uint16_t
    interval_min;                  /**< Connection Interval Min.      Unit: 1.25ms.            Range: 0x000A - 0x0C80 (7.5ms - 4s) */
    uint16_t
    interval_max;                  /**< Connection Interval Max.      Unit: 1.25ms.            Range: 0x000A - 0x0C80 (7.5ms - 4s) */
    uint16_t
    latency;                       /**< Connection Latency.           Unit: Connection Events. Range: 0x0000 - 0x01F4              */
    uint16_t
    supervision_timeout;           /**< Supervision Timeout.          Unit: 10ms.              Range: 0x000A - 0x0C80 (100ms - 32s)*/
    uint16_t
    ce_length_min;                 /**< Connection Event Length Min.  Unit: Connection Events. Range: 0x0000 - 0xFFFF              */
    uint16_t
    ce_length_max;                 /**< Connection Event Length Max.  Unit: Connection Events. Range: 0x0000 - 0xFFFF              */
    uint32_t
    attribute_protocol_timeout_ms; /**< Attribute protocol timeout in milliseconds                                                 */
} mico_bt_smart_connection_settings_t;

/**
 *  Bluetooth Smart Extended Inquiry Response (EIR) data structure
 */
typedef struct {
    uint8_t length;   /**< Length        */
    uint8_t type;     /**< Type          */
    uint8_t data[1];  /**< Start of data */
} mico_bt_smart_eir_data_structure_t;

/**
 *  Bluetooth Smart security settings
 */
typedef struct {
    uint16_t                          timeout_second;              /**< Timeout in second. Default is 30 seconds      */
    mico_bt_io_capabilities_t         io_capabilities;             /**< Device I/O capability                         */
    mico_bt_smart_auth_req_t          authentication_requirements; /**< Authentication requirements                   */
    mico_bt_smart_oob_auth_t          oob_authentication;          /**< OOB authentication data                       */
    uint8_t                           max_encryption_key_size;     /**< Encryption key size (7 to 16 bytes)           */
    mico_bt_smart_key_distribution_t  master_key_distribution;     /**< Bit mask of master/initiator key distribution */
    mico_bt_smart_key_distribution_t  slave_key_distribution;      /**< Bit mask of slave/responder key distribution  */
} mico_bt_smart_security_settings_t;

/**
 *  Bluetooth Smart Peer Device Bond Info
 */
typedef struct {
    mico_bt_device_address_t
    peer_address; /**< Bonded peer device address                                                                            */
    mico_bt_smart_address_type_t
    address_type; /**< Peer device's address type                                                                            */
    uint8_t
    irk [16];     /**< Peer device's Identity Resolving Key (IRK). Used for random address generation and resolution         */
    uint8_t
    csrk[16];     /**< Peer device's Connection Signature Resolving Key (CSRK). Used for signing data and verifying messages */
    uint8_t
    ltk [16];     /**< Peer device's Long Term Key (LTK). Used for encryption                                                */
    uint8_t
    rand[8];      /**< Peer device's Random Number (Rand). Used for identifying LTK                                          */
    uint16_t
    ediv;         /**< Peer device's Encrypted Diversifier (EDIV). Used for identifying LTK                                  */
} mico_bt_smart_bond_info_t;

/**
 * Bluetooth Smart Device Bond Request Event Parameters.
 */
typedef struct {
    /* Type of the request */
    mico_bt_smart_bond_request_type_t type;

    union {
        /* MICO_BT_SMART_BOND_PASS_KEY_REQ */
        struct {
            mico_bt_device_address_t addr;
        } passkey;

        /* MICO_BT_SMART_BOND_USR_CONFIRM_REQ */
        struct {
            mico_bt_device_address_t addr;
            /* An up to six-digit number (from 0 to 999999) to display or
             * request remote entry
             */
            uint32_t passkey;
        } confirm;

        /* MICO_BT_SMART_BOND_OOB_DATA_REQ */
        struct {
            mico_bt_device_address_t addr;
        } oob_data;

        /* MICO_BT_SMART_BOND_PASS_KEY_NOTIFY */
        struct {
            mico_bt_device_address_t addr;
            uint32_t                 passkey;
        } notify;
    } u;
} mico_bt_smart_bond_request_t;

/*
 * Bluetooth Smart Device Bond Replay information.
 */
typedef struct {
    /* Type of the request */
    mico_bt_smart_bond_request_type_t type;

    /* reply status */
    mico_bt_result_t res;

    /* reply data -- only valid when 'res' is MICO_BT_SUCCESS */
    union {
        /* Used for MICO_BT_SMART_BOND_USER_CONFIRM */
        struct {
            mico_bt_device_address_t    addr;
        } confirm;

        /* Used for MICO_BT_SMART_BOND_PASS_KEY_REQ */
        struct {
            mico_bt_device_address_t    addr;
            uint32_t                    passkey;
        } passkey;

        /* Used for MICO_BT_SMART_BOND_OOB_DATA_REQ */
        struct {
            mico_bt_device_address_t    addr;
            uint8_t                    *data;
            uint8_t                     len;
        } oob_data;
    } u;
} mico_bt_smart_bond_reply_t;

#pragma pack()

/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * Bluetooth Smart scan complete callback
 */
typedef event_handler_t mico_bt_smart_scan_complete_callback_t;

/**
 * Bluetooth Smart advertising report callback
 */
typedef OSStatus (*mico_bt_smart_advertising_report_callback_t)  ( const mico_bt_smart_advertising_report_t *result );

/**
 * Bluetooth Smart Device Bond Callback.
 */
typedef OSStatus (*mico_bt_smart_bonding_callback_t)(const mico_bt_smart_bond_request_t *request);

/**
 * Bluetooth Smart advertising complete callback
 */
typedef event_handler_t mico_bt_smart_advertising_complete_callback_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/** mico_bt_smart_bond_reply
 *
 *  This function is called to reply BLE Bond Event posted by #mico_bt_smart_bonding_callback_t.
 *  You should fill a strcutre #mico_bt_smart_bond_reply_t to complete this bonding procedure.
 *
 *  @param[in] response     the response to current bonding event.
 *
 *  @return OSStatus
 */
extern OSStatus mico_bt_smart_bond_reply(const mico_bt_smart_bond_reply_t *response);

#ifdef __cplusplus
} /* extern "C" */
#endif
