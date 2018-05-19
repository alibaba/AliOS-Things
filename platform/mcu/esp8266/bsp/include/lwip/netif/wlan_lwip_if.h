/*
 *  Copyright (c) 2010-2011 	Espressif System
 *   
*/

#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#include "lwip/err.h"

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif, struct pbuf *p);

#ifndef IOT_SIP_MODE
sint8 ieee80211_output_pbuf(struct netif *ifp, struct pbuf* pb);
#else
sint8 ieee80211_output_pbuf(struct ieee80211_conn *conn, esf_buf *eb);
#endif

#endif /*  _WLAN_LWIP_IF_H_ */
