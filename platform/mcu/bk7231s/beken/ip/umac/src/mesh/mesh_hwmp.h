/**
 ****************************************************************************************
 *
 * @file mesh_hwmp.h
 *
 * @brief Mesh Module Definitions. Hybrid Wireless Mesh Protocol
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 ****************************************************************************************
 */

#ifndef _MESH_HWMP_H_
#define _MESH_HWMP_H_

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
#include "mesh.h"

#if (RW_MESH_EN)

#include "mm_timer.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/**
 * --------------------------------------------------------------------------------------
 * Path Request (PREQ) Information Element - IEEE Std 802.11-2012 Section 8.4.2.115
 * --------------------------------------------------------------------------------------
 */

/// Minimum length of information contained in the PREQ Information Element
#define MESH_HWMP_PREQ_INFO_MIN_LENGTH       (37)
/// Offset of the Flags field in the PREQ Information Element
#define MESH_HWMP_PREQ_FLAGS_OFFSET          (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Hop Count field in the PREQ Information Element
#define MESH_HWMP_PREQ_HOP_CNT_OFFSET        (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Element TTL field in the PREQ Information Element
#define MESH_HWMP_PREQ_ELT_TTL_OFFSET        (MAC_INFOELT_INFO_OFT + 2)
/// Offset of the Path Discovery ID field in the PREQ Information Element
#define MESH_HWMP_PREQ_PATH_DISC_ID_OFFSET   (MAC_INFOELT_INFO_OFT + 3)
/// Offset of the Originator Mesh STA Address field in the PREQ Information Element
#define MESH_HWMP_PREQ_ORIG_ADDR_OFFSET      (MAC_INFOELT_INFO_OFT + 7)
/// Offset of the Originator HWMP Sequence Number field in the PREQ Information Element
#define MESH_HWMP_PREQ_ORIG_SN_OFFSET        (MAC_INFOELT_INFO_OFT + 13)
/// Offset of the Originator External Address field in the PREQ Information Element
#define MESH_HWMP_PREQ_ORIG_EXT_ADDR_OFFSET  (MAC_INFOELT_INFO_OFT + 17)
/// Offset of the Lifetime field in the PREQ Information Element (if Originator External Address not present)
#define MESH_HWMP_PREQ_LIFETIME_OFFSET       (MAC_INFOELT_INFO_OFT + 17)
/// Offset of the Metric field in the PREQ Information Element (if Originator External Address not present)
#define MESH_HWMP_PREQ_METRIC_OFFSET         (MAC_INFOELT_INFO_OFT + 21)
/// Offset of the Target Count field in the PREQ Information Element (if Originator External Address not present)
#define MESH_HWMP_PREQ_TARGET_CNT_OFFSET     (MAC_INFOELT_INFO_OFT + 25)
/// Offset of the Targets Information in the PREQ Information Element (if Originator External Address not present)
#define MESH_HWMP_PREQ_TARGET_INFO_OFFSET    (MAC_INFOELT_INFO_OFT + 26)
/// Length of Per Target Information in the PREQ Information Element
#define MESH_HWMP_PREQ_TARGET_INFO_LEN       (11)

/// Offset of Per Target Flags fields in the Per Target Information
#define MESH_HWMP_PREQ_TARGET_FLAGS_OFFSET   (0)
/// Offset of Per Target Address fields in the Per Target Information
#define MESH_HWMP_PREQ_TARGET_ADDR_OFFSET    (1)
/// Offset of Per Target HWMP SN fields in the Per Target Information
#define MESH_HWMP_PREQ_TARGET_SN_OFFSET      (7)

/**
 * --------------------------------------------------------------------------------------
 * Path Request (PREQ) Flags field
 * --------------------------------------------------------------------------------------
 */

