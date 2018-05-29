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
/** @file ipv6_utils.h
 *
 * @defgroup iot_utils Common utils
 * @ingroup iot_sdk_common
 * @{
 * @brief Abstracts common IOT macros needed by IOT modules.
 *
 * @details Common macros related to IOT are defined here for use by all IOT modules.
 */

#ifndef IPV6_UTILS_H__
#define IPV6_UTILS_H__

#include <stdint.h>
#include <stdbool.h>
#include "iot_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Function for calculating checksum using IPv6 algorithm.
 *
 * @param[in]  p_data      Pointer to the data needs to be checksummed.
 * @param[in]  len         Length of the data.
 * @param[in]  p_checksum  Pointer to actual value of checksum.
 * @param[out] p_checksum  Value of calculated checksum.
 *
 * @retval None.
 */
void ipv6_checksum_calculate(const uint8_t * p_data,
                             uint16_t        len,
                             uint16_t      * p_checksum,
                             bool            flip_zero);


/**@brief Function for initializing default values of IPv6 Header.
 *
 * @note  Function initializes Version, Traffic Class, Flow Label, Next Header, Hop Limit and
 *        Length fields.
 *
 * @param[in] p_ip_header Pointer to the IPv6 Header.
 *
 * @retval None.
 */
void ipv6_header_init(ipv6_header_t * p_ip_header);

#ifdef __cplusplus
}
#endif

#endif //IPV6_UTILS_H__

/**@} */
