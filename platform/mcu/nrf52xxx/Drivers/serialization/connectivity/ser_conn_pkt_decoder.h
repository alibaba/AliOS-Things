/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
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
/**
 * @addtogroup ser_conn Connectivity application code
 * @ingroup ble_sdk_lib_serialization
 */

/** @file
 *
 * @defgroup ser_pkt_decoder Packets decoder in the Connectivity Chip
 * @{
 * @ingroup ser_conn
 *
 * @brief   Decoder for serialized packets from the Application Chip.
 *
 * @details This file contains declaration of common function used for processing packets (packets
 *          dispatcher) received by the transport layer.
 */

#ifndef SER_CONN_PKT_DECODER_H__
#define SER_CONN_PKT_DECODER_H__

#include <stdint.h>
#include "ser_hal_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief A function for dispatching packets from the Application Chip to an appropriate decoder.
 *
 * @details    The function is called to process received packets from a transport layer.
 *             The function analyzes packet type and calls appropriate command decoder which among
 *             other things processes command and sends a response. Then a received packet is freed.
 *
 * @param[in]  p_rx_pkt_params   A pointer to a transport layer event of type
 *                               @ref ser_hal_transport_evt_rx_pkt_received_params_t.
 *
 * @retval    NRF_SUCCESS           Operation success.
 * @retval    NRF_ERROR_NULL        Operation failure. NULL pointer supplied.
 * @retval    NRF_ERROR_INTERNAL    Operation failure. Internal error ocurred.
 */
uint32_t ser_conn_received_pkt_process(
        ser_hal_transport_evt_rx_pkt_received_params_t * p_rx_pkt_params);


#ifdef __cplusplus
}
#endif

#endif /* SER_CONN_PKT_DECODER_H__ */

/** @} */
