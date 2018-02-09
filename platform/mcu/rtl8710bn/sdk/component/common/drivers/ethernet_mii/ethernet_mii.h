#ifndef __MII_ETHERNETIF_H__
#define __MII_ETHERNETIF_H__

#include "lwip_netconf.h"

#define MII_TX_DESC_CNT		4
#define MII_RX_DESC_CNT		10

#define ETHERNET_IDX (NET_IF_NUM - 1)
#define WLAN_IDX 0

typedef void (*link_up_down_callback)(int blinkup);

#endif // __MII_ETHERNETIF_H__