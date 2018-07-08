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

/** @file
 *
 * @defgroup background_dfu_operation background_dfu_operation.h
 * @{
 * @ingroup background_dfu
 * @brief Background DFU operations.
 *
 */

#ifndef BACKGROUND_DFU_OPERATION_H_
#define BACKGROUND_DFU_OPERATION_H_

#include <stdint.h>

#include "nrf_dfu_handling_error.h"

/** @brief Select DFU object.
 *
 *  @param[in]  object_type    Object type which should be selected.
 *  @param[out] p_max_obj_size Returns max size of an object of the selected type.
 *  @param[out] p_offset       Returns current offset (size) of an object of the selected type.
 *
 *  @return Operation result code.
 */
nrf_dfu_res_code_t background_dfu_op_select(uint32_t   object_type,
                                            uint32_t * p_max_obj_size,
                                            uint32_t * p_offset);

/** @brief Create DFU object.
 *
 *  @param[in] object_type  Object type which should be selected.
 *  @param[in] object_size  Size of an object to create.
 *
 *  @return Operation result code.
 */
nrf_dfu_res_code_t background_dfu_op_create(uint32_t object_type, uint32_t object_size);

/** @brief Write DFU object.
 *
 *  @param[in] object_type     Object type which should be selected.
 *  @param[in] p_payload       A pointer to data which should be written to the object.
 *  @param[in] payload_length  Length, in bytes, of data which should be written to the object.
 *
 *  @return Operation result code.
 */
nrf_dfu_res_code_t background_dfu_op_write(uint32_t        object_type,
                                           const uint8_t * p_payload,
                                           uint16_t        payload_length);

/** @brief Calculate DFU object CRC.
 *
 *  @param[in] object_type Object type which should be selected.
 *
 *  @return Operation result code.
 */
nrf_dfu_res_code_t background_dfu_op_crc(uint32_t object_type);

/** @brief Execute selected DFU.
 *
 *  @param[in] object_type Object type which should be selected.
 *
 *  @return Operation result code.
 */
nrf_dfu_res_code_t background_dfu_op_execute(uint32_t object_type);

#endif /* BACKGROUND_DFU_OPERATION_H_ */
