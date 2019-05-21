/**
 ****************************************************************************************
 *
 * @file bfr.h
 *
 * @brief Beamformer Module Definitions.
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 ****************************************************************************************
 */

#ifndef _BFR_H_
#define _BFR_H_

/**
 ****************************************************************************************
 * @defgroup BFR BFR
 * @ingroup LMAC
 * @brief Beamformer Implementation (LMAC Part).
 * @{
 ****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

/// RW Stack Configuration
#include "rwnx_config.h"

#if (RW_BFMER_EN)

/// Linked List Library
#include "co_list.h"
/// MAC Frame Definition
#include "mac_frame.h"
/// RX Software Descriptor Definition
#include "rx_swdesc.h"
/// For DMA Descriptor Definition
#include "dma.h"
/// For TX Descriptor Definition
#include "tx_swdesc.h"
/// For VIF Definition
#include "vif_mgmt.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/**
 * --------------------------------------------------------------------------------------
 * BFR Memory Configuration
 * --------------------------------------------------------------------------------------
 */

/**
 * Maximal number of beamforming reports that can be downloaded in BFR Memory.
 * Shall be a multiple of 2.
 */
#define BFR_MEM_REP_NB_MAX      (6)
/// Base address of BFR Memory in HW memory (FPGA B)
#define BFR_MEM_BASE_ADDR       (0x60C05000)
//#define BFR_MEM_BASE_ADDR       (0x10E00000)
/// Size of section in BFR Memory containing pointer to reports (in bytes)
#define BFM_MEM_PTR_SIZE        (BFR_MEM_REP_NB_MAX * 2)
/// Start Address of remaining BFR Memory (Offset from BFR_MEM_BASE_ADDR)
#define BFR_MEM_START_ADDR      (BFM_MEM_PTR_SIZE)
/// Number of memory nodes that can be used to describe state of BFR Memory
#define BFR_MEM_NODE_NB         (BFR_MEM_REP_NB_MAX)

/**
 * --------------------------------------------------------------------------------------
 * Frame Information for NPDA
 * --------------------------------------------------------------------------------------
 */

/**
 * Length of NDP Announcement Frame sent during SU Calibration (in bytes).
 * See 802.11ac-2013, section 8.3.1.20
 */
#define BFR_NDPA_SU_LEN         (MAC_LONG_CTRLFRAME_LEN + 3)
/// Length of a STA Information field
#define BFR_NDPA_STA_INFO_LEN   (2)
/// Maximal sounding dialog token value (63)
#define BFR_NDPA_TOKEN_MAX      (0x3F)
/// Position of Sounding Dialog Token Number in Sounding Dialog Token field
#define BFR_NPDA_TOKEN_POS      (2)
/// Mask allowing to get/set Dialog Token value
#define BFR_NDPA_TOKEN_MASK     (BFR_NDPA_TOKEN_MAX << BFR_NPDA_TOKEN_POS)
/// Position of AID12 field in STA Info field
#define BFR_NPDA_AID12_POS      (0)
/// Mask allowing to get/set AID12 value
#define BFR_NDPA_AID12_MASK     (0xFFF)
/// Position of Feedback Type field in STA Info field
#define BFR_NPDA_FBACK_POS      (12)
/// Mask allowing to get/set Feedback Type value
#define BFR_NDPA_FBACK_MASK     (0x1000)
/// Position of NC Index field in STA Info field
#define BFR_NPDA_NC_IDX_POS     (13)
/// Mask allowing to get/set NC Index value
#define BFR_NDPA_NC_IDX_MASK    (0xFE00)

/// VHT NDP Announcement frame format (see 802.11ac-2013, section 8.3.1.20)
/// Offset of Dialog Token field (from beginning of frame)
#define BFR_NDPA_DIALOG_TOKEN_OFFSET        (16)
/// Offset of first STA Info field (from beginning of frame)
#define BFR_NDPA_STA_INFO_OFFSET            (17)
/// Length of STA Info field
#define BFR_NDPA_STA_INFO_LENGTH            (2)

/**
 * --------------------------------------------------------------------------------------
 * Frame Information for BRP (Beamforming Report Poll)
 * --------------------------------------------------------------------------------------
 */

/**
 * Length of Beamforming Report Poll Frame (in bytes).
 * See 802.11ac-2013, section 8.3.1.21
 */
#define BFR_BRP_LEN             (MAC_LONG_CTRLFRAME_LEN + 1)

/// Beamforming Report Poll frame format (see 802.11ac-2013, section 8.3.1.21)
/// Offset of Retransmission Bitmap field
#define BFR_BRP_RETX_BITMAP_OFFSET          (16)

/**
 * --------------------------------------------------------------------------------------
 * Frame Information for VHT MIMO Control field
 * --------------------------------------------------------------------------------------
 */

/// Offset of NC Index field
#define BFR_MIMO_CTRL_NC_OFFSET         (0)
/// Offset of NR Index field
#define BFR_MIMO_CTRL_NR_OFFSET         (3)
/// Offset of Channel Width field
#define BFR_MIMO_CTRL_CHAN_WIDTH_OFFSET (6)
/// Offset of Grouping field
#define BFR_MIMO_CTRL_GROUPING_OFFSET   (8)
/// Offset of Codebook Information field
#define BFR_MIMO_CTRL_CODEBOOK_OFFSET   (10)
/// Offset of Feedback Type field
#define BFR_MIMO_CTRL_FEEDBACK_OFFSET   (11)
/// Offset of Remaining Feedback Segments field
#define BFR_MIMO_CTRL_REM_SEGM_OFFSET   (12)
/// Offset of First Feedback Segment field
#define BFR_MIMO_CTRL_FIRST_SEGM_OFFSET (15)
/// Offset of Reserved Bits
#define BFR_MIMO_CTRL_RESERVED_OFFSET   (16)
/// Offset of Sounding Dialog Token Number field
#define BFR_MIMO_CTRL_TOKEN_OFFSET      (18)

/// Non-shifted mask for NC Index field
#define BFR_MIMO_CTRL_NC_MASK           (0x7)
/// Non-shifted mask for NR Index field
#define BFR_MIMO_CTRL_NR_MASK           (0x7)
/// Non-shifted mask for Channel Width field
#define BFR_MIMO_CTRL_CHAN_WIDTH_MASK   (0x3)
/// Non-shifted mask for Grouping field
#define BFR_MIMO_CTRL_GROUPING_MASK     (0x3)
/// Non-shifted mask for Codebook Information field
#define BFR_MIMO_CTRL_CODEBOOK_MASK     (0x1)
/// Non-shifted mask for Feedback Type field
#define BFR_MIMO_CTRL_FEEDBACK_MASK     (0x1)
/// Non-shifted mask for Remaining Feedback Segments field
#define BFR_MIMO_CTRL_REM_SEGM_MASK     (0x7)
/// Non-shifted mask for First Feedback Segment field
#define BFR_MIMO_CTRL_FIRST_SEGM_MASK   (0x1)
/// Non-shifted mask for Sounding Dialog Token Number field
#define BFR_MIMO_CTRL_TOKEN_MASK        (0x3F)

/**
 * --------------------------------------------------------------------------------------
 * Miscellaneous
 * --------------------------------------------------------------------------------------
 */

/// Delay parameter used to know if SU calibration has to be started due to traffic (in us)
#define BFR_SU_CALIB_MIN_DELAY          (10000)
/// Maximum time between two SU calibrations for a STA (in us)
#define BFR_SU_CALIB_MAX_DELAY          (500000)
/// Invalid SMM Index (no report can be used for the transmission)
#define BFR_INVALID_SMM_INDEX           (0xFF)

/// VHT Compressed Beamforming frame format (see 802.11ac-2013, section 8.5.23.2)
/// Offset of Category field
#define BFR_VHT_BFR_CATEGORY_OFFSET         (24)
/// Offset of VHT Action field
#define BFR_VHT_BFR_VHT_ACTION_OFFSET       (25)
/// Offset of VHT MIMO Control field
#define BFR_VHT_BFR_VHT_MIMO_CTRL_OFFSET    (26)
/// Offset of Report
#define BFR_VHT_REPORT_OFFSET               (29)
/// Invalid NC value
#define BFR_INVALID_NC                      (0xFF)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// RX Upload status returned in bfr_rx_frame_ind function
enum bfr_rx_status
{
    /// Frame is not a VHT Compressed Beamforming Frame or peer STA is not beamformee capable
    BFR_RX_STATUS_NOT_VALID = 0,
    /// Frame is expected and we can handle upload of the report in the host memory
    BFR_RX_STATUS_VALID,
    /// Frame was expected but not enough DMA descriptors have been found for the upload
    BFR_RX_STATUS_ERROR,
};

/// Feedback Type values
enum bfr_fback_type_val
{
    /// SU Feedback
    BFR_SU_TYPE = 0,
    /// MU Feedback
    BFR_MU_TYPE,
};

/// BFR Environment status values
enum bfr_status
{
    /// BFR Module is IDLE
    BFR_STATUS_IDLE = 0,
    /// SU Calibration is in progress
    BFR_STATUS_SU_CALIB,
    /// MU Calibration is in progress
    BFR_STATUS_MU_CALIB,
};

