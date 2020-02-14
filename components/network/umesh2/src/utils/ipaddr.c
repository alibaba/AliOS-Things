#include <stdint.h>
#include <stdlib.h>
#include "define.h"
#include "log.h"

extern const uint8_t UMESH_BSSID[];

/*
    for (index = 0; index < 3; index++) {
        memset(ip6_addr.m8, 0, sizeof(ip6_addr.m8));
        ip6_addr.m32[0] = htonl(0xfc000000);
        ip6_addr.m32[1] = htonl(main_netid);
        if (index == 0) {
            ip6_addr.m32[3] = \
                htonl((get_sub_netid(umesh_get_meshnetid()) << 16) | umesh_mm_get_local_sid());
        } else if (index == 1) {
            memcpy(&ip6_addr.m8[8], umesh_mm_get_local_uuid(), 8);
        } else {  // mcast addr
            ip6_addr.m8[0] = 0xff;
            ip6_addr.m8[1] = 0x08;
            ip6_addr.m8[6] = (uint8_t)(main_netid >> 8);
            ip6_addr.m8[7] = (uint8_t)main_netid;
        }
        netif_ip6_addr_set_state(&g_la_state.adpif, index, IP6_ADDR_INVALID);
        IP6_ADDR(&addr6, ip6_addr.m32[0], ip6_addr.m32[1],
                 ip6_addr.m32[2], ip6_addr.m32[3]);
        ip6_addr_copy(*(ip_2_ip6(&g_la_state.adpif.ip6_addr[index])), addr6);
        netif_ip6_addr_set_state(&g_la_state.adpif, index, IP6_ADDR_VALID);
    }

*/

int umesh_get_ipv6(const uint8_t *mac, uint8_t *ip6_out)
{
    if (mac == NULL || ip6_out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    memset(ip6_out, 0, 16);
    ip6_out[0] = 0xfc;

    memcpy(&ip6_out[4], UMESH_BSSID, 6);
    memcpy(&ip6_out[10], mac, 6);
    return 0;
}

// int umesh_get_ipv6(const uint8_t *mac, uint8_t *ip6_out)
// {
//     if (mac == NULL || ip6_out == NULL) {
//         return UMESH_ERR_NULL_POINTER;
//     }
//     ip6_out[0] = 0xfe;
//     ip6_out[1] = 0x80;
//     memcpy(&ip6_out[8], mac, 3);
//     ip6_out[11] = 0xff;
//     ip6_out[12] = 0xfe;
//     memcpy(&ip6_out[13], mac + 3, 3);
//     log_hex("ip6 pre", ip6_out, 16);
//      ip6_out[8] = (ip6_out[8] & (~BIT1)) | ((~ip6_out[8]) & BIT1);
//     log_hex("ip6 final", ip6_out, 16);
//     return 0;
// }

int umesh_get_ipv6_mcast(uint8_t *ip6_out)
{
    if (ip6_out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    memset(ip6_out, 0, 16);
    ip6_out[0] = 0xff;
    ip6_out[1] = 0x02;
    ip6_out[15] = 0x01;
    return 0;
}
/*fd f8 93 30 fe 30*/
/*b0 f8 93 30 fe 30*/
// int umesh_resolve_mac_addr(uint8_t *ip6, uint8_t *mac)
// {
//     if (ip6 == NULL || mac == NULL) {
//         return UMESH_ERR_NULL_POINTER;
//     }
//     log_hex("ip:", ip6, 16);
//     ip6[8] = (ip6[8] & (~BIT1)) | ((~ip6[8]) & BIT1);
//     memcpy(mac, &ip6[8], 3);
//     memcpy(mac + 3, &ip6[13], 3);
//     log_hex("get mac from ip", mac, 6);
//     return 0;

// }
int umesh_resolve_mac_addr(uint8_t *ip6, uint8_t *mac)
{
    if (ip6 == NULL || mac == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    log_hex("ip:", ip6, 16);

    memcpy(mac, &ip6[10], 6);
    log_hex("get mac from ip", mac, 6);
    return 0;

}