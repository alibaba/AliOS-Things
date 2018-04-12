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
 * @defgroup background_dfu_operation background_dfu_operation.c
 * @{
 * @ingroup background_dfu
 * @brief Background DFU operations implementation.
 *
 */

#include "background_dfu_operation.h"

#include "sdk_config.h"
#include "nrf_dfu_req_handler.h"

#define NRF_LOG_MODULE_NAME background_dfu

#define NRF_LOG_LEVEL       BACKGROUND_DFU_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  BACKGROUND_DFU_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR BACKGROUND_DFU_CONFIG_DEBUG_COLOR

#include "nrf_log.h"

nrf_dfu_res_code_t background_dfu_op_select(uint32_t   object_type,
                                            uint32_t * p_max_obj_size,
                                            uint32_t * p_offset)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res;

    memset(&dfu_req, 0, sizeof(dfu_req));

    dfu_req.req_type = NRF_DFU_OBJECT_OP_SELECT;
    dfu_req.obj_type = object_type;

    res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
    if (res_code == NRF_DFU_RES_CODE_SUCCESS)
    {
        NRF_LOG_DEBUG("Object selected max_size: %u offs: %u crc: %x",
                         dfu_res.max_size, dfu_res.offset, dfu_res.crc);

        *p_max_obj_size = dfu_res.max_size;
        *p_offset       = dfu_res.offset;
    }

    return res_code;
}

nrf_dfu_res_code_t background_dfu_op_create(uint32_t object_type, uint32_t object_size)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res;

    memset(&dfu_req, 0, sizeof(dfu_req));

    dfu_req.req_type    = NRF_DFU_OBJECT_OP_CREATE;
    dfu_req.object_size = object_size;
    dfu_req.obj_type    = object_type;

    res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
    if (res_code == NRF_DFU_RES_CODE_SUCCESS)
    {
        NRF_LOG_DEBUG("Object created [type=%u, size=%u]", object_type, object_size);
    }

    return res_code;
}

nrf_dfu_res_code_t background_dfu_op_write(uint32_t        object_type,
                                           const uint8_t * p_payload,
                                           uint16_t        payload_length)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res;

    memset(&dfu_req, 0, sizeof(dfu_req));

    dfu_req.obj_type = object_type;
    dfu_req.req_type = NRF_DFU_OBJECT_OP_WRITE;
    dfu_req.p_req    = (uint8_t *)p_payload;
    dfu_req.req_len  = payload_length;

    res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
    if (res_code == NRF_DFU_RES_CODE_SUCCESS)
    {
        NRF_LOG_DEBUG("Object written: %u", payload_length);
    }

    return res_code;
}

nrf_dfu_res_code_t background_dfu_op_crc(uint32_t object_type)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res;

    memset(&dfu_req, 0, sizeof(dfu_req));

    // Calculate CRC of the object
    dfu_req.obj_type = object_type;
    dfu_req.req_type = NRF_DFU_OBJECT_OP_CRC;

    res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
    if (res_code == NRF_DFU_RES_CODE_SUCCESS)
    {
        NRF_LOG_DEBUG("Object CRC: %x", dfu_res.crc);
    }

    return res_code;
}

nrf_dfu_res_code_t background_dfu_op_execute(uint32_t object_type)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res;

    memset(&dfu_req, 0, sizeof(dfu_req));

    dfu_req.obj_type = object_type;
    dfu_req.req_type = NRF_DFU_OBJECT_OP_EXECUTE;

    res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
    if (res_code == NRF_DFU_RES_CODE_SUCCESS)
    {
        NRF_LOG_DEBUG("Object executed");
    }

    return res_code;
}