/// Offset of Gate Announcement subfield in the PREQ Flag field
#define MESH_HWMP_PREQ_FLAG_GATE_ANNCE_OFFSET   (0)
/// Offset of Addressing Mode subfield in the PREQ Flag field
#define MESH_HWMP_PREQ_FLAG_ADDR_MODE_OFFSET    (1)
/// Offset of Proactive PREP subfield in the PREQ Flag field
#define MESH_HWMP_PREQ_FLAG_PRO_PREP_OFFSET     (2)
/// Offset of Address Extension (AE) subfield in the PREQ Flag field
#define MESH_HWMP_PREQ_FLAG_AE_OFFSET           (6)

/**
 * --------------------------------------------------------------------------------------
 * Path Request (PREQ) Per Target Flags field
 * --------------------------------------------------------------------------------------
 */

/// Offset of Target Only subfield in the PREQ Per Target Flag field
#define MESH_HWMP_PREQ_PT_FLAG_TO_OFFSET   (0)

/**
 * --------------------------------------------------------------------------------------
 * Path Reply (PREP) Information Element - IEEE Std 802.11-2012 Section 8.4.2.116
 * --------------------------------------------------------------------------------------
 */

/// Minimum length of information contained in the PREP Information Element
#define MESH_HWMP_PREP_INFO_MIN_LENGTH        (31)
/// Offset of the Flags field in the PREP Information Element
#define MESH_HWMP_PREP_FLAGS_OFFSET           (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Hop Count field in the PREP Information Element
#define MESH_HWMP_PREP_HOP_CNT_OFFSET         (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Element TTL field in the PREP Information Element
#define MESH_HWMP_PREP_ELT_TTL_OFFSET         (MAC_INFOELT_INFO_OFT + 2)
/// Offset of the Target Mesh STA Address field in the PREP Information Element
#define MESH_HWMP_PREP_TARGET_ADDR_OFFSET     (MAC_INFOELT_INFO_OFT + 3)
/// Offset of the Target HWMP Sequence Number field in the PREP Information Element
#define MESH_HWMP_PREP_TARGET_SN_OFFSET       (MAC_INFOELT_INFO_OFT + 9)
/// Offset of the Target External Address field in the PREP Information Element
#define MESH_HWMP_PREP_TARGET_EXT_ADDR_OFFSET (MAC_INFOELT_INFO_OFT + 13)
/// Offset of the Lifetime field in the PREP Information Element (if Target External Address not present)
#define MESH_HWMP_PREP_LIFETIME_OFFSET        (MAC_INFOELT_INFO_OFT + 13)
/// Offset of the Metric field in the PREP Information Element (if Target External Address not present)
#define MESH_HWMP_PREP_METRIC_OFFSET          (MAC_INFOELT_INFO_OFT + 17)
/// Offset of the Originator Mesh STA Address field in the PREP Information Element (if Target External Address not present)
#define MESH_HWMP_PREP_ORIG_ADDR_OFFSET       (MAC_INFOELT_INFO_OFT + 21)
/// Offset of the Originator HWMP Sequence Number in the PREP Information Element (if Target External Address not present)
#define MESH_HWMP_PREP_ORIG_SN_OFFSET         (MAC_INFOELT_INFO_OFT + 27)

/**
 * --------------------------------------------------------------------------------------
 * Path Reply (PREP) Flags field
 * --------------------------------------------------------------------------------------
 */

/// Offset of Address External subfield in the PREP Flag field
#define MESH_HWMP_PREP_FLAG_AE_OFFSET        (6)

/**
 * --------------------------------------------------------------------------------------
 * Path Error (PERR) Information Element - IEEE Std 802.11-2012 Section 8.4.2.117
 * --------------------------------------------------------------------------------------
 */

/// Minimal length of PERR IE information (number of destination = 0)
#define MESH_HWMP_PERR_INFO_MIN_LEN          (2)
/// Offset of the Element TTL field in the PERR Information Element
#define MESH_HWMP_PERR_ELT_TTL_OFFSET        (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Number of Destination field in the PERR Information Element
#define MESH_HWMP_PERR_NB_DEST_OFFSET        (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Destination Information in the PERR Information Element
#define MESH_HWMP_PERR_DEST_INFO_OFFSET      (MAC_INFOELT_INFO_OFT + 2)

/// Minimal length of Destination Information
#define MESH_HWMP_PERR_DEST_INFO_MIN_LEN     (13)
/// Offset of Flags field in the Destination Information
#define MESH_HWMP_PERR_FLAGS_OFFSET          (0)
/// Offset of Destination Address field in the Destination Information
#define MESH_HWMP_PERR_DEST_ADDR_OFFSET      (1)
/// Offset of HWMP Sequence NUmber in the Destination Information
#define MESH_HWMP_PERR_SN_OFFSET             (7)
/// Offset of Reason Code in the Destination Information, if Destination External Address not present
#define MESH_HWMP_PERR_REASON_CODE_OFFSET    (11)

/// AE bit mask for Flags value in PERR Information Element
#define MESH_HWMP_PERR_FLAGS_AE_MASK         (CO_BIT(6))

/**
 * --------------------------------------------------------------------------------------
 * Root Announcement (RANN) Information Element - IEEE Std 802.11-2012 Section 8.4.2.114
 * --------------------------------------------------------------------------------------
 */

/// Length of information contained in the RANN Information Element
#define MESH_HWMP_RANN_INFO_LENGTH          (21)
/// Offset of the Flags field in the RANN Information Element
#define MESH_HWMP_RANN_FLAGS_OFFSET         (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Hop Count field in the RANN Information Element
#define MESH_HWMP_RANN_HOP_CNT_OFFSET       (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Element TTL field in the RANN Information Element
#define MESH_HWMP_RANN_ELT_TTL_OFFSET       (MAC_INFOELT_INFO_OFT + 2)
/// Offset of the Root Mesh STA Address field in the RANN Information Element
#define MESH_HWMP_RANN_ROOT_ADDR_OFFSET     (MAC_INFOELT_INFO_OFT + 3)
/// Offset of the HWMP Sequence Number field in the RANN Information Element
#define MESH_HWMP_RANN_SN_OFFSET            (MAC_INFOELT_INFO_OFT + 9)
/// Offset of the Interval field in the RANN Information Element
#define MESH_HWMP_RANN_INTERVAL_OFFSET      (MAC_INFOELT_INFO_OFT + 13)
/// Offset of the Metric field in the RANN Information Element
#define MESH_HWMP_RANN_METRIC_OFFSET        (MAC_INFOELT_INFO_OFT + 17)

/// Mask for the Gate Announcement bit in the Flags field
#define MESH_HWMP_RANN_FLAGS_GA_MASK        (0x01)

/**
 * --------------------------------------------------------------------------------------
 * Gate Announcement (GANN) Information Element - IEEE Std 802.11-2012 Section 8.4.2.113
 * --------------------------------------------------------------------------------------
 */

/// Length of information contained in the GANN Information Element
#define MESH_HWMP_GANN_INFO_LENGTH          (15)
/// Offset of the Flags field in the GANN Information Element
#define MESH_HWMP_GANN_FLAGS_OFFSET         (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Hop Count field in the GANN Information Element
#define MESH_HWMP_GANN_HOP_CNT_OFFSET       (MAC_INFOELT_INFO_OFT + 1)
/// Offset of the Element TTL field in the GANN Information Element
#define MESH_HWMP_GANN_ELT_TTL_OFFSET       (MAC_INFOELT_INFO_OFT + 2)
/// Offset of the Gate Mesh STA Address field in the GANN Information Element
#define MESH_HWMP_GANN_MESH_ADDR_OFFSET     (MAC_INFOELT_INFO_OFT + 3)
/// Offset of the GANN Sequence Number field in the GANN Information Element
#define MESH_HWMP_GANN_SN_OFFSET            (MAC_INFOELT_INFO_OFT + 9)
/// Offset of the Interval field in the GANN Information Element
#define MESH_HWMP_GANN_INTERVAL_OFFSET      (MAC_INFOELT_INFO_OFT + 13)

/**
 * --------------------------------------------------------------------------------------
 * Mesh Link Metric Report Information Element - IEEE Std 802.11-2012 Section 8.4.2.102
 * --------------------------------------------------------------------------------------
 */

