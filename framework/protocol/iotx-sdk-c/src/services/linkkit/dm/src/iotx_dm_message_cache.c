#include "iot_import.h"
#include "lite-utils.h"
#include "lite-list.h"
#include "iotx_dm_common.h"
#include "iotx_dm_message_cache.h"
#include "iotx_dm_message.h"

iotx_dmc_ctx_t g_iotx_dmc_ctx;

iotx_dmc_ctx_t* _iotx_dmc_get_ctx(void)
{
	return &g_iotx_dmc_ctx;
}

static void _iotx_dmc_mutex_lock(void)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	if (ctx->mutex) {HAL_MutexLock(ctx->mutex);}
}

static void _iotx_dmc_mutex_unlock(void)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	if (ctx->mutex) {HAL_MutexUnlock(ctx->mutex);}
}

int iotx_dmc_init(void)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();

	memset(ctx,0,sizeof(iotx_dmc_ctx_t));

	/* Create Mutex */
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}

	/* Init Message Cache List */
	INIT_LIST_HEAD(&ctx->dmc_list);

	return SUCCESS_RETURN;
}

int iotx_dmc_deinit(void)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	iotx_dmc_node_t *node = NULL;
	iotx_dmc_node_t *next = NULL;

	_iotx_dmc_mutex_lock();
	list_for_each_entry_safe(node,next,&ctx->dmc_list,linked_list,iotx_dmc_node_t) {
		list_del(&node->linked_list);
		if (node->data) {DM_free(node->data);}
		DM_free(node);
		_iotx_dmc_mutex_unlock();
	}
	_iotx_dmc_mutex_unlock();
	
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}

	return SUCCESS_RETURN;
}

int iotx_dmc_msg_insert(int msgid, int devid, iotx_dm_event_types_t type, char *data)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	iotx_dmc_node_t *node = NULL;

	node = DM_malloc(sizeof(iotx_dmc_node_t));
	if (node == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(node,0,sizeof(iotx_dmc_node_t));

	node->msgid = msgid;
	node->devid = devid;
	node->response_type = type;
	node->data = data;
	node->ctime = HAL_UptimeMs();
	INIT_LIST_HEAD(&node->linked_list);

	_iotx_dmc_mutex_lock();
	list_add_tail(&node->linked_list,&ctx->dmc_list);
	_iotx_dmc_mutex_unlock();
	
	return SUCCESS_RETURN;
}

int iotx_dmc_msg_search(_IN_ int msgid, _OU_ iotx_dmc_node_t **node)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	iotx_dmc_node_t *search_node = NULL;

	if (msgid <= 0 || node == NULL || *node != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmc_mutex_lock();
	list_for_each_entry(search_node,&ctx->dmc_list,linked_list,iotx_dmc_node_t) {
		if (search_node->msgid == msgid) {
			*node = search_node;
			_iotx_dmc_mutex_unlock();
			return SUCCESS_RETURN;
		}
	}

	_iotx_dmc_mutex_unlock();
	return FAIL_RETURN;
}

int iotx_dmc_msg_remove(int msgid)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	iotx_dmc_node_t *node = NULL;
	iotx_dmc_node_t *next = NULL;

	_iotx_dmc_mutex_lock();
	list_for_each_entry_safe(node,next,&ctx->dmc_list,linked_list,iotx_dmc_node_t) {
		if (node->msgid == msgid) {
			list_del(&node->linked_list);
			if (node->data) {DM_free(node->data);}
			DM_free(node);
			dm_log_debug("Remove Message ID: %d",msgid);
			_iotx_dmc_mutex_unlock();
			return SUCCESS_RETURN;
		}
	}

	_iotx_dmc_mutex_unlock();
	return FAIL_RETURN;
}

void iotx_dmc_msg_tick(void)
{
	iotx_dmc_ctx_t *ctx = _iotx_dmc_get_ctx();
	iotx_dmc_node_t *node = NULL;
	iotx_dmc_node_t *next = NULL;
	uint64_t current_time = HAL_UptimeMs();

	_iotx_dmc_mutex_lock();
	list_for_each_entry_safe(node,next,&ctx->dmc_list,linked_list,iotx_dmc_node_t) {
		if (current_time < node->ctime) {node->ctime = current_time;}
		if (current_time - node->ctime >= IOTX_DCS_MSG_TIMEOUT_MS_DEFAULT) {
			dm_log_debug("Message ID Timeout: %d",node->msgid);
			/* Send Timeout Message To User */
			iotx_dmsg_send_msg_timeout_to_user(node->msgid,node->devid,node->response_type);
			list_del(&node->linked_list);
			if (node->data) {DM_free(node->data);}
			DM_free(node);
		}
	}
	_iotx_dmc_mutex_unlock();
}

