/**
 ****************************************************************************************
 *
 * @file rxu_cntrl.h
 *
 * @brief Conatins Rx Module's structures, constants, and helper function declarations
 *
 * This header file will contain the definitions of the structures and constants
 * used only by the Rx, and the Rx module's Helper function declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _RXU_CNTRL_H_
#define _RXU_CNTRL_H_

/** @defgroup UMACRX RXU
 * @ingroup UMAC
 * @brief Structure definitions and Helper function declarations of the UMAC's Rx Module.
 */

/** @addtogroup UMACRX
 * @{
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwnx_config.h"
#include "co_int.h"
#include "co_bool.h"
#include "co_status.h"
#include "mac.h"
#include "mac_frame.h"
#include "mm_timer.h"
#include "me_mic.h"
#include "rx_swdesc.h"

//#define RXU_DEBUG

#ifdef RXU_DEBUG
#define RXU_PRT                 os_printf
#define RXU_WPRT                warning_prf
#else
#define RXU_PRT                  os_null_printf
#define RXU_WPRT                 os_null_printf
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
	
	/// Ethernet Protocol IDs
	/// IPX over DIX
#define RX_ETH_PROT_ID_IPX          (0x8137)
	/// Appletalk AARP
#define RX_ETH_PROT_ID_AARP         (0x80F3)
	
	/// Ethernet Header Length (2 MAC Addresses (6 bytes each) + Len/Type Field (2 bytes))
#define RX_CNTRL_ETH_HDR_LEN        (14)
	/// LLC/SNAP length
#define RX_CNTRL_ETH_LLC_SNAP_LEN   (8)
	
	/*
	 * MACROS
	 ****************************************************************************************
	 */
	
	/// Indicate if the received packet is a Data packet by reading the Frame Control Field
#define RXL_CNTRL_IS_DATA(frame_cntrl)                                                   \
			   ((frame_cntrl & MAC_FCTRL_TYPE_MASK) == MAC_FCTRL_DATA_T)
	/// Indicate if the received packet is a QOS Data packet by reading the Frame Control Field
