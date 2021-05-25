#ifndef __NETDEV_IPADDR_H__
#define __NETDEV_IPADDR_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Only supports the IPV4 protocol */
#ifndef NETDEV_IPV4
#define NETDEV_IPV4           1
#endif

#ifndef NETDEV_IPV6
#define NETDEV_IPV6           0
#endif

#ifdef NETDEV_IPV4
/** IPv4 only: set the IP address given as an u32_t */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** IPv4 only: get the IP address as an u32_t */
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

#define IP4ADDR_STRLEN_MAX  16
#endif /* NETIF_IPV4 */

#if NETDEV_IPV4 && NETDEV_IPV6
/* IP address types for use in ip_addr_t.type member */
enum netdev_ip_addr_type
{
    /** IPv4 */
    IPADDR_TYPE_V4 = 0U,
    /** IPv6 */
    IPADDR_TYPE_V6 = 6U,
    /** IPv4+IPv6 ("dual-stack") */
    IPADDR_TYPE_ANY = 46U
};

#endif /* NETIF_IPV4 && NETIF_IPV6 */

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_IPADDR_H__ */
