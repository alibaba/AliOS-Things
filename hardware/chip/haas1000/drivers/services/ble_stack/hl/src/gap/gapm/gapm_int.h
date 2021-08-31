#ifndef _GAPM_INT_H_
#define _GAPM_INT_H_

/**
 ****************************************************************************************
 * @addtogroup GAPM_INT Generic Access Profile Manager Internal
 * @ingroup GAPM
 * @brief defines for internal GAPM usage
 *
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "gapm.h"
#include "gapm_task.h"
#if (BLE_DEPRECATED_API)
#include "gapc.h"
#endif //(BLE_DEPRECATED_API)
#include "co_bt.h"


/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of GAP Manager process
#define GAPM_IDX_MAX                                 BLE_CONNECTION_MAX

/// Scan filter size
#define GAPM_SCAN_FILTER_SIZE   10

/// check if current role is supported by configuration
#define GAPM_IS_ROLE_SUPPORTED(role_type)\
    ((gapm_env.role & (role_type)) == (role_type))

/// Number of supported activities in host
#define GAPM_ACTV_NB            (BLE_ACTIVITY_MAX)
/// Maximum number of advertising report that can be reassembled in parallel
#define GAPM_REPORT_NB_MAX      (5)
/// Invalid activity identifier
#define GAPM_ACTV_INVALID_IDX   (0xFF)

/*
 * INTERNAL API TYPES
 ****************************************************************************************
 */


/// Retrieve information about memory usage
struct gapm_dbg_get_mem_info_cmd
{
    /// GAPM requested operation:
    ///  - GAPM_DBG_GET_MEM_INFO: Get memory usage
    uint8_t operation;
};

/// Indication containing information about memory usage.
struct gapm_dbg_mem_info_ind
{
    /// peak of memory usage measured
    uint32_t max_mem_used;
    /// memory size currently used into each heaps.
    uint16_t mem_used[KE_MEM_BLOCK_MAX];
};


/// Operation type
enum gapm_op_type
{
    /// Configuration operation
    GAPM_OP_CFG         = 0x00,

    /// Air mode operation (scanning, advertising, connection establishment)
    /// Note: Restriction, only one air operation supported.
    GAPM_OP_AIR         = 0x01,
    #if (SECURE_CONNECTIONS)
    /// GAP State for DH Key Generation
    GAPM_OP_DHKEY       = 0x02,
    #endif // (SECURE_CONNECTIONS)
    /// Max number of operations
    GAPM_OP_MAX
};

/// GAPM states
enum gapm_state_id
{
    /// Idle state - no on going operation
    GAPM_IDLE,

    /// Busy state - Configuration operation on going
    GAPM_CFG_BUSY       = 1 << GAPM_OP_CFG,
    /// Busy state - Air operation on going
    GAPM_AIR_BUSY       = 1 << GAPM_OP_AIR,
    #if (SECURE_CONNECTIONS)
    /// Busy state - DH Key Calculation operation on going
    GAPM_DHKEY_BUSY     = 1 << GAPM_OP_DHKEY,
    #endif //  (SECURE_CONNECTIONS)

    /// Reset state - Reset operation on going
    GAPM_DEVICE_SETUP   = 1 << GAPM_OP_MAX,

    GAPM_STATE_MAX
};

/// Device configuration flags
///    7    6    5    4    3    2    1    0
/// +-----+----+----+----+----+----+----+----+
/// | DBG | SC | CP | GA | RE |CTLP|HSTP|ADDR|
/// +-----+----+----+----+----+----+----+----+
/// - Bit [0-2]: Address Type @see enum gapm_cfg_flag
///             Bit 0: 0 = Public address, 1 = Private address
///             Bit 1: 0 = Host-based Privacy disabled, 1 = enabled - DEPRECATED
///             Bit 2: 0 = Controller-based Privacy disabled, 1 = enabled
///
/// - Bit [3]  : Address to renew (only if privacy is enabled, 1 address to renew else 0)
/// - Bit [4]  : Generated Address type (1 = Resolvable, 0 = Non Resolvable)
/// - Bit [5]  : Preferred Connection parameters present in GAP DB
/// - Bit [6]  : Service Change feature present
/// - Bit [7]  : Enable Debug mode
/// Configuration flag bit description
enum gapm_cfg_flag_def
{
    /// Address Type
    GAPM_MASK_ADDR_TYPE           = 0x07,
    GAPM_POS_ADDR_TYPE            = 0x00,
    /// Address to renew
    GAPM_MASK_ADDR_RENEW          = 0x08,
    GAPM_POS_ADDR_RENEW           = 0x03,
    /// Generated Address type
    GAPM_MASK_RESOLV_ADDR         = 0x10,
    GAPM_POS_RESOLV_ADDR          = 0x04,
    /// Preferred Connection parameters present in GAP DB
    GAPM_MASK_PREF_CON_PAR_PRES   = 0x20,
    GAPM_POS_PREF_CON_PAR_PRES    = 0x05,
    /// Service Change feature present
    GAPM_MASK_SVC_CHG_EN          = 0x40,
    GAPM_POS_SVC_CHG_EN           = 0x06,

#if (RW_DEBUG)
    /// L2CAP Debug Mode used to force LE-frames
    GAPM_MASK_DBG_MODE_EN         = 0x80,
    GAPM_POS_DBG_MODE_EN          = 0x07,
#endif // (RW_DEBUG)
};


