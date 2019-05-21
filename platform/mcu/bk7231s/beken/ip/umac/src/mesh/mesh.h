/**
 ****************************************************************************************
 *
 * @file mesh.h
 *
 * @brief Mesh Module Definitions.
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 ****************************************************************************************
 */

#ifndef _MESH_H_
#define _MESH_H_

/**
 ****************************************************************************************
 * @defgroup MESH MESH
 * @ingroup UMAC
 * @brief Mesh Module Definitions.
 * @{
 ****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

/// RW Stack Configuration
#include "rwnx_config.h"

#if (RW_MESH_EN)

#include "dma.h"
#include "hal_dma.h"
#include "rxu_task.h"
#include "mm_timer.h"
#include "tx_swdesc.h"
#include "sta_mgmt.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/**
 * --------------------------------------------------------------------------------------
 * Mesh Configuration Information Element
 * --------------------------------------------------------------------------------------
 */

/// Length of information contained in the Mesh Configuration Information Element
#define MESH_CONF_IE_INFO_LENGTH              (7)
/// Offset of the Active Path Selection Protocol Identifier in the Mesh Configuration IE
#define MESH_CONF_IE_PATH_SEL_PROT_OFFSET     (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Active Path Selection Metric Identifier in the Mesh Configuration IE
#define MESH_CONF_IE_PATH_METRIC_PROT_OFFSET  (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Congestion Control Mode Identifier in the Mesh Configuration IE
#define MESH_CONF_IE_CONG_CTRL_MODE_OFFSET    (MAC_INFOELT_INFO_OFT + 2)
/// Offset of the Synchronization Method Identifier in the Mesh Configuration IE
#define MESH_CONF_IE_SYNC_METHOD_OFFSET       (MAC_INFOELT_INFO_OFT + 3)
/// Offset of the Authentication Protocol in the Mesh Configuration IE
#define MESH_CONF_IE_AUTH_PROT_OFFSET         (MAC_INFOELT_INFO_OFT + 4)
/// Offset of the Mesh Formation Information in the Mesh Configuration IE
#define MESH_CONF_IE_MESH_FORM_INFO_OFFSET    (MAC_INFOELT_INFO_OFT + 5)
/// Offset of the Mesh Capabilities in the Mesh Configuration IE
#define MESH_CONF_IE_MESH_CAP_OFFSET          (MAC_INFOELT_INFO_OFT + 6)

/// Active Path Selection Protocol Identifier field value - Hybrid Wireless Mesh Protocol (HWMP)
#define MESH_CONF_PATH_SEL_PROT_HWMP          (1)

/// Active Path Selection Metric Identifier field value - Airtime link metric
#define MESH_CONF_PATH_METRIC_PROT_AIRTIME    (1)

/// Congestion Control Mode Identifier field value - Not Activated
#define MESH_CONF_CONG_CTRL_NOT_ACTIVE        (0)
/// Congestion Control Mode Identifier field value - Congestion Control Signaling Protocol (CCSP)
#define MESH_CONF_CONG_CTRL_CCSP              (1)

/// Synchronization Method Identifier field value - Neighbor offset synchronization method
#define MESH_CONF_SYNC_METHOD_NEIGHBOR        (1)

/// Authentication Protocol Identifier field value - No authentication
#define MESH_CONF_AUTH_PROTO_NO_AUTH          (0)
/// Authentication Protocol Identifier field value - Simultaneous Authentication of Equals (SAE)
#define MESH_CONF_AUTH_PROTO_SAE              (1)
/// Authentication Protocol Identifier field value - IEEE 802.1X
#define MESH_CONF_AUTH_PROTO_802_1X           (2)


/// ***************************** Mesh Formation Information *****************************
/// Offset of Number of peerings field in the Mesh Formation Information field
#define MESH_FORM_INFO_NB_PEER_OFFSET         (1)
/// Mask allowing to extract the Number of peerings value fron the Mesh Formation Information value
#define MESH_FORM_INFO_NB_PEER_MASK           (0x3F)

/// ******************************** Mesh Capabilities************************************
/// Accepting Additional Mesh Peerings
#define MESH_CAPA_ACCEPT_MESH_PEER_MASK       (CO_BIT(0))
/// MCCA Supported
#define MESH_CAPA_MCCA_SUPPORT_MASK           (CO_BIT(1))
/// MCCA Enabled
#define MESH_CAPA_MCCA_ENABLED_MASK           (CO_BIT(2))
/// Forwarding Supported (dot11MeshForwarding)
#define MESH_CAPA_FORWARD_SUPPORT_MASK        (CO_BIT(3))
/// MBCA Enabled
#define MESH_CAPA_MBCA_ENABLED_MASK           (CO_BIT(4))
/// TBTT Adjustment Supported
#define MESH_CAPA_TBTT_ADJUST_MASK            (CO_BIT(5))
/// Power Save
#define MESH_CAPA_POWER_SAVE_MASK             (CO_BIT(6))

/**
 * --------------------------------------------------------------------------------------
 * Mesh Awake Window Information Element
 * --------------------------------------------------------------------------------------
 */

/// Length of information contained in the Mesh Awake Window Information Element
#define MESH_AWAKE_WINDOW_IE_INFO_LENGTH      (2)

/**
 * --------------------------------------------------------------------------------------
 * Mesh Control Field
 * --------------------------------------------------------------------------------------
 */

/// Minimal Length of Mesh Control Field
#define MESH_CTRL_MIN_LEN               (6)
/// Offset of Mesh Flags subfield in the Mesh Control field
#define MESH_CTRL_FLAGS_OFFSET          (0)
/// Offset of Mesh TTL subfield in the Mesh Control field
#define MESH_CTRL_TTL_OFFSET            (1)
/// Offset of Mesh Sequence Number subfield in the Mesh Control field
#define MESH_CTRL_SN_OFFSET             (2)
/// Mask allowing to read Address Extension Mode in the Mesh Flags subfield
#define MESH_CTRL_FLAGS_AE_MODE_MASK    (0x3)
/// Offset of Mesh Address Extension in the Mesh Control field
#define MESH_CTRL_MESH_AE_OFFSET        (6)

/**
 * --------------------------------------------------------------------------------------
 * Miscellaneous
 * --------------------------------------------------------------------------------------
 */

/// Maximum length of the Mesh ID
#define MESH_MESHID_MAX_LEN          (32)
/// Maximum length of IEs to append at the end of the beacon
#define MESH_MAX_IE_LEN              (96)
/// Default TTL value
#define MESH_DEFAULT_TTL_VAL         (0x1F)
/// Invalid Mesh VIF structure index
#define MESH_INVALID_MESH_IDX        (RW_MESH_VIF_NB)
/// Margin allowing to decide if next TBTT is already in the past or not
#define MESH_TBTT_TIMER_MARGIN       (100)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Mesh Operation Codes
enum mesh_op_code
{
    /// No operation
    MESH_OP_CODE_NONE = 0,
    /// Start a Mesh Point
    MESH_OP_CODE_START,
    /// Stop a Mesh Point
    MESH_OP_CODE_STOP,
    /// Add a STA entry
    MESH_OP_CODE_STA_ADD,

    MESH_OP_CODE_MAX
};

/// Bit Field Status Bits for struct mesh_vif_info_tag's status
enum mesh_status_bit
{
    /// Active
    MESH_STATUS_ACTIVE = 0,
    /// Is Mesh Gate (dot11MeshGateAnnouncementProtocol)
    MESH_STATUS_GATE,
    /// Mesh Awake Window of the peer is open
    MESH_STATUS_PEER_MAW,
};

/// Self-Protected Action Field values (see IEEE Std 802.11-2012 section 8.5.16.1)
enum mesh_sprot_act
{
    /// 0 is reserved
    MESH_SPROT_ACT_RESERVED = 0,
    /// Mesh Peering Open
    MESH_SPROT_ACT_OPEN,
    /// Mesh Peering Confirm
    MESH_SPROT_ACT_CONFIRM,
    /// Mesh Peering Close
    MESH_SPROT_ACT_CLOSE,
    /// Mesh Group Key Inform
    MESH_SPROT_ACT_GROUP_KEY_INFORM,
    /// Mesh Group Key Acknowledge
    MESH_SPROT_ACT_GROUP_KEY_ACK,

    MESH_SPROT_ACT_MAX
};

