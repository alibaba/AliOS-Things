/** @file
 *
 * Runtime Bluetooth stack configuration parameters
 *
 */
#include "mico_bt_cfg.h"

/*****************************************************************************
 * BTE stack configuration
 ****************************************************************************/
mico_bt_cfg_settings_t mico_bt_cfg_settings = {
    .device_name               = (uint8_t *)"MiCO BT Gateway", /**< Local device name (NULL terminated) */
    .device_class              = { 0x00, 0x00, 0x00 },        /**< Local device class */
    .security_requirement_mask = BTM_SEC_NONE,                /**< Security requirements mask (BTM_SEC_NONE, or combination of BTM_SEC_IN_AUTHENTICATE, BTM_SEC_OUT_AUTHENTICATE, BTM_SEC_ENCRYPT (see #mico_bt_sec_level_e)) */
    .max_simultaneous_links    = 3,                           /**< Maximum number simultaneous links to different devices */

    /* Scan and advertisement configuration */
    .br_edr_scan_cfg =                 /**< BR/EDR scan settings */
    {
        .inquiry_scan_type     = BTM_SCAN_TYPE_STANDARD,                      /**< Inquiry scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
        .inquiry_scan_interval = MICO_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL,  /**< Inquiry scan interval (default: MICO_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL) */
        .inquiry_scan_window   = MICO_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW,    /**< Inquiry scan window (default: MICO_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW) */
        .page_scan_type        = BTM_SCAN_TYPE_STANDARD,                      /**< Page scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
        .page_scan_interval    = MICO_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL,     /**< Page scan interval (default: MICO_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL) */
        .page_scan_window      = MICO_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW,       /**< Page scan window (default: MICO_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW) */
    },
    .ble_scan_cfg =                    /**< BLE scan settings */
    {
        .scan_mode                    = BTM_BLE_SCAN_MODE_PASSIVE,                         /**< BLE scan mode (BTM_BLE_SCAN_MODE_PASSIVE, BTM_BLE_SCAN_MODE_ACTIVE) */
        .scan_filter_policy           = BTM_BLE_SCAN_FILTER_POLICY_NONE,
        .high_duty_scan_interval      = MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL,      /**< High duty scan interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL) */
        .high_duty_scan_window        = MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW,        /**< High duty scan window (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW) */
        .high_duty_scan_duration      = 3,                                                 /**< High duty scan duration in seconds (0 for infinite) */
        .low_duty_scan_interval       = MICO_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL,       /**< Low duty scan interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL) */
        .low_duty_scan_window         = MICO_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW,         /**< Low duty scan window (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW) */
        .low_duty_scan_duration       = 3,                                                 /**< Low duty scan duration in seconds (0 for infinite) */
        .high_duty_conn_scan_interval = MICO_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL, /**< High duty cycle connection scan interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL) */
        .high_duty_conn_scan_window   = MICO_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW,   /**< High duty cycle connection scan window (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW) */
        .high_duty_conn_duration      = 0,                                                 /**< High duty cycle connection duration in seconds (0 for infinite) */
        .low_duty_conn_scan_interval  = MICO_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL,  /**< Low duty cycle connection scan interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL) */
        .low_duty_conn_scan_window    = MICO_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW,    /**< Low duty cycle connection scan window (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW) */
        .low_duty_conn_duration       = 0,                                                 /**< Low duty cycle connection duration in seconds (0 for infinite) */
        .conn_min_interval            = MICO_BT_CFG_DEFAULT_CONN_MIN_INTERVAL,            /**< Minimum connection interval (default: MICO_BT_CFG_DEFAULT_CONN_MIN_INTERVAL) */
        .conn_max_interval            = MICO_BT_CFG_DEFAULT_CONN_MAX_INTERVAL,            /**< Maximum connection interval (default: MICO_BT_CFG_DEFAULT_CONN_MAX_INTERVAL) */
        .conn_latency                 = MICO_BT_CFG_DEFAULT_CONN_LATENCY,                 /**< Connection latency (default: MICO_BT_CFG_DEFAULT_CONN_LATENCY) */
        .conn_supervision_timeout     = MICO_BT_CFG_DEFAULT_CONN_SUPERVISION_TIMEOUT,     /**< Connection link supervision timeout (default: MICO_BT_CFG_DEFAULT_ CONN_SUPERVISION_TIMEOUT) */
    },
    .ble_advert_cfg =                  /**< BLE advertisement settings */
    {
        .channel_map                     = ( BTM_BLE_ADVERT_CHNL_37 | BTM_BLE_ADVERT_CHNL_38 | BTM_BLE_ADVERT_CHNL_39 ),    /**< Advertising channel map (mask of BTM_BLE_ADVERT_CHNL_37, BTM_BLE_ADVERT_CHNL_38, BTM_BLE_ADVERT_CHNL_39) */
        .high_duty_min_interval          = MICO_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,          /**< High duty undirected connectable advert minimum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL) */
        .high_duty_max_interval          = MICO_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL,          /**< High duty undirected connectable advert maximum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL) */
        .high_duty_duration              = 30,                                                       /**< High duty advertising duration in seconds (0 for infinite) */
        .low_duty_min_interval           = MICO_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,           /**< Low duty undirected connectable advert minimum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL) */
        .low_duty_max_interval           = MICO_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL,           /**< Low duty undirected connectable advert maximum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL) */
        .low_duty_duration               = 30,                                                       /**< Low duty advertising duration in seconds (0 for infinite) */
        .high_duty_directed_min_interval = MICO_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL, /**< high duty directed adv minimum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
        .high_duty_directed_max_interval = MICO_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL, /**< high duty directed adv maximum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL) */
        .low_duty_directed_min_interval  = MICO_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL,  /**< Low duty directed adv minimum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
        .low_duty_directed_max_interval  = MICO_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL,  /**< Low duty directed adv maximum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL) */
        .low_duty_directed_duration      = 30,                                                       /**< Low duty directed advertising duration in seconds (0 for infinite) */
        .high_duty_nonconn_min_interval  = MICO_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL,  /**< High duty non-connectable adv minimum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL) */
        .high_duty_nonconn_max_interval  = MICO_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL,  /**< High duty non-connectable adv maximum advertising interval (default: MICO_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL) */
        .high_duty_nonconn_duration      = 30,                                                       /**< High duty non-connectable advertising duration in seconds (0 for infinite) */
        .low_duty_nonconn_min_interval   = MICO_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL,   /**< Low duty non-connectable adv minimum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL) */
        .low_duty_nonconn_max_interval   = MICO_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL,   /**< Low duty non-connectable adv maximum advertising interval (default: MICO_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL) */
        .low_duty_nonconn_duration       = 30,                                                       /**< Low duty non-connectable advertising duration in seconds (0 for infinite) */
    },

    /* GATT configuration */
    .gatt_cfg =                       /**< GATT settings */
    {
        .appearance       = APPEARANCE_GENERIC_TAG, /**< GATT appearance (see #gatt_appearance_e) */
        .client_max_links = 3,   /**< Client config: maximum number of servers that local client can connect to  */
        .server_max_links = 0,   /**< Server config: maximum number of remote clients connections allowed by the local */
        .max_attr_len     = 360, /**< Maximum attribute length; gki_cfg must have a corresponding buffer pool that can hold this length */
    },
    /* Application managed l2cap protocol configuration */
    .rfcomm_cfg =
    {
        .max_links = 0,  /**< Maximum number of simultaneous connected remote devices*/
        .max_ports = 0   /**< Maximum number of simultaneous RFCOMM ports */
    },
    .l2cap_application =
    {
        .max_links       = 0, /**< Maximum number of application-managed l2cap links (BR/EDR and LE) */
        /* BR EDR l2cap configuration */
        .max_psm         = 0, /**< Maximum number of application-managed BR/EDR PSMs */
        .max_channels    = 0, /**< Maximum number of application-managed BR/EDR channels  */

        /* LE L2cap connection-oriented channels configuration */
        .max_le_psm      = 3, /**< Maximum number of application-managed LE PSMs */
        .max_le_channels = 3, /**< Maximum number of application-managed LE channels */
    },
    /* Audio/Video Distribution configuration */
    .avdt_cfg =
    {
        .max_links = 0, /**< Maximum simultaneous audio/video links */
    },

    /* Audio/Video Remote Control configuration */
    .avrc_cfg =
    {
        .roles     = 0, /**< Mask of local roles supported (AVRC_CONN_INITIATOR|AVRC_CONN_ACCEPTOR) */
        .max_links = 0  /**< Maximum simultaneous remote control links */
    },

    .addr_resolution_db_size = 20,
};


/*****************************************************************************
 * mico_bt_stack buffer pool configuration
 *
 * Configure buffer pools used by the stack
 *
 * Pools must be ordered in increasing buf_size.
 * If a pool runs out of buffers, the next  pool will be used
 *****************************************************************************/
const mico_bt_cfg_buf_pool_t mico_bt_cfg_buf_pools[MICO_BT_CFG_NUM_BUF_POOLS] = {
    /*  { buf_size, buf_count } */
    { 64,  4  }, /* Small Buffer Pool */
    { 360, 4  }, /* Medium Buffer Pool (used for HCI & RFCOMM control messages, min recommended size is 360) */
    { 360, 12 }, /* Large Buffer Pool  (used for HCI ACL messages) */
    { 600, 1  }, /* Extra Large Buffer Pool - Used for avdt media packets and miscellaneous (if not needed, set buf_count to 0) */
};
