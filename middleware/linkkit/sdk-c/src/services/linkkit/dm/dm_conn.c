/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "dm_cm_wrapper.h"
#include "dm_conn.h"

static dm_conn_ctx g_dm_conn_ctx;

static dm_conn_ctx* _dm_conn_get_ctx(void)
{
	return &g_dm_conn_ctx;
}

int dm_conn_init(void)
{
	int res = 0;
	dm_conn_ctx *ctx = _dm_conn_get_ctx();
	memset(ctx,0,sizeof(dm_conn_ctx));
	
	/* Create Cloud Connectivity */
	res = dm_cmw_conn_cloud_mqtt_init(&ctx->cloud_connectivity);
	if (res == ERROR_NO_MEM) {goto ERROR;}

	/* Create Local Connectivity */
#ifdef CONFIG_DM_SUPPORT_LOCAL_CONN
	res = dm_cmw_conn_local_alcs_init(&ctx->local_connectivity);
	if (res == ERROR_NO_MEM) {goto ERROR;}
#endif

	return SUCCESS_RETURN;
ERROR:
#ifdef CONFIG_DM_SUPPORT_LOCAL_CONN
	if (ctx->local_connectivity) {dm_cmw_conn_destroy(&ctx->local_connectivity);}
#endif
	if (ctx->cloud_connectivity) {dm_cmw_conn_destroy(&ctx->cloud_connectivity);}

	return FAIL_RETURN;
}

int dm_conn_deinit(void)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();
#ifdef CONFIG_DM_SUPPORT_LOCAL_CONN
	if (ctx->local_connectivity) {dm_cmw_conn_destroy(&ctx->local_connectivity);}
#endif
	if (ctx->cloud_connectivity) {dm_cmw_conn_destroy(&ctx->cloud_connectivity);}

	return SUCCESS_RETURN;
}

void* dm_conn_get_cloud_conn(void)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	return ctx->cloud_connectivity;
}

void* dm_conn_get_local_conn(void)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	return ctx->local_connectivity;
}

void dm_conn_set_cloud_conn_state(int connected)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	ctx->cloud_connected = (connected == 0)?(0):(1);
}

int dm_conn_get_cloud_conn_state(void)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	return ctx->cloud_connected;
}

void dm_conn_set_local_conn_state(int connected)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	ctx->local_connected = (connected == 0)?(0):(1);
}

int dm_conn_get_local_conn_state(void)
{
	dm_conn_ctx *ctx = _dm_conn_get_ctx();

	return ctx->local_connected;
}