/// Mesh Action Field values (see IEEE Std 802.11-2012 section 8.5.17.1)
enum mesh_act
{
    /// Mesh Link Metric Report
    MESH_ACT_MLINK_METRIC_REPORT = 0,
    /// HWMP Mesh Path Selection
    MESH_ACT_HWMP_MESH_PATH_SELECTION,
    /// Gate Announcement
    MESH_ACT_GATE_ANNOUNCE,
    /// Congestion Control Notification
    MESH_ACT_CONGESTION_CTRL_NOTIF,
    /// MCCA Setup Request
    MESH_ACT_MCCA_SETUP_REQ,
    /// MCCA Setup Reply
    MESH_ACT_MCCA_SETUP_REPLY,
    /// MCCA Advertisement Request
    MESH_ACT_MCCA_ADV_REQ,
    /// MCCA Advertisement
    MESH_ACT_MCCA_ADV,
    /// MCCA Teardown
    MESH_ACT_MCCA_TEARDOWN,
    /// TBTT Adjustment Request
    MESH_ACT_TBTT_ADJUST_REQ,
    /// TBTT Adjustment Response
    MESH_ACT_TBTT_ADJUST_RSP,

    MESH_ACT_MAX
};

/// Mesh Root Mode codes
enum mesh_root_mode
{
    /// The mesh STA is not a root mesh STA
    MESH_RMODE_NO_ROOT  = 0,
    /// The mesh STA is a root mesh STA
    MESH_RMODE_ROOT,
    /// The mesh STA is a root mesh STA supporting Proactive PREQ with proactive PREP subfield set to 0
    MESH_RMODE_PROACT_PREP0,
    /// The mesh STA is a root mesh STA supporting Proactive PREQ with proactive PREP subfield set to 1
    MESH_RMODE_PROACT_PREP1,
    /// The mesh STA is a root mesh STA supporting Proactive RANN
    MESH_RMODE_PROACT_RANN,

    MESH_RMODE_MAX,
};

/// Bit fields for mesh_update_req message's flags value
enum mesh_update_flags_bit
{
    /// Root Mode
    MESH_UPDATE_FLAGS_ROOT_MODE_BIT = 0,
    /// Gate Mode
    MESH_UPDATE_FLAGS_GATE_MODE_BIT,
    /// Mesh Forwarding
    MESH_UPDATE_FLAGS_MESH_FWD_BIT,
    /// Local PS mode
    MESH_UPDATE_FLAGS_LOCAL_PSM_BIT,

    MESH_UPDATE_FLAGS_BIT_MAX
};

/// Mesh Close Reason Codes (see IEEE Std 802.11-2012 section 8.4.1.7)
enum mesh_close_reason
{
    /**
     * Mesh Peering instance has been cancelled with a reason other than reaching
     * the maximum number of peer mesh STAs
     */
    MESH_REASON_PEERING_CANCELLED = 52,
    /**
     * The mesh STA has reached the supported maximum number of peer mesh STAs
     */
    MESH_REASON_MAX_PEERS,
    /**
     * The received information violates the Mesh Configuration policy configured
     * in the mesh STA profile
     */
    MESH_REASON_CONFIG_VIOLATION,
    /**
     * The mesh STA has received a Mesh Peering Close message requesting to close the
     * mesh peering
     */
    MESH_REASON_CLOSE_RCVD,
    /**
     * The mesh STA has resent dot11MeshMaxRetries Mesh Peering Open messagesm without
     * receiving a Mesh Peering Confirm message
     */
    MESH_REASON_MAX_RETRIES,
    /**
     * The confirmTimer for the mesh peering instance has timed out
     */
    MESH_REASON_CONFIRM_TIMEOUT,
    /**
     * The mesh STA fails to unwrap the GTK or the values in the wrapped contents do not
     * match
     */
    MESH_REASON_INVALID_GTK,
    /**
     * The mesh STA has received inconsistent information about the mesh parameters between
     * Mesh Peering Management frames
     */
    MESH_REASON_INCONSISTENT_PARAM,
};

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

// Forward declaration
struct vif_info_tag;
struct txl_frame_desc_tag;

/// Definition of buffer for additional IEs
struct mesh_add_ies_tag
{
    /// DMA descriptor to download the IEs from host memory
    struct dma_desc dma_desc;
    /// Buffer space for IEs
    uint32_t buf[MESH_MAX_IE_LEN/4];
};

