#include "iot_import.h"
#include "lite-utils.h"
#include "lite-cjson.h"
#include "lite-list.h"
#include "iotx_dm_common.h"
#include "iotx_dm_manager.h"
#include "iotx_dm_shadow.h"
#include "iotx_dm_cm_wrapper.h"
#include "iotx_dm_msg_dispatch.h"
#include "iotx_dm_ipc.h"
#include "iotx_dm_message.h"
#include "iot_export_dm.h"

static iotx_dmgr_ctx g_iotx_dmgr = {0};

static iotx_dmgr_ctx* _iotx_dmgr_get_ctx(void)
{
	return &g_iotx_dmgr;
}

static void _iotx_dmgr_mutex_lock(void)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	if (ctx->mutex) {HAL_MutexLock(ctx->mutex);}
}

static void _iotx_dmgr_mutex_unlock(void)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	if (ctx->mutex) {HAL_MutexUnlock(ctx->mutex);}
}

static int _iotx_dmgr_next_devid(void)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();

	return ctx->global_devid++;
}

static int _iotx_dmgr_search_dev_by_devid(_IN_ int devid, _OU_ iotx_dmgr_dev_node_t **node)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *search_node = NULL;
	
	list_for_each_entry(search_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		if (search_node->devid == devid) {
			dm_log_debug("Device Found, devid: %d",devid);
			if (node) {*node = search_node;}
			return SUCCESS_RETURN;
		}
	}

	dm_log_debug("Device Not Found, devid: %d",devid);
	return FAIL_RETURN;
}

static int _iotx_dmgr_search_dev_by_pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmgr_dev_node_t **node)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *search_node = NULL;
	
	list_for_each_entry(search_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		if ((strlen(search_node->product_key) == strlen(product_key)) &&
			(memcmp(search_node->product_key,product_key,strlen(product_key)) == 0) &&
			(strlen(search_node->device_name) == strlen(device_name)) &&
			(memcmp(search_node->device_name,device_name,strlen(device_name)) == 0)) {
			dm_log_debug("Device Found, Product Key: %s, Device Name: %s",product_key,device_name);
			if (node) {*node = search_node;}
			return SUCCESS_RETURN;
		}
	}

	dm_log_debug("Device Not Found, Product Key: %s, Device Name: %s",product_key,device_name);
	return FAIL_RETURN;
}

static int _iotx_dmgr_insert_dev(_IN_ int devid, _IN_ int dev_type, char product_key[PRODUCT_KEY_MAXLEN], char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,NULL);
	if (res == SUCCESS_RETURN) {return FAIL_RETURN;}
	
	node = DM_malloc(sizeof(iotx_dmgr_dev_node_t));
	if (node == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(node,0,sizeof(iotx_dmgr_dev_node_t));

	node->devid = devid;
	node->dev_type = dev_type;
	memcpy(node->product_key,product_key,strlen(product_key));
	memcpy(node->device_name,device_name,strlen(device_name));
	node->sub_status.generic_index = IOTX_DMGR_DEV_SUB_START;
	node->sub_status.service_event_index = IOTX_DMGR_DEV_SUB_START;
	INIT_LIST_HEAD(&node->linked_list);

	list_add_tail(&node->linked_list,&ctx->dev_list);

	return SUCCESS_RETURN;
}

static void _iotx_dmgr_destroy_devlist(void)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *del_node = NULL;
	iotx_dmgr_dev_node_t *next_node = NULL;
	int index = 0;
	
	list_for_each_entry_safe(del_node,next_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		list_del(&del_node->linked_list);
		for (index = 0;index < del_node->sub_status.service_event_number;index++) {
			if (*(del_node->sub_status.service_event + index) == NULL) {continue;}
			DM_free(*(del_node->sub_status.service_event + index));
		}
		if (del_node->sub_status.service_event) {DM_free(del_node->sub_status.service_event);}
		iotx_dsw_destroy(&del_node->dev_shadow);
		DM_free(del_node);
	}
}

static int _iotx_dmgr_legacy_thing_created(int devid)
{
	int res = 0, message_len = 0;
	const char *thing_created_fmt = "{\"devid\":%d}";
	char *message = NULL;
	
	message_len = strlen(thing_created_fmt) + IOTX_DCM_UINT32_STRLEN + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,thing_created_fmt,devid);
	
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_LEGACY_THING_CREATED,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_init(void)
{
	int res = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	
	memset(ctx,0,sizeof(iotx_dmgr_ctx));
	
	/* Create Mutex */
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		goto ERROR;
	}

	/* Init Device Id*/
	ctx->global_devid = IOTX_DMGR_LOCAL_NODE_DEVID + 1;
	
	/* Init Device List */
	INIT_LIST_HEAD(&ctx->dev_list);

	/* Local Node */
	HAL_GetProductKey(product_key);
	HAL_GetDeviceName(device_name);
	res = _iotx_dmgr_insert_dev(IOTX_DMGR_LOCAL_NODE_DEVID,IOTX_DM_DEVICE_TYPE,product_key,device_name);
	if (res != SUCCESS_RETURN) {goto ERROR;}

	_iotx_dmgr_legacy_thing_created(IOTX_DMGR_LOCAL_NODE_DEVID);
		
	return SUCCESS_RETURN;
	
ERROR:
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}
	memset(ctx,0,sizeof(iotx_dmgr_ctx));
	return FAIL_RETURN;
}