/// Length of information in the Metric Report Information Element - Response case
#define MESH_HWMP_MLMR_RSP_INFO_LENGTH      (5)
/// Offset of the Flags field in the Mesh Link Metric Report Information Element
#define MESH_HWMP_MLMR_FLAGS_OFFSET         (MAC_INFOELT_INFO_OFT + 0)
/// Offset of the Link Metric field in Mesh Link Metric Report Information Element
#define MESH_HWMP_MLMR_METRIC_OFFSET        (MAC_INFOELT_INFO_OFT + 1)

/// Mask allowing to read request bit in flags field of the Mesh Link Metric Report IE
#define MESH_HWMP_MLMR_REQUEST_BIT_MASK     (CO_BIT(0))

/**
 * --------------------------------------------------------------------------------------
 * Path Status Bits
 * --------------------------------------------------------------------------------------
 */

/// Indicate that discovery is currently performed upon host request
#define MESH_HWMP_CREATING_BIT              (CO_BIT(0))
/// Indicate that path is active and can be used
#define MESH_HWMP_ACTIVE_BIT                (CO_BIT(1))
/// Indicate that path information are being updated
#define MESH_HWMP_UPDATING_BIT              (CO_BIT(2))
/// Indicate that path's next hop has been fixed by host and cannot be modified
#define MESH_HWMP_FIXED_BIT                 (CO_BIT(3))
/// Indicate if path target is Root
#define MESH_HWMP_IS_TGT_ROOT_BIT           (CO_BIT(4))
/// Indicate if path target is Gate
#define MESH_HWMP_IS_TGT_GATE_BIT           (CO_BIT(5))

/**
 * --------------------------------------------------------------------------------------
 * Miscellaneous
 * --------------------------------------------------------------------------------------
 */

/*
 * The interval of time (in TUs) that it takes for an HWMP information element to propagate accros the mesh
 * (dot11MeshHWMPnetDiameterTraversalTime)
 */
#define MESH_HWMP_DEFAULT_TRAV_TIME_TU      (500)
/*
 * Interval of time to wait before giving up on path discovery, i.e before sending the
 * MESH_CREATE_PATH_CFM message (in TUs)
 */
#define MESH_HWMP_DEFAULT_MIN_DISC_T0_TU    (2 * MESH_HWMP_DEFAULT_TRAV_TIME_TU)
/// Maximal number of retries for a PREQ we can send to a target
#define MESH_HWMP_DEFAULT_PREQ_MAX_RETRIES  (3)
/// Default path timeout in milliseconds
#define MESH_HWMP_DEFAULT_PATH_TO           (5000)
/**
 * Minimum interval of time during which a mesh STA can send only one PREQ Information
 * Element (in TUs).
 * dot11MeshHWMPpreqMinInterval
 */
#define MESH_HWMP_DEFAULT_PREQ_INTV         (10)
/**
 * Minimum interval of time during whicj a mesh STA can send only one PERR Information
 * Element (in TUs).
 * dot11MeshHWMPperrMinInterval
 */
#define MESH_HWMP_DEFAULT_PERR_INTV         (10)
/*
 * Number of TUs between the periodic transmissions of:
 *      - Root Announcements (dot11MeshHWMPRannInterval)
 *      - Proactive PREQ (dot11MeshHWMProotInterval)
 *      - Gate Announcements
 */
#define MESH_HWMP_DEFAULT_PROACT_INTV_TU    (5000)
/// Default Element TTL valu
#define MESH_HWMP_DEFAULT_ELT_TTL           (4)

/// TODO [LT] - in microseconds
#define MESH_HWMP_METRIC_OVERHEAD           (256)