#define RXL_CNTRL_IS_QOS_DATA(frame_cntrl)                                               \
			   ((frame_cntrl & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_QOS_DATA)
	/// Indicate if the received packet is a Management packet by reading the Frame Control Field
#define RXL_CNTRL_IS_MGT(frame_cntrl)                                                    \
			   ((frame_cntrl & MAC_FCTRL_TYPE_MASK) == MAC_FCTRL_MGT_T)
	/// Indicate if the received packet is a Control packet by reading the Frame Control Field
#define RXL_CNTRL_IS_CTRL(frame_cntrl)                                                   \
			   ((frame_cntrl & MAC_FCTRL_TYPE_MASK) == MAC_FCTRL_CTRL_T)
	
	/// Indicate if the received frame was encrypted (Protected Bit set to 1)
#define RXL_CNTRL_IS_PROTECTED(frame_cntrl)                                               \
			   ((frame_cntrl & MAC_FCTRL_PROTECTEDFRAME) == MAC_FCTRL_PROTECTEDFRAME)
	

	
enum
{
	CIPHER_OPEN_SYSTEM = 0,
	CIPHER_WEP,
	CIPHER_TKIP,
	CIPHER_CCMP,
	CIPHER_TKIP_COMPATIBILITY
};

/// Size of the pool containing Reassembly structure
#define RX_CNTRL_DEFRAG_POOL_SIZE       (3)
/// Maximum time we can wait for a fragment (in us)
#define RX_CNTRL_DEFRAG_MAX_WAIT        (100000)

#if (NX_REORD)
/// Size of the pool containing reordering structure
#define RX_CNTRL_REORD_POOL_SIZE        (NX_REORD)
/// Size of the windows used in reordering operations
#define RX_CNTRL_REORD_WIN_SIZE         (NX_REORD_BUF_SIZE)
/// Maximum time we can wait for an SN (in us)
#define RX_CNTRL_REORD_MAX_WAIT         (50000)
#endif //(NX_REORD)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

enum rx_status_bits
{
    /// The buffer can be forwarded to the networking stack
    RX_STAT_FORWARD = CO_BIT(0),
    /// A new buffer has to be allocated
    RX_STAT_ALLOC = CO_BIT(1),
    /// The buffer has to be deleted
    RX_STAT_DELETE = CO_BIT(2),
    /// The length of the buffer has to be updated
    RX_STAT_LEN_UPDATE = CO_BIT(3),
    /// The length in the Ethernet header has to be updated
    RX_STAT_ETH_LEN_UPDATE = CO_BIT(4),
    /// Simple copy
    RX_STAT_COPY = CO_BIT(5),
};

/// Packet contains an A-MSDU
#define RX_FLAGS_IS_AMSDU_BIT         CO_BIT(0)
/// Packet contains a 802.11 MPDU
#define RX_FLAGS_IS_MPDU_BIT          CO_BIT(1)
/// Packet contains 4 addresses
#define RX_FLAGS_4_ADDR_BIT           CO_BIT(2)
/// Packet is a Mesh Beacon received from an unknown Mesh STA
#define RX_FLAGS_NEW_MESH_PEER_BIT    CO_BIT(3)
/// Offset of the VIF index field
#define RX_FLAGS_VIF_INDEX_OFT  8
/// Mask of the VIF index field
#define RX_FLAGS_VIF_INDEX_MSK  (0xFF << RX_FLAGS_VIF_INDEX_OFT)
/// Offset of the STA index field
#define RX_FLAGS_STA_INDEX_OFT  16
/// Mask of the STA index field
#define RX_FLAGS_STA_INDEX_MSK  (0xFF << RX_FLAGS_STA_INDEX_OFT)
/// Offset of the destination STA index field
#define RX_FLAGS_DST_INDEX_OFT  24
/// Mask of the destination STA index field
#define RX_FLAGS_DST_INDEX_MSK  (0xFF << RX_FLAGS_DST_INDEX_OFT)

// Bit field positions for frame_info value (struct rxu_cntrl_rx_status)
enum rxu_cntrl_frame_info_pos
{
    /// Received frame was encrypted using TKIP method, so a MIC check is required
    RXU_CNTRL_MIC_CHECK_NEEDED = CO_BIT(0),
    /// Received frame has to pass the PN replay check
    RXU_CNTRL_PN_CHECK_NEEDED = CO_BIT(1),
    /// Received frame is a Mesh Beacon received from an unknown Mesh STA
    RXU_CNTRL_NEW_MESH_PEER   = CO_BIT(2),
};

enum rxu_cntrl_defrag_status
{
    RXU_CNTRL_DEFRAG_FIRST_FRAG     = 0,
    RXU_CNTRL_DEFRAG_FRAG,
    RXU_CNTRL_DEFRAG_NOT_FRAG,
    RXU_CNTRL_DEFRAG_REJECT,

    RXU_CNTRL_DEFRAG_MAX
};


/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
// forward declarations
struct rx_swdesc;
struct rx_dmadesc;
struct vif_info_tag;

/// Structure used for RX MIC checking
struct rxu_mic_calc
{
    /// MIC computation structure
    struct mic_calc mic_calc;
    /// Last 8 bytes of fragment (used in Reassembly procedure only)
    uint32_t last_bytes[2];
};


/// Structure used during reassembly
struct rxu_cntrl_defrag
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;

    /// Indicate if all fragments have been received
    bool all_rcv;
    /// Indicate if Ethernet Header contains the length field
    bool eth_len_present;
    /// Station Index
    uint8_t sta_idx;
    /// Traffic Index
    uint8_t tid;
    /// Next Expected FN
    uint8_t next_fn;
    /// Fragment MAC Header length
    uint8_t mac_hdr_len;
    /// Sequence Number
    uint16_t sn;
    /// Length to be copied using DMA
    uint16_t cpy_len;
    /// Total Frame Length
    uint16_t frame_len;
    /// DMA address of the packet
    uint32_t dma_addr;
    /// Host buffer address of the first fragment
    uint32_t host_id;
    /// Last time of activity
    uint32_t time;
    /// MIC computation structure
    struct rxu_mic_calc mic;
    /// MM timer structure used for flushing the fragments if the packet is not complete
    struct mm_timer_tag timer;
};

