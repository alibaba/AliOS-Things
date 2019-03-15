/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NET_WLAN_ETHERNETIF_H_
#define _NET_WLAN_ETHERNETIF_H_

#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

enum wlan_mode {
  WLAN_MODE_STA = 0,   /* Infrastructure station */
  WLAN_MODE_HOSTAP,    /* Software Access Point */
  WLAN_MODE_MONITOR,   /* Monitor mode */
  WLAN_MODE_NUM,
  WLAN_MODE_INVALID = WLAN_MODE_NUM
};

struct netif *ethernetif_create(enum wlan_mode mode);
void ethernetif_delete(struct netif *nif);
err_t ethernetif_input(struct netif *nif, struct pbuf *p);
#if (LWIP_MBUF_SUPPORT == 0)
err_t ethernetif_raw_input(struct netif *nif, uint8_t *data, u16_t len);
#endif
enum wlan_mode ethernetif_get_mode(struct netif *nif);

#ifdef __cplusplus
}
#endif

#endif /* _NET_WLAN_ETHERNETIF_H_ */
