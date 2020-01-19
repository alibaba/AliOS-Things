#include <stdint.h>
#include <stdlib.h>
#include "define.h"
#include "log.h"

extern const uint8_t UMESH_BSSID[];

int umesh_get_ipv6(const uint8_t *mac, uint8_t *ip6_out)
{
    if (mac == NULL || ip6_out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    ip6_out[0] = 0xfe;
    ip6_out[1] = 0x80;
    memcpy(&ip6_out[8], mac, 3);
    ip6_out[11] = 0xff;
    ip6_out[12] = 0xfe;
    memcpy(&ip6_out[13], mac + 3, 3);
    ip6_out[8] = (ip6_out[8] & (~BIT1)) | ~(ip6_out[8] & BIT1);
    return 0;
}

int umesh_get_ipv6_mcast(uint8_t *ip6_out)
{
    if (ip6_out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    ip6_out[0] = 0xff;
    ip6_out[1] = 0x08;
    memcpy(&ip6_out[4], UMESH_BSSID, 4);

    return 0;
}

int umesh_resolve_mac_addr(uint8_t *ip6, uint8_t *mac)
{
    if (ip6 == NULL || mac == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    log_hex("ip:", ip6, 16);
    ip6[8] = (ip6[8] & (~BIT1)) | ~(ip6[8] & BIT1);
    memcpy(mac, &ip6[8], 3);
    memcpy(mac + 3, &ip6[13], 3);
    log_hex("get mac from ip", mac, 6);
    return 0;

}