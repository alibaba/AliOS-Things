/**
 *
 * \file
 *
 * \brief
 *
 * Copyright (c) 2016-2018 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef NET_INIT_H_INCLUDED
#define NET_INIT_H_INCLUDED

#define NET_IF_STA				0x1
#define NET_IF_C				0x2

#define NET_MODE_USE_DHCP		0x1
#define NET_MODE_USE_STATIC		0x2
#define NET_MODE_AP				0x4
#define NET_MODE_USE_LINK_LOCAL	0x8
#define NET_MODE_USE_DHCP_SVR	0x10

#define NET_IF_REQ_DHCP_CONF (M2M_P2P_CMD_BASE + 100)

typedef struct {
	uint8_t	*u8StaticIP;
	/*!< The static IP assigned to the device.
	*/
	uint8_t	*u8SubnetMask;
	/*!< Subnet mask for the local area network.
	*/
	uint8_t	*u8StaticIPv6;
} tstrM2MIPConfig2;

void net_init(void);
void net_set_mode(uint32_t net_if, uint32_t mode);
void net_interface_up(uint32_t net_if);
void net_interface_down(uint32_t net_if);
void net_add_wilc_netif(void);
void net_remove_wilc_netif(void);
int net_in_tcpip_task(void);

#endif /* NET_INIT_H_INCLUDED */
