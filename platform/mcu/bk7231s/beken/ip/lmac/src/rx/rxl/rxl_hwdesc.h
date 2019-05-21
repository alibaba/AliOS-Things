/**
 ****************************************************************************************
 *
 * @file rxl_hwdesc.h
 *
 * @brief File contains the declaration of pool's elements.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _RXL_HWDESC_H_
#define _RXL_HWDESC_H_

/**
 ****************************************************************************************
 * @defgroup RX_HWDESC RX_HWDESC
 * @ingroup RX
 * @brief Rx buffer/descriptor management
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for ASSERT
#include "arch.h"
// for __INLINE
#include "compiler.h"
// For co_status_t
#include "co_status.h"
// For the register parameters
#include "reg_mac_pl.h"
// For rx_payload_desc
#include "hal_desc.h"

/*
 * TYPE DEFINITION
 ****************************************************************************************
 */

/// Buffer Management control structure
struct rxl_hwdesc_env_tag
{
    /// First RX payload buffer descriptor chained to HW
    struct rx_pbd *first;
    /// Last RX payload buffer descriptor chained to HW
    struct rx_pbd *last;
    /// Free (spare) RX payload buffer descriptor (not chained to HW)
    struct rx_pbd *free;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// RXL HW descriptor environment variable
extern struct rxl_hwdesc_env_tag rx_hwdesc_env;

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
/// Length of the pattern for final DMA transfer
#define RXL_LAST_DMA_PATTERN_LEN    4

/// Length of the RX SW information
#define RXL_ADD_INFO_LEN   (sizeof_b(struct phy_channel_info) + 4)

/// Length of the receive vectors
#define RXL_HEADER_INFO_LEN   (RXL_HWDESC_RXV_LEN + RXL_LAST_DMA_PATTERN_LEN +           \
                               RXL_ADD_INFO_LEN)

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the pools of element
 * Function to initialize the Rx buffer and descriptor pools
 ****************************************************************************************
 */
void rxl_hwdesc_init(void);

/**
 ****************************************************************************************
 * @brief Appends a descriptor to the list visible to HW
 * Function to initialize the freed HW desc and tail it to the list of HW descriptors. The
 * element pointers are linked and the valid bit is set. The New Head/New Tail bit is also
 * set accordingly. It also updates the pointer pointing to the last desc tailed.
 *
 * @param[in]   desc      HW desc to be freed and appended
 ****************************************************************************************
 */
void rxl_hd_append(struct rx_dmadesc *desc);

/**
 ****************************************************************************************
 * @brief Appends a list of RX payload buffer descriptors to the list visible to HW
 * Function to initialize the freed HW desc and tail it to the list of HW descriptors. The
 * element pointers are linked and the valid bit is set. The New Head/New Tail bit is also
 * set accordingly. It also updates the pointer pointing to the last desc tailed.
 *
 * @param[in]   first      First buffer descriptor of the list
 * @param[in]   last       Last buffer descriptor of the list
 * @param[in]   spare      Spare descriptor that won't be chained immediately to HW
 ****************************************************************************************
 */
void rxl_pd_append(struct rx_pbd *first, struct rx_pbd* last, struct rx_pbd* spare);

/// @} // end of group RXHWDESC

#endif // _RXL_HWDESC_H_

