/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _HTTP_DNS_OPT_H_
#define _HTTP_DNS_OPT_H_

#ifndef HTTP_DNS_DISABLED
#define HTTP_DNS_DISABLED 0
#endif

#ifndef HTTP_DNS_USER_ID
#define HTTP_DNS_USER_ID "100000"
#endif

#ifndef HTTP_DNS_TIMEOUT
#define HTTP_DNS_TIMEOUT 10
#endif

#ifndef HTTP_DNS_CONNECT_TIMEOUT
#define HTTP_DNS_CONNECT_TIMEOUT 5
#endif

#ifndef HTTP_DNS_MAX_CACHE_AGE
#define HTTP_DNS_MAX_CACHE_AGE (15 * 60)
#endif

#ifndef HTTP_DNS_MAX_SERVER_NUM
#define HTTP_DNS_MAX_SERVER_NUM 6
#endif

#ifndef HTTP_DNS_TASK_PRIO
#define HTTP_DNS_TASK_PRIO   32
#endif

#ifndef HTTP_DNS_TASK_STACK
#define HTTP_DNS_TASK_STACK (8 * 1024)
#endif

#ifndef HTTP_DNS_QRY_INTV_MS
#define HTTP_DNS_QRY_INTV_MS 50
#endif

#endif