int iotx_dmgr_deinit(void)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();

	_iotx_dmgr_mutex_lock();
	_iotx_dmgr_destroy_devlist();
	_iotx_dmgr_mutex_unlock();
	
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_device_create(_IN_ int dev_type, _IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid)
{
	int res = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (product_key == NULL || device_name == NULL ||
		strlen(product_key) >= PRODUCT_KEY_MAXLEN ||
		strlen(device_name) >= DEVICE_NAME_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_pkdn(product_key,device_name,&node);
	if (res == SUCCESS_RETURN) {
		if (devid) {*devid = node->devid;}
		return SUCCESS_RETURN;
	}
	
	node = DM_malloc(sizeof(iotx_dmgr_dev_node_t));
	if (node == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(node,0,sizeof(iotx_dmgr_dev_node_t));

	node->devid = _iotx_dmgr_next_devid();
	node->dev_type = dev_type;
	node->dev_shadow = NULL;
	memcpy(node->product_key,product_key,strlen(product_key));
	memcpy(node->device_name,device_name,strlen(device_name));
	node->dev_status = IOTX_DMGR_DEV_STATUS_AUTHORIZED;
	node->tsl_source = IOTX_DM_TSL_SOURCE_CLOUD;
	INIT_LIST_HEAD(&node->linked_list);

	list_add_tail(&node->linked_list,&ctx->dev_list);

	if (devid) {*devid = node->devid;}
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_device_destroy(_IN_ int devid)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (node->devid == IOTX_DMGR_LOCAL_NODE_DEVID) {
		dm_log_warning(IOTX_DM_LOG_DMGR_DELETE_DEVICE_ITSELF);
		return FAIL_RETURN;
	}

	list_del(&node->linked_list);

	if (node->dev_shadow) {iotx_dsw_destroy(&node->dev_shadow);}
	DM_free(node);

	return SUCCESS_RETURN;
}

int iotx_dmgr_device_number(void)
{
	int index = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *search_node = NULL;
	
	list_for_each_entry(search_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		index++;
	}

	return index;
}

int iotx_dmgr_get_devid_by_index(_IN_ int index, _OU_ int *devid)
{
	int search_index = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *search_node = NULL;

	if (index < 0 || devid == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	list_for_each_entry(search_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		if (search_index == index) {
			*devid = search_node->devid;
			return SUCCESS_RETURN;
		}
		search_index++;
	}

	return FAIL_RETURN;
}

int iotx_dmgr_get_next_devid(_IN_ int devid, _OU_ int *devid_next)
{
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dmgr_dev_node_t *search_node = NULL;
	iotx_dmgr_dev_node_t *next_node = NULL;

	if (devid < 0 || devid_next == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	list_for_each_entry(next_node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		if (search_node && search_node->devid == devid) {
			*devid_next = next_node->devid;
			return SUCCESS_RETURN;
		}
		
		if (next_node->devid == devid) {
			search_node = next_node;
		}
	}

	return FAIL_RETURN;
}

int iotx_dmgr_search_device_by_devid(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN], _OU_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (product_key == NULL || device_name == NULL || device_secret == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	memcpy(product_key,node->product_key,strlen(node->product_key));
	memcpy(device_name,node->device_name,strlen(node->device_name));
	memcpy(device_secret,node->device_secret,strlen(node->device_secret));

	return SUCCESS_RETURN;
}

int iotx_dmgr_search_device_by_pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (product_key == NULL || device_name == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_pkdn(product_key,device_name,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (devid) {*devid = node->devid;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_search_device_node_by_devid(_IN_ int devid, _OU_ void **node)
{
	int res = 0;
	iotx_dmgr_dev_node_t *search_node = NULL;
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&search_node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (node) {*node = (void *)search_node;}
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_search_devid_by_device_node(_IN_ void *node, _OU_ int *devid)
{
	iotx_dmgr_dev_node_t *search_node = (iotx_dmgr_dev_node_t *)node;
	
	if (node == NULL || devid == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	*devid = search_node->devid;

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t tsl_source)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || tsl_source < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	node->tsl_source = tsl_source;
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t *tsl_source)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || tsl_source == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*tsl_source = node->tsl_source;
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_shadow(_IN_ int devid, void **shadow)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || shadow == NULL || *shadow != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*shadow = node->dev_shadow;
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_type(_IN_ int devid, _OU_ int *dev_type)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || dev_type == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*dev_type = node->dev_type;

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_enable(_IN_ int devid)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	node->status = IOTX_DMGR_DEV_AVAIL_ENABLE;

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_disable(_IN_ int devid)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	node->status = IOTX_DMGR_DEV_AVAIL_DISABLE;

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_avail(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dm_dev_avail_t *status)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (product_key == NULL || device_name == NULL || status == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_pkdn(product_key,device_name,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*status = node->status;

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_status(_IN_ int devid, _IN_ iotx_dm_dev_status_t status)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);	
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	node->dev_status = status;
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || status == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);	
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*status = node->dev_status;
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_sub_generic_index(_IN_ int devid, _IN_ int index)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	dm_log_debug("Set Generic Index: %d",index);
	
	node->sub_status.generic_index = index;
	node->sub_status.ctime = HAL_UptimeMs();

	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_sub_generic_index(_IN_ int devid, _OU_ int *index)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	*index = node->sub_status.generic_index;

	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_sub_service_event(_IN_ int devid, _IN_ int number, _IN_ char **service_event)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || number <= 0 || service_event == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	dm_log_debug("Current Service Event Number: %d",number);
	node->sub_status.service_event_number = number;
	node->sub_status.service_event = service_event;

	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_set_dev_sub_service_event_index(_IN_ int devid, _IN_ int index)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	node->sub_status.service_event_index = index;
	
	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_sub_service_event_number(_IN_ int devid, _OU_ int *number)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || number == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	*number = node->sub_status.service_event_number;
	
	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_sub_service_event_index(_IN_ int devid, _OU_ int *index)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	*index = node->sub_status.service_event_index;

	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_dev_sub_service_event(_IN_ int devid, _IN_ int index, _OU_ char **service_event)
{
	int res = 0;
	char *service_event_refer = NULL;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index < 0 || service_event == NULL || *service_event != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	if (index >= 0 || index < node->sub_status.service_event_number) {
		service_event_refer = *(node->sub_status.service_event + index);
		if (service_event_refer == NULL) {return FAIL_RETURN;}
		*service_event = DM_malloc(strlen(service_event_refer) + 1);
		if (*service_event == NULL) {
			dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			_iotx_dmgr_mutex_unlock();
			return FAIL_RETURN;
		}
		memset(*service_event,0,strlen(service_event_refer) + 1);
		memcpy(*service_event,service_event_refer,strlen(service_event_refer));
	}
	
	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

int iotx_dmgr_clear_dev_sub_service_event(_IN_ int devid)
{
	int res = 0,index = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	_iotx_dmgr_mutex_lock();
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {_iotx_dmgr_mutex_unlock();return FAIL_RETURN;}

	if (node->sub_status.service_event == NULL) {_iotx_dmgr_mutex_unlock();return SUCCESS_RETURN;}
	
	for (index = 0;index < node->sub_status.service_event_number;index++) {
		if (*(node->sub_status.service_event + index) == NULL) {continue;}
		dm_log_debug("Current Clear Service Event Topic: %s",*(node->sub_status.service_event + index));
		DM_free(*(node->sub_status.service_event + index));
	}

	DM_free(node->sub_status.service_event);
	node->sub_status.service_event_number = 0;
	
	_iotx_dmgr_mutex_unlock();
	return SUCCESS_RETURN;
}

void iotx_dmgr_dev_sub_status_check(void)
{
	int res = 0;
	iotx_dmgr_ctx *ctx = _iotx_dmgr_get_ctx();
	iotx_dcs_topic_mapping_t *dcs_mapping = iotx_dcs_get_topic_mapping();
	int dcs_mapping_size = iotx_dcs_get_topic_mapping_size();
	iotx_dmgr_dev_node_t *node = NULL;
	uint64_t current_time = HAL_UptimeMs();
	char *service_name = NULL;

	_iotx_dmgr_mutex_lock();
	list_for_each_entry(node,&ctx->dev_list,linked_list,iotx_dmgr_dev_node_t) {
		if (node->dev_status != IOTX_DMGR_DEV_STATUS_LOGINED && node->devid != IOTX_DMGR_LOCAL_NODE_DEVID) {continue;}
		
		/* Check Generic Topic */
		if (node->sub_status.generic_index > IOTX_DMGR_DEV_SUB_START && node->sub_status.generic_index < dcs_mapping_size) {
			if (node->sub_status.ctime > current_time) {node->sub_status.ctime = current_time;}
			if (current_time - node->sub_status.ctime >= IOTX_DMGR_DEV_SUB_TIMEOUT_MS) {
				node->sub_status.ctime = current_time;
				dm_log_debug("Retry Generic Subscribe, devid: %d, index: %d",node->devid,node->sub_status.generic_index);
				res = iotx_dcm_service_name((char *)dcs_mapping[node->sub_status.generic_index].service_prefix,
											(char *)dcs_mapping[node->sub_status.generic_index].service_name,
											node->product_key,node->device_name,&service_name);
				if (res != SUCCESS_RETURN) {continue;}

				iotx_dcs_topic_subscribe(service_name);
				DM_free(service_name);
			}
			_iotx_dmgr_mutex_unlock();
			return;
		}

		/* Check Service Event Topic */
		if (node->sub_status.service_event_index > IOTX_DMGR_DEV_SUB_START && node->sub_status.service_event_index < node->sub_status.service_event_number) {
			if (node->sub_status.ctime > current_time) {node->sub_status.ctime = current_time;}
			if (current_time - node->sub_status.ctime >= IOTX_DMGR_DEV_SUB_TIMEOUT_MS) {
				node->sub_status.ctime = current_time;
				dm_log_debug("Retry Service Event Subscribe, devid: %d, : %s",*(node->sub_status.service_event + node->sub_status.service_event_index));
				iotx_dcs_topic_subscribe(*(node->sub_status.service_event + node->sub_status.service_event_index));
			}
			_iotx_dmgr_mutex_unlock();
			return;
		}
	}
	_iotx_dmgr_mutex_unlock();
}

int iotx_dmgr_set_tsl(int devid, iotx_dm_tsl_type_t tsl_type, const char *tsl, int tsl_len)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (tsl == NULL || tsl_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_DMGR_DEVICE_NOT_FOUND,devid);
		return FAIL_RETURN;
	}

	res = iotx_dsw_create(tsl_type,tsl,tsl_len,&node->dev_shadow);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_product_key(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN])
{
	int res  = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || product_key == NULL || strlen(product_key) >= PRODUCT_KEY_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	memcpy(product_key,node->product_key,strlen(node->product_key));
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_get_device_name(_IN_ int devid, _OU_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res  = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || device_name == NULL || strlen(device_name) >= DEVICE_NAME_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	memcpy(device_name,node->device_name,strlen(node->device_name));
	
	return SUCCESS_RETURN;
}

int iotx_dmgr_set_device_secret(_IN_ int devid, _IN_ char device_secret[DEVICE_SECRET_MAXLEN])
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || device_secret == NULL ||
		strlen(device_secret) >= DEVICE_SECRET_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	memset(node->device_secret,0,DEVICE_SECRET_MAXLEN);
	memcpy(node->device_secret,device_secret,strlen(device_secret));

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_device_secret(_IN_ int devid, _OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || device_secret == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	memcpy(device_secret,node->device_secret,strlen(node->device_secret));

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_property_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (key == NULL || key_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_property_data(node->dev_shadow,key,key_len,data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_service_input_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (key == NULL || key_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_service_input_data(node->dev_shadow,key,key_len,data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_service_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (key == NULL || key_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_service_output_data(node->dev_shadow,key,key_len,data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_event_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (key == NULL || key_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_event_output_data(node->dev_shadow,key,key_len,data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_data_type(_IN_ void *data, _OU_ iotx_dsw_data_type_e *type)
{
	if (data == NULL || type == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	return iotx_dsw_get_data_type(data,type);
}

int iotx_dmgr_get_property_number(_IN_ int devid, _OU_ int *number)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || number == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return iotx_dsw_get_property_number(node->dev_shadow,number);
}


int iotx_dmgr_get_service_number(_IN_ int devid, _OU_ int *number)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || number == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return iotx_dsw_get_service_number(node->dev_shadow,number);
}

int iotx_dmgr_get_event_number(_IN_ int devid, _OU_ int *number)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || number == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return iotx_dsw_get_event_number(node->dev_shadow,number);
}

int iotx_dmgr_get_property_by_index(_IN_ int devid, _IN_ int index, _OU_ void **property)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return iotx_dsw_get_property_by_index(node->dev_shadow,index,property);
}

int iotx_dmgr_get_service_by_index(_IN_ int devid, _IN_ int index, _OU_ void **service)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return iotx_dsw_get_service_by_index(node->dev_shadow,index,service);
}

int iotx_dmgr_get_event_by_index(_IN_ int devid, _IN_ int index, _OU_ void **event)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || index < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return iotx_dsw_get_event_by_index(node->dev_shadow,index,event);
}

int iotx_dmgr_get_service_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **service)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || identifier == NULL || service == NULL || *service != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return iotx_dsw_get_service_by_identifier(node->dev_shadow,identifier,service);
}

int iotx_dmgr_get_event_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **event)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || identifier == NULL || event == NULL || *event != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	return iotx_dsw_get_event_by_identifier(node->dev_shadow,identifier,event);
}

int iotx_dmgr_get_property_identifier(_IN_ void *property, _OU_ char **identifier)
{
	if (property == NULL || identifier == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	return iotx_dsw_get_property_identifier(property,identifier);
}

int iotx_dmgr_get_service_method(_IN_ void *service, _OU_ char **method)
{
	if (service == NULL || method == NULL || *method != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	return iotx_dsw_get_service_method(service,method);
}

int iotx_dmgr_get_event_method(_IN_ void *event, _OU_ char **method)
{
	if (event == NULL || method == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	return iotx_dsw_get_event_method(event,method);
}

int iotx_dmgr_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dsw_set_property_value(node->dev_shadow,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_property_value(node->dev_shadow,key,key_len,value);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_event_output_value(_IN_ int devid, _IN_ char * key,_IN_ int key_len,_IN_ void * value,_IN_ int value_len)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dsw_set_event_output_value(node->dev_shadow,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_event_output_value(node->dev_shadow,key,key_len,value);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_service_input_value(_IN_ int devid, _IN_ char * key,_IN_ int key_len,_IN_ void * value,_IN_ int value_len)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dsw_set_service_input_value(node->dev_shadow,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_service_input_value(node->dev_shadow,key,key_len,value);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_set_service_output_value(_IN_ int devid, _IN_ char * key,_IN_ int key_len,_IN_ void * value,_IN_ int value_len)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dsw_set_service_output_value(node->dev_shadow,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_get_service_output_value(node->dev_shadow,key,key_len,value);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_assemble_property(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dsw_assemble_property(node->dev_shadow,identifier,identifier_len,lite);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_assemble_event_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_assemble_event_output(node->dev_shadow,identifier,identifier_len,lite);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmgr_assemble_service_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;
	
	if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = _iotx_dmgr_search_dev_by_devid(devid,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dsw_assemble_service_output(node->dev_shadow,identifier,identifier_len,lite);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}
