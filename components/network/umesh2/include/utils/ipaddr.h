

#ifndef UMESH_IPADDR_H_
#define UMESH_IPADDR_H_

#include <stdint.h>

int umesh_get_ipv6(const uint8_t *mac, uint8_t *ip6_out);

int umesh_get_ipv6_mcast(uint8_t *ip6_out);
#endif /* UMESH_IPADDR_H_ */