/// Mesh Configuration
struct mesh_conf_tag
{
    /// Path Selection Protocol
    uint8_t path_sel_proto;
    /// Path Selection Metric
    uint8_t path_sel_metric;
    /// Congestion Control
    uint8_t congest_ctrl;
    /// Synchronization Method
    uint8_t sync_method;
    /// Authentication Protocol
    uint8_t auth_proto;
    /// Mesh Formation Information
    uint8_t mesh_form;
    /// Capabilities
    uint8_t mesh_cap;
};

/// Structure containing information about peer's TBTT
struct mesh_tbtt_info
{
    /// List Header
    struct co_list_hdr list_hdr;
    /// Next TBTT time
    uint32_t tbtt_time;
};

/// Mesh Link Information
struct mesh_link_info_tag
{
    /// Retry Timer
    struct mm_timer_tag retry_timer;
    /// Confirm Timer
    struct mm_timer_tag cfm_timer;
    /// Holding Timer
    struct mm_timer_tag holding_timer;
    /// Timer for Peer Mesh Awake Window
    struct mm_timer_tag peer_maw_tmr;
    /// Peer TBTT Information
    struct mesh_tbtt_info tbtt_info;
    /// Local Link ID
    uint16_t local_link_id;
    /// Peer Link ID
    uint16_t peer_link_id;
    /// Mesh Peering Management Status (see enum mesh_mpm_state)
    uint8_t mpm_state;
    /// Retry Counter for the Mesh Peering Open Frame
    uint8_t open_retry_nb;
    /// STA Index
    uint8_t sta_idx;
    /// VIF Index
    uint8_t vif_idx;
    /// Mesh VIF Index
    uint8_t mvif_idx;
    /// Close Reason Code
    uint8_t reason_code;
    /// Peer PS Mode
    uint8_t peer_ps_mode;
    /// Prevent sleep
    uint8_t prevent_sleep;
    /// Status
    uint8_t status;
    /// Last RSPI sent in a mesh ps trigger frame
    uint8_t last_rspi;
};

// Mesh VIF Information
struct mesh_vif_info_tag
{
    /// Current Mesh Configuration
    struct mesh_conf_tag mesh_conf;
    /// List of used path structures
    struct co_list used_path;
    /// List of STA proxied by this mesh interface
    struct co_list proxy_list;
    /// Ordered list of upcoming TBTTs (local and peers)
    struct co_list tbtt_list;
    /// Local TBTT Information
    struct mesh_tbtt_info tbtt_info;
    /// Interval Timer for RANN, Proactive PREQ or GANN frame
    struct mm_timer_tag intv_timer;
    /// Timer for Local Mesh Awake Window
    struct mm_timer_tag local_maw_tmr;
    /// Time at which last PERR frame has been sent (dot11MeshHWMPperrMinInterval TUs between two PERR)
    uint32_t last_perr_time;
    /// Time at which last PREQ frame has been sent (dot11MeshHWMPpreqMinInterval TUs between two PREQ)
    uint32_t last_preq_time;
    /// Mesh Sequence Number
    uint32_t mesh_sn;
    /// Address of Mesh Configuration IE in the beacon
    uint32_t mconf_bcn_addr;
    /// Address of Mesh Awake Window IE in the beacon
    uint32_t maw_bcn_addr;
    /// Address of Vendor IEs in Host Memory
    uint32_t vendor_ies_addr;
    /// Local Mesh Awake Window Duration (in TUs)
    uint16_t local_maw_dur;
    /// Mesh ID
    uint8_t mesh_id[MESH_MESHID_MAX_LEN];
    /// Mesh ID Length
    uint8_t mesh_id_len;
    /// Vendor IEs length
    uint8_t vendor_ies_len;
    /// DTIM Period
    uint8_t dtim_period;
    /// VIF Index
    uint8_t vif_idx;
    /// MVIF Index
    uint8_t mvif_idx;
    /// Prevent sleep status
    uint8_t prevent_sleep;
    /// Status
    uint8_t status;
    /// Root Mode (dot11MeshHWMPRootMode)
    uint8_t root_mode;
    /// Element TTL, set in Mesh Path Selection frames
    uint8_t elt_ttl;
    /// Number of expected confirmations for PS transitions
    uint8_t nb_exp_cfm;
    /// Indicate if Mesh Peering Management (MPM) protocol is handled by userspace
    bool user_mpm;
    /// Indicate if authentication is used on this VIF
    bool is_auth;
};

