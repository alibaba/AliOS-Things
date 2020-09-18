/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_MAIN_H
#define __BT_MESH_MAIN_H
#include "mesh_hal_os.h"
#include "adv.h"
/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */
 /*zaiyuan add, */
#define CONFIG_MAX_GENIE_USERS 10

typedef enum {
    BT_MESH_NO_OUTPUT       = 0,
    BT_MESH_BLINK           = BIT(0),
    BT_MESH_BEEP            = BIT(1),
    BT_MESH_VIBRATE         = BIT(2),
    BT_MESH_DISPLAY_NUMBER  = BIT(3),
    BT_MESH_DISPLAY_STRING  = BIT(4),
} bt_mesh_output_action_t;

typedef enum {
    BT_MESH_NO_INPUT      = 0,
    BT_MESH_PUSH          = BIT(0),
    BT_MESH_TWIST         = BIT(1),
    BT_MESH_ENTER_NUMBER  = BIT(2),
    BT_MESH_ENTER_STRING  = BIT(3),
} bt_mesh_input_action_t;

typedef enum {
    BT_MESH_PROV_ADV   = BIT(0),
    BT_MESH_PROV_GATT  = BIT(1),
} bt_mesh_prov_bearer_t;

typedef enum {
    BT_MESH_PROV_OOB_OTHER     = BIT(0),
    BT_MESH_PROV_OOB_URI       = BIT(1),
    BT_MESH_PROV_OOB_2D_CODE   = BIT(2),
    BT_MESH_PROV_OOB_BAR_CODE  = BIT(3),
    BT_MESH_PROV_OOB_NFC       = BIT(4),
    BT_MESH_PROV_OOB_NUMBER    = BIT(5),
    BT_MESH_PROV_OOB_STRING    = BIT(6),
    /* 7 - 10 are reserved */
    BT_MESH_PROV_OOB_ON_BOX    = BIT(11),
    BT_MESH_PROV_OOB_IN_BOX    = BIT(12),
    BT_MESH_PROV_OOB_ON_PAPER  = BIT(13),
    BT_MESH_PROV_OOB_IN_MANUAL = BIT(14),
    BT_MESH_PROV_OOB_ON_DEV    = BIT(15),
} bt_mesh_prov_oob_info_t;

#define MSG_GROUP(x)        (((x)>>16)&0xFFFF)
#define MSG_ID(group, id)   (((group)<<16)|((id)&0xFFFF))
#define MSG_SUBID(id)       ((id)&0xFFFF)

typedef enum {
    MSG_GROUP_ADV,
    MSG_GROUP_PROV,
    MSG_GROUP_SIGMESH,
}bt_mesh_msg_group;

typedef enum {
    MSG_ID_SCAN_RESULT,
}bt_mesh_adv_id;

typedef enum {
    MSG_ID_PROV_SEND_INVITE,
    MSG_ID_PROV_SEND_START,
    MSG_ID_PROV_SEND_CONFIRM,
    MSG_ID_PROV_AUTH_RESULT,
}bt_mesh_prov_id;

typedef enum {
    MSG_ID_SIGMESH_AGGRESSIVE,
    MSG_ID_SIGMESH_A2DP_STATE,
}bt_mesh_sigmesh_id;

struct sigmesh_aggressive_t {
    bool enable;
};

struct sigmesh_a2dp_state_t {
    bool state;
};

typedef enum{
	ALI_MESH_LINK_OPEN = 1,
	ALI_MESH_LINK_CLOSE,
	ALI_MESH_COMPLETE,
	ALI_MESH_AUTH_ACTIVE
}e_bt_mesh_evt;

typedef struct{
	e_bt_mesh_evt evt;
	uint8_t *data;
	uint16_t data_len;
}s_bt_mesh_evt;

