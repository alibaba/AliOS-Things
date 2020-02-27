/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef UAGENT_API_H
#define UAGENT_API_H
#include <stdbool.h>
#include <stdio.h>
#include "uagent_config.h"
#include "uagent/uagent_type.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    char initd;
    char service_started;
    ua_send_policy_t to_console;
    char cloud_connect;
    char pk[PK_STR_SIZE];
    char dn[DN_STR_SIZE];
} uagent_service_attr_t;
extern uagent_service_attr_t uagent_service_attr;

#define UAGENT_INITED_FINISH (UAGENT_INITED == uagent_service_attr.initd)

void uagent_init_mutex(void);

bool uagent_get_mutex(void);

void uagent_release_mutex(void);

int add_list(mod_func_t *mod_func_tree, const ua_func_t func, char *func_name);
int del_list(mod_func_t *mod_func_tree, const ua_func_t func);


typedef int (*on_recv_handler) (const unsigned short len, const char* str);

int uagent_ext_comm_init(const char pk[12], const char dn[36], on_recv_handler handler);

int uagent_ext_comm_publish(const unsigned short len, void* data, const bool conn_orient);

int uagent_ack(const service_agent_rlt_t rlt,
    const unsigned long id,
    const ua_mod_t mod,
    const ua_func_t type,
    const void* info_str);

/* REMARK: return 1 indicates connecting */
char check_connect();

void start_monitor_delay_out();
void stop_monitor_delay_out();


#define UAGENT_DEBUG(fmt, ...) //printf("<D>UA %s[%d]: "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)


#define UAGENT_INFO(fmt, ...) printf("<I>UA %s[%d]: "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)

#define UAGENT_ERR(fmt, ...) printf("<E>UA %s[%d]: "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* !UAGENT_API_H */
