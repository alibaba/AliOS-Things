#include "lite-utils.h"
#include "lite-list.h"
#include "iotx_dm_common.h"
#include "iotx_dm_ipc.h"

iotx_dipc_t g_iotx_dipc;

static iotx_dipc_t* _iotx_dipc_get_ctx(void) {
	return &g_iotx_dipc;
}

static void _iotx_dipc_lock(void)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();
	if (ctx->mutex) HAL_MutexLock(ctx->mutex);
}

static void _iotx_dipc_unlock(void)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();
	if (ctx->mutex) HAL_MutexUnlock(ctx->mutex);
}

int iotx_dipc_init(int max_size)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();

	memset(ctx,0,sizeof(iotx_dipc_t));

	//Create Mutex
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Init List
	ctx->msg_list.max_size = max_size;
	INIT_LIST_HEAD(&ctx->msg_list.message_list);

	return SUCCESS_RETURN;
}

void iotx_dipc_deinit(void)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();
	iotx_dipc_msg_node_t *del_node = NULL;
	iotx_dipc_msg_node_t *next_node = NULL;
	iotx_dipc_msg_t *del_msg = NULL;
	
	if(ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}

	list_for_each_entry_safe(del_node,next_node,&ctx->msg_list.message_list,linked_list,iotx_dipc_msg_node_t) {
		//Free Message
		del_msg = (iotx_dipc_msg_t *)del_node->data;
		if (del_msg->data) {DM_free(del_msg->data);}
		DM_free(del_msg);del_msg = NULL;

		//Free Node
		list_del(&del_node->linked_list);
		DM_free(del_node);
	}
}

int iotx_dipc_msg_insert(void *data)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();
	iotx_dipc_msg_node_t *node = NULL;
	
	if (data == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dipc_lock();
	
	if (ctx->msg_list.size >= ctx->msg_list.max_size) {
		dm_log_warning(IOTX_DM_LOG_IPC_MESSAGE_LIST_FULL);
		_iotx_dipc_unlock();
		return FAIL_RETURN;
	}
	
	node = DM_malloc(sizeof(iotx_dipc_msg_node_t));
	if (node == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dipc_unlock();
		return FAIL_RETURN;
	}
	memset(node,0,sizeof(iotx_dipc_msg_node_t));

	node->data = data;
	INIT_LIST_HEAD(&node->linked_list);
	ctx->msg_list.size++;
	list_add_tail(&node->linked_list,&ctx->msg_list.message_list);

	_iotx_dipc_unlock();
	return SUCCESS_RETURN;
}

int iotx_dipc_msg_next(void **data)
{
	iotx_dipc_t *ctx = _iotx_dipc_get_ctx();

	if (data == NULL || *data != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dipc_lock();
	
	if (list_empty(&ctx->msg_list.message_list)) {_iotx_dipc_unlock();return FAIL_RETURN;}
	
	iotx_dipc_msg_node_t *node = list_first_entry(&ctx->msg_list.message_list,iotx_dipc_msg_node_t,linked_list);
	list_del(&node->linked_list);
	ctx->msg_list.size--;
	
	*data = node->data;
	DM_free(node);

	_iotx_dipc_unlock();
	return SUCCESS_RETURN;
}

