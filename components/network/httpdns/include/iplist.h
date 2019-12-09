#ifndef IPV4_LIST_H
#define IPV4_LIST_H

#include <network/network.h>
#include "list.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct ipv4_list {
    struct list_head list;
    char ipv4_addr[INET_ADDRSTRLEN];
} ipv4_list_t;

void iplist_init(struct list_head * ips);

int iplist_add(struct list_head *ips, const char *ip);

void iplist_print(struct list_head *ips);

int iplist_rotate(struct list_head *ips);

struct list_head * iplist_dup(struct list_head *dips, struct list_head *sips);

ipv4_list_t * iplist_get(struct list_head *ips, int index);

int iplist_size(struct list_head *ips);

void iplist_free(struct list_head *ips);

#ifdef __cplusplus
}
#endif

#endif
