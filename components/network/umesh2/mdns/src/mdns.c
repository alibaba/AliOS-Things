
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

#include "utils.h"
#include "ulog/ulog.h"
#include "network/network.h"
#include "network/umesh2/mdns/mdns.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

#define MDNS_PKT_MAXSZ 1024 // read/write buffer size

typedef void *multicast_if;

static uint64_t hal_now_ms()
{
    return aos_now_ms();
}

struct timeval os_deadline = {
    .tv_sec = 0,
    .tv_usec = 100000,
};

struct mdns_svc {
    char *name;
    enum mdns_type type;
    mdns_announce_callback announce_callback;

    void *p_cookie;
    struct mdns_svc *next;
};

struct mdns_conn {
    int sock;
    multicast_if if_addr;
    struct mdns_ip mdns_ip;  // IP address and family of the interface
};

struct mdns_ctx {
    struct mdns_conn *conns;
    uint32_t nb_conns;
    //struct sockaddr_in6 addr;
    struct sockaddr_storage addr;
    struct mdns_svc *services;
};

static int32_t mdns_resolve(struct mdns_ctx *ctx, const char *addr,  uint16_t  port);
static int32_t mdns_write_hdr(uint8_t *buf, uint16_t *left, const struct mdns_hdr *hdr);
static int32_t strrcmp(const char *, const char *);

static uint32_t mdns_list_interfaces(multicast_if **pp_intfs, struct mdns_ip **pp_mdns_ips, uint32_t *p_nb_intf,
                                     int ai_family)
{
    multicast_if *intfs;
    struct mdns_ip *mdns_ips;
    *pp_intfs = intfs = hal_malloc(sizeof(*intfs));
    if (intfs == NULL) {
        return (MDNS_ERROR);
    }
    memset(intfs, 0, sizeof(*intfs));
    *pp_mdns_ips = mdns_ips = hal_malloc(sizeof(*mdns_ips));
    if (mdns_ips == NULL) {
        hal_free(intfs);
        *pp_intfs = NULL;
        return (MDNS_ERROR);
    }
    memset(mdns_ips, 0, sizeof(*mdns_ips));
    *p_nb_intf = 1;
    return (0);
}

