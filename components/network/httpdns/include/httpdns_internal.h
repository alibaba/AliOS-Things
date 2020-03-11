/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef HTTPDNS_INTERNAL_H
#define HTTPDNS_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "iplist.h"

typedef void * httpdns_mutex_t;

typedef struct mem_struct {
    size_t size;
    char *memory;
} memory_t;

typedef struct {
   char * url;
   int conn_timeout;
   int timeout;
   memory_t response;
} http_ctx_t;

struct dns_cache {
    struct list_head host_list;
    char * host_name;
    struct timespec ts;
};

typedef struct httpdns_connection {
    http_ctx_t ctx;
    struct dns_cache * dns;
    int prefer_local;
} httpdns_connection_t;

/* http wrapper */
int http_ctx_init(http_ctx_t *ctx, char *url, int conn_timeout, int timeout);

memory_t* http_ctx_get(http_ctx_t *ctx);

void http_ctx_destroy(http_ctx_t *ctx);


typedef void * (*httpdns_routine_t)(void * arg);

/* os wrapper */
void httpdns_thread_msleep(const unsigned int millisec);

httpdns_mutex_t httpdns_mutex_create_recursive(void);

void httpdns_mutex_lock(httpdns_mutex_t mutex);

void httpdns_mutex_unlock(httpdns_mutex_t mutex);

void httpdns_mutex_delete(httpdns_mutex_t mutex);

void get_clock_time(struct timespec *time);

void httpdns_resolv_thread(httpdns_connection_t *conn, httpdns_routine_t routine, int async);

#ifdef __cplusplus
}
#endif

#endif