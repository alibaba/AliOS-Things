/**
 ****************************************************************************************
 *
 * @file txl_frame.h
 *
 * @brief Management of internal frame generation.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXL_FRAME_H_
#define _TXL_FRAME_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "include.h" 

#include "co_int.h"
#include "co_list.h"
#include "tx_swdesc.h"
#include "txl_buffer.h"

#if (NX_P2P_GO)
#include "p2p.h"
#endif //(NX_P2P_GO)

#if (RW_MESH_EN)
#include "mesh.h"
#endif //(RW_MESH_EN)

#if NX_TX_FRAME
/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// Type of transmission parameters
enum
{
    /// Default TX parameters for 2.4GHz - 1Mbps, no protection
    TX_DEFAULT_24G,
    /// Default TX parameters for 5GHz - 6Mbps, no protection
    TX_DEFAULT_5G,
    /// Default TX parameters for NDPA and BRP transmissions
    TX_DEFAULT_NDPA_BRP,
    /// Default TX parameters for NDP transmissions
    TX_DEFAULT_NDP,
    /// Custom TX parameters
    TX_CUSTOM
};

/// Type of frame descriptor
enum
{
    /// Internal frame descriptor, i.e part of the generic frame module
    TX_INT,
    /// External frame descriptor
    TX_EXT
};

/// Pointer to confirmation function
typedef void (*cfm_func_ptr)(void *, uint32_t);

/// TX frame confirmation descriptor
struct txl_frame_cfm_tag
{
    /// Function to be called when TX has been completed
    cfm_func_ptr cfm_func;
    /// Void pointer to be passed to the confirmation function after TX completion
    void *env;
};

/// TX frame descriptor
struct txl_frame_desc_tag
{
    /// "Normal" descriptor of the TX path
    struct txdesc txdesc;
    /// Confirmation descriptor used after TX completion to report the status
    struct txl_frame_cfm_tag cfm;
    /// Type of the frame descriptor (TX_INT or TX_EXT)
    uint8_t type;
    /// Indicate if frame transmission has been postponed (valid only if type is TX_INT)
    bool postponed;
    /// Indicate if frame descriptor can be freed
    bool keep_desc;
};


/// Environment variables of the TX frame block
struct txl_frame_env_tag
{
    /// List of free TX frame descriptors
    struct co_list desc_free;

    /// List of confirmed TX frame descriptors
    struct co_list desc_done;
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// TX frame environment variables
extern struct txl_frame_env_tag txl_frame_env;

/// TX frame buffer pool
extern uint32_t txl_frame_pool[NX_TXFRAME_CNT][(sizeof(struct txl_buffer_tag) + NX_TXFRAME_LEN) / 4];

#if NX_BCN_AUTONOMOUS_TX
/// Pool of TIM IE buffers
extern uint32_t txl_tim_ie_pool[NX_VIRT_DEV_MAX][CO_ALIGN4_HI(MAC_TIM_BMP_OFT + 1)/4];
/// Pool of TIM virtual bitmap
extern uint32_t txl_tim_bitmap_pool[NX_VIRT_DEV_MAX][CO_ALIGN4_HI(MAC_TIM_SIZE)/4];
/// Pool of TIM descriptors
extern struct tx_pbd txl_tim_desc[NX_VIRT_DEV_MAX][2];
/// Beacon buffer pool
extern uint32_t txl_bcn_pool[NX_VIRT_DEV_MAX][(sizeof(struct txl_buffer_tag) + NX_BCNFRAME_LEN) / 4];
/// TX beacon header descriptor pool
extern struct tx_hw_desc_s txl_bcn_hwdesc_pool[NX_VIRT_DEV_MAX];
/// TX payload buffer descriptor for the post-TIM part of the beacon
extern struct tx_pbd txl_bcn_end_desc[NX_VIRT_DEV_MAX];
extern struct txl_buffer_control txl_bcn_buf_ctrl[NX_VIRT_DEV_MAX];

#if (NX_P2P_GO)
/// Pool of P2P NOA payload descriptors
extern struct tx_pbd txl_p2p_noa_desc[NX_VIRT_DEV_MAX];
/// Pool of P2P NOA Attributes
extern uint16_t txl_p2p_noa_ie_pool[NX_VIRT_DEV_MAX][P2P_NOA_IE_BUFFER_LEN];
#endif //(NX_P2P_GO)

#if (RW_MESH_EN)
// Payload Descriptor for additional IEs of Mesh Beacon
extern struct tx_pbd txl_mesh_add_ies_desc[RW_MESH_VIF_NB];
// Buffer containing the additional IEs of Mesh Beacon
extern struct mesh_add_ies_tag txl_mesh_add_ies[RW_MESH_VIF_NB];
#endif //(RW_MESH_EN)
#endif //(NX_BCN_AUTONOMOUS_TX)

/// TX frame header descriptor pool
extern struct tx_hw_desc_s txl_frame_hwdesc_pool[NX_TXFRAME_CNT];

/// Default policy table for 2.4GHz band
extern struct txl_buffer_control txl_buffer_control_24G;
/// Default policy table for 5GHz band
extern struct txl_buffer_control txl_buffer_control_5G;
#if (RW_BFMER_EN)
/// Default policy table for transmission of NDPA and BRP frames sent during Beamforming Sounding procedure
extern struct txl_buffer_control txl_buffer_control_ndpa_brp;
/// Default policy table for NDP frame sent during Beamforming Sounding procedure
extern struct txl_buffer_control txl_buffer_control_ndp;
#endif //(RW_BFMER_EN)

extern struct tx_policy_tbl txl_frame_pol_24G ;
extern struct tx_policy_tbl txl_frame_pol_5G ;

extern struct txl_buffer_control txl_frame_buf_ctrl[NX_TXFRAME_CNT];

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the TX frame module environment.
 *
 ****************************************************************************************
 */
