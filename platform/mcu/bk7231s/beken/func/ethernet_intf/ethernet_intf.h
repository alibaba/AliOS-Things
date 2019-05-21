#ifndef __ETHERNET_INTF_H__
#define __ETHERNET_INTF_H__ 

#include "mac.h"

extern char wireless_ip[4];
extern char wireless_mask[4];
extern char wireless_gw[4];

extern int hwaddr_aton(const char *txt, u8 *addr);

#endif