#if (NX_REORD)
/// Structure describing an element of the RX reordering table
struct rxu_cntrl_reord_elt
{
    /// Packet number of the received packet (used for replay check)
    uint64_t pn;
    /// Host Buffer Address
    uint32_t host_id;
    /// flag indicating if the PN must be verified
    bool pn_check;
};

/// RX reordering environment declaration
struct rxu_cntrl_reord
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;

    /// Time at which last waited SN has been received
    uint32_t sn_rx_time;
    /// Pointer to the key structure used for PN check
    struct key_info_tag *key;
    /**
     * Lowest expected to be received (from 0 to 4096)
     *     = WinStartB IEEE Std 802.11-2012 section 9.21.7.6.1
     */
    uint16_t win_start;
    // Current position in the RX packet status array
    uint8_t rx_status_pos;
    // Number of packet received out of order and waiting for next sn
    uint8_t ooo_pkt_cnt;
    /// TID of the reordering structure
    uint8_t tid;
    // Reordering buffer elements
    struct rxu_cntrl_reord_elt elt[RX_CNTRL_REORD_WIN_SIZE];
    /// MM timer structure used for flushing the reordering buffer
    struct mm_timer_tag timer;
};
#endif //(NX_REORD)

/// Structure in which we store much information about the current packet
struct rx_cntrl_rx_status
{
    /// Frame Control field
    uint16_t frame_cntl;
    /// Sequence control field
    uint16_t seq_cntl;
    #if (RW_MESH_EN)
    /// Address of QoS Control field
    uint32_t a_qos_ctrl;
    #endif //(RW_MESH_EN)
    /// Sequence Number
    uint16_t sn;
    /// Fragment Number
    uint8_t fn;
    /// Traffic Index
    uint8_t tid;
    /// MAC Header Length
    uint8_t machdr_len;
    #if (RW_MESH_EN)
    /// Mesh Control Length
    uint8_t mesh_ctrl_len;
    #endif //(RW_MESH_EN)
    /// Index of the sending STA, if known
    uint8_t sta_idx;
    /// Index of the receiving VIF, if known
    uint8_t vif_idx;
    /// Index of the destination STA, if known (AP mode only)
    uint8_t dst_idx;
    /// Packet number (used for replay check in TKIP, CCMP and WAPI)
    uint64_t pn;
    /// Status information field indicated by the HW
    uint32_t statinfo;
    /// Address of the host buffer
    uint32_t host_buf_addr;
    /// Pointer to the key structure that will be used for PN replay and MIC check
    struct key_info_tag *key;
    /// Destination Address and Source Address
    struct mac_addr da;
    /// Source Address
    struct mac_addr sa;
    /// Internal information about the received frame
    uint8_t frame_info;
    /// Indicate if Ethernet Header contains the length field
    bool eth_len_present;
    /// Payload offset in the frame after MAC header conversion
    uint8_t payl_offset;
};

struct rx_cntrl_ipcdesc
{
    /// Host Buffer Address
    uint32_t host_id;
};

/// Structure used for Duplicate Filtering of packet received on unregistered STA
struct rx_cntrl_dupli
{
    /// Last received source address
    struct mac_addr last_src_addr;
    /// Last received sequence control value
    uint16_t last_seq_cntl;
};

/// Structure used for Duplicate Filtering of packet received on unregistered STA
struct rx_cntrl_pm_mon
{
    /// Source address that we monitor
    struct mac_addr addr;
    /// PM state of the device we monitor
    uint8_t pm_state;
    /// Flag indicating if the monitoring is enabled or not
    bool mon;
};


/// The context structure of the UMAC's Rx module
struct rxu_cntrl_env_tag
{
    struct rx_cntrl_rx_status rx_status;

    /// List of RX Descriptor elements that are currently under DMA transfer
    struct co_list rxdesc_pending;
    /// List of RX Descriptor elements that are ready for DMA transfer
    struct co_list rxdesc_ready;
    /// Information about currently handled packet
    struct rx_cntrl_ipcdesc rx_ipcdesc_stat;
    /// Index of the next hal_host_rxdesc structure to use
    uint8_t rxdesc_idx;

