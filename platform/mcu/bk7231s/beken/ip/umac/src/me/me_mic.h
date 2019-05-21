/**
 ****************************************************************************************
 *
 * @file me_mic.h
 *
 * @brief The MIC Calculator utility declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _ME_MIC_H_
#define _ME_MIC_H_

/** @defgroup MIC_CALC MIC_CALC
 * @ingroup ME
 * @brief The MIC Calculator's structure and methods declarations.
 *
 * @addtogroup MIC_CALC
 * @{
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_int.h"
#include "mac.h"

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */

/**
 * ========================== Context Definition ===========================
 * This structure contains the attributes that defines the current state
 * of the MIC key Calculator
 */
struct mic_calc
{
    /**
     * Holds the 1st 4 bytes of the intermediate MIC key during the
     * calculation process (little-end).
     */
    uint32_t mic_key_least;
    /**
     * Holds the last 4 bytes of the intermediate MIC key during the
     * calculation process (little-end).
     */
    uint32_t mic_key_most;

    /// Contain the latest incomplete M(i) block
    uint32_t last_m_i;
    /// Length of M(i) that has already been received
    uint8_t  last_m_i_len;
};

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ***************************************************************************************
 * @brief Initializes mic_calc attributes before proceeding with MIC key calculations.
 *        As exposed in the IEEE 802.11-2012 specification, section 11.4.2.3.2,
 *        figure 11-9, the MIC is computed on a MSDU including the Priority field, 3
 *        reserved octets, SA field and DA field.
 *        The total length of these fields is 16 bytes. Hence the MIC generation can be
 *        started by providing the 4 first blocks to the Michael block function.
 *
 * @param[in,out] mic_calc_ptr
 * @param[in] mic_key_ptr  Initial MIC key
 * @param[in] da           Destination Address to use as part of the MIC header
 * @param[in] sa           Source Address to use as part of the MIC header
 * @param[in] tid          Packet priority to use as part of the MIC header
 *
 ***************************************************************************************
 */
void me_mic_init(struct mic_calc *mic_calc_ptr, uint32_t *mic_key_ptr,
                 struct mac_addr *da, struct mac_addr *sa, uint8_t tid);

/**
 ****************************************************************************************
 * @brief Performs MIC Key calculations over a continuous block in memory with an
 *        arbitrary starting point and length.
 *
 * @param[in,out] mic_calc_ptr
 * @param[in] start_ptr     Pointer to the data
 * @param[in] data_len      Length of the provided data
 *
 ****************************************************************************************
 */
void me_mic_calc(struct mic_calc *mic_calc_ptr, uint32_t start_ptr,
                 uint32_t data_len);

/**
 ****************************************************************************************
 * @brief Used to terminate the MIC calculation session and retrieve the resultant MIC key.
 *        As specified in IEEE 802.11-2012 specification, section 11.4.2.3.3, the message
 *        is padded at the end with a single octet with value 0x5A, followed by between
 *        4 and 7 zero octets. The padding in not transmitted with the MSDU; it is used to
 *        simplify the computation over the final block.
 *        By construction M(n-1) = 0 and M(n-2) != 0
 *
 * @param[in,out] mic_calc_ptr
 *
 ****************************************************************************************
 */
void me_mic_end(struct mic_calc *mic_calc_ptr);

/** @}
*/

#endif // _ME_MIC_H_
