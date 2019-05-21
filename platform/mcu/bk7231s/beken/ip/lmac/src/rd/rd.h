/**
 ****************************************************************************************
 * @file rxl_cntrl.h
 *
 * @brief Declarations of radar detection driver.
 *
 * Copyright (C) RivieraWaves 2011-2016
 ****************************************************************************************
 */

#ifndef _RD_H_
#define _RD_H_

/**
 ****************************************************************************************
 * @defgroup RD RD
 * @ingroup LMAC
 * @brief Management of the radar pulses received from PHY
 * @{
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

// for co_list
#include "co_list.h"

// for co_ring
#include "co_ring.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// RX control environment declaration.
struct rd_env_tag
{
    ///List of free radar events
    struct co_list event_free_list;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Radar environment variable
extern struct rd_env_tag rd_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief This function initializes the radar pulse manager
 ****************************************************************************************
 */
void rd_init(void);

/**
 ****************************************************************************************
 * @brief Indicate to the RD module that a radar event has been detected by the PHY.
 *
 * @param[in] rd_idx Index of the radar detection chain on which pulses have been detected
 *
 ****************************************************************************************
 */
void rd_event_ind(int rd_idx);

/// @}

#endif // _RD_H_
