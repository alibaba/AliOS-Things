/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "service.h"
#ifdef __cplusplus
extern "C" {
#endif

int sm_init(void);
int sm_exit(void);
int sm_add_service(service_t *);
int sm_del_service(service_t *);
int sm_get_service_state(service_t *);
int sm_query_service(void);
service_t *sm_get_service(char *);
int sm_attach_service(char *, service_cb);
int sm_detach_service(char *, service_cb);
const char *sm_code2string(int);
#ifdef __cplusplus
}
#endif

#endif