struct bt_mesh_user_t {
    u32_t genie_addr;    
    u8_t  net_key[16];                  /* NET key pointer, stored in provisioner_prov.c */
    u16_t net_idx;                      /* Node provision net_idx */
	u32_t iv_index;                     /* Current IV Index */
	u32_t seq:24,                       /* Next outgoing sequence number */
	      iv_update:1,                  /* 1 if IV Update in Progress */
	      ivu_initiator:1,              /* IV Update initiated by us */
	      ivu_test:1,                   /* IV Update test mode */
	      pending_update:1,             /* Update blocked by SDU in progress */
	      valid:1;                      /* 0 if unused */
    u8_t  flags;                        /* Node key refresh flag and iv update flag */
    bool  active;
};

struct bt_mesh_data_info_t {
    u8_t older_uid;    
    struct bt_mesh_user_t mesh_user[CONFIG_MAX_GENIE_USERS];
};


#define BT_MESH_MAX_BDADDR_LEN  18
#define BT_MESH_MAX_STR_LEN     128

#define BT_MESH_VERSION_LEN (30)
#define BT_MESH_UUID_SIZE   (16)
#define BT_MESH_KEY_SIZE    (16)
#define BT_MESH_PUBLIC_KEY_SIZE    (16)
#define BT_MESH_BLE_ADDR_LEN     (6)
#define BT_MESH_AUTHENTICATION_SIZE  (16)
#define BT_MESH_DEVKEY_SIZE BT_MESH_KEY_SIZE
#define BT_MESH_COMPOSITION_DATA_FIXED_FIELD_LEN  (10)


/* Provisioning error code used for #BT_MESH_PROV_ERROR_T */
typedef enum {
    BT_MESH_PROV_SUCCESS = 0,         /**< Provisioning success */
    BT_MESH_PROV_FAILED_ERROR_CODE_INVALID_PDU = 1,        /**< The provisioning protocol PDU is not recognized by the device */
    BT_MESH_PROV_FAILED_ERROR_CODE_INVALID_FORMAT = 2,     /**< The arguments of the protocol PDUs are outside expected values or the length of the PDU is different than expected */
    BT_MESH_PROV_FAILED_ERROR_CODE_UNEXPECTED_PDU = 3,     /**< The PDU received was not expected at this moment of the procedure */
    BT_MESH_PROV_FAILED_ERROR_CODE_CONFIRMATION_FAILED = 4, /**< The computed confirmation value was not successfully verified */
    BT_MESH_PROV_FAILED_ERROR_CODE_OUT_OF_RESOURCES = 5,   /**< The provisioning protocol cannot be continued due to insufficient resources in the device */
    BT_MESH_PROV_FAILED_ERROR_CODE_DECRYPTION_FAILED = 6,  /**< The Data block was not successfully decrypted */
    BT_MESH_PROV_FAILED_ERROR_CODE_UNEXPECTED_ERROR = 7,   /**< An unexpected error occurred that may not be recoverable */
    BT_MESH_PROV_FAILED_ERROR_CODE_CANNOT_ASSIGN_ADDR = 8, /**< The device cannot assign consecutive unicast addresses to all elements */
    BT_MESH_PROV_FAILED_ERROR_CODE_TRANSACTION_TIMEOUT = 200, /**MTK private enum field, reserve 200 elements for original SDK */
    BT_MESH_PROV_FAILED_ERROR_CODE_PROVISION_TIMEOUT = 201,
    BT_MESH_PROV_FAILED_ERROR_CODE_AUTHENTICATION_FAILED = 202,
    BT_MESH_PROV_FAILED_ERROR_CODE_SDK_INTERNAL = 300,     /**< Alibaba: private error code for ERROR_CODE_INTERNAL */
    BT_MESH_PROV_FAILED_ERROR_CODE_INVALID_GW_INFO = 301,  /**< Alibaba: private error code for ERROR_CODE_INTERNAL */
    BT_MESH_PROV_FAILED_ERROR_CODE_GW_CONN_TOUT = 302,     /**< Alibaba: private error code for ERROR_CODE_GW_CONN_TOUT */
    BT_MESH_PROV_FAILED_ERROR_CODE_GW_CONFIRM = 303,       /**< Alibaba: private error code for ERROR_CODE_GW_CONFIRM */
    BT_MESH_PROV_FAILED_ERROR_CODE_GW_AUTH = 304,          /**< Alibaba: private error code for ERROR_CODE_GW_AUTH */
    BT_MESH_PROV_FAILED_ERROR_CC_TOUT = 305,               /**< Alibaba: private error code for ERROR_CODE_CC_TOUT */
} BT_MESH_PROV_ERROR_T;

