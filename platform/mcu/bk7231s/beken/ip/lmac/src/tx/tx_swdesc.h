/**
 ****************************************************************************************
 *
 * @file tx_swdesc.h
 *
 * @brief Definition of Tx SW descriptors
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TX_SWDESC_H_
#define _TX_SWDESC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"

#include "rwnx_config.h"
#include "mac.h"
#include "hal_desc.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#if (NX_AMPDU_TX)
///AMDPU delimiter length in bytes
#define DELIMITER_LEN             4
#endif //(NX_AMPDU_TX)

#if (RW_BFMER_EN)
/**
 * Masks for bits in tx_flags value in struct umacdesc
 */
#define TX_SWDESC_UMAC_BEAMFORM_MASK     (CO_BIT(0))
#endif //(RW_BFMER_EN)

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */

struct txl_buffer_tag;
struct tx_dmadesc;

/// Descriptor filled by the Host
struct hostdesc
{    
    #if NX_AMSDU_TX
    uint32_t orig_addr[NX_TX_PAYLOAD_MAX];
    /// Pointers to packet payloads
    uint32_t packet_addr[NX_TX_PAYLOAD_MAX];
    /// Sizes of the MPDU/MSDU payloads
    uint16_t packet_len[NX_TX_PAYLOAD_MAX];
    /// Number of payloads forming the MPDU
    uint8_t packet_cnt;
    #else
    uint32_t orig_addr;
    /// Pointer to packet payload
    uint32_t packet_addr;
    /// Size of the payload
    uint16_t packet_len;
    #endif //(NX_AMSDU_TX)

    /// Address of the status descriptor in host memory (used for confirmation upload)
    uint32_t status_desc_addr;
    /// Destination Address
    struct mac_addr eth_dest_addr;
    /// Source Address
    struct mac_addr eth_src_addr;
    /// Ethernet Type
    uint16_t ethertype;
    /// PN that was used for first transmission of this MPDU
    uint16_t pn[4];
    /// Sequence Number used for first transmission of this MPDU
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    /// Packet TID
    uint8_t tid;
    /// VIF index
    uint8_t vif_idx;
    /// Station Id (0xFF if station is unknown)
    uint8_t staid;
    
    #if RW_MUMIMO_TX_EN
    /// MU-MIMO information (GroupId and User Position in the group) - The GroupId
    /// is located on bits 0-5 and the User Position on bits 6-7. The GroupId value is set
    /// to 63 if MU-MIMO shall not be used
    uint8_t mumimo_info;
    #endif
    
    /// TX flags
    uint16_t flags;
};

/// Descriptor filled by the UMAC
struct umacdesc
{
    #if (RW_BFMER_EN)
    /**
     * Flags from UMAC which do not fit with tx_hd.macctrlinfo2 format
     *      Bit 0 - Indicate if frame can be beamformed when sent as a singleton
     */
    uint16_t tx_flags;
    #endif //(RW_BFMER_EN)
    
    #if NX_AMPDU_TX
    ///First Sequence Number of the BlockAck window
    uint16_t sn_win;
    /// Flags from UMAC (match tx_hd.macctrlinfo2 format)
    uint32_t flags;
    /// PHY related flags field - nss, rate, GI type, BW type - filled by driver
    uint32_t phy_flags;
    #endif //(NX_AMPDU_TX)
	
    /// Pointer to the buffer control to use
    struct txl_buffer_control *buf_control;
    /// Buffer data offset for DMA Download
    uint32_t buff_offset;
    /// Length of the payload
    uint16_t payl_len;
    /// Header length (MAC + IV + EIV + 802.2 Header)
    uint8_t head_len;
    /// 802.2 header length
    uint8_t hdr_len_802_2;
    /// Tail length (MIC + ICV)
    uint8_t tail_len;
    
    #if (RW_MESH_EN)
    /// Indicate if Mesh Control field is present in the sent frame
    bool has_mesh_ctrl;
    /// Number of External Addresses to be inserted in the Mesh Control field
    uint8_t nb_ext_addr;
    /// Index of the path to be used
    uint8_t path_idx;
    /// Index of the proxy information to be used
    uint8_t proxy_idx;
    #endif //(RW_MESH_EN)
};

/// Descriptor filled by the LMAC
struct lmacdesc
{
    /// Pointer to the optional Aggregation descriptor
    struct tx_agg_desc *agg_desc;
    #if NX_AMSDU_TX
    /// Pointer to the embedded buffers
    struct txl_buffer_tag *buffer[NX_TX_PAYLOAD_MAX];
    #else
    /// Pointer to the embedded buffer
    struct txl_buffer_tag *buffer;
    #endif //(NX_AMSDU_TX)
    #if (RW_BFMER_EN)
    /// BFR Node used for the transmission
    struct bfr_mem_node *p_bfr_node;
    #endif //(RW_BFMER_EN)
    /// Pointer to the TX confirmation structure
    struct tx_hw_desc_s *hw_desc;
};

/// LMAC Tx Descriptor
#define TXDESC_STA_IDLE                 0
#define TXDESC_STA_USED                 1

struct txdesc
{	
	/// Pointer to the next element in the queue
	struct co_list_hdr list_hdr;
	
	/// Information provided by Host
	struct hostdesc host;
	
	/// Information provided by UMAC
	struct umacdesc umac;
	
	/// Information provided by LMAC
	struct lmacdesc lmac;
	
	uint32_t status;
};

