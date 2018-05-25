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

//#include "process.h"
#include "soc_types.h"
#include "ieee80211.h"
#include "wificonf.h"

#ifndef __IEEE80211_MGMT_H__
#define __IEEE80211_MGMT_H__

/*---------------------------------------------------------------------------*/

typedef struct MGMT_MSG {
    u32 wsid:8;
    u32 action:8;
    u32 resv1:8;
    u32 resv2:8;
} MGMT_MSG;

typedef struct MGMT_WSIDTIMEREVENT {
    u32 wsid_nexttime[2];
    u32 next_wsid;
} MGMT_NEXTTIMER;

void Scan_AP_Task( void *args );
void wifi_mgmt_task(void *args);
void rx_process_mgmt() ;
u8 getAvailableIndex();

s32 tx_probe_req(u8 wsid);
s32 tx_authentication_req_seq1 (u8 wsid, u8 alg);
void softap_rx_assoc_req(struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void rx_association_rsp (struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void softap_rx_reassoc_req(struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void softap_rx_probe_request(struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void rx_probe_rsp (struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void rx_process_check_ap(struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void rx_mgmt_disassoc(struct ieee80211_hdr_3addr * mgmt, u8 bssid);
void rx_mgmt_authentication (struct ieee80211_hdr_3addr * mgmt, u8 wsid);
void rx_mgmt_deauth(struct ieee80211_hdr_3addr * mgmt, u8 bssid);
void rx_mgmt_action (struct ieee80211_hdr_3addr * mgmt, u8 bssid);
void rx_probe_rsp_handler (struct ieee80211_hdr_3addr *mgmt, u8 wsid);
void mgmt_msg_post(u8 isisr, u8 wsid, u8 action);
int wifi_connect_start(u8 staid, u8 activeconn, void (*cbfn)(WIFI_RSP*));

typedef void (*mgmt_Handler)(struct ieee80211_hdr_3addr *m, u8 wsid);

#endif /* __IEEE80211_MGMT_H__ */