typedef struct {
    uint8_t number_of_elements;   /**< Number of elements supported by the device */
    uint16_t algorithms;          /**< Supported algorithms and other capabilities */
    uint8_t public_key_type;      /**< Supported public key types */
    uint8_t static_oob_type;      /**< Supported static OOB Types */
    uint8_t output_oob_size;      /**< Maximum size of Output OOB supported */
    uint16_t output_oob_action;   /**< Supported Output OOB Actions */
    uint8_t input_oob_size;       /**< Maximum size in octets of Input OOB supported */
    uint16_t input_oob_action;    /**< Supported Input OOB Actions */
} BT_MESH_PROV_CAPABILITIES_T;

typedef struct {
    BT_MESH_PROV_CAPABILITIES_T cap;     /**< The capabilities detail value. */
} BT_MESH_EVT_PROV_CAPABILITIES_T;

typedef struct {
    uint8_t method;   /**< Authentication Method used */
    uint8_t action;   /**< Selected Output OOB Action or Input OOB Action or 0x00 */
    uint8_t size;     /**< Size of the Output OOB used or size of the Input OOB used or 0x00 */
} BT_MESH_EVT_PROV_REQUEST_AUTH_T;

typedef struct {
    uint8_t *pk;    /**< The public key received. */
} BT_MESH_EVT_PROV_SHOW_PK_T;

typedef struct {
    uint8_t auth[BT_MESH_AUTHENTICATION_SIZE];  /**< The authentication value received. */
} BT_MESH_EVT_PROV_SHOW_AUTH_T;


typedef struct {
    BT_MESH_PROV_ERROR_T reason;       /**< Indicate the provisioning process success or failed reason. */
    uint16_t address;      /**< Indicate the target unicast address. */
    uint8_t device_key[BT_MESH_DEVKEY_SIZE]; /**< Indicate the device key. */
    bool success;       /**< Indicate the provisioning process is successfull or not. */
    bool gatt_bearer;
} BT_MESH_EVT_PROV_DONE_T;

typedef struct {
    uint8_t uuid[BT_MESH_UUID_SIZE];   /**< The unprovisioned device UUID. */
    uint16_t oob_info;                  /**< The OOB information of unprovisioned device. */
    uint8_t *uri_hash;                  /**< The Uri hash value of unprovisioned device, may be NULL. */
} BT_MESH_PROV_UNPROVISIONED_DEVICE_T;

typedef struct {
    uint32_t total_count;   //total number of the scanned ud
    uint32_t current_index; //the index of current notified ud, start from 1. (<= total_count)
    BT_MESH_PROV_UNPROVISIONED_DEVICE_T ud;  //current ud info
} BT_MESH_EVT_PROV_SCAN_UD_T;

typedef enum {
    BT_MESH_PROV_FACTOR_CONFIRMATION_KEY,
    BT_MESH_PROV_FACTOR_RANDOM_PROVISIONER,
    BT_MESH_PROV_FACTOR_RANDOM_DEVICE,
    BT_MESH_PROV_FACTOR_CONFIRMATION_PROVISIONER,
    BT_MESH_PROV_FACTOR_CONFIRMATION_DEVICE,
    BT_MESH_PROV_FACTOR_PUB_KEY,
    BT_MESH_PROV_FACTOR_AUTHEN_VALUE,
    BT_MESH_PROV_FACTOR_AUTHEN_RESULT,
} BT_MESH_PROV_FACTOR_TYPE_T;

//This is a common structure for provisioning factor data, which may be set from APP or notified to APP. It is implementation dependent.
typedef struct {
    BT_MESH_PROV_FACTOR_TYPE_T type;
    uint8_t *buf;
    uint16_t buf_len;
} BT_MESH_PROV_FACTOR_T;