/// Index of an invalid path
#define MESH_HWMP_INVALID_PATH_IDX          (0xFF)
/// Index of an invalid proxy information structure
#define MESH_HWMP_INVALID_PROXY_IDX         (0xFF)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Internal code identifying a HWMP Path Selection frame
enum mesh_hwmp_frame_type
{
    /// Path Request
    MESH_HWMP_FRAME_PREQ = 0,
    /// Path Reply
    MESH_HWMP_FRAME_PREP,
    /// Path Error
    MESH_HWMP_FRAME_PERR,
    /// Root Announcement
    MESH_HWMP_FRAME_RANN,
};

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Mesh Proxy Information structure
struct mesh_hwmp_proxy_tag
{
    /*
     * List Header
     * (either for free_proxy in struct mesh_hwmp_env_tag or for prox_sta in struct mesh_hwmp_path_tag or
     * for prox_sta in struct mesh_vif_info_tag)
     */
    struct co_list_hdr list_hdr;
    /// Mesh VIF index
    uint8_t mvif_idx;
    /// Path index
    uint8_t path_idx;
    /// MAC Address of the proxied STA
    struct mac_addr sta_mac_addr;
    /// Last time at which a frame has been received from or transmitted to the proxied STA
    uint32_t last_act_time;
};

/// Mesh Path Information structure
struct mesh_hwmp_path_tag
{
    /*
     * List Header
     * (either for free_path list in struct mesh_hwmp_env_tag or used_path in struct mesh_vif_info_tag)
     */
    struct co_list_hdr list_hdr;
    /// List of STA proxied by the target mesh STA
    struct co_list proxy_list;
    /// Path Timeout Timer
    struct mm_timer_tag to_timer;
    /// Air metric to the target
    uint32_t metric;
    /// Target HWMP Sequence Number
    uint32_t tgt_sn;
    /// Target MAC Address
    struct mac_addr tgt_mac_addr;
    /// STA index of the next hop station - can directly be the target
    uint8_t nhop_sta_idx;
    /// Path index
    uint8_t path_idx;
    /// VIF index
    uint8_t vif_idx;
    /// Hop Count towards the target
    uint8_t hop_cnt;
    /// Number of PREQ retries
    uint8_t nb_preq_retries;
    /// Status
    uint8_t status;
};

/// Mesh HWMP global environment structure
struct mesh_hwmp_env_tag
{
    /// List of free path structures
    struct co_list free_path;
    /// List of free proxy structures
    struct co_list free_proxy;
    /// Local HWMP sequence number
    uint32_t local_sn;
    /// Local Path Discovery ID
    uint32_t local_pdisc_id;
};

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Mesh HWMP Global Environment
extern struct mesh_hwmp_env_tag mesh_hwmp_env;
/// Mesh Path Information Pool
extern struct mesh_hwmp_path_tag mesh_hwmp_path_pool[RW_MESH_PATH_NB];
/// Mesh Proxy Information Pool
extern struct mesh_hwmp_proxy_tag mesh_hwmp_proxy_pool[RW_MESH_PROXY_NB];

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Mesh HWMP module
 ****************************************************************************************
 */
void mesh_hwmp_init(void);

/**
 ****************************************************************************************
 * @brief Handle reception of a HWMP Path Selection frame.
 *
 * @param[in] p_param    Received RXU_MGT_IDX message. Contains information about the interface
 *                       and link on which action frame has been received. It also contains
 *                       the full frame that could be forwarded to another STA.
 * @param[in] p_ie_addr  Pointer to the path selection information element (PREQ, PREP, PERR or
 *                       RANN).
 ****************************************************************************************
 */
void mesh_hwmp_handle_frame(struct rxu_mgt_ind *p_param, uint32_t *p_ie_addr);

/**
 ****************************************************************************************
 * @brief Initiate creation of a path between a STA originator and a Mesh STA target.
 *
 * @param[in] p_vif_entry       Information about VIF on which path has to be created
 * @param[in] p_tgt_mac_addr    MAC Address of the Mesh STA defined as path target
 * @param[in] p_orig_mac_addr   MAC Address of the External STA locally proxied and for which
 *                              path has to be created.
 * @param[in] host              Boolean value indicating if path is created upon host request.
 ****************************************************************************************
 */
void mesh_hwmp_path_create(struct vif_info_tag *p_vif_entry, struct mac_addr *p_tgt_mac_addr,
                           struct mac_addr *p_orig_mac_addr, bool host);

