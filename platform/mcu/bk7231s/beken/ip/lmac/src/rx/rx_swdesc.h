/**
 ****************************************************************************************
 * @file rx_swdesc.h
 *
 * @brief LMAC SW descriptor data structures.
 *
 * Copyright (C) RivieraWaves 2011-2016
 ****************************************************************************************
 */

#ifndef _RX_SWDESC_H_
#define _RX_SWDESC_H_

/**
 ****************************************************************************************
 * @defgroup RX RX
 * @ingroup LMAC
 * @brief Definition of RX Path functionality
 ****************************************************************************************
 */
/// @}

/**
 ****************************************************************************************
 * @defgroup RX_SWDESC RX_SWDESC
 * @ingroup RX
 * @brief Rx buffer/descriptor management
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
//standard includes
#include "co_int.h"
//for ASSERT_ERR macro
#include "arch.h"

// for co_list
#include "co_list.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
// forward declarations
struct rx_dmadesc ;
/// LMAC SW descriptor to manage the LMAC receive operations.
struct rx_swdesc
{
    /// Pointer used to form the list.
    struct co_list_hdr list_hdr;
    /// Holds the MAC HW header descriptors list head pointer
    struct rx_dmadesc *dma_hdrdesc;
    /// Pointer to the last payload buffer descriptor
    struct rx_pbd *last_pbd;
    /// Pointer to the spare payload buffer descriptor
    struct rx_pbd *spare_pbd;
};

extern struct rx_swdesc rx_swdesc_tab[NX_RXDESC_CNT];

/**
 * @name External API
 * @{
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief API to initialize the LMAC SW descriptor pool.
 ****************************************************************************************
 */
void rx_swdesc_init(void);

/// @} END External API

/// @} END addtogroup

#endif // _RXL_SWDESC_H_
