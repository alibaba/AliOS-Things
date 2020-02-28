
#ifndef __MDNS_RR_H__
#define __MDNS_RR_H__

#include <stdint.h>
#include "network/network.h"

# ifdef __cplusplus
extern "C" {
# endif

#define IPV4_ADDRSTRLEN 16
#define IPV6_ADDRSTRLEN 46

#define MDNS_TXT_MAX_LEN 128

enum mdns_type {
    RR_A    = 0x01,
    RR_PTR  = 0x0C,
    RR_TXT  = 0x10,
    RR_AAAA = 0x1C,
    RR_SRV  = 0x21,
};

enum mdns_class {
    RR_IN = 0x01,
};

struct mdns_data_srv {
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    char     *target;
};

struct mdns_data_txt {
    char txt[MDNS_TXT_MAX_LEN];
    struct mdns_data_txt *next;

};

struct mdns_data_ptr {
    char *domain;
};

struct mdns_data_a {
    char   addr_str[IPV4_ADDRSTRLEN];
    struct in_addr addr;
};

struct mdns_data_aaaa {
    char   addr_str[IPV6_ADDRSTRLEN];
    struct in6_addr addr;
};

union mdns_data {
    struct mdns_data_srv  SRV;
    struct mdns_data_txt  *TXT;
    struct mdns_data_ptr  PTR;
    struct mdns_data_a    A;
    struct mdns_data_aaaa AAAA;
};

struct mdns_entry {
    char     *name;
    uint16_t type;
    uint16_t class : 15;
    uint16_t msbit : 1; // unicast query | cache flush (RFC 6762)

    /* Answers only */
    uint32_t ttl;
    uint16_t data_len;
    union    mdns_data data;
    struct mdns_entry *next;
};

const uint8_t *mdns_read(const uint8_t *, uint32_t *, const uint8_t *, struct mdns_entry *, int8_t ans);
uint32_t mdns_write(uint8_t *, const struct mdns_entry *, int8_t ans);
void mdns_item_print(const struct mdns_entry *);
void mdns_free(struct mdns_entry *);

# ifdef __cplusplus
}
# endif

#endif /* __MDNS_RR_H__ */
