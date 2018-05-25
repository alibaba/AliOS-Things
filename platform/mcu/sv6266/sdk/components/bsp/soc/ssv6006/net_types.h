#ifndef NET_TYPES_H
#define NET_TYPES_H

#include "soc_types.h"

#define	ETHER_ADDR_LEN	6
#define	LLC_HEADER_LEN	6
#define	ETHER_TYPE_LEN	2
#define	ETHER_HDR_LEN	14
#define	ETHER_MAC_LEN	12		//802.3 DA+SA

#define ETH_ADDR_FORMAT                 "%02X:%02X:%02X:%02X:%02X:%02X"
#define ETH_ADDR(a)                     ((ETHER_ADDR *)(a))->addr[0], ((ETHER_ADDR *)(a))->addr[1], \
                                        ((ETHER_ADDR *)(a))->addr[2], ((ETHER_ADDR *)(a))->addr[3], \
                                        ((ETHER_ADDR *)(a))->addr[4], ((ETHER_ADDR *)(a))->addr[5]

#define IS_EQUAL_MACADDR(m1, m2)        (!memcmp((m1), (m2), ETHER_ADDR_LEN))

struct ETHER_ADDR_st
{
    u8      addr[ETHER_ADDR_LEN];
};


typedef struct ETHER_ADDR_st        ETHER_ADDR;



#endif /* end of include guard: NET_TYPES_H */
