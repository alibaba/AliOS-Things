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
#ifndef BLE_L2CAP_STRUCT_SERIALIZATION_H
#define BLE_L2CAP_STRUCT_SERIALIZATION_H

#include "ble_l2cap.h"

#ifdef __cplusplus
extern "C" {
#endif

#if NRF_SD_BLE_API_VERSION >= 5
uint32_t ble_l2cap_conn_cfg_t_enc(void const * const p_void_struct,
                                  uint8_t * const    p_buf,
                                  uint32_t           buf_len,
                                  uint32_t * const   p_index);

uint32_t ble_l2cap_conn_cfg_t_dec(uint8_t const * const p_buf,
                                  uint32_t              buf_len,
                                  uint32_t * const      p_index,
                                  void * const          p_void_struct);

uint32_t ble_l2cap_ch_rx_params_t_enc(void const * const p_void_struct,
                                      uint8_t * const    p_buf,
                                      uint32_t           buf_len,
                                      uint32_t * const   p_index);

uint32_t ble_l2cap_ch_rx_params_t_dec(uint8_t const * const p_buf,
                                      uint32_t              buf_len,
                                      uint32_t * const      p_index,
                                      void * const          p_void_struct);

uint32_t ble_l2cap_ch_setup_params_t_enc(void const * const p_void_struct,
                                         uint8_t * const    p_buf,
                                         uint32_t           buf_len,
                                         uint32_t * const   p_index);

uint32_t ble_l2cap_ch_setup_params_t_dec(uint8_t const * const p_buf,
                                         uint32_t              buf_len,
                                         uint32_t * const      p_index,
                                         void * const          p_void_struct);

uint32_t ble_l2cap_ch_tx_params_t_enc(void const * const p_void_struct,
                                      uint8_t * const    p_buf,
                                      uint32_t           buf_len,
                                      uint32_t * const   p_index);

uint32_t ble_l2cap_ch_tx_params_t_dec(uint8_t const * const p_buf,
                                      uint32_t              buf_len,
                                      uint32_t * const      p_index,
                                      void * const          p_void_struct);
#endif

#ifdef __cplusplus
}
#endif

#endif /*BLE_L2CAP_STRUCT_SERIALIZATION_H*/