/// Security level
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// |MI |      RFU      |EKS|SEC_LVL|
/// +---+---+---+---+---+---+---+---+
/// bit[0-1]: Security level requirement (0=NO_AUTH, 1=UNAUTH, 2=AUTH, 3=SEC_CON)
/// bit[2]  : Encryption Key Size length must have 16 bytes
/// bit[7]  : Multi-instantiated task
enum  gapm_le_psm_sec_mask
{
    /// bit[0-1]: Security level requirement (0=NO_AUTH, 1=UNAUTH, 2=AUTH, 3=SEC_CON)
    /// bit[2]  : Encryption Key Size length must have 16 bytes
    GAPM_LE_PSM_SEC_LVL_MASK  = 0x07,
    /// bit[7]  : Multi-instantiated task
    GAPM_LE_PSM_MI_TASK_MASK  = 0x80,
};

/// Activity states
enum gapm_actv_state
{
    /// Activity is being created - next state is CREATED
    GAPM_ACTV_CREATING = 0,
    /// Activity has been successfully created
    GAPM_ACTV_CREATED,
    /// Activity is being started - next state is STARTED
    GAPM_ACTV_STARTING,
    /// Activity has been successfully started
    GAPM_ACTV_STARTED,
    /// Activity is being stopped - next state is CREATED
    GAPM_ACTV_STOPPING,
    /// Activity is being deleted - no next state
    GAPM_ACTV_DELETING,
};

/// Activity information bit field values
enum gapm_actv_info
{
    /// Scanning activity has been started
    GAPM_ACTV_INFO_SCAN_STARTED_BIT         = (1 << 0),
    /// Initiating activity has been started
    GAPM_ACTV_INFO_INIT_STARTED_BIT         = (2 << 0),
};

/// Connection role
enum gapm_conn_role
{
    /// Master
    GAPM_ROLE_MASTER = 0,
    /// Slave
    GAPM_ROLE_SLAVE,
};

/*
 * MACROS
 ****************************************************************************************
 */