typedef enum
{
    BT_MESH_REPORT_TYPE_IND = 0x00,                 ///< Type for ADV_IND found (passive)
    BT_MESH_REPORT_TYPE_DIRECT_IND = 0x01,          ///< Type for ADV_DIRECT_IND found (passive)
    BT_MESH_REPORT_TYPE_SCAN_IND    = 0x02,         ///< Type for ADV_SCAN_IND found (passive)
    BT_MESH_REPORT_TYPE_NONCONN_IND  = 0x03,        ///< Type for ADV_NONCONN_IND found (passive)
    BT_MESH_REPORT_TYPE_SCAN_RSP = 0x04             ///< Type for SCAN_RSP found (active)
} BT_MESH_REPORT_TYPE;

typedef enum {
    BT_MESH_BLE_ADDR_TYPE_PUBLIC = 0,                /**< public address */
    BT_MESH_BLE_ADDR_TYPE_RANDOM_STATIC = 1,         /**< random static address */
    BT_MESH_BLE_ADDR_TYPE_RANDOM_RESOLVABLE = 2,     /**< random resolvable addresss */
    BT_MESH_BLE_ADDR_TYPE_RANDOM_NON_RESOLVABLE = 3, /**< random non resolvable address */
} BT_MESH_BLE_ADDR_TYPE_T;

typedef struct {
    BT_MESH_BLE_ADDR_TYPE_T addr_type;               /**< address type */
    int8_t addr[BT_MESH_MAX_BDADDR_LEN];                      /**< address byte array */
} BT_MESH_BLE_ADDR_T;

typedef struct
{
    BT_MESH_BLE_ADDR_T      peer_addr;
    int32_t                 rssi;
    BT_MESH_REPORT_TYPE     type;
    uint8_t                 dlen;
    uint8_t                 data[BT_MESH_ADV_DATA_SIZE];
} BT_MESH_EVT_ADV_REPORT_T;

typedef enum {
    BT_MESH_KEY_REFRESH_STATE_NONE = 0,    /**< Key refresh phase 0. Indicates normal device operation. */
    BT_MESH_KEY_REFRESH_STATE_1 = 1,       /**< Key refresh phase 1. Old keys are used for packet transmission, but new keys can be used to receive messages. */
    BT_MESH_KEY_REFRESH_STATE_2 = 2,       /**< Key refresh phase 2. New keys are used for packet transmission, but old keys can be used to receive messages. */
    BT_MESH_KEY_REFRESH_STATE_3 = 3,       /**< Key refresh phase 3. Used to complete a key refresh procedure and transition back to phase 0. */
} BT_MESH_KEY_REFRESH_STATE_T;

typedef struct {
    uint16_t netkey_index;  /**< The network key index. */
    BT_MESH_KEY_REFRESH_STATE_T phase;          /**< Current key refresh phase. */
} BT_MESH_EVT_KEY_REFRESH_T;

typedef enum {
    BT_MESH_IV_UPDATE_STATE_NORMAL = 0,        /**< Indicates IV update is in normal operation. */
    BT_MESH_IV_UPDATE_STATE_IN_PROGRESS = 1,   /**< Indicates IV update is in progress. */
} BT_MESH_IV_UPDATE_STATE_T;

typedef struct {
    uint32_t iv_index;  /**< The IV index currently used for sending messages. */
    BT_MESH_IV_UPDATE_STATE_T state;      /**< Current IV update state.*/
    uint8_t iv_phase;
}BT_MESH_EVT_IV_UPDATE_T;

typedef struct {
    uint32_t seq_num;
}BT_MESH_EVT_SEQ_CHANGE_T;

typedef struct {
    uint16_t address;
    uint8_t active;
}BT_MESH_EVT_HEARTBEAT_T;

