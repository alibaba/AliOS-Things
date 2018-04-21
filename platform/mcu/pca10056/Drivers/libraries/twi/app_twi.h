/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#ifndef APP_TWI_H__
#define APP_TWI_H__

#include "../twi_mngr/nrf_twi_mngr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 */

/**
 * @defgroup app_twi, nrf_twi_mngr_wrapper
 * @{
 * @ingroup app_common
 *
 * @brief nrf_twi_mngr wrapper.
 *
 * @warning This is a duplicate of nrf_twi_mngr module, app_twi is soon to be deprecated.
 *          Switch to nrf_twi_mngr as soon as possible.
 *          To switch replace every app_twi component according to macros below.
 */

//If TWIM is present buffers can only be in RAM

/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_BUFFERS_IN_RAM
 */
#define APP_TWI_BUFFERS_IN_RAM NRF_TWI_MNGR_BUFFERS_IN_RAM

/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_BUFFER_LOC_IND
 */
#define APP_TWI_BUFFER_LOC_IND NRF_TWI_MNGR_BUFFER_LOC_IND

/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_NO_STOP
 */
#define APP_TWI_NO_STOP       NRF_TWI_MNGR_NO_STOP


/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_WRITE
 */
#define APP_TWI_WRITE         NRF_TWI_MNGR_WRITE
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_READ
 */
#define APP_TWI_READ          NRF_TWI_MNGR_READ
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_TRANSFER
 */
#define APP_TWI_TRANSFER      NRF_TWI_MNGR_TRANSFER
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_WRITE_OP
 */
#define APP_TWI_WRITE_OP      NRF_TWI_MNGR_WRITE_OP
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_READ_OP
 */
#define APP_TWI_READ_OP       NRF_TWI_MNGR_READ_OP
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_IS_READ_OP
 */
#define APP_TWI_IS_READ_OP    NRF_TWI_MNGR_IS_READ_OP
/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_OP_ADDRESS
 */
#define APP_TWI_OP_ADDRESS    NRF_TWI_MNGR_OP_ADDRESS

/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_callback_t
 */
#define app_twi_callback_t    nrf_twi_mngr_callback_t
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_transfer_t
 */
#define app_twi_transfer_t    nrf_twi_mngr_transfer_t
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_transaction_t
 */
#define app_twi_transaction_t nrf_twi_mngr_transaction_t
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_cb_t
 */
#define app_twi_cb_t          nrf_twi_mngr_cb_t
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_t
 */
#define app_twi_t             nrf_twi_mngr_t

/**
 * @brief Macro for forwarding the new implementation. See @ref NRF_TWI_MNGR_DEF
 */
#define APP_TWI_DEF           NRF_TWI_MNGR_DEF

/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_init
 */
#define app_twi_init          nrf_twi_mngr_init
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_uninit
 */
#define app_twi_uninit        nrf_twi_mngr_uninit
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_schedule
 */
#define app_twi_schedule      nrf_twi_mngr_schedule
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_perform
 */
#define app_twi_perform(p_nrf_twi_mngr, p_transfers, number_of_transfers, user_function) \
	nrf_twi_mngr_perform(p_nrf_twi_mngr, NULL, p_transfers, number_of_transfers, user_function)
/**
 * @brief Macro for forwarding the new implementation. See @ref nrf_twi_mngr_is_idle
 */
#define app_twi_is_idle       nrf_twi_mngr_is_idle


/**
 *@}
 **/


#ifdef __cplusplus
}
#endif

#endif // APP_TWI_H__