void txl_frame_init(bool reset);

/**
 ****************************************************************************************
 * @brief Basic initializations of an external TX frame descriptor (e.g such as a beacon
 * descriptor when autonomous beacon transmission is used).
 *
 * @param[in] frame   Pointer to the frame descriptor to be initialized
 * @param[in] buffer  Pointer to the buffer descriptor that need to be attached to the
 *                    frame descriptor
 * @param[in] hwdesc  Pointer to the HW descriptors that need to be attached to the frame
 *                    descriptor
 * @param[in] bufctrl Pointer to the buffer control structure associated with this desc
 *
 ****************************************************************************************
 */
void txl_frame_init_desc(struct txl_frame_desc_tag *frame,
                         struct txl_buffer_tag *buffer,
                         struct tx_hw_desc_s *hwdesc,
                         struct txl_buffer_control *bufctrl);

/**
 ****************************************************************************************
 * @brief Get a TX frame descriptor.
 * This function is called by any module that needs to transmit a packet generated
 * internally. The caller can choose whether it prefers to let the TX frame module
 * configure automatically the TX parameters to their default values or to configure
 * manually those parameters.
 *
 * @param[in] type      Flag indicating if the sender wants to fully control the TX
 *                      parameters or use default parameters.
 *                      The values that can be put here are @ref TX_DEFAULT_24G,
 *                      @ref TX_DEFAULT_5G or @ref TX_CUSTOM.
 * @param[in] len       Length of the packet to be transmitted (including FCS length)
 *
 * @return A pointer to the allocated TX frame descriptor, or NULL if no descriptors are
 * available anymore.
 *
 ****************************************************************************************
 */
struct txl_frame_desc_tag *txl_frame_get(int type, int len);

/**
 ****************************************************************************************
 * @brief Push a TX frame descriptor.
 * This function is called by any module after a call to txl_frame_get. It takes the
 * descriptor returned by this call as parameter.
 *
 * @param[in] desc      Pointer to the descriptor previously returned by @ref txl_frame_get
 * @param[in] ac        Access category on which the frame shall be transmitted
 *
 ****************************************************************************************
 */
bool txl_frame_push(struct txl_frame_desc_tag *desc, uint8_t ac);

/**
 ****************************************************************************************
 * @brief Push a TX descriptor in the TX frame confirmation queue
 * This function is called under interrupt by the TX control module once the frame has
 * been transmitted.
 *
 * @param[in] txdesc            TX descriptor to be pushed in the confirmation queue
 *
 ****************************************************************************************
 */