typedef enum
{
    BT_MESH_OTA_ERROR_CODE_SUCCESS,                  /**< Error code of indicating success.*/
    BT_MESH_OTA_ERROR_CODE_WRONG_FIRMWARE_ID,        /**< Error code of inidcating wrong firmware id.*/
    BT_MESH_OTA_ERROR_CODE_BUSY,                     /**< Error code of inidcating busy of distributor*/
    BT_MESH_OTA_ERROR_CODE_NO_RESPONSE,              /**< Error code of inidcating no response of distributor*/
    BT_MESH_OTA_ERROR_CODE_USER_STOP,                /**< Error code of inidcating user interuption*/
} BT_MESH_OTA_ERROR_CODE_T;

typedef enum
{
    BT_MESH_OTA_EVENT_DISTRIBUTION_STARTING, /**< Event id for informing status of a new distribution was starting.*/
    BT_MESH_OTA_EVENT_DISTRIBUTION_STARTED,  /**< Event id for informing status of a new distribution was started.*/
    BT_MESH_OTA_EVENT_DISTRIBUTION_ONGOING,  /**< Event id for informing status of the distribution was ongoing.*/
    BT_MESH_OTA_EVENT_DISTRIBUTION_STOP,     /**< Event id for informing status of the distirbution was stopped.*/
    BT_MESH_OTA_EVENT_DISTRIBUTION_QUEUED,   /**< Event id for informing status of a new distribution was queued.*/
    BT_MESH_OTA_EVENT_DISTRIBUTION_DFU_READY,  /**< Event id for informing status of the distribution was dfu ready.*/
} BT_MESH_OTA_EVENT_T;

typedef enum
{
    BT_MESH_OTA_NODE_UPDATE_STATUS_SUCCESS = 0,
    BT_MESH_OTA_NODE_UPDATE_STATUS_IN_PROGRESS,
    BT_MESH_OTA_NODE_UPDATE_STATUS_CANCELED,    //failed with some problem
    BT_MESH_OTA_NODE_UPDATE_STATUS_DFU_READY,
} BT_MESH_OTA_NODE_UPDATE_STATUS_T;

typedef struct
{
    uint16_t addr;
    BT_MESH_OTA_NODE_UPDATE_STATUS_T status;
} BT_MESH_OTA_NODE_UPDATE_STATUS_INFO_T;

typedef struct {
    BT_MESH_OTA_EVENT_T event_id; /** Event id*/
    BT_MESH_OTA_ERROR_CODE_T error_code; /**< Status code*/
    uint32_t serial_number; /**< Serial number*/
    uint32_t firmware_id;   /**< Firmware id*/
    uint32_t time_escaped; /**< time escaped from started*/
    uint16_t nodes_num;
    BT_MESH_OTA_NODE_UPDATE_STATUS_INFO_T *nodes_status; //this is an array with element number of nodes_num
} BT_MESH_EVT_OTA_T;

typedef enum {
    BT_MESH_FRIENDSHIP_TERMINATED = 0,         /**< The friendship is terminated. */
    BT_MESH_FRIENDSHIP_ESTABLISHED = 1,        /**< The friendship is successfully established. */
    BT_MESH_FRIENDSHIP_ESTABLISH_FAILED = 2,       /**< The friendship is not established. */
    BT_MESH_FRIENDSHIP_REQUEST_FRIEND_TIMEOUT = 3, /**< Request friend procedure timeout. The status is only received when low power feature in use. */
    BT_MESH_FRIENDSHIP_SELECT_FRIEND_TIMEOUT = 4,  /**< Select friend procedure timeout. The status is only received when low power feature in use. */
} BT_MESH_FRIDSHIP_STATUS_T;

typedef struct {
    uint16_t address;                                   /**< Indicates the friend or the low-power node unicast address. */
    BT_MESH_FRIDSHIP_STATUS_T status;                /**< Indicates the friendship status between the nodes. */
} BT_MESH_EVT_FRIDSHIP_STATUS_T;