/**
 ****************************************************************************************
 * @brief Check if a path to a provided MAC address exist.
 *
 * @param[in] p_vif_entry       Interface on which path has to be found
 * @param[in] p_mac_addr        MAC Address to be looking for
 *
 * @return true if a path to the provided target has been found, else false.
 ****************************************************************************************
 */
bool mesh_hwmp_path_check(struct vif_info_tag *p_vif_entry, struct mac_addr *p_mac_addr,
                          struct umacdesc *p_umac_desc);

/**
 ****************************************************************************************
 * @brief Update or delete the path to a provided target address.
 *
 * @param[in] p_vif_entry       Interface on which the path has been created
 * @param[in] delete            Indicate if the path must be updated or deleted
 * @param[in] p_tgt_mac_addr    Path's target MAC Address
 * @param[in] p_nhop_mac_addr   MAC Address of the next hop STA to be used in order to
 *                              reach the target. The provided address must match with the
 *                              address of a Mesh STA connected on this VIF.
 ****************************************************************************************
 */
void mesh_hwmp_path_update(struct vif_info_tag *p_vif_entry, bool delete,
                           struct mac_addr *p_tgt_mac_addr, struct mac_addr *p_nhop_mac_addr);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
void mesh_hwmp_intv_timer_cb(void *p_env);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
void mesh_hwmp_mvif_update(struct vif_info_tag *p_vif_entry, bool ready);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
void mesh_hwmp_sta_del(struct mesh_vif_info_tag *p_mvif_entry, uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Check MAC Header on a received Mesh Data frame in order to detect if the received
 * frame is for the provided VIF or for an external STA that would be proxied by this VIF, or
 * if the frame has to be forwarded to another Mesh STA.
 *
 * @param[in] p_vif_entry       Interface on which the frame has been received
 * @param[in] sta_idx           Index of Mesh STA originator of the frame
 * @param[in] a_frame           Address of received frame's MAC Header in our memory
 * @param[in|out] p_rx_status   RX status structure provided to the host. If the frame has
 *                              to be forwarded the dest_idx value will be set to the index
 *                              of the next hop STA, else dest_idx value is INVALID_STA_IDX.
 *
 * @return CO_OK if frame can be handled or CO_FAIL if frame has to be discarded.
 ****************************************************************************************
 */
bool mesh_hwmp_check_data_dest(struct vif_info_tag *p_vif_entry, uint8_t sta_idx, uint32_t a_frame,
                               struct rx_cntrl_rx_status *p_rx_status);

/**
 ****************************************************************************************
 * @brief Add a proxy entry for a newly discovered external STA.
 *
 * @param[in] p_mvif_entry          Mesh VIF on which proxied device has been discovered.
 * @param[in] p_mpath_entry         Path to the Mesh STA to be addressed in order to reach
 *                                  the external STA. If NULL, the provided interface is a
 *                                  proxy for the provided external STA.
 * @param[in] p_sta_mac_addr        MAC Address of the proxied external STA
 ****************************************************************************************
 */
void mesh_hwmp_add_proxy_info(struct mesh_vif_info_tag *p_mvif_entry, struct mesh_hwmp_path_tag *p_mpath_entry,
                              struct mac_addr *p_sta_mac_addr);

/**
 ****************************************************************************************
 * @brief Look for a proxy entry for a provided MAC Address.
 *
 * @param[in] p_mac_addr        MAC Address of the external STA to be found
 * @param[in] local             Indicate if the searched external STA is locally proxied
 *                              or if it is proxied by a known Mesh STA
 *
 * @return NULL if no proxy entry has been found, else the mesh_hwmp_proxy_tag structure
 * containing all the information allowing to reach the required STA.
 ****************************************************************************************
 */
struct mesh_hwmp_proxy_tag *mesh_hwmp_get_proxy_info(struct mac_addr *p_mac_addr, bool local);

#endif //(RW_MESH_EN)

/// @}

#endif // _MESH_HWMP_H_