/// Description of the TX API
struct txdesc_api
{
    /// Information provided by Host
    struct hostdesc host;    
};


/// Descriptor used for Host/Emb TX frame information exchange
struct txdesc_host
{
    /// Flag indicating if the TX descriptor is ready (different from 0) or not (equal to 0)
    uint32_t ready;

    /// API of the embedded part
    struct txdesc_api api;
};


#if NX_EMB
// Prevent including the whole world in host case

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct txdesc *txdesc_array[NX_TXQ_CNT][RW_USER_MAX];
extern const int nx_txdesc_cnt[NX_TXQ_CNT];
extern const int nx_txuser_cnt[NX_TXQ_CNT];
extern uint32_t tx_desc_idx[NX_TXQ_CNT];
extern const int nx_txdesc_cnt_msk[NX_TXQ_CNT];

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initializes the Tx descriptor pool
 *
 ****************************************************************************************
 */
void tx_txdesc_init(void);

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if a MPDU is split accross several host buffers (e.g.
 * A-MSDU)
 * @param[in] txdesc  Txdesc pointer to access its packet count.
 * @return Boolean True if MPDU is split and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_int_frame(struct txdesc * txdesc)
{
    #if NX_AMSDU_TX
    return (txdesc->host.packet_addr[0] == 0);
    #else
    return (txdesc->host.packet_addr == 0);
    #endif
}

#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Return a boolean indicating if a MPDU is split across several host buffers (e.g.
 * A-MSDU)
 * @param[in] txdesc  Txdesc pointer to access its packet count.
 * @return Boolean True if MPDU is split and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_split(struct txdesc * txdesc)
{
    return (txdesc->host.packet_cnt > 1);
}
#endif

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being first in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is first in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_qos_data(struct txdesc *txdesc)
{
    return (txdesc->host.tid != 0xFF);
}

#if NX_AMPDU_TX
/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being aggregatable or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is aggregatable and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_agg(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & AMPDU_BIT) == AMPDU_BIT);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being first in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is first in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_first(struct txdesc *txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_FIRST);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being intermediate in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is intermediate in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_interm(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_INT);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being last in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is last in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_last(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_LAST);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU having no position in an AMPDU yet
 * (inherent check for aggregatable too)
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU has no position in AMPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_unpos(struct txdesc * txdesc)
{
    //the definition of Which descriptor with 00 for position may be for the AMPDU THD
    //but we're using the 00 also until MPDU has a position, then for HW the right things
    //will be set in macctrlinfo2
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_EXTRA);
}

/**
 ****************************************************************************************
 * @brief Set position of MPDU inside an AMPDU
 * @param[in] txdesc  Txdesc pointer to modify its flags elements.
 * @param[in] pos     Which Descriptor value for the position in AMPDU - see hal_desc.h
 * @return Boolean True if MPDU is aggregatable and false otherwise
 ****************************************************************************************
 */
__INLINE void set_mpdu_pos(struct txdesc * txdesc, uint32_t pos)
{
    txdesc->umac.flags = ( txdesc->umac.flags         &
                                 (~(WHICHDESC_MSK & (~AMPDU_BIT))) ) | pos;
}
#endif

#if RW_MUMIMO_TX_EN
/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the MPDU is part of a MU-MIMO PPDU
 * @param[in] txdesc  Pointer to the MPDU TX descriptor
 * @return true if MPDU is part of a MU-MIMO PPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_in_mumimo_ppdu(struct txdesc * txdesc)
{
    struct tx_agg_desc *agg_desc = txdesc->lmac.agg_desc;

    return ((agg_desc != NULL) && (agg_desc->prim_agg_desc != NULL));
}

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the MPDU is part of the A-MPDU sent on the
 * primary HW queue.
 * @note This function assumes that the MPDU is sent as part of a MU-MIMO PPDU
 * @param[in] txdesc  Pointer to the MPDU TX descriptor
 * @return true if MPDU is part of the primary MU-MIMO PPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_primary_user(struct txdesc * txdesc)
{
    struct tx_agg_desc *agg_desc = txdesc->lmac.agg_desc;

    return (agg_desc == agg_desc->prim_agg_desc);
}

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the GroupID passed as parameter is a MU-MIMO one.
 * @param[in] group_id  GroupID to be checked
 * @return true if the GroupId is a MU-MIMO one, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mumimo_group_id(uint8_t group_id)
{
    return (group_id > 0) && (group_id < 63);
}

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO GroupId from the TX descriptor
 *
 * @param[in] txdesc  Pointer to the TX descriptor
 *
 * @return The MU-MIMO GroupId included in the TX descriptor
 ****************************************************************************************
 */
__INLINE uint8_t get_group_id(struct txdesc * txdesc)
{
    return (txdesc->host.mumimo_info & 0x3F);
}
#endif

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO User Position from the TX descriptor
 *
 * @param[in] txdesc  Pointer to the TX descriptor
 *
 * @return The MU-MIMO GroupId included in the TX descriptor
 ****************************************************************************************
 */
__INLINE uint8_t get_user_pos(struct txdesc * txdesc)
{
    #if RW_MUMIMO_TX_EN
    return (txdesc->host.mumimo_info >> 6);
    #else
    return 0;
    #endif
}


#endif // NX_EMB

struct txdesc *tx_txdesc_prepare(UINT32 ac);
void tx_txdesc_obtain(struct txdesc *desc, UINT32 ac);

/// @}
/// @}

#endif // _CO_SWDESC_H_
