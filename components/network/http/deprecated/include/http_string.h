/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HTTP_STRING_H
#define HTTP_STRING_H

#include "http_client.h"

bool http_str_search(char *src, char *searched, uint32_t offset,
                     uint32_t scope, httpc_param_t *param);

bool http_str_insensitive_cmp(char *src, char *dest, uint32_t len);

#endif
