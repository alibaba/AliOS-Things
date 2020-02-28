
#ifndef __MDNS_H__
#define __MDNS_H__

#include "network/umesh2/mdns/mdns_rr.h"

# ifdef __cplusplus
extern "C" {
# endif


struct mdns_ctx;

#define MDNS_PORT        5353
#define MDNS_ADDR_IPV6   "FF02::FB"

#define GET_RCODE(x)    (x&0x000f)
#define SET_RCODE(x,c)  (x|(c&0x000f))

#define GET_OPCODE(x)   (x&0x7800)
#define SET_OPCODE(x,c) (x|(c&0x7800))

enum mdns_hdr_flag {
    FLAG_QR = 1 << 15, // Question/Response
    FLAG_AA = 1 << 10, // Authoritative Answer
    FLAG_TC = 1 <<  9, // Truncated Response
    FLAG_RD = 1 <<  8, // Recursion Desired
    FLAG_RA = 1 <<  7, // Recursion Available
    FLAG_Z  = 1 <<  6, // Reserved
    FLAG_AD = 1 <<  5, // Authentic Data
    FLAG_CD = 1 <<  4, // Checking Disabled
};

enum mdns_match_type {
    MDNS_MATCH_NAME,
    MDNS_MATCH_ALL,
};

struct mdns_hdr {
    uint16_t id;
    uint16_t flags;
    uint16_t num_qn;
    uint16_t num_ans_rr;
    uint16_t num_auth_rr;
    uint16_t num_add_rr;
};

struct mdns_ip {
    unsigned int family;
    union {
        struct {
            struct in_addr addr;
        } ipv4;
        struct {
            struct in6_addr addr;
        } ipv6;
    };
};

typedef void (*mdns_listen_callback)(void *, int, const struct mdns_entry *);
typedef void (*mdns_announce_callback)(void *, int, const struct mdns_ip *, const struct mdns_entry *);

typedef int (*mdns_stop_func)(void *);

int mdns_init(struct mdns_ctx **ctx, const char *addr, unsigned short port);

int mdns_destroy(struct mdns_ctx *ctx);

int mdns_send(const struct mdns_ctx *ctx, const struct mdns_hdr *hdr, const struct mdns_entry *entries);

void mdns_print(const struct mdns_entry *);

int mdns_announce(struct mdns_ctx *ctx, const char *service, enum mdns_type type,
                  mdns_announce_callback callback, void *context);

int mdns_start(const struct mdns_ctx *ctx, const char *const names[],
               unsigned int nb_names, enum mdns_type type, unsigned int interval, enum mdns_match_type match_type,
               mdns_stop_func stop, mdns_listen_callback callback, void *p_cookie);


# ifdef __cplusplus
}
# endif

#endif /* __MDNS_H__ */