/// BFR STA Information Status Bit Positions
enum bfr_sta_info_status
{
    /// Peer STA is MU beamforming capable
    BFR_STA_MU_CAPABLE = 0,
    /// A valid report has been uploaded in host memory
    BFR_STA_HAS_VALID_REPORT,
    /// Report is being downloaded from host to phy memory for this STA
    BFR_STA_DOWNLOAD,
    /// Calibration in progress for this STA
    BFR_STA_CALIBRATE,
    /// Report download from host memory not permitted due to a calibration
    BFR_STA_NO_DOWNLOAD = BFR_STA_CALIBRATE,
    /// Report has been updated in host memory
    BFR_STA_REPORT_UPDATED,
};

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure representing a piece of BFR memory
struct bfr_mem_node
{
    /// Double Linked List Header
    struct co_list_hdr hdr;
    /// Start Address
    uint16_t start_addr;
    /// Length
    uint16_t length;
    /// Node Index
    uint8_t idx;
    /// Number of frames linked with this report and pending for transmission
    uint8_t nb_frames;
    /// SMM Index used for this node
    uint8_t smm_index;
    /// Index of the STA using this node
    uint8_t sta_idx;
};

/// Structure representing the environment used to manage all transmissions as a beamformer
struct bfr_env_tag
{
    /// List of available memory nodes
    struct co_list free_nodes;
    /// List of used memory nodes sorted by increasing address
    struct co_list nodes;
    /// List of handled RX Descriptors
    struct co_list rx_descs;
    /// List of calibrations to be executed
    struct co_list calib_sched;
    /// End address of BFR Memory
    uint16_t bfr_mem_eaddr;
    /// Number of expected reports during MU calibration
    uint8_t nb_reports;
    /// BFR Status
    uint8_t status;
    /// Current Dialog Token
    uint8_t token;
    /// Number of spatial streams (Nr Index = nss - 1)
    uint8_t bfer_nss;
    /// Number of memory node currently used (i.e with nb_frames value higher than 0)
    uint8_t nb_used_nodes;
    /// Bandwidth used for the current calibration
    uint8_t calib_bw;
    /// Indicate if Beamformer is enabled => means supported at MAC and PHY level
    bool bfr_enabled;
};

/// Structure containing required information for BFR transmission to a given STA
struct bfr_info_sta
{
    /// List header for calibration scheduling
    struct co_list_hdr hdr;
    /// Pointer to the memory node describing report position in phy memory
    struct bfr_mem_node *p_node;
    /// Last SU calibration time
    uint32_t last_su_calib_time;
    /// Address of BFR Report in Host Memory
    uint32_t host_report_addr;
    /// Length of BFR Report in Host Memory (in bytes)
    uint16_t host_report_len;
    /// STA Info field value to insert in NDPA frame
    uint16_t sta_info;
    /// Last token used for calibration
    uint8_t last_token;
    /// Status bit field (see enum bfr_sta_info_status)
    uint8_t status;
    /// Number of feedback segments whose upload completion has still to be confirmed
    uint8_t nb_segments;
    /// STA Index - INVALID_STA_IDX if no connection or if peer STA is not beamformee capable
    uint8_t sta_idx;
    /// VIF Index
    uint8_t vif_idx;
    /// Number of spatial stream the peer STA can receive (=> Nc Index)
    uint8_t bfee_nss;
    /// Last received Nc
    uint8_t last_rx_nc;
};

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Beamformer common environment
extern struct bfr_env_tag bfr_env;

/// BFR Information for each STA
extern struct bfr_info_sta bfr_sta_infos[NX_REMOTE_STA_MAX];


/**
 * INLINED FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Return if Beamformer role can be used based on HW capabilities.
 ****************************************************************************************
 */
__INLINE bool bfr_is_enabled(void)
{
    return (bfr_env.bfr_enabled);
}

/**
 ****************************************************************************************
 * @brief Return if a given station has been successfully calibrated.
 ****************************************************************************************
 */
__INLINE bool bfr_is_calibrated(uint8_t sta_idx)
{
    // Get BFR information for this STA
    struct bfr_info_sta *p_bfr_info = &bfr_sta_infos[sta_idx];

    // Check if STA has a valid report available
    return ((p_bfr_info->status & CO_BIT(BFR_STA_HAS_VALID_REPORT)) != 0);
}

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize stack for support of TX Beamforming
 ****************************************************************************************
 */
void bfr_init(void);

/**
 ****************************************************************************************
 * @brief Configure the Beamformer environment for a STA that is at least SU Beamformee
 * capable (this check has to be done before call of this function).
 * Once the link with the STA is disconnected, bfr_del_sta_ind function should be called
 * in order to clean the environment.
 * @param[in] sta_idx       STA Index
 * @param[in] mu_capable    Indicate if peer STA is MU capable
 * @param[in] aid           Association ID (relevant only if VIF type is AP)
 * @param[in] host_addr     Address in host memory at which received beamforming reports
 *                          can be uploaded using a DMA transfer.
 * @param[in] nss           Maximum number of spatial stream supported by the STA
 ****************************************************************************************
 */
void bfr_add_sta_ind(uint8_t sta_idx, bool mu_capable, uint16_t aid, uint32_t host_addr, uint8_t nss);

/**
 ****************************************************************************************
 * @brief Clean the Beamformer environment previously allocated for a given STA. This
 * function is supposed to be called once the link with the STA has been disconnected.
 * @param[in] sta_idx       STA Index
 ****************************************************************************************
 */
void bfr_del_sta_ind(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Inform BFR module about reception of a frame. This function will check if a
 * calibration was in progress with the STA, if the received frame is well the expected
 * beamforming report.
 * In that case, the received report will be uploaded in host memory using a DMA transfer.
 * Note that this function could be called several times for a report as the report can be
 * splited into several segment, hence several MPDU composing a same AMPDU.
 * @param[in] sta_idx       Index of the STA for which report has been received
 * @param[in] p_swdesc      RX Descriptor containing all the details about the received
 *                          frame
 * @param[in] p_frame       Frame data
 * @return A status indicating if the frame content has been uploaded in host memory or
 *         not (see enum bfr_rx_status).
 *              -> If status is BFR_RX_STATUS_NOT_VALID, packet is not a VHT Compressed
 *                 Report and can be forwarded to the host
 *              -> If status is BFR_RX_STATUS_VALID, the frame is an expected VHT Compressed
 *                 Report and the report is being uploaded in host memory. The BFR module is
 *                 in charge of the SW descriptor.
 *              -> If status is BFR_RX_STATUS_ERROR, the frame is a VHT Compressed Report but
 *                 the report was not expected or an error has been raised during the DMA
 *                 transfer preparation.
 ****************************************************************************************
 */
uint8_t bfr_rx_frame_ind(uint8_t sta_idx, struct rx_swdesc *p_swdesc, uint8_t *p_frame);

/**
 ****************************************************************************************
 * @brief This function is supposed to be called before each transmission of the first
 * MPDU of an A-MPDU or a singleton MPDU to a STA that is at least SU Beamformee capable.
 * Its purpose is to check if a Beamforming report is already available in the phy memory.
 * If the report has already been downloaded, the known smm index is returned.
 * Else if a report is available in the host memory and can be inserted in the phy memory,
 * start the download procedure.
 * In any other cases the returned smm index will be BFR_INVALID_SMM_INDEX.
 * @param[in] p_txdesc        TX Descriptor containing information about the frame to transmit
 * @param[in] p_dma_desc_bfr  DMA descriptor to be fulfilled in case download of a report
 *                            from the host memory to the phy memory is required.
 * @param[in] pkt_idx         Packet index for the case where transmission of AMSDUs is supported
 * @return True if a report download has been prepared, else False
 ****************************************************************************************
 */
bool bfr_tx_frame_ind(struct txdesc *p_txdesc, struct dma_desc *p_dma_desc_bfr, uint8_t pkt_idx);

/**
 ****************************************************************************************
 * @brief This function is aimed to be called once the payload of a beamformed frame has
 * been download through DMA. It is in charge of updating the the policy table:
 *      - Resetting the smoothing bit.
 *      - Setting the beamformed bit and the SMM index.
 * @param[in] p_txdesc  TX Descriptor containing information about the frame to transmit
 ****************************************************************************************
 */
void bfr_tx_configure(struct txdesc *p_txdesc);

/**
 ****************************************************************************************
 * @brief Handle confirmation of transmission of a beamformed frame.
 * Note that this function has to be called ONLY if the frame was programmed as a beamformed
 * frame.
 * It is in charge of decrementing the nb_frames counter for the report used for the
 * beamforming.
 * @param[in] p_txdesc  TX Descriptor containing information about the frame to transmit
 ****************************************************************************************
 */
void bfr_tx_cfm(struct txdesc *p_txdesc);

/**
 ****************************************************************************************
 * @brief Initiate start of a new calibration for a given VIF. This function is in charge
 * of grouping the STA that are MU capable.
 * @param[in] p_vif_entry       VIF Entry information
 ****************************************************************************************
 */
void bfr_calibrate(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Return the latest Nc value received in a Beamforming report for a given beamformee.
 *
 * @param[in] sta_idx     STA Index of the Beamformee device
 ****************************************************************************************
 */
uint8_t bfr_get_last_nc(uint8_t sta_idx);

#if RW_MUMIMO_TX_EN
/**
 ****************************************************************************************
 * @brief Handle group update request sent by the upper layers
 *
 * @param[in] param   Pointer to the group update message parameters
 ****************************************************************************************
 */
void bfr_group_update_req(struct mm_mu_group_update_req const *param);
#endif //RW_MUMIMO_TX_EN

#endif //(RW_BFMER_EN)

/// @}

#endif // _BFR_H_
