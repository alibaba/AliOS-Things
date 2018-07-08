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
#include "ble_dtm_conn.h"
#include "dtm_uart.h"
#include "nrf_error.h"
#include "ble_serialization.h"

uint32_t ble_dtm_init_req_dec(uint8_t const * const p_buf, uint16_t packet_len,
                              app_uart_stream_comm_params_t * p_comm_params)
{
    SER_ASSERT_NOT_NULL(p_buf);
    SER_ASSERT_NOT_NULL(p_comm_params);

    uint32_t index    = 0;
    uint32_t err_code = NRF_SUCCESS;

    err_code = uint8_t_dec(p_buf, packet_len, &index, &p_comm_params->tx_pin_no);
    SER_ASSERT(err_code == NRF_SUCCESS, err_code);

    err_code = uint8_t_dec(p_buf, packet_len, &index, &p_comm_params->rx_pin_no);
    SER_ASSERT(err_code == NRF_SUCCESS, err_code);

    err_code = uint8_t_dec(p_buf, packet_len, &index, &p_comm_params->baud_rate);
    SER_ASSERT(err_code == NRF_SUCCESS, err_code);

    SER_ASSERT(index == packet_len, NRF_ERROR_INVALID_LENGTH);

    return err_code;
}

uint32_t ble_dtm_init_rsp_enc(uint32_t return_code, uint8_t * const p_buf,
                              uint32_t * const p_buf_len)
{
    SER_ASSERT_NOT_NULL(p_buf);
    SER_ASSERT_NOT_NULL(p_buf_len);

    uint32_t index    = 0;
    uint32_t err_code = NRF_SUCCESS;

    err_code = uint32_t_enc(&return_code, p_buf, *p_buf_len, &index);
    SER_ASSERT(err_code == NRF_SUCCESS, err_code);

    *p_buf_len = index;

    return err_code;
}
