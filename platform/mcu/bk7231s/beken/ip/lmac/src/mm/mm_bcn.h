/**
 ****************************************************************************************
 * @file mm.h
 *
 * @brief MAC Management module definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MM_BCN_H_
#define _MM_BCN_H_

#include "co_int.h"
#include "co_bool.h"
#include "mac_common.h"
#include "co_utils.h"
#include "ke_timer.h"
#include "mm_task.h"
#include "hal_desc.h"
#include "hal_machw.h"
#include "hal_dma.h"

#if (NX_P2P_GO)
#include "p2p.h"
#endif //(NX_P2P_GO)

#define CSA_ACTION_COUNT              (12)

#if NX_BCN_AUTONOMOUS_TX
/*
 * DEFINES
 ****************************************************************************************
 */
// Forward declaration
struct vif_info_tag;

/// MM BCN environmenent structure
struct mm_bcn_env_tag
{
    /// Pointer to the beacon parameter structure
    struct mm_bcn_change_req const*param;
    struct mm_bcn_change_req *param_csa_after;
    
    /// Number of beacon transmission confirmation still awaited
    int tx_cfm;
    /// Flag indicating if beacon transmission has to be programmed immediately
    /// after download
    bool tx_pending;
    /// Flag indicating if beacon update is ongoing
    bool update_ongoing;
    /// Flag indicating if the beacon has to be updated immediately
    /// after the transmission
    bool update_pending;
    /// Structure used for beacon download by the DMA
    struct hal_dma_desc_tag dma;
    /// List containing the TIM update requests
    struct co_list tim_list;
    #if (NX_P2P_GO)
    /// P2P NOA Change Request
    uint8_t p2p_noa_req[NX_VIRT_DEV_MAX];
    #endif //(NX_P2P_GO)
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct mm_bcn_env_tag mm_bcn_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the MM BCN environment
 ****************************************************************************************
 */
void mm_bcn_init(void);



/**
 ****************************************************************************************
 * @brief Initialization of some BCN descriptors at AP VIF creation
 *
 * @param[in]  vif_entry Pointer to the VIF that needs to be initialized
 ****************************************************************************************
 */
void mm_bcn_init_vif(struct vif_info_tag *vif_entry);

/**
 ****************************************************************************************
 * @brief Proceed to the update of the beacon frame for the specified VIF
 * @param[in] param  Parameters of the new beacon
 ****************************************************************************************
 */
void mm_bcn_change(struct mm_bcn_change_req const *param);

/**
 ****************************************************************************************
 * @brief Proceed to the update of the TIM IE for the specified VIF
 * @param[in] param  Parameters of the TIM
 ****************************************************************************************
 */
void mm_tim_update(struct mm_tim_update_req const *param);
void mm_csa_event_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
void mm_bcn_transmit(void);
void mm_switch_channel(struct vif_info_tag *p_vif_entry);
void mm_channel_switch_init(struct vif_info_tag *vif_entry, uint32_t freq,
                                    uint32_t csa_count);
void mm_csa_beacon_change(struct mm_bcn_change_req *param,
                                    struct mm_bcn_change_req *param_csa_after);

#if (NX_P2P_GO)
void mm_bcn_update_p2p_noa(uint8_t vif_index, uint8_t operation);
#endif //(NX_P2P_GO)

#if (RW_MESH_EN)
/**
 ****************************************************************************************
 * @brief Return a pointer to buffer that will contains the Beacon data sent on a given
 * VIF.
 *
 * @param[in] vif_index     VIF Index on which beacon will be sent
 ****************************************************************************************
 */
struct txl_buffer_tag *mm_bcn_get_buffer(uint8_t vif_index);
#endif //(RW_MESH_EN)

int mm_bcn_get_tx_cfm(void);

#endif

/// @} end of group

#endif // _MM_H_
