/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AMP_DNS_H_
#define _AMP_DNS_H_

#define DNS_RESULT_COUNT             (3)
int dns_getaddrinfo(char *domain, char *ip[DNS_RESULT_COUNT]);

#endif