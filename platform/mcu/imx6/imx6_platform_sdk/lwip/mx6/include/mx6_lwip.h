/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(__MX6_LWIP_H__)
#define __MX6_LWIP_H__

#include "sdk_types.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"

#if CHIP_MX6DQ || CHIP_MX6SDL
#include "enet/enet.h"
#elif CHIP_MX6SL
#include "fec/fec.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

enum _netif_init_mode {
    kDHCP,
    kAutoIP,
    kStaticIP,
    kNoIPInit
};

typedef enum _netif_init_mode netif_init_mode_t;

//! @brief The Ethernet device struct.
#if CHIP_MX6DQ || CHIP_MX6SDL
extern imx_enet_priv_t * g_en0;
#elif CHIP_MX6SL
extern imx_fec_priv_t * g_en0;
#endif

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief 
 */
void mx6_init_lwip(netif_init_mode_t mode, ip_addr_t *addr, ip_addr_t * mask, ip_addr_t * gw);

/*!
 * @brief Utility function for managing a timer.
 *
 * @param lastTime Pointer to the variable holding the last time the timer was invoked. This
 *  variable is managed by this function. The caller only needs to provide storage and initialize
 *  the variable to 0.
 * @param interval_ms The timer interval in milliseconds.
 */
bool check_and_update_ms_timer(uint32_t * lastTime, uint32_t interval_ms);

/*!
 * @brief Perform the loop processing required for lwIP.
 *
 * Calls enet_poll_for_packet() to process incoming Ethernet packets. Also invokes the
 * various lwIP timer routines at the appropriate intervals.
 */
void mx6_run_lwip(struct netif *netif);

/*!
 * @brief Set the MAC address to use for the enet/fec interface.
 *
 * Call this function to set the MAC address before creating the lwIP netif by calling
 * netif_add(). When enet_init() is invoked as part of the netif initialization, it will
 * use the MAC address set with this function.
 */
void enet_set_mac(const uint8_t * macAddress);

/*! 
 * @brief Initialize the mx6 enet/fec ethernet interface driver.
 *
 * Pass this function as the init function parameter for netif_add().
 */
err_t enet_init(struct netif *netif);

/*! 
 * @brief Check if a packet has been received and pass it to lwIP.
 */
void enet_poll_for_packet(struct netif * netif);

#if defined(__cplusplus)
}
#endif

#endif //__MX6_LWIP_H__
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