static int32_t mdns_resolve(struct mdns_ctx *ctx, const char *addr,  uint16_t  port)
{
    char buf[6];
    int ret;
    struct addrinfo hints, *res = NULL;
    multicast_if *ifaddrs = NULL;
    struct mdns_ip *mdns_ips = NULL;
    uint32_t i;
    int status;

    sprintf(buf, "%hu", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
    ret = getaddrinfo(addr, buf, &hints, &res);
    if (ret != 0) {
        return (MDNS_LKPERR);
    }

    status = mdns_list_interfaces(&ifaddrs, &mdns_ips, &ctx->nb_conns, res->ai_family);
    if (status < 0) {
        freeaddrinfo(res);
        return (status);
    }
    if (ctx->nb_conns == 0) {
        freeaddrinfo(res);
        return (MDNS_NETERR);
    }
    memcpy(&ctx->addr, res->ai_addr, res->ai_addrlen);

    ctx->conns = hal_malloc(ctx->nb_conns * sizeof(*ctx->conns));
    if (ctx->conns == NULL) {
        hal_free(ifaddrs);
        hal_free(mdns_ips);
        freeaddrinfo(res);
        return (MDNS_ERROR);
    }
    for (i = 0; i < ctx->nb_conns; ++i) {
        ctx->conns[i].sock = INVALID_SOCKET;
        ctx->conns[i].if_addr = ifaddrs[i];
        ctx->conns[i].mdns_ip = mdns_ips[i];
        ctx->conns[i].mdns_ip.family = res->ai_family;
    }
    hal_free(ifaddrs);
    hal_free(mdns_ips);
    freeaddrinfo(res);
    return (0);
}


int mdns_init(struct mdns_ctx **p_ctx, const char *addr, uint16_t  port)
{
    int res;

    struct mdns_ctx *ctx;

    if (p_ctx == NULL) {
        return (MDNS_ERROR);
    }
    *p_ctx = NULL;

    ctx = hal_malloc(sizeof(struct mdns_ctx));
    if (ctx == NULL) {
        return (MDNS_ERROR);
    }
    ctx->services = NULL;
    ctx->conns = NULL;
    ctx->nb_conns = 0;


    res = mdns_resolve(ctx, addr, port);
    if (res < 0) {
        return mdns_destroy(ctx), (res);
    }

    for (uint32_t i = 0; i < ctx->nb_conns; ++i) {
        ctx->conns[i].sock = lwip_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        if (ctx->conns[i].sock < 0) {
            log_e("lwip_socket err res = %d", res);
            return mdns_destroy(ctx), (MDNS_NETERR);
        }
        //res = lwip_bind(ctx->conns[i].sock, (const struct sockaddr_in6 *)&ctx->addr, sizeof(&ctx->addr));
        res = lwip_bind(ctx->conns[i].sock, (const struct sockaddr_in6 *)&ctx->addr, sizeof(ctx->addr));
        if (res < 0) {
            log_e("lwip_bind err res = %d", res);
            return mdns_destroy(ctx), (MDNS_NETERR);
        }

    }
    *p_ctx = ctx;
    return (0);
}

int mdns_destroy(struct mdns_ctx *ctx)
{
    if (ctx != NULL) {
        for (uint32_t i = 0; i < ctx->nb_conns; ++i) {
            struct mdns_conn *conn = &ctx->conns[i];
            if (conn->sock != INVALID_SOCKET) {
                close(conn->sock);
                conn->sock = INVALID_SOCKET;
            }
        }
        hal_free(ctx->conns);
        if (ctx->services) {
            struct mdns_svc *svc;

            while ((svc = ctx->services)) {
                ctx->services = ctx->services->next;
                if (svc->name) {
                    hal_free(svc->name);
                }
                hal_free(svc);
            }
        }

        hal_free(ctx);
    }

    return (0);
}

static int32_t mdns_write_hdr(uint8_t *ptr, uint16_t *left, const struct mdns_hdr *hdr)
{
    uint8_t *p = ptr;

    p = write_u16(p, left, hdr->id);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    p = write_u16(p, left, hdr->flags);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    p = write_u16(p, left, hdr->num_qn);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    p = write_u16(p, left, hdr->num_ans_rr);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    p = write_u16(p, left, hdr->num_auth_rr);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    p = write_u16(p, left, hdr->num_add_rr);
    if (p == NULL) {
        return (MDNS_ERROR);
    }
    return (p - ptr);
}


int mdns_send(const struct mdns_ctx *ctx, const struct mdns_hdr *hdr, const struct mdns_entry *entries)
{
    const struct mdns_entry *entry = entries;
    uint32_t n = 0, l, r;
    uint8_t *buf;
    uint16_t buf_len = MDNS_PKT_MAXSZ;
    if (!entries) {
        return (MDNS_ERROR);
    }

    buf = hal_malloc(buf_len);
    if (buf == NULL) {
        return MDNS_STDERR;
    }
    memset(buf, 0, buf_len);
    l = mdns_write_hdr(buf, &buf_len, hdr);
    if (l < 0) {
        hal_free(buf);
        return (MDNS_ERROR);
    }

    n += l;
    for (entry = entries; entry; entry = entry->next) {
        l = mdns_write(buf + n, &buf_len, entry, (hdr->flags & FLAG_QR) > 0);
        if (l < 0) {
            hal_free(buf);
            return (MDNS_STDERR);
        }

        n += l;
        if (n > MDNS_PKT_MAXSZ) {
            log_e("mdns packet too large!give up");
            hal_free(buf);
            return (MDNS_STDERR);
        }
    }
    for (uint32_t i = 0; i < ctx->nb_conns; ++i) {
        r = lwip_sendto(ctx->conns[i].sock, (const char *) buf, n, 0,
                        (const struct sockaddr *) &ctx->addr, ss_len(&ctx->addr));

        // r = sendto(ctx->conns[i].sock, (const char *) buf, n, 0,
        //        (const struct sockaddr *) &ctx->addr, ss_len(&ctx->addr));
        if (r < 0) {
            hal_free(buf);
            return (MDNS_NETERR);
        }
    }
    hal_free(buf);
    return (0);
}

static void mdns_entries_free(struct mdns_entry *entries)
{
    struct mdns_entry *entry;

    while ((entry = entries)) {
        entries = entries->next;
        mdns_free(entry);
        hal_free(entry);
    }
}

static const uint8_t *mdns_read_header(const uint8_t *ptr, uint32_t *n, struct mdns_hdr *hdr)
{
    if (n <= sizeof(struct mdns_hdr)) {
        //errno = ENOSPC;
        return NULL;
    }
    ptr = read_u16(ptr, n, &hdr->id);
    ptr = read_u16(ptr, n, &hdr->flags);
    ptr = read_u16(ptr, n, &hdr->num_qn);
    ptr = read_u16(ptr, n, &hdr->num_ans_rr);
    ptr = read_u16(ptr, n, &hdr->num_auth_rr);
    ptr = read_u16(ptr, n, &hdr->num_add_rr);
    return ptr;
}

static int32_t mdns_recv(const struct mdns_conn *conn, struct mdns_hdr *hdr, struct mdns_entry **entries)
{
    uint8_t *buf;
    uint32_t num_entry, n;
    uint32_t length;
    struct mdns_entry *entry;

    *entries = NULL;
    buf = hal_malloc(MDNS_PKT_MAXSZ);
    if (buf == NULL) {
        return MDNS_STDERR;
    }
    memset(buf, 0, MDNS_PKT_MAXSZ);

    if ((length = lwip_recv(conn->sock, (char *) buf, MDNS_PKT_MAXSZ, 0)) < 0) {
        hal_free(buf);
        return (MDNS_NETERR);
    }
    n = (uint32_t)length;
    const uint8_t *ptr = mdns_read_header(buf, &n, hdr);
    if (ptr == NULL) {
        hal_free(buf);
        return (MDNS_NETERR);
    }


    num_entry = hdr->num_qn + hdr->num_ans_rr + hdr->num_add_rr;
    for (uint32_t i = 0; i < num_entry; ++i) {
        entry = hal_malloc(sizeof(struct mdns_entry));
        if (!entry) {
            goto err;
        }
        memset(entry, 0, sizeof(struct mdns_entry));

        ptr = mdns_read(ptr, &n, buf, entry, i >= hdr->num_qn);
        if (!ptr) {
            log_e("mdns_read err");
            mdns_free(entry);
            hal_free(entry);
            goto err;
        }

        entry->next = *entries;
        *entries = entry;
    }
    if (*entries == NULL) {
        hal_free(buf);
        return (MDNS_ERROR);
    }

    hal_free(buf);
    return (0);
err:
    log_e("mdns_recv err");
    mdns_entries_free(*entries);
    *entries = NULL;
    hal_free(buf);
    return (MDNS_ERROR);
}


void mdns_print(const struct mdns_entry *entry)
{
    hal_printf("[");
    while (entry) {
        mdns_item_print(entry);
        if (entry->next) {
            hal_printf(",");
        }
        entry = entry->next;
    }
    hal_printf("]\n");
}


static int32_t strrcmp(const char *s1, const char *s2)
{
    uint32_t m, n;

    if (!s1 || !s2) {
        return (1);
    }
    m = strlen(s1);
    n = strlen(s2);
    if (n > m) {
        return (1);
    }
    return (strncmp(s1 + m - n, s2, n));
}

static int32_t mdns_listen_probe_network(const struct mdns_ctx *ctx, const char *const names[],
        uint32_t nb_names, enum mdns_match_type match_type, mdns_listen_callback callback,
        void *p_cookie)
{
    struct mdns_hdr ahdr = {0};
    struct mdns_entry *entries;
    struct mdns_svc *svc;
    fd_set working_set;
    struct timeval timeout;
    int32_t max_fd = 0;
    int32_t r;
    int i;
    for (i = 0; i < ctx->nb_conns; ++i) {
        if (max_fd < ctx->conns[i].sock) {
            max_fd = ctx->conns[i].sock;
        }

    }

    FD_ZERO(&working_set);

    FD_SET(max_fd, &working_set);
    timeout.tv_sec = MDNS_POLL_TIMEOUT_S;
    timeout.tv_usec = 0;


    r = select(max_fd + 1, &working_set, NULL, NULL, &timeout);
    if (r <= 0) {
        return r;
    }

    for (i = 0; i < ctx->nb_conns; ++i) {
        if (FD_ISSET(ctx->conns[i].sock, &working_set) == 0) {
            continue;
        }

        r = mdns_recv(&ctx->conns[i], &ahdr, &entries);
        if (r < 0) {
            log_e("---MDNS_NETERR -----");
            mdns_entries_free(entries);
            continue;
        }

        if (ahdr.num_ans_rr + ahdr.num_add_rr == 0) {
            mdns_entries_free(entries);
            continue;
        }

        if (match_type == MDNS_MATCH_ALL) {
            callback(p_cookie, r, entries);
        } else {
            for (i = 0; i < nb_names; ++i) {
                for (struct mdns_entry *entry = entries; entry; entry = entry->next) {
                    if (!strrcmp(entry->name, names[i])) {
                        callback(p_cookie, r, entries);
                        break;
                    } else {
                        log_w("service name not match");
                    }
                }
            }
        }

        mdns_entries_free(entries);
    }

    return 0;
}

int mdns_announce(struct mdns_ctx *ctx, const char *service, enum mdns_type type,
                  mdns_announce_callback callback, void *p_cookie)
{
    if (!callback) {
        return (MDNS_ERROR);
    }

    struct mdns_svc *svc = (struct mdns_svc *) hal_malloc(sizeof(struct mdns_svc));
    if (!svc) {
        return (MDNS_ERROR);
    }
    memset(svc, 0, sizeof(struct mdns_svc));

    svc->name = strdup(service);
    svc->type = type;
    svc->announce_callback = callback;
    svc->p_cookie = p_cookie;
    svc->next  = ctx->services;

    ctx->services = svc;
    return (0);
}

int mdns_start(const struct mdns_ctx *ctx, const char *const names[],
                uint32_t  nb_names, enum mdns_type type,  uint32_t  interval, enum mdns_match_type match_type,
               mdns_stop_func stop, mdns_listen_callback callback, void *p_cookie)
{
    if (ctx->nb_conns == 0) {
        return (MDNS_ERROR);
    }
    int r;
    uint64_t t1, t2;

    struct mdns_svc *svc = NULL;

    for (uint32_t i = 0; i < ctx->nb_conns; ++i) {
        if (setsockopt(ctx->conns[i].sock, SOL_SOCKET, SO_SNDTIMEO, (const void *) &os_deadline, sizeof(os_deadline)) < 0) {
            log_e("setsockopt err");
            return (MDNS_NETERR);
        }
    }

    for (svc = ctx->services; svc; svc = svc->next) {
        svc->announce_callback(svc->name, svc->p_cookie);
    }
    for (t1 = t2 = hal_now_ms(); stop(p_cookie) == false; t2 = hal_now_ms()) {
        if (t2 - t1 >= interval * 1000) {
            log_d("send self srv");
            for (svc = ctx->services; svc; svc = svc->next) {
                svc->announce_callback(svc->name, svc->p_cookie);
            }
            t1 = t2;
        }
        mdns_listen_probe_network(ctx, names, nb_names, match_type, callback, p_cookie);
    }

    return (0);
}
