/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A MAC protocol implementation that does not do anything.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

//#include "net/mac/mac.h"
//#include "uip.h"
//#include "uipopt.h"
#include "radio.h"
#include "soc_types.h"
#include "wifinetstack.h"

#ifndef __IEEE80211_MAC_H__
#define __IEEE80211_MAC_H__

#include "attrs.h"
extern const struct mac_driver ieee80211_mac;

void rx_process_data (u8 id) ATTRIBUTE_SECTION_FAST;
int is_protectframe(u16 fc) ;
int is_data(u16 fc) ;
int is_mgmt(u16 fc) ;
s32 tx_nulldata (int pwmode, u8 id);
void tx_pspoll ();
static inline unsigned compare_ether_addr(const u8 *addr1, const u8 *addr2) ;
static inline bool ether_addr_equal(const u8 *addr1, const u8 *addr2) ;
static inline bool valid_multicast_addr(const u8 *addr) ;
void mac_output(u8 ifid, u8 *data, u32 len) ATTRIBUTE_SECTION_FAST;


static inline unsigned compare_ether_addr(const u8 *addr1, const u8*addr2)
{
    const u16 *a = (const u16 *) addr1;
    const u16 *b = (const u16 *) addr2;

    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) != 0;
}

static inline bool ether_addr_equal(const u8 *addr1, const u8 *addr2)
{
    return !compare_ether_addr(addr1, addr2);
}

static inline bool valid_multicast_addr(const u8 *addr)
{
    return ((addr[0]==0x01) && (addr[1]==0x00) && (addr[2]==0x5e) && (addr[3] == (addr[3]&0x7f)));
}

static inline bool valid_multicast_addr_ipv6(const u8 *addr,const u8 *local_mac)
{
   return ((addr[0]==0x33) && (addr[1]==0x33) && (addr[2]==0xff) && (addr[3]==local_mac[3]) && (addr[4]==local_mac[4]) && (addr[5]==local_mac[5]));	//fixedme!! for valid ipv6 multicast bits
}

typedef struct eth_header {
    u8 dest_addr[6];
    u8 src_addr[6];
    u16 type;
    u8 data[0];
} eth_header;

#define BROADCAST_80211_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
#define MULTICAST_IPV6_ADDR {0x33, 0x33, 0x00, 0x00, 0x00, 0x01}

int is_data(u16 fc);

void mac_raw_dump(char *data, s32 len);

#endif /* __IEEE80211_MAC_H__ */
