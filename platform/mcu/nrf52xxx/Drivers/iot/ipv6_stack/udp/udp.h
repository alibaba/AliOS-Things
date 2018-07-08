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
/** @cond */
 /** @file udp.h
 *
 * @defgroup iot_udp UDP Module Header.
 * @ingroup iot_sdk
 * @{
 * @brief User Datagram Protocol module header defining interface between
 *        UDP and other IP stack layers which are not exposed to the application.
 */

#ifndef UDP_H__
#define UDP_H__

#include "sdk_config.h"
#include "sdk_common.h"
#include "ipv6_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Module initialization function called from ipv6_init(). This shall not be called by the
 *         application explicitly.
 *
 * @retval NRF_SUCCESS on successful execution of procedure, else an error code indicating reason
 *                     for failure.
 */
uint32_t udp_init(void);


/**
 * @brief  Function to feed incoming UDP packets to the module. To be called by the IPv6
 *         stack only and never by the application.
 *
 * @param[in] p_interface Identifies network interface on which the packet is received.
 * @param[in] p_ip_header IP Header of the UDP packet being fed to the module.
 * @param[in] p_packet    UDP packet being notified to the module. p_packet->p_payload points the
 *                        IPv6 payload and p_packet->length indicates total length of the payload.
 *
 * @note This routine is called by the stack with next header field value is set to  UDP protocol
 *       value of 17.
 *
 * @retval NRF_SUCCESS on successful handling of the packet, else an error code indicating reason
 *                     for failure.
 */
uint32_t udp_input(const iot_interface_t  * p_interface,
                   const ipv6_header_t    * p_ip_header,
                   iot_pbuffer_t          * p_packet);

#ifdef __cplusplus
}
#endif

#endif //UDP_H__

/**@} */

/** @endcond */