/// Macro used to retrieve field
#define GAPM_F_GET(data, field)\
        (((data) & (GAPM_MASK_ ## field)) >> (GAPM_POS_ ## field))

/// Macro used to set field
#define GAPM_F_SET(data, field, val)\
    (data) = (((data) & ~(GAPM_MASK_ ## field)) \
           | ((val << (GAPM_POS_ ## field)) & (GAPM_MASK_ ## field)))

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Information about registered LE_PSM
struct gapm_le_psm_info
{
    /// List header structure to put information within a list
    struct co_list_hdr hdr;

    /// LE Protocol/Service Multiplexer
    uint16_t le_psm;

    /// Registered task identifier
    ke_task_id_t task_id;

    /// Security level
    ///   7   6   5   4   3   2   1   0
    /// +---+---+---+---+---+---+---+---+
    /// |MI |      RFU      |EKS|SEC_LVL|
    /// +---+---+---+---+---+---+---+---+
    /// bit[0-1]: Security level requirement (0=NO_AUTH, 1=UNAUTH, 2=AUTH, 3=SEC_CON)
    /// bit[2]  : Encryption Key Size length must have 16 bytes
    /// bit[7]  : Multi-instantiated task
    uint8_t sec_lvl;

    /// Number of established link
    uint8_t nb_est_lk;
};

/// GAP Manager activity structure (common part for advertising, scanning,
/// initiating and periodic synchronization activities)
struct gapm_actv_tag
{
    /// Identifier
    uint8_t idx;
    /// Type (@see enum gap_actv_type)
    uint8_t type;
    /// Subtype - meaning depends on activity type
    ///  - Advertising activity: @see enum gap_adv_subtype
    ///  - Scanning activity: @see enum gap_scan_subtype
    ///  - Initiating activity: @see enum gap_init_subtype
    ///  - Periodic Synchronization activity: @see enum gap_period_sync_subtype
    uint8_t subtype;
    /// State (@see enum gapm_actv_state)
    uint8_t state;
    /// Information bit field, meaning depends on activity type
    ///   - Bit 0-6: meaning depends on activity type
    ///   - Bit 7: Random Address must be renewed
    uint8_t info;
    /// Own address type
    uint8_t own_addr_type;
    /// Next expected HCI event opcode
    uint16_t next_exp_opcode;
    /// ID for . This task will receive GAPM_ACTIVITY_STOPPED_IND message that might not
    /// be trigger by application
    ke_task_id_t requester;
    /// BD Address used by the activity (can be different if controller privacy is used and
    /// application chose to use a resolvable private address)
    bd_addr_t addr;
};

struct gapm_report_elem
{
    /// List Header
    struct co_list_hdr list_hdr;
    /// Report fragment information
    struct ext_adv_report report_frag;
};

struct gapm_report_list
{
    // List of received reports (@see struct gapm_report_elem)
    struct co_list report_list;
    // Advertiser address
    struct gap_bdaddr adv_addr;
    // Received length
    uint16_t length;
};

/// GAP Manager activity structure for advertising activity
struct gapm_actv_adv_tag
{
    /// Common activity parameters
    struct gapm_actv_tag common;
    /// Data offset for the set advertising data procedure
    uint16_t data_offset;
    /// Advertising mode (@see enum gap_adv_disc_mode)
    uint8_t mode;
    /// Stored status
    uint8_t kept_status;
    /// Selected TX power
    uint8_t tx_pwr;
};

/// GAP Manager activity structure for scanning activity
struct gapm_actv_scan_tag
{
    /// Common activity parameters
    struct gapm_actv_tag common;
    /// Lists used to reassemble report fragment
    struct gapm_report_list report_lists[GAPM_REPORT_NB_MAX];
    /// Scan filtering Array
    struct gap_bdaddr *p_scan_filter;
};

/// GAP Manager activity structure for initiating activity
struct gapm_actv_init_tag
{
    /// Common activity parameters
    struct gapm_actv_tag common;
    /// Initiating parameters
    struct gap_init_param init_param;
    /// Number of connection to be established for automatic connection
    ///    -> Number of devices in the white list when GAPM_ACTIVITY_START_CMD is received
    uint8_t nb_auto_conn;
    /// Stored status
    uint8_t kept_status;
};

/// GAP Manager activity structure for periodic synchronization activity
struct gapm_actv_per_sync_tag
{
    /// Common activity parameters
    struct gapm_actv_tag common;
};

/// GAP Manager environment structure
struct gapm_env_tag
{
    /// Request operation Kernel message
    void* operation[GAPM_OP_MAX];

    /// Array of pointers to the allocated activities
    /// TODO [LT] IMPROVEMENT - Could be dynamically allocated based on read number of sets supported by controller
    struct gapm_actv_tag *actvs[GAPM_ACTV_NB];

    #if (BLE_DEPRECATED_API)
    #if (BLE_OBSERVER)
    /// Scan filtering Array
    struct gap_bdaddr* scan_filter;
    #endif // (BLE_OBSERVER)
    #endif //(BLE_DEPRECATED_API)

    #if (BLE_LECB)
    /// Registered list of LE Protocol/Service Multiplexer for LE Credit Based Connection
    struct co_list reg_le_psm;
    #endif // (BLE_LECB)

    #if (BLE_ATTS)
    /// GAP service start handle
    uint16_t svc_start_hdl;
    #endif // (BLE_ATTS)

    #if (BLE_BROADCASTER)
    uint16_t max_adv_data_len;
    #endif //(BLE_BROADCASTER)

    /// Duration before regenerate device address when privacy is enabled.
    uint16_t renew_dur;
    /// Device IRK used for resolvable random BD address generation (MSB -> LSB)
    struct gap_sec_key irk;

    /// Current device Address
    bd_addr_t addr;
	bd_addr_t connectedAddr[6];
    /// Device Role
    uint8_t role;
    /// Number of BLE connection
    uint8_t connections;
    /// Device configuration flags - (@see enum gapm_cfg_flag_def)
    uint8_t cfg_flags;
    /// Pairing mode authorized (see enum gapm_pairing_mode)
    uint8_t pairing_mode;
    /// Maximum device MTU size
    uint16_t max_mtu;
    /// Maximum device MPS size
    uint16_t max_mps;
    #if (SECURE_CONNECTIONS)
    /// Local device Public Key
    public_key_t public_key;
    private_key_t private_key;
    private_key_t host_DHKey;

    #endif // (SECURE_CONNECTIONS)

    #if(BLE_AUDIO)
    /// Audio configuration flag (see gapm_audio_cfg_flag)
    uint16_t audio_cfg;
    #endif // (BLE_AUDIO)

    #if(BLE_EMB_PRESENT && HCI_TL_SUPPORT)
    /// In Full mode, by default the AHI API is used, but if an HCI Reset is received,
    /// TL is switched to HCI and embedded host is disabled
    bool embedded_host;
    #endif // (BLE_EMB_PRESENT && HCI_TL_SUPPORT)

    #if (BLE_LECB)
    /// Maximum number of allowed LE Credit Based channels
    uint8_t max_nb_lecb;
    /// Current number of LE Credit Based channel connections established
    uint8_t nb_lecb;
    #endif // (BLE_LECB)

    #if (BLE_DEPRECATED_API)
    /// Indicate if deprecated API must be used
    bool use_deprecated_api;
    #endif //(BLE_DEPRECATED_API)

    #if (BLE_BROADCASTER)
    /// Number of advertising sets supported by controller
    uint8_t max_adv_set;
    /// Number of created advertising activities
    uint8_t nb_adv_actv;
    #endif //(BLE_BROADCASTER)
    #if (BLE_OBSERVER)
    /// Bit field making management of activity easier (@see enum gapm_actv_info)
    uint8_t actv_info;
    #endif //(BLE_OBSERVER)
    /// Activity identifier currently used for air operation
    uint8_t actv_idx;
    /// Number of created activities
    uint8_t created_actvs;
    /// Number of started activities
    uint8_t started_actvs;
    #if (BLE_OBSERVER)
    /// Number of devices in the white list
    uint8_t nb_dev_wl;
    #endif //(BLE_OBSERVER)
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
/// GAP Manager environment variable.
extern struct gapm_env_tag gapm_env;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize GAP attribute database
 *
 * @param[in] start_hdl Service Start Handle
 * @param[in] feat      Attribute database features
 *
 * @return status code of attribute database initialization
 *  - @ref ATT_ERR_NO_ERROR: If database creation succeeds.
 *  - @ref ATT_ERR_INVALID_HANDLE: If start_hdl given in parameter + nb of attribute override
 *                            some existing services handles.
 *  - @ref ATT_ERR_INSUFF_RESOURCE: There is not enough memory to allocate service buffer.
 *                           or of new attribute cannot be added because all expected
 *                           attributes already add
 ****************************************************************************************
 */
uint8_t gapm_init_attr(uint16_t start_hdl, uint32_t feat);

void gapm_send_raw_complete_evt(uint8_t op_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send operation completed message according to operation type.
 * Perform also am operation clean-up
 *
 * @param[in] op_type Operation type
 * @param[in] status  Command status
 *****************************************************************************************
 */
void gapm_send_complete_evt(uint8_t op_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send operation completed message with status error code not related to a
 * running operation.
 *
 * @param[in] operation Operation code
 * @param[in] requester requester of operation
 * @param[in] status    Error status code
 ****************************************************************************************
 */
void gapm_send_error_evt(uint8_t operation, const ke_task_id_t requester, uint8_t status);


/**
 ****************************************************************************************
 * @brief Get operation pointer
 *
 * @param[in] op_type       Operation type.
 *
 * @return operation pointer on going
 ****************************************************************************************
 */
static __INLINE void* gapm_get_operation_ptr(uint8_t op_type)
{
    ASSERT_ERR(op_type < GAPM_OP_MAX);
    // return operation pointer
    return gapm_env.operation[op_type];
}


/**
 ****************************************************************************************
 * @brief Set operation pointer
 *
 * @param[in] op_type       Operation type.
 * @param[in] op            Operation pointer.
 *
 ****************************************************************************************
 */
static __INLINE void gapm_set_operation_ptr(uint8_t op_type, void* op)
{
    ASSERT_ERR(op_type < GAPM_OP_MAX);
    // update operation pointer
    gapm_env.operation[op_type] = op;
}


/**
 ****************************************************************************************
 * @brief Check if current operation can be processed or not.
 * if it can be proceed, initialize an operation request.
 * If a command complete event with error code can be triggered.
 *
 * Function returns how the message should be handled by message handler.
 *
 * @param[in] op_type       Operation type.
 * @param[in] op_msg        Requested operation message (note op_msg cannot be null)
 * @param[in] supp_ops      Supported operations array.
 *                          Latest array value shall be GAPM_NO_OP.
 *
 * @return operation can be executed if message status equals KE_MSG_NO_FREE,
 * else nothing to do, just exit from the handler.
 ****************************************************************************************
 */
int gapm_process_op(uint8_t op_type, void* op_msg, enum gapm_operation* supp_ops);

/**
 ****************************************************************************************
 * @brief Get operation on going
 *
 * @param[in] op_type       Operation type.
 *
 * @return operation code on going
 ****************************************************************************************
 */
uint8_t gapm_get_operation(uint8_t op_type);

/**
 ****************************************************************************************
 * @brief Get requester of on going operation
 *
 * @param[in] op_type       Operation type.
 *
 * @return task that requests to execute the operation
 ****************************************************************************************
 */
ke_task_id_t gapm_get_requester(uint8_t op_type);




#if (BLE_GAPC)
/**
 ****************************************************************************************
 * @brief A connection has been created, initialize host stack to be ready for connection.
 *
 * @param[in] msgid      Message id
 * @param[in] operation  Air operation type
 * @param[in] con_params Connection parameters from lower layers
 *
 * @return Connection index allocated to the new connection.
 ****************************************************************************************
 */
uint8_t gapm_con_create(ke_msg_id_t const msgid, uint8_t operation, struct hci_le_enh_con_cmp_evt const *con_params);

#endif // (BLE_GAPC)


/**
 ****************************************************************************************
 * @brief Set the maximal MTU value
 *
 * @param[in] mtu   Max MTU value (Minimum is 23)
 ****************************************************************************************
 */
void gapm_set_max_mtu(uint16_t mtu);

/**
 ****************************************************************************************
 * @brief Set the maximal MPS value
 *
 * @param[in] mps   Max MPS value (Minimum is 23)
 ****************************************************************************************
 */
void gapm_set_max_mps(uint16_t mps);

/**
 ****************************************************************************************
 * @brief Checks validity of the address type
 *
 * @param[in] addr_type   Address type
 ****************************************************************************************
 */
uint8_t gapm_addr_check( uint8_t addr_type);

#if (BLE_DEPRECATED_API)
/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
uint8_t gapm_privacy_cfg_check(uint8_t privacy_cfg);
#endif //(BLE_DEPRECATED_API)

/**
 ****************************************************************************************
 * @brief Checks validity of the Data Length Suggested values
 *
 * @param[in] sugg_oct   Suggested octets
 * @param[in] sugg_time  Suggested time
 ****************************************************************************************
 */
uint8_t gapm_dle_val_check(uint16_t sugg_oct, uint16_t sugg_time);



/**
 ****************************************************************************************
 * @brief Update task state
 *
 * @param[in] operation that modify the state
 * @param[in] set state to busy (true) or idle (false)
 *
 ****************************************************************************************
 */
void gapm_update_state(uint8_t operation, bool busy);



#if (BLE_LECB)


/**
 ****************************************************************************************
 * @brief Find Information about LE_PSM registered in GAPM
 *
 * @param[in] le_psm    LE Protocol/Service Multiplexer
 *
 * @return Null if not found or LE_PSM info structure
 ****************************************************************************************
 */
struct gapm_le_psm_info* gapm_le_psm_find(uint16_t le_psm);


/**
 ****************************************************************************************
 * @brief Remove all registered LE_PSM
 ****************************************************************************************
 */
void gapm_le_psm_cleanup(void);
#endif // (BLE_LECB)

#if (BLE_DEPRECATED_API)
/**
 ****************************************************************************************
 * Continue address update for air operation
 *
 * @param[in] opcode HCI event operation code
 * @param[in] status HCI command status
 ****************************************************************************************
 */
void gapm_air_addr_update_cont(uint16_t opcode, uint8_t status);

int gapm_cmp_evt_leg_air_handler(ke_msg_id_t const msgid, struct gapm_cmp_evt const *cmp_evt,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id);
int gapm_addr_renew_to_ind_leg_air_handler(ke_msg_id_t const msgid, void const *param,
                   ke_task_id_t const dest_id, ke_task_id_t const src_id);
int hci_le_enh_con_cmp_evt_leg_air_handler(uint16_t opcode, struct hci_le_enh_con_cmp_evt const *event);
int gapc_cmp_evt_leg_air_handler(ke_msg_id_t const msgid, struct gapc_cmp_evt const *cmp_evt,
                    ke_task_id_t const dest_id, ke_task_id_t const src_id);
int gapc_peer_att_info_ind_leg_air_handler(ke_msg_id_t const msgid, struct gapc_peer_att_info_ind const *p_event,
                                           ke_task_id_t const dest_id, ke_task_id_t const src_id);
#endif //(BLE_DEPRECATED_API)

void gapm_actv_init(bool reset);
struct gapm_actv_tag *gapm_actv_alloc(uint8_t actv_idx, uint8_t size);
bool gapm_actv_retrieve_cmd_cmp_evt(struct gapm_actv_tag **pp_actv, uint16_t opcode);
uint8_t gapm_actv_get_hci_own_addr_type(uint8_t app_addr_type);
void gapm_actv_created(struct gapm_actv_tag *p_actv, uint8_t error);
void gapm_actv_started(struct gapm_actv_tag *p_actv, uint8_t error);
void gapm_actv_stopped(struct gapm_actv_tag *p_actv, uint8_t status);
void gapm_actv_deleted(struct gapm_actv_tag *p_actv);
void gapm_actv_send_gen_rand_addr(struct gapm_actv_tag *p_actv);
void gapm_actv_addr_gen_ind(uint8_t *p_prand, uint8_t *p_hash);
void gapm_actv_addr_set_ind(void);

void gapm_adv_send_hci_le_set_adv_set_rand_addr_cmd(struct gapm_actv_tag *p_actv);
uint8_t gapm_adv_create(uint8_t actv_idx, struct gapm_activity_create_adv_cmd *p_param);
uint8_t gapm_adv_start(struct gapm_actv_tag *p_actv, struct gapm_activity_start_cmd *p_param);
void gapm_adv_stop(struct gapm_actv_tag *p_actv);
void gapm_adv_delete(struct gapm_actv_tag *p_actv);

void gapm_scan_actv_clean(struct gapm_actv_tag *p_actv);
uint8_t gapm_scan_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);
uint8_t gapm_scan_start(struct gapm_actv_tag *p_actv, struct gapm_activity_start_cmd *p_param);
void gapm_scan_stop(struct gapm_actv_tag *p_actv);
void gapm_scan_delete(struct gapm_actv_tag *p_actv);
void gapm_scan_addr_set_ind(struct gapm_actv_tag *p_actv);

uint8_t gapm_init_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);
uint8_t gapm_init_start(struct gapm_actv_tag *p_actv, struct gapm_activity_start_cmd *p_param);
void gapm_init_stop(struct gapm_actv_tag *p_actv);
void gapm_init_delete(struct gapm_actv_tag *p_actv);
bool gapm_init_connection_ind(uint8_t conidx);
void gapm_init_addr_set_ind(struct gapm_actv_tag *p_actv);

uint8_t gapm_per_sync_create(uint8_t actv_idx, struct gapm_activity_create_cmd *p_param);
uint8_t gapm_per_sync_start(struct gapm_actv_tag *p_actv, struct gapm_activity_start_cmd *p_param);
void gapm_per_sync_stop(struct gapm_actv_tag *p_actv);
void gapm_per_sync_delete(struct gapm_actv_tag *p_actv);

/**
 ****************************************************************************************
 * @brief Sends a basic HCI command (with no parameter)
 *
 * @param[in] opcode       Command opcode
 ****************************************************************************************
 */
void hci_basic_cmd_send_2_controller(uint16_t opcode);


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

/// @} GAPM_INT

#endif /* _GAPM_INT_H_ */
