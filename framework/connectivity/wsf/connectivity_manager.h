/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CONNECTIVITY_MANAGER_H
#define CONNECTIVITY_MANAGER_H

#include "connectivity.h"

int cm_init();
int cm_add_conn(connectivity_t *conn);
int cm_del_conn(connectivity_t *conn);
int cm_get_conn_state(connectivity_t *conn);
int cm_query_conn();
connectivity_t *cm_get_conn(char *conn_name);
int cm_bind_conn(char *conn_name, connectivity_cb listen);
int cm_release_conn(char *conn_name, connectivity_cb listen);
char *cm_code2string(int state);

#endif