    #if (NX_REORD)
    /// List of free RX Reordering structures
    struct co_list rxu_reord_free;
    #endif //(NX_REORD)

    /// List of free Reassembly structures
    struct co_list rxu_defrag_free;
    /// List of used Reassembly structures
    struct co_list rxu_defrag_used;
    /// Unknown STA duplicate filtering information
    struct rx_cntrl_dupli rxu_dupli;
    /// Pointer to this stations MAC Address
    struct mac_addr *mac_addr_ptr;
    /// PM bit monitoring
    struct rx_cntrl_pm_mon pm_mon;
    /// Time to run parameter used.
    uint32_t ttr;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct rxu_cntrl_env_tag rxu_cntrl_env;

#if (NX_REORD)
/// Pool of reordering structures
extern struct rxu_cntrl_reord rxu_cntrl_reord_pool[RX_CNTRL_REORD_POOL_SIZE];
#endif //(NX_REORD)

/// Pool of Reassembly structures
extern struct rxu_cntrl_defrag rxu_cntrl_defrag_pool[RX_CNTRL_DEFRAG_POOL_SIZE];

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Upper MAC RX frame handler.
 * This function is responsible for executing the different procedures required upon a
 * frame reception (reassembly, reordering, duplicate filtering, management frame routing
 * , MAC to ethernet header conversion, etc.).
 *
 * @param[in] A pointer to the current RX descriptor
 *
 * @return true if the frame needs to be uploaded to the host memory, false otherwise
 *
 ****************************************************************************************
 */
bool rxu_cntrl_frame_handle(struct rx_swdesc* swdesc);

#if (NX_REORD)
struct sta_info_tag;
/**
 ****************************************************************************************
 * @brief Allocate and initialize a reordering structure upon a new RX BlockAck agreement
 * creation.
 *
 * @param[in] sta Pointer to the STA entry involved in this agreement
 * @param[in] tid TID of the agreement
 * @param[in] ssn Sequence number which the block ack window has to be initialized with
 *
 * @return true if the reordering structure was correctly allocated, false otherwise
 ****************************************************************************************
 */
bool rxu_cntrl_reord_create(struct sta_info_tag *sta, uint8_t tid, uint16_t ssn);

/**
 ****************************************************************************************
 * @brief Delete a reordering structure previously allocated.
 *
 * @param[in] sta Pointer to the STA entry involved in this agreement
 * @param[in] tid TID of the agreement
 ****************************************************************************************
 */
void rxu_cntrl_reord_delete(struct sta_info_tag *sta, uint8_t tid);

#endif //(NX_REORD)

/**
 ****************************************************************************************
 * @brief initializes the rx_context.
 *
 * This function is called when initializing the UMAC's Rx Context at system
 * start up or at system reset
 ****************************************************************************************
 */
void rxu_cntrl_init(void);

/**
 ****************************************************************************************
 * @brief Start the PM bit monitoring on incoming frames for which the TA is unknown.
 * The monitoring has to be started when the association response has been transmitted,
 * and will end up when the upper layers have added the new STA to the FW.
 *
 * @param[in] addr Pointer to the MAC address that we need to monitor.
 ****************************************************************************************
 */
void rxu_cntrl_monitor_pm(struct mac_addr *addr);

/**
 ****************************************************************************************
 * @brief This function returns the PM state of the station for which the PM monitoring
 * was started. If no packets were received from this device, then the station is
 * considered as active.
 *
 * @return The PM state of the station (@ref PS_MODE_OFF for active and @ref PS_MODE_ON
 * for passive)
 ****************************************************************************************
 */
uint8_t rxu_cntrl_get_pm(void);

/**
 ****************************************************************************************
 * @brief This function is a kernel event handler that is triggered when new RX
 * descriptors are pushed by the host. This event is triggered only in case no more
 * descriptors were available and the RX handling was therefore stopped.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void rxu_cntrl_evt(int dummy);


/// @}

#endif // _RXU_CNTRL_H_