/// Mesh Environment
struct mesh_env_tag
{
    /// General Purpose DMA descriptor to be used for download of IEs from host memory
    struct hal_dma_desc_tag gp_dma_desc;
    /// Pointer to a message kernel stored during an operation
    void *p_msg;
    /// VIF Index for which current operation is performed
    uint8_t op_vif_idx;
    /// Current Operation Code
    uint8_t op_code;
    /// Maximum number of peer link open retries that can be sent
    uint8_t open_max_retries;
    /// Local PS mode (same for all our peer mesh STAs)
    uint8_t local_ps_mode;
    /// Non Peer PS Mode
    uint8_t non_peer_ps_mode;
    /// New Local PS mode
    uint8_t nlocal_ps_mode;
    /// Number of expected confirmations for PS transitions
    uint8_t nb_exp_cfm;
    /// Number of Mesh VIFs currently used
    uint8_t nb_mesh_vifs;
    /// Number of Mesh Links currently established
    uint8_t nb_mesh_links;
};

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Mesh Global Environment
extern struct mesh_env_tag mesh_env;
/// Mesh Link Information
extern struct mesh_link_info_tag mesh_link_info_tab[RW_MESH_LINK_NB];
/// Mesh VIF Information
extern struct mesh_vif_info_tag mesh_vif_info_tab[RW_MESH_VIF_NB];

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize stack for support of Wireless Mesh Networks
 ****************************************************************************************
 */
void mesh_init(void);

/**
 ****************************************************************************************
 * @brief Configure a @see struct mesh_link_info_tag structure for a new mesh peering
 * established on a provided Mesh VIF
 *
 * @param[in] p_sta_entry   STA Information structure
 * @param[in] mvif_idx      Index of the Mesh VIF on which the new peering has occured.
 ****************************************************************************************
 */
void mesh_conn_mlink_info(struct sta_info_tag *p_sta_entry, uint8_t mvif_idx);

/**
 ****************************************************************************************
 * @brief Link an available Mesh VIF information structure with a given VIF entry.
 *
 * @param[in] p_vif_entry     Pointer to the VIF entry structure
 *
 * @return Pointer to the linked Mesh VIF information structure
 ****************************************************************************************
 */
