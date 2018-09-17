/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_CONN_H_
#define _DM_CONN_H_

typedef struct {
	int cloud_connected;
	void *cloud_connectivity;
	int local_connected;
	void *local_connectivity;
}dm_conn_ctx;

int dm_conn_init(void);
int dm_conn_deinit(void);
void* dm_conn_get_cloud_conn(void);
void* dm_conn_get_local_conn(void);
void dm_conn_set_cloud_conn_state(int connected);
int dm_conn_get_cloud_conn_state(void);
void dm_conn_set_local_conn_state(int connected);
int dm_conn_get_local_conn_state(void);

#endif