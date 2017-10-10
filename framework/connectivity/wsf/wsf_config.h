/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_CONFIG_H
#define WSF_CONFIG_H

#include "wsf_defines.h"


#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct wsf_config_t {
    int   heartbeat_interval;
    unsigned short server_port;
    unsigned short version;
    int   request_timeout;  //seconds
    int   max_msg_queue_length;
    int   max_msg_recv_length;
    int   enable_ssl;
    int   ssl_version;
    int   user_heartbeat_interval;
    char  *server_name;
    const char  *global_ca_str;
    unsigned int ca_str_len;
} wsf_config_t;

extern wsf_config_t *wsf_get_config();

extern wsf_code wsf_init_config();

extern wsf_code wsf_destroy_config();

#ifdef  __cplusplus
}
#endif

#endif