typedef struct {
    union {
        BT_MESH_EVT_PROV_CAPABILITIES_T      prov_cap;           /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_CAPABILITIES */
        BT_MESH_EVT_PROV_REQUEST_AUTH_T      prov_request_auth;  /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE */
        BT_MESH_EVT_PROV_SHOW_PK_T           prov_show_pk;       /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY */
        BT_MESH_EVT_PROV_SHOW_AUTH_T         prov_show_auth;     /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE */
        BT_MESH_EVT_PROV_DONE_T              prov_done;          /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_DONE */
        BT_MESH_EVT_PROV_SCAN_UD_T           prov_scan_ud;       /**<  parameter of mesh event @ref BT_MESH_EVT_PROV_SCAN_UD_RESULT */
        BT_MESH_PROV_FACTOR_T                prov_factor;
        BT_MESH_EVT_ADV_REPORT_T             adv_report;
        BT_MESH_EVT_KEY_REFRESH_T            key_refresh;
        BT_MESH_EVT_IV_UPDATE_T              iv_update;          /**<  parameter of mesh event @ref BLE_MESH_EVT_IV_UPDATE */
        BT_MESH_EVT_SEQ_CHANGE_T             seq_change;
        BT_MESH_EVT_HEARTBEAT_T              heartbeat;
        BT_MESH_EVT_OTA_T                    ota_evt;
        BT_MESH_EVT_FRIDSHIP_STATUS_T        friendship_status;
    } mesh;
} BT_MESH_EVT_T;


typedef enum {
    BT_MESH_EVT_INIT_DONE = 0,
    BT_MESH_EVT_PROV_CAPABILITIES,
    BT_MESH_EVT_PROV_REQUEST_OOB_PUBLIC_KEY,
    BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE,
    BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY,
    BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE,
    BT_MESH_EVT_PROV_DONE,
    BT_MESH_EVT_PROV_SCAN_UD_RESULT,
    BT_MESH_EVT_CONFIG_RESET,
    BT_MESH_EVT_FRIENDSHIP_STATUS,                     /**< Event for mesh friendship status change. */
    BT_MESH_EVT_LPN_FRIEND_OFFER,                      /**< Event for mesh LPN receiving friend offer. */
    BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM,  /**< Event for mesh LPN receiving friend subscription list confirm. */
    BT_MESH_EVT_HEARTBEAT,                             /**< Event for mesh heartbeat. */
    BT_MESH_EVT_IV_UPDATE,                             /**< Event for mesh IV index update. */
    BT_MESH_EVT_KEY_REFRESH,                           /**< Event for mesh key refresh. */

    //The above enum value match with SDK
    BT_MESH_EVT_UD_RESULT_COMPLETE,
    BT_MESH_EVT_PROV_FACTOR,
    BT_MESH_EVT_SEQ_CHANGE,
    BT_MESH_EVT_OTA_EVENT,
    BT_MESH_EVT_ADV_REPORT,
#ifdef __ANDROID__
    ANDROID_BT_ADAPTER_RESTARTED = 0xfffe,              /**< Event for android bluetooth adapter restarted. */
#endif
} BT_MESH_EVENT_ID;

typedef struct {
    uint32_t evt_id; ///< Event ID
    union
    {
      BT_MESH_EVT_T mesh_evt; ///< MESH event structure
    }evt;
} BT_MESH_EVENT_T;

/** Provisioning properties & capabilities. */
struct bt_mesh_prov {
    /** The UUID that's used when advertising as unprovisioned */
    const u8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    bt_mesh_prov_oob_info_t oob_info;

    /** public key type */
    u8_t        public_key_type;
    
    /** Static OOB value */
    const u8_t *static_val;
    /** Static OOB value length */
    u8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    u8_t        output_size;
    /** Supported Output OOB Actions */
    u16_t       output_actions;

    /* Maximum size of Input OOB supported */
    u8_t        input_size;
    /** Supported Input OOB Actions */
    u16_t       input_actions;

    /** @brief Output of a number is requested.
     *
     *  This callback notifies the application that it should
     *  output the given number using the given action.
     *
     *  @param act Action for outputting the number.
     *  @param num Number to be outputted.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_number)(bt_mesh_output_action_t act, u32_t num);

    /** @brief Output of a string is requested.
     *
     *  This callback notifies the application that it should
     *  display the given string to the user.
     *
     *  @param str String to be displayed.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_string)(const char *str);

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  request input from the user using the given action. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *
     *  @param act Action for inputting data.
     *  @param num Maximum size of the inputted data.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*input)(bt_mesh_input_action_t act, u8_t size);

    /** @brief Provisioning link has been opened.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_close)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx NetKeyIndex given during provisioning.
     *  @param addr Primary element address.
     */
    void        (*complete)(u16_t net_idx, u16_t addr);

    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be reprovisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void        (*reset)(void);
};

struct bt_mesh_provisioner {
    /* Provisioner device uuid */
    const u8_t *prov_uuid;

    /*
     * Primary element address of the provisioner, for
     * temporary provisioner no need to initialize it.
     */
    const u16_t prov_unicast_addr;

    /*
     * Starting unicast address going to assigned, for
     * temporary provisioner no need to initialize it.
     */
    u16_t prov_start_address;

    /* Attention timer contained in Provisioning Invite */
    u8_t  prov_attention;

    /* Provisioner provisioning Algorithm */
    u8_t  prov_algorithm;

    /* Provisioner public key oob */
    u8_t  prov_pub_key_oob;

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  read device's public key with OOB
     *
     *  @param remote_pub_key Public key pointer of the device.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_pub_key_oob_cb)(u8_t remote_pub_key[64]);

    /* Provisioner static oob value */
    u8_t *prov_static_oob_val;

    /* Provisioner static oob value length */
    u8_t  prov_static_oob_len;

    /** @brief Provisioner input a number read from device output
     *
     *  This callback notifies the application that it should
     *  input the number given by the device.
     *
     *  @param act:  The output action of the device
     *  @param size: The output size of the device
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_input_num)(bt_mesh_output_action_t act, u8_t size);

    /** @brief Provisioner output a number to the device
     *
     *  This callback notifies the application that it should
     *  output the number to the device.
     *
     *  @param act:  The input action of the device
     *  @param size: The input size of the device
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_output_num)(bt_mesh_input_action_t act, u8_t size);

    /*
     * Key refresh and IV update flag, for temporary provisioner no
     * need to initialize it.
     */
    u8_t  flags;

    /*
     * IV index, for temporary provisioner no need to initialize it.
     */
    u32_t iv_index;

    /** @brief Provisioner has opened a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void (*prov_link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioner has closed a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     *  @param reason Provisioning link close reason(disconnect reason)
     *                0xFF: disconnect due to provisioner_pb_gatt_disable()
     */
    void (*prov_link_close)(bt_mesh_prov_bearer_t bearer, u8_t reason);

#ifdef CONFIG_MESH_STACK_FOR_GENIE
    void (*prov_complete)(int node_idx, const u8_t device_uuid[16],
                          u16_t unicast_addr, u8_t element_num,
                          u16_t netkey_idx, bool gatt_flag, const u8_t device_key[16]);
#else
    /** @brief Provision one device is complete.
     *
     *  This callback notifies the application that provisioner has
     *  successfully provisioned a device, and the node has been assigned
     *  with the specified NetKeyIndex and primary element address.
     *
     *  @param node_idx     Node index within the node(provisioned device) queue.
     *  @param device_uuid  Provisioned device device uuid pointer.
     *  @param unicast_addr Provisioned device assigned unicast address.
     *  @param element_num  Provisioned device element number.
     *  @param netkey_idx   Provisioned device assigned netkey index.
     */
    void (*prov_complete)(int node_idx, const u8_t device_uuid[16],
                          u16_t unicast_addr, u8_t element_num,
                          u16_t netkey_idx, bool gatt_flag);
#endif
};

/** @brief Initialize Mesh support
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_number(u32_t num);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers);

/**
 * @}
 */

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh Bluetooth Mesh
 * @ingroup bluetooth
 * @{
 */

/* Primary Network Key index */
#define BT_MESH_NET_PRIMARY                 0x000

#define BT_MESH_RELAY_DISABLED              0x00
#define BT_MESH_RELAY_ENABLED               0x01
#define BT_MESH_RELAY_NOT_SUPPORTED         0x02

#define BT_MESH_BEACON_DISABLED             0x00
#define BT_MESH_BEACON_ENABLED              0x01

#define BT_MESH_GATT_PROXY_DISABLED         0x00
#define BT_MESH_GATT_PROXY_ENABLED          0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BT_MESH_FRIEND_DISABLED             0x00
#define BT_MESH_FRIEND_ENABLED              0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BT_MESH_NODE_IDENTITY_STOPPED       0x00
#define BT_MESH_NODE_IDENTITY_RUNNING       0x01
#define BT_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BT_MESH_FEAT_RELAY                  BIT(0)
#define BT_MESH_FEAT_PROXY                  BIT(1)
#define BT_MESH_FEAT_FRIEND                 BIT(2)
#define BT_MESH_FEAT_LOW_POWER              BIT(3)

void bt_mesh_setup(u32_t seq, u16_t addr);

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(const struct bt_mesh_prov *prov,
                 const struct bt_mesh_comp *comp,
                 const struct bt_mesh_provisioner *provisioner);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  reprovisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
void bt_mesh_reset(void);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param seq      Sequence Number (0 if newly provisioned).
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
                      u8_t flags, u32_t iv_index, u32_t seq,
                      u16_t addr, const u8_t dev_key[16]);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_set(bool enable);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established));

/**
 * @}
 */
bool bt_mesh_is_provisioner_en(void);
bool bt_mesh_is_provisioned(void);

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers);
int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers);

struct bt_mesh_model *bt_mesh_elem_first_model(void);

int bt_mesh_prov_cap_cb(
    u8_t number_of_elements,
    u16_t algorithms,
    u8_t public_key_type,
    u8_t static_oob_type,
    u8_t output_oob_size,
    u16_t output_oob_action,
    u8_t input_oob_size,
    u16_t input_oob_action);

int bt_mesh_prov_init_done_cb(void);

int bt_mesh_prov_req_oob_pub_key_cb (void);

int bt_mesh_prov_req_auth_cb (u8_t method, u8_t action, u8_t size);

int bt_mesh_prov_show_pub_key_cb (u8_t *pk);

int bt_mesh_prov_show_auth_value_cb (u8_t *auth);

int bt_mesh_prov_done_cb (u16_t address, u8_t *device_key, bool success, bool gatt_bearer, s32_t reason);

int bt_mesh_prov_scan_ud_cb (
    u8_t *uuid,
    u16_t oob_info,
    u8_t *uri_hash);

int bt_mesh_prov_config_reset_cb (u8_t *pk);

int bt_mesh_prov_friend_status_cb (u16_t address, u16_t status);

int bt_mesh_prov_heartbeat_cb (u16_t address, u8_t active);

int bt_mesh_prov_iv_update_cb (u32_t iv_index, u8_t state, u8_t iv_phase);

int bt_mesh_prov_key_refresh_cb (u16_t netkey_index, u8_t state);

int bt_mesh_prov_ud_result_comp_cb (void);

int bt_mesh_prov_factor_cb (u8_t type, u8_t *buf, u16_t buf_len);

int bt_mesh_prov_seq_change_comp_cb (u32_t seq_num);

int RTOSGenie_mesh_provisioner_scan(u8_t start, s32_t duration);
int RTOSGenie_mesh_init(void);
int bt_mesh_enable_aggressiveSetting(bool enable);
struct bt_mesh_user_t *mesh_main_get_mesh_user(u16_t genie_uniaddr, u16_t net_id, bool is_need_load);
bool mesh_main_save_user_info(void);
int mesh_data_add_user_info(u16_t genie_uniaddr, u16_t net_id, u32_t iv_index, u32_t seq, u8_t *netkey, bool save_flag);

#endif /* __BT_MESH_MAIN_H */
