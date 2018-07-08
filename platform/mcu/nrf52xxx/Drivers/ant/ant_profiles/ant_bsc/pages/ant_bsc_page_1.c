/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
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
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(ANT_BSC)

#include "ant_bsc_page_1.h"
#include "ant_bsc_utils.h"
#include "app_util.h"
#include "nordic_common.h"

#define NRF_LOG_MODULE_NAME ant_bcs_page_1
#if ANT_BSC_PAGE_1_LOG_ENABLED
#define NRF_LOG_LEVEL       ANT_BSC_PAGE_1_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  ANT_BSC_PAGE_1_INFO_COLOR
#else // ANT_BSC_PAGE_1_LOG_ENABLED
#define NRF_LOG_LEVEL       0
#endif // ANT_BSC_PAGE_1_LOG_ENABLED
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

/**@brief BSC page 1 data layout structure. */
typedef struct
{
    uint8_t cumulative_operating_time[3];
    uint8_t reserved[4];
}ant_bsc_page1_data_layout_t;

/**@brief Function for printing speed or cadence page1 data. */
static void page1_data_log(ant_bsc_page1_data_t const * p_page_data)
{
    NRF_LOG_INFO("Cumulative operating time: %ud %uh %um %us",
              (unsigned int)ANT_BSC_OPERATING_DAYS(p_page_data->operating_time),
              (unsigned int)ANT_BSC_OPERATING_HOURS(p_page_data->operating_time),
              (unsigned int)ANT_BSC_OPERATING_MINUTES(p_page_data->operating_time),
              (unsigned int)ANT_BSC_OPERATING_SECONDS(p_page_data->operating_time));
}

void ant_bsc_page_1_encode(uint8_t * p_page_buffer, ant_bsc_page1_data_t const * p_page_data)
{
    ant_bsc_page1_data_layout_t * p_outcoming_data = (ant_bsc_page1_data_layout_t *)p_page_buffer;

    UNUSED_PARAMETER(uint24_encode(p_page_data->operating_time,
                     p_outcoming_data->cumulative_operating_time));
    page1_data_log( p_page_data);
}

void ant_bsc_page_1_decode(uint8_t const * p_page_buffer, ant_bsc_page1_data_t * p_page_data)
{
    ant_bsc_page1_data_layout_t const * p_incoming_data = (ant_bsc_page1_data_layout_t *)p_page_buffer;

    p_page_data->operating_time = uint24_decode(p_incoming_data->cumulative_operating_time);

    page1_data_log( p_page_data);
}

#endif // NRF_MODULE_ENABLED(ANT_BSC)

