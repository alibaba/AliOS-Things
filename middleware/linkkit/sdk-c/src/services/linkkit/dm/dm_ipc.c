#include "iotx_dm_internal.h"
#include "dm_ipc.h"

dm_ipc_t g_dm_ipc;

static dm_ipc_t* _dm_ipc_get_ctx(void) {
	return &g_dm_ipc;
}

static void _dm_ipc_lock(void)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();
	if (ctx->mutex) HAL_MutexLock(ctx->mutex);
}

static void _dm_ipc_unlock(void)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();
	if (ctx->mutex) HAL_MutexUnlock(ctx->mutex);
}

int dm_ipc_init(int max_size)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();

	memset(ctx,0,sizeof(dm_ipc_t));

	//Create Mutex
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Init List
	ctx->msg_list.max_size = max_size;
	INIT_LIST_HEAD(&ctx->msg_list.message_list);

	return SUCCESS_RETURN;
}

void dm_ipc_deinit(void)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();
	dm_ipc_msg_node_t *del_node = NULL;
	dm_ipc_msg_node_t *next_node = NULL;
	dm_ipc_msg_t *del_msg = NULL;
	
	if(ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}

	list_for_each_entry_safe(del_node,next_node,&ctx->msg_list.message_list,linked_list,dm_ipc_msg_node_t) {
		//Free Message
		del_msg = (dm_ipc_msg_t *)del_node->data;
		if (del_msg->data) {DM_free(del_msg->data);}
		DM_free(del_msg);del_msg = NULL;

		//Free Node
		list_del(&del_node->linked_list);
		DM_free(del_node);
	}
}

int dm_ipc_msg_insert(void *data)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();
	dm_ipc_msg_node_t *node = NULL;
	
	if (data == NULL) {
		dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_dm_ipc_lock();
	
	if (ctx->msg_list.size >= ctx->msg_list.max_size) {
		dm_log_warning(DM_UTILS_LOG_IPC_MESSAGE_LIST_FULL);
		_dm_ipc_unlock();
		return FAIL_RETURN;
	}
	
	node = DM_malloc(sizeof(dm_ipc_msg_node_t));
	if (node == NULL) {
		dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
		_dm_ipc_unlock();
		return FAIL_RETURN;
	}
	memset(node,0,sizeof(dm_ipc_msg_node_t));

	node->data = data;
	INIT_LIST_HEAD(&node->linked_list);
	ctx->msg_list.size++;
	list_add_tail(&node->linked_list,&ctx->msg_list.message_list);

	_dm_ipc_unlock();
	return SUCCESS_RETURN;
}

int dm_ipc_msg_next(void **data)
{
	dm_ipc_t *ctx = _dm_ipc_get_ctx();

	if (data == NULL || *data != NULL) {
		dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_dm_ipc_lock();
	
	if (list_empty(&ctx->msg_list.message_list)) {_dm_ipc_unlock();return FAIL_RETURN;}
	
	dm_ipc_msg_node_t *node = list_first_entry(&ctx->msg_list.message_list,dm_ipc_msg_node_t,linked_list);
	list_del(&node->linked_list);
	ctx->msg_list.size--;
	
	*data = node->data;
	DM_free(node);

	_dm_ipc_unlock();
	return SUCCESS_RETURN;
}