struct mesh_vif_info_tag *mesh_get_mvif_entry(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Goes to MESH_BUSY state and store the provided operation code.
 *
 * @param[in] op_code   Operation code (@see enum mesh_op_code)
 * @param[in] vif_idx   Index of the VIF for which operation is started
 ****************************************************************************************
 */
void mesh_set_operation(uint8_t op_code, uint8_t vif_idx);

/**
 ****************************************************************************************
 * @brief Goes back to MESH_IDLE state and clean the operation information.
 ****************************************************************************************
 */
void mesh_clear_operation(void);

/**
 ****************************************************************************************
 * @brief Initiate "disconnection" of provided Mesh Link.
 *
 * @param[in] p_mlink_info  Information about the link to disconnect
 ****************************************************************************************
 */
void mesh_cancel_link(struct mesh_link_info_tag *p_mlink_info);

/**
 ****************************************************************************************
 * @brief Update number of peerings for a given Mesh VIF
 *
 * @param[in] p_mvif_info   Mesh VIF information
 * @param[in] add           Indicate if a peering has been added/removed
 ****************************************************************************************
 */
void mesh_update_nb_peerings(struct mesh_vif_info_tag *p_mvif_info, bool add);

/**
 ****************************************************************************************
 * @brief Add Mesh ID Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2-33: Mesh ID
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] mesh_id_len     Length of the provided Mesh ID.
 * @param[in] p_mesh_id       Pointer to the Mesh ID value.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t mesh_add_ie_mesh_id(uint32_t *frame_addr, uint8_t mesh_id_len, uint8_t *p_mesh_id);

/**
 ****************************************************************************************
 * @brief Add Mesh Configuration Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2: Active Path Selection Protocol Identifier
 *          Byte 3: Active Path Selection Metric Identifier
 *          Byte 4: Congestion Control Mode Identifier
 *          Byte 5: Synchronization Method Identifier
 *          Byte 6: Authentication Protocol Identifier
 *          Byte 7: Mesh Formation Info
 *          Byte 8: Mesh Capability
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] p_mesh_conf     Pointer to the structure containing the mesh configuration.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t mesh_add_ie_mesh_conf(uint32_t *frame_addr, struct mesh_conf_tag *p_mesh_conf);

/**
 ****************************************************************************************
 * @brief Send a MM_SET_BEACON_INT_REQ message to the LMAC layer.
 *
 * @param[in] p_vif_entry   Mesh Point VIF Information
 ****************************************************************************************
 */
void mesh_send_set_beacon_int_req(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Send a MM_SET_BSSID_REQ message to the LMAC layer.
 *
 * @param[in] p_vif_entry   Mesh Point VIF Information
 ****************************************************************************************
 */
void mesh_send_set_bssid_req(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Send a ME_SET_PS_DISABLE_REQ message to the ME module in order to enable or disable
 * support of Power Mode mode for the Mesh Point VIF.
 *
 * @param[in] p_vif_entry   Mesh Point VIF Information
 * @param[in] ps_disable    Indicate if PS support must be enabled or not for this VIF
 ****************************************************************************************
 */
void mesh_send_ps_disable_req(struct vif_info_tag *p_vif_entry, bool ps_disable);

/**
 ****************************************************************************************
 * @brief Send a ME_SET_ACTIVE_REQ message to the ME module in order to enable or disable
 * use of the Mesh Point VIF
 *
 * @param[in] p_vif_entry   Mesh Point VIF Information
 * @param[in] active        Indicate if VIF is activated or not.
 ****************************************************************************************
 */
void mesh_send_set_active_req(struct vif_info_tag *p_vif_entry, bool active);

/**
 ****************************************************************************************
 * @brief Send a MESH_PEER_UPDATE_IND message to the host in order to indicate that a
 * peering has been established or closed.
 *
 * @param[in] p_mlink_info  Mesh Link Information
 * @param[in] estab         True if the peering has been established, else false
 ****************************************************************************************
 */
void mesh_send_peer_update_ind(struct mesh_link_info_tag *p_mlink_info, bool estab);

/**
 ****************************************************************************************
 * @brief Send a MM_SET_VIF_STATE_REQ message to the LMAC in order to start/stop sending of
 * beacons on the Mesh Point VIF.
 *
 * @param[in] p_vif_entry   Mesh Point VIF Information
 * @param[in] active        Indicate if VIF has to be activated/unactivated
 ****************************************************************************************
 */
void mesh_send_set_vif_state_req(struct vif_info_tag *p_vif_entry, bool active);

/**
 ****************************************************************************************
 * @brief Send a MESH_PEER_INFO_RSP message to the host.
 *
 * @param[in] sta_idx           STA Index provided in the MESH_PEER_INFO_REQ message
 * @param[in] p_mlink_info      Pointer to the Mesh Link Information structure containing
 * the information to be sent to the host. If status is CO_FAIL, do not consider it.
 ****************************************************************************************
 */
void mesh_send_peer_info_rsp(uint8_t sta_idx, struct mesh_link_info_tag *p_mlink_info);

/**
 ****************************************************************************************
 * @brief Send a MESH_START_CFM message to the host after reception of a MESH_START_REQ.
 *
 * @param[in] vif_index     Index of the VIF for which start has been requested
 * @param[in] status        Request status
 ****************************************************************************************
 */
void mesh_send_start_cfm(uint8_t vif_index, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send a MESH_STOP_CFM message to the host after reception of a MESH_STOP_REQ.
 *
 * @param[in] vif_index     Index of the VIF for which stop has been requested
 * @param[in] status        Request status
 ****************************************************************************************
 */
void mesh_send_stop_cfm(uint8_t vif_index, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send a MESH_UPDATE_CFM message to the host.
 *
 * @param[in] status        Status of MESH_UPDATE_REQ handling
 ****************************************************************************************
 */
void mesh_send_update_cfm(uint8_t status);

/**
 ****************************************************************************************
 * @brief Check the Mesh ID sent by a newly discovered mesh STA. In a mesh BSS all mesh
 * STAs use the same Mesh ID (see IEEE Std 802-11-2012, section 13.2.3)
 *
 * @param[in] a_ie          Address of Mesh ID Information Element
 *
 * @return true if the received ID matches with our ID, else false.
 ****************************************************************************************
 */
struct mesh_vif_info_tag *mesh_check_mesh_id(uint32_t a_ie);

/**
 ****************************************************************************************
 * @brief Parse received IEs and internal status in order to decide if we can start peering
 * with a newly discovered mesh STA.
 * This function is called after reception of following frames:
 *      - Mesh Beacon
 *      - Mesh Peering Open
 *      - Mesh Probe Response
 * when the source MAC address in an unknown address.
 * If mesh STA is considered as a candidate mesh STA, the state of the Mesh task is set to
 * BUSY and a MM_STA_ADD_REQ is sent to the LMAC.
 *
 * @param[in] peer_addr     Source address of the frame
 * @param[in] a_ies         Address of received information elements
 * @param[in] len_ies       Length of received information elements
 *
 * @return true if peering with the mesh STA is accepted, else false.
 ****************************************************************************************
 */
bool mesh_accept_new_peer(struct mac_addr *p_peer_addr, uint8_t vif_idx, uint32_t a_ies, uint16_t len_ies);

/**
 ****************************************************************************************
 * @brief Build the beacon for the Mesh Point interface based on the information stored
 * in the VIF Information structure and the Mesh environment.
 *
 * @param[in] p_vif_entry       VIF Information
 * @param[in] p_mvif_entry      MVIF Information
 ****************************************************************************************
 */
void mesh_build_beacon(struct vif_info_tag *p_vif_entry, struct mesh_vif_info_tag *p_mvif_entry);

/**
 ****************************************************************************************
 * @brief Handle reception of a beacon sent by a Mesh STA (ie containing a Mesh ID
 * Information element
 *
 * @param[in] p_param   Content of the RXU_MGT_IND message containing the frame information
 ****************************************************************************************
 */
bool mesh_handle_beacon(struct rxu_mgt_ind *p_param);

/**
 ****************************************************************************************
 * @brief Handle reception of a management action frame sent by a Mesh STA.
 *
 * @param[in] p_param   Content of the RXU_MGT_IND message containing the frame information
 ****************************************************************************************
 */
bool mesh_handle_action_frame(struct rxu_mgt_ind *p_param, bool check_capa);

/**
 ****************************************************************************************
 * @brief Prepare the DMA transfer for download of additional information elements.
 *
 * @param[in] ie_addr   Address of Information Elements in host memory
 * @param[in] ie_len    Length of Information Elements (> 0)
 ****************************************************************************************
 */
void mesh_download_vendor_ies(struct mesh_vif_info_tag *p_mvif_info);

/**
 ****************************************************************************************
 * @brief This function is called during beacon build so that mesh dynamic parameters can
 * be updated.
 *
 * @param[in] p_vif_beacon      VIF for which beacon is built
 ****************************************************************************************
 */
void mesh_update_beacon(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Allocate a frame to be used for transmission of a Mesh action frame
 *
 * @param[in] p_vif_entry   VIF on which the frame has to be sent
 * @param[in|out] p_frame   Pointer to the pointer variable that will contain the address
 *                          of the allocated frame descriptor.
 * @param[in|out] p_addr    Pointer to the variable that will contain the address of the
 *                          payload buffer.
 *
 * @return true if the frame has been successfully allocated, else false.
 ****************************************************************************************
 */
bool mesh_tx_get_frame(struct vif_info_tag *p_vif_entry,
                       struct txl_frame_desc_tag **p_frame, uint32_t *p_addr);

/**
 ****************************************************************************************
 * @brief Prepare transmission of a Mesh Data frame by computing the length of the Mesh
 * Control field to be inserted in the MAC Header
 *
 * @param[in] p_vif_entry       VIF on which the frame has to be transmitted
 * @param[in] p_host_desc       TX configuration provided by the host
 * @param[in] p_umac_desc       TX configuration set by UMAC
 ****************************************************************************************
 */
int mesh_tx_data_prepare(struct vif_info_tag *p_vif_entry, struct hostdesc *p_host_desc,
                         struct umacdesc *p_umac_desc);

/**
 ****************************************************************************************
 * @brief Fill the Mesh Control fields for a QoS Data frame to be transmitted.
 *
 * @param[in] p_vif_entry   VIF Entry on which frame will be sent
 * @param[in] a_mesh_ctrl   Address of the Mesh Control field in the memory
 ****************************************************************************************
 */
void mesh_tx_data_fill_mesh_ctrl(struct vif_info_tag *p_vif_entry, uint32_t a_mesh_ctrl,
                                 struct hostdesc *p_host_desc, struct umacdesc *p_umac_desc);

/**
 ****************************************************************************************
 * @brief Update TBTT information either for provided VIF or for a peer Mesh STA on this
 * VIF. This function is in charge of sorting upcoming TBTT by order of occurence.
 *
 * @param[in] p_vif_entry       VIF on which TBTT will occur
 * @param[in] p_sta_entry       STA on which TBTT will occur. NULL if local TBTT
 * @param[in] next_tbtt         Time at which TBTT will occur
 ****************************************************************************************
 */
void mesh_update_tbtt_info(struct vif_info_tag *p_vif_entry, struct sta_info_tag *p_sta_entry,
                       uint32_t next_tbtt);

/**
 ****************************************************************************************
 * @brief Parse the content of the QoS Control field received as part of a QoS Data frame
 * and compute the length of the Mesh Control field. This length has to be added to the
 * computed MAC Header length.
 *
 * @param[in] p_vif_entry   VIF entry on which the data frame has been received
 * @param[in] a_qos_ctrl    Address of QoS Control field in the memory
 ****************************************************************************************
 */
int mesh_rx_get_machdr_add_len(struct vif_info_tag *p_vif_entry, uint32_t a_qos_ctrl);

/**
 ****************************************************************************************
 * @brief TODO[LT]
 ****************************************************************************************
 */
bool mesh_accept_frame(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Extract Basic Rate Set supported by a Mesh STA.
 *
 * @param p_rate_set    Pointer to the mac_rateset structure to fill with rate information
 * received from mesh STA
 * @param a_ies         Address of Information Elements in the memory
 * @param len_ies       Length of the Information Elements to be parsed
 ****************************************************************************************
 */
void mesh_extract_sta_rateset(struct mac_rateset *p_rateset, uint32_t a_ies, uint16_t len_ies);

/**
 ****************************************************************************************
 * @brief TODO[LT]
 ****************************************************************************************
 */
void mesh_add_sta_cfm(uint8_t vif_idx, uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief TODO[LT]
 ****************************************************************************************
 */
void mesh_del_sta_cfm(uint8_t mlink_idx);

/**
 ****************************************************************************************
 * @brief TODO[LT]
 ****************************************************************************************
 */
void mesh_check_peer_sta_capa(uint8_t sta_idx, uint32_t a_ies, uint16_t len_ies);

/**
 ****************************************************************************************
 * @brief Return the length of the additional Information Elements stored in the
 * txl_mesh_add_ies structure and that has to be added at beacon end.
 *
 * @param[in] mvif_idx  Mesh index for which the value has to be returned
 *
 * @return Length of additional Information Elements
 ****************************************************************************************
 */
__INLINE uint8_t mesh_get_vendor_ies_len(uint8_t mvif_idx)
{
    // Get MVIF information
    struct mesh_vif_info_tag *p_mvif_entry = &mesh_vif_info_tab[mvif_idx];

    return (p_mvif_entry->vendor_ies_len);
}

/**
 ****************************************************************************************
 * @brief Return the current number of peering for a given mesh VIF. This value is
 * extracted from the Mesh Formation value contained in the Mesh Configuration for this
 * VIF.
 *
 * @param[in] p_mvif_entry      Pointer to the Mesh VIF Information structure
 *
 * @return Number of peering for the provided Mesh VIF
 ****************************************************************************************
 */
__INLINE uint8_t mesh_get_nb_peers(struct mesh_vif_info_tag *p_mvif_entry)
{
    return ((p_mvif_entry->mesh_conf.mesh_form >> MESH_FORM_INFO_NB_PEER_OFFSET)
                                                & MESH_FORM_INFO_NB_PEER_MASK);
}

#endif //(RW_MESH_EN)

/// @}

#endif // _MESH_H_