void txl_frame_cfm(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Release a descriptor allocated for a packet that has not been sent
 *
 * @param[in] txdesc            TX descriptor to be pushed in the confirmation queue
 * @param[in] postponed         Indicate if released frame was a postponed frame
 *
 ****************************************************************************************
 */
void txl_frame_release(struct txdesc *txdesc, bool postponed);

/**
 ****************************************************************************************
 * @brief Background event handler of the Tx frame module
 *
 * This function parses the list of confirmed frames and call the appropriate callbacks.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void txl_frame_evt(int dummy);


/**
 ****************************************************************************************
 * @brief Builds a NULL frame with RA address pointing to the STA passed as parameter.
 *
 * Once the NULL frame has been transmitted, the confirmation function is called.
 *
 * @param[in] sta_idx   Index of the station the NULL frame has to be transmitted to
 * @param[in] cfm       Pointer to the function to be called after frame transmission
 * @param[in] env       Environment variable to be put as parameter to the confirmation
 *                      function
 *
 * @return CO_OK if frame was correctly pushed to TX path, CO_FAIL otherwise
 *
 ****************************************************************************************
 */
uint8_t txl_frame_send_null_frame(uint8_t sta_idx, cfm_func_ptr cfm, void *env);

/**
 ****************************************************************************************
 * @brief Builds a QoS NULL frame with RA address pointing to the STA passed as parameter.
 * The UP field is put to 7, i.e. the highest priority.
 *
 * Once the NULL frame has been transmitted, the confirmation function is called.
 *
 * @param[in] sta_idx   Index of the station the NULL frame has to be transmitted to
 * @param[in] qos       Value to set in the QoS control field
 * @param[in] cfm       Pointer to the function to be called after frame transmission
 * @param[in] env       Environment variable to be put as parameter to the confirmation
 *                      function
 *
 * @return CO_OK if frame was correctly pushed to TX path, CO_FAIL otherwise
 *
 ****************************************************************************************
 */
uint8_t txl_frame_send_qosnull_frame(uint8_t sta_idx, uint16_t qos, cfm_func_ptr cfm, void *env);

#if (TDLS_ENABLE)
/**
 ****************************************************************************************
 * @brief Builds a TDLS Channel Switch Request frame.
 * 
 * Once the TDLS Channel Switch Request frame has been transmitted, the confirmation
 * function is called.
 *
 * @param[in] param     Parameters of the TDLS Channel Switch Request
 * @param[in] cfm       Pointer to the function to be called after frame transmission
 *
 * @return CO_OK if frame was correctly pushed to TX path, CO_FAIL otherwise
 *
 ****************************************************************************************
 */
uint8_t txl_frame_send_tdls_channel_switch_req_frame(struct tdls_chan_switch_req const *param, cfm_func_ptr cfm);

/**
 ****************************************************************************************
 * @brief Builds a TDLS Channel Switch Response frame.
 * 
 * Once the TDLS Channel Switch Response frame has been transmitted, the confirmation
 * function is called.
 *
 * @param[in] vif_entry Pointer to the VIF entry
 * @param[in] status    Status to be set in the TDLS Channel Switch Response
 * @param[in] cfm       Pointer to the function to be called after frame transmission
 *
 * @return CO_OK if frame was correctly pushed to TX path, CO_FAIL otherwise
 *
 ****************************************************************************************
 */
uint8_t txl_frame_send_tdls_channel_switch_rsp_frame(struct vif_info_tag *vif_entry, uint16_t status, cfm_func_ptr cfm);

/**
 ****************************************************************************************
 * @brief Builds a TDLS Peer Traffic Indication frame.
 * 
 * Once the TDLS Peer Traffic Indication frame has been transmitted, the confirmation
 * function is called.
 *
 * @param[in] param     Parameters of the TDLS Peer Traffic Indication
 * @param[in] cfm       Pointer to the function to be called after frame transmission
 *
 * @return CO_OK if frame was correctly pushed to TX path, CO_FAIL otherwise
 *
 ****************************************************************************************
 */
uint8_t txl_frame_send_tdls_peer_traffic_ind_frame(struct tdls_peer_traffic_ind_req const *param, cfm_func_ptr cfm);
#endif

#if CFG_TX_EVM_TEST
extern void txl_evm_set_hw_rate(HW_RATE_E rate, uint32_t is_2_4G);
#endif


#endif

/// @}

#endif // _TXL_FRAME_H_
