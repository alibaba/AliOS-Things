#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iplist.h"

#include "ulog/ulog.h"

#define TAG "httpdns"
#define HTTPDNS_INFO(fmt,arg...)  LOGI(TAG, fmt,##arg)

void iplist_init(struct list_head * ips)
{
    INIT_LIST_HEAD(ips);
}

int iplist_add(struct list_head *ips, const char *ip)
{
    ipv4_list_t * t = malloc(sizeof(ipv4_list_t));
    if(NULL != t) {
        memset(t, 0, sizeof(ipv4_list_t));
        snprintf(t->ipv4_addr, INET_ADDRSTRLEN, "%s", ip);
        list_add_tail(&(t->list), ips);
        return 0;
    }

    return -1;
}

void iplist_print(struct list_head *ips)
{
    ipv4_list_t *s;
    if(NULL == ips) {
        return;
    }
    list_for_each_entry(s, ips, list) {
        HTTPDNS_INFO("<HTTPDNS> %s : print ipv4 = %s\n", __func__, s->ipv4_addr);
    }
}

struct list_head * iplist_dup(struct list_head *dips, struct list_head *sips)
{

    ipv4_list_t *s;
    if(NULL == dips || NULL == sips) {
        return NULL;
    }
    iplist_init(dips);
    list_for_each_entry(s, sips, list) {
        iplist_add(dips, s->ipv4_addr);
    }
    return dips;
}

ipv4_list_t * iplist_get(struct list_head *ips, int index)
{
    int i = 0;
    ipv4_list_t *s;
    if(NULL == ips || index < 0) {
        return NULL;
    }
    list_for_each_entry(s, ips, list) {
        if(i == index) {
            return s;
        } else {
            i++;
        }
    }
    return NULL;
}

int iplist_rotate(struct list_head *ips)
{
    ipv4_list_t * tmp;
    if(NULL == ips)
        return 0;

    if(iplist_size(ips) <= 1)
        return 0;

    tmp = list_first_entry(ips, ipv4_list_t, list);
    list_del_init(&tmp->list);
    list_add_tail(&tmp->list, ips);
    return 1;
}

int iplist_size(struct list_head *ips)
{
    ipv4_list_t *s;
    int i = 0;
    list_for_each_entry(s, ips, list) {
        i++;
    }
    return i;
}

void iplist_free(struct list_head *ips)
{
    ipv4_list_t *s, *t;
    if(NULL == ips) {
        return;
    }
    list_for_each_entry_safe(s, t, ips, list) {
        list_del(&(s->list));
        free(s);
    }
}

