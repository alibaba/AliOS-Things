/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "dm_manager.h"
#include "dm_shadow.h"
#include "dm_cm_wrapper.h"
#include "dm_dispatch.h"
#include "dm_ipc.h"
#include "dm_message.h"
#include "dm_message_cache.h"
#include "dm_subscribe.h"
#include "dm_opt.h"
#include "iotx_dm.h"

static dm_mgr_ctx g_dm_mgr = {0};

static dm_mgr_ctx *_dm_mgr_get_ctx(void)
{
    return &g_dm_mgr;
}

static void _dm_mgr_mutex_lock(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _dm_mgr_mutex_unlock(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

static int _dm_mgr_next_devid(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();

    return ctx->global_devid++;
}

static int _dm_mgr_search_dev_by_devid(_IN_ int devid, _OU_ dm_mgr_dev_node_t **node)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_node->devid == devid) {
            dm_log_debug("Device Found, devid: %d", devid);
            if (node) {
                *node = search_node;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, devid: %d", devid);
    return FAIL_RETURN;
}

static int _dm_mgr_search_dev_by_pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN],
                                      _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ dm_mgr_dev_node_t **node)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if ((strlen(search_node->product_key) == strlen(product_key)) &&
            (memcmp(search_node->product_key, product_key, strlen(product_key)) == 0) &&
            (strlen(search_node->device_name) == strlen(device_name)) &&
            (memcmp(search_node->device_name, device_name, strlen(device_name)) == 0)) {
            dm_log_debug("Device Found, Product Key: %s, Device Name: %s", product_key, device_name);
            if (node) {
                *node = search_node;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, Product Key: %s, Device Name: %s", product_key, device_name);
    return FAIL_RETURN;
}

static int _dm_mgr_insert_dev(_IN_ int devid, _IN_ int dev_type, char product_key[PRODUCT_KEY_MAXLEN],
                              char device_name[DEVICE_NAME_MAXLEN])
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || product_key == NULL || strlen(product_key) >= PRODUCT_KEY_MAXLEN ||
        device_name == NULL || strlen(device_name) >= DEVICE_NAME_MAXLEN) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, NULL);
    if (res == SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node = DM_malloc(sizeof(dm_mgr_dev_node_t));
    if (node == NULL) {
        dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(node, 0, sizeof(dm_mgr_dev_node_t));

    node->devid = devid;
    node->dev_type = dev_type;
    memcpy(node->product_key, product_key, strlen(product_key));
    memcpy(node->device_name, device_name, strlen(device_name));
    node->sub_status.generic_index = DM_MGR_DEV_SUB_START;
    node->sub_status.service_event_index = DM_MGR_DEV_SUB_START;
    INIT_LIST_HEAD(&node->linked_list);

    list_add_tail(&node->linked_list, &ctx->dev_list);

    return SUCCESS_RETURN;
}

static void _dm_mgr_destroy_devlist(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *del_node = NULL;
    dm_mgr_dev_node_t *next_node = NULL;
    int index = 0;

    list_for_each_entry_safe(del_node, next_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        list_del(&del_node->linked_list);
        for (index = 0; index < del_node->sub_status.service_event_number; index++) {
            if (*(del_node->sub_status.service_event + index) == NULL) {
                continue;
            }
            DM_free(*(del_node->sub_status.service_event + index));
        }
        if (del_node->sub_status.service_event) {
            DM_free(del_node->sub_status.service_event);
        }
        #ifdef DEPRECATED_LINKKIT
        dm_shw_destroy(&del_node->dev_shadow);
        #endif
        DM_free(del_node);
    }
}

#ifdef DEPRECATED_LINKKIT
static int _dm_mgr_legacy_thing_created(int devid)
{
    int res = 0, message_len = 0;
    const char *thing_created_fmt = "{\"devid\":%d}";
    char *message = NULL;

    message_len = strlen(thing_created_fmt) + DM_UTILS_UINT32_STRLEN + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, thing_created_fmt, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_LEGACY_THING_CREATED, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif

int dm_mgr_init(void)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    memset(ctx, 0, sizeof(dm_mgr_ctx));

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        goto ERROR;
    }

    /* Init Device Id*/
    ctx->global_devid = IOTX_DM_LOCAL_NODE_DEVID + 1;

    /* Init Device List */
    INIT_LIST_HEAD(&ctx->dev_list);

    /* Local Node */
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    res = _dm_mgr_insert_dev(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_DEVICE_TYPE, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }

#ifdef DEPRECATED_LINKKIT
    _dm_mgr_legacy_thing_created(IOTX_DM_LOCAL_NODE_DEVID);
#endif

    return SUCCESS_RETURN;

ERROR:
    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }
    memset(ctx, 0, sizeof(dm_mgr_ctx));
    return FAIL_RETURN;
}

int dm_mgr_deinit(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();

    _dm_mgr_mutex_lock();
    _dm_mgr_destroy_devlist();
    _dm_mgr_mutex_unlock();

    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }

    return SUCCESS_RETURN;
}

int dm_mgr_device_create(_IN_ int dev_type, _IN_ char product_key[PRODUCT_KEY_MAXLEN],
                         _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL ||
        strlen(product_key) >= PRODUCT_KEY_MAXLEN ||
        strlen(device_name) >= DEVICE_NAME_MAXLEN) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res == SUCCESS_RETURN) {
        if (devid) {
            *devid = node->devid;
        }
        return FAIL_RETURN;
    }

    node = DM_malloc(sizeof(dm_mgr_dev_node_t));
    if (node == NULL) {
        dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(node, 0, sizeof(dm_mgr_dev_node_t));

    node->devid = _dm_mgr_next_devid();
    node->dev_type = dev_type;
    node->dev_shadow = NULL;
    memcpy(node->product_key, product_key, strlen(product_key));
    memcpy(node->device_name, device_name, strlen(device_name));
    node->dev_status = IOTX_DM_DEV_STATUS_AUTHORIZED;
    node->tsl_source = IOTX_DM_TSL_SOURCE_CLOUD;
    node->sub_status.generic_index = DM_MGR_DEV_SUB_START;
    node->sub_status.service_event_index = DM_MGR_DEV_SUB_START;
    INIT_LIST_HEAD(&node->linked_list);

    list_add_tail(&node->linked_list, &ctx->dev_list);

    if (devid) {
        *devid = node->devid;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_device_destroy(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node->devid == IOTX_DM_LOCAL_NODE_DEVID) {
        dm_log_warning(DM_UTILS_LOG_DMGR_DELETE_DEVICE_ITSELF);
        return FAIL_RETURN;
    }

    list_del(&node->linked_list);

    if (node->dev_shadow) {
        #ifdef DEPRECATED_LINKKIT
        dm_shw_destroy(&node->dev_shadow);
        #endif
    }
    DM_free(node);

    return SUCCESS_RETURN;
}

int dm_mgr_device_number(void)
{
    int index = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        index++;
    }

    return index;
}

int dm_mgr_get_devid_by_index(_IN_ int index, _OU_ int *devid)
{
    int search_index = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    if (index < 0 || devid == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_index == index) {
            *devid = search_node->devid;
            return SUCCESS_RETURN;
        }
        search_index++;
    }

    return FAIL_RETURN;
}

int dm_mgr_get_next_devid(_IN_ int devid, _OU_ int *devid_next)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;
    dm_mgr_dev_node_t *next_node = NULL;

    if (devid < 0 || devid_next == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    list_for_each_entry(next_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
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

int dm_mgr_search_device_by_devid(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN],
                                  _OU_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL || device_secret == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memcpy(product_key, node->product_key, strlen(node->product_key));
    memcpy(device_name, node->device_name, strlen(node->device_name));
    memcpy(device_secret, node->device_secret, strlen(node->device_secret));

    return SUCCESS_RETURN;
}

int dm_mgr_search_device_by_pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],
                                 _OU_ int *devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (devid) {
        *devid = node->devid;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_search_device_node_by_devid(_IN_ int devid, _OU_ void **node)
{
    int res = 0;
    dm_mgr_dev_node_t *search_node = NULL;

    res = _dm_mgr_search_dev_by_devid(devid, &search_node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node) {
        *node = (void *)search_node;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_type(_IN_ int devid, _OU_ int *dev_type)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || dev_type == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *dev_type = node->dev_type;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_enable(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->status = IOTX_DM_DEV_AVAIL_ENABLE;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_disable(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->status = IOTX_DM_DEV_AVAIL_DISABLE;

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_avail(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],
                         _OU_ iotx_dm_dev_avail_t *status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL || status == NULL ||
        (strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
        (strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *status = node->status;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_status(_IN_ int devid, _IN_ iotx_dm_dev_status_t status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->dev_status = status;

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || status == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *status = node->dev_status;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_sub_generic_index(_IN_ int devid, _IN_ int index)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    dm_log_debug("Set Generic Index: %d", index);

    node->sub_status.generic_index = index;
    node->sub_status.ctime = HAL_UptimeMs();

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_sub_generic_index(_IN_ int devid, _OU_ int *index)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    *index = node->sub_status.generic_index;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_sub_service_event(_IN_ int devid, _IN_ int number, _IN_ char **service_event)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number <= 0 || service_event == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    dm_log_debug("Current Service Event Number: %d", number);
    node->sub_status.service_event_number = number;
    node->sub_status.service_event = service_event;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_sub_service_event_index(_IN_ int devid, _IN_ int index)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    node->sub_status.service_event_index = index;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_sub_service_event_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    *number = node->sub_status.service_event_number;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_sub_service_event_index(_IN_ int devid, _OU_ int *index)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    *index = node->sub_status.service_event_index;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_sub_service_event(_IN_ int devid, _IN_ int index, _OU_ char **service_event)
{
    int res = 0;
    char *service_event_refer = NULL;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0 || service_event == NULL || *service_event != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    if (index >= 0 && index < node->sub_status.service_event_number) {
        service_event_refer = *(node->sub_status.service_event + index);
        if (service_event_refer == NULL) {
            return FAIL_RETURN;
        }
        *service_event = DM_malloc(strlen(service_event_refer) + 1);
        if (*service_event == NULL) {
            dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
            _dm_mgr_mutex_unlock();
            return FAIL_RETURN;
        }
        memset(*service_event, 0, strlen(service_event_refer) + 1);
        memcpy(*service_event, service_event_refer, strlen(service_event_refer));
    }

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

int dm_mgr_clear_dev_sub_service_event(_IN_ int devid)
{
    int res = 0, index = 0;
    dm_mgr_dev_node_t *node = NULL;

    _dm_mgr_mutex_lock();
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        _dm_mgr_mutex_unlock();
        return FAIL_RETURN;
    }

    if (node->sub_status.service_event == NULL) {
        _dm_mgr_mutex_unlock();
        return SUCCESS_RETURN;
    }

    for (index = 0; index < node->sub_status.service_event_number; index++) {
        if (*(node->sub_status.service_event + index) == NULL) {
            continue;
        }
        dm_log_debug("Current Clear Service Event Topic: %s", *(node->sub_status.service_event + index));
        DM_free(*(node->sub_status.service_event + index));
    }

    DM_free(node->sub_status.service_event);
    node->sub_status.service_event_number = 0;

    _dm_mgr_mutex_unlock();
    return SUCCESS_RETURN;
}

void dm_mgr_dev_sub_status_check(void)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_disp_topic_mapping_t *dcs_mapping = dm_disp_get_topic_mapping();
    int dcs_mapping_size = dm_disp_get_topic_mapping_size();
    dm_mgr_dev_node_t *node = NULL;
    uint64_t current_time = HAL_UptimeMs();
    char *service_name = NULL;

    _dm_mgr_mutex_lock();
    list_for_each_entry(node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (node->dev_status != IOTX_DM_DEV_STATUS_LOGINED && node->devid != IOTX_DM_LOCAL_NODE_DEVID) {
            continue;
        }

        /* Check Generic Topic */
        if (node->sub_status.generic_index > DM_MGR_DEV_SUB_START && node->sub_status.generic_index < dcs_mapping_size) {
            if (node->sub_status.ctime > current_time) {
                node->sub_status.ctime = current_time;
            }
            if (current_time - node->sub_status.ctime >= DM_MGR_DEV_SUB_TIMEOUT_MS) {
                node->sub_status.ctime = current_time;
                dm_log_debug("Retry Generic Subscribe, devid: %d, index: %d", node->devid, node->sub_status.generic_index);
                res = dm_utils_service_name((char *)dcs_mapping[node->sub_status.generic_index].service_prefix,
                                            (char *)dcs_mapping[node->sub_status.generic_index].service_name,
                                            node->product_key, node->device_name, &service_name);
                if (res != SUCCESS_RETURN) {
                    continue;
                }
                dm_sub_multi(&service_name, 1);
                DM_free(service_name);
            }
            _dm_mgr_mutex_unlock();
            return;
        }

        /* Check Service Event Topic */
        if (node->sub_status.service_event_index > DM_MGR_DEV_SUB_START
            && node->sub_status.service_event_index < node->sub_status.service_event_number) {
            if (node->sub_status.ctime > current_time) {
                node->sub_status.ctime = current_time;
            }
            if (current_time - node->sub_status.ctime >= DM_MGR_DEV_SUB_TIMEOUT_MS) {
                node->sub_status.ctime = current_time;
                dm_log_debug("Retry Service Event Subscribe, devid: %d",
                             *(node->sub_status.service_event + node->sub_status.service_event_index));
                dm_sub_multi(node->sub_status.service_event + node->sub_status.service_event_index, 1);
            }
            _dm_mgr_mutex_unlock();
            return;
        }
    }
    _dm_mgr_mutex_unlock();
}

int dm_mgr_set_device_secret(_IN_ int devid, _IN_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || device_secret == NULL ||
        strlen(device_secret) >= DEVICE_SECRET_MAXLEN) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(node->device_secret, 0, DEVICE_SECRET_MAXLEN);
    memcpy(node->device_secret, device_secret, strlen(device_secret));

    return SUCCESS_RETURN;
}

#ifdef CONFIG_DM_DEVTYPE_GATEWAY
int dm_mgr_upstream_thing_sub_register(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_SUB_REGISTER;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_sub_register(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_sub_unregister(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_SUB_UNREGISTER;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_sub_unregister(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_add(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_TOPO_ADD;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_topo_add(node->product_key, node->device_name, node->device_secret, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_ADD_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_delete(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_TOPO_DELETE;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_topo_delete(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_DELETE_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_get(void)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_TOPO_GET;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    res = _dm_mgr_search_dev_by_pkdn(request.product_key, request.device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Params And Method */
    res = dm_msg_thing_topo_get(&request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = node->devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_GET_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_list_found(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_LIST_FOUND;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_list_found(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_ADD_NOTIFY_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_combine_login(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_EXT_SESSION_PREFIX;
    request.service_name = DM_DISP_COMBINE_LOGIN;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_combine_login(node->product_key, node->device_name, node->device_secret, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_COMBINE_LOGIN_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_combine_logout(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node->dev_status < IOTX_DM_DEV_STATUS_LOGINED) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_EXT_SESSION_PREFIX;
    request.service_name = DM_DISP_COMBINE_LOGOUT;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_combine_logout(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}
#endif

int dm_mgr_upstream_thing_property_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_EVENT_PROPERTY_POST;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_property_post(node->product_key, node->device_name, payload, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_all(&request);
    if (res == SUCCESS_RETURN) {
        int prop_post_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_PROPERTY_POST_REPLY, &prop_post_reply);
        if (res == SUCCESS_RETURN && prop_post_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY, NULL);
        }
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_event_post(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *method,
                                     _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    char *service_name = NULL;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 ||
        method == NULL || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    service_name_len = strlen(DM_DISP_THING_EVENT_POST) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_DISP_THING_EVENT_POST, identifier_len, identifier);

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = service_name;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_event_post(node->product_key, node->device_name, method, payload, &request);
    if (res != SUCCESS_RETURN) {
        DM_free(service_name);
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_all(&request);
    if (res == SUCCESS_RETURN) {
        int event_post_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &event_post_reply);
        if (res == SUCCESS_RETURN && event_post_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY, NULL);
        }
        res = request.msgid;
    }

    DM_free(service_name);
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_deviceinfo_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_DEVICEINFO_UPDATE;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_deviceinfo_update(node->product_key, node->device_name, payload, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_deviceinfo_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_DEVICEINFO_DELETE;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_deviceinfo_delete(node->product_key, node->device_name, payload, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_dsltemplate_get(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_DSLTEMPLATE_GET;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_dsltemplate_get(&request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_dynamictsl_get(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_DYNAMICTSL_GET;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Get Params And Method */
    res = dm_msg_thing_dynamictsl_get(&request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY, NULL);
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_model_up_raw(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    char *uri = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_MODEL_UP_RAW;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Request URI */
    res = dm_utils_service_name(request.service_prefix, request.service_name,
                                request.product_key, request.device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_info("DM Send Raw Data:");
    HEXDUMP_INFO(payload, payload_len);

    res = dm_cmw_send_to_all(uri, payload, payload_len, NULL);
    if (res != SUCCESS_RETURN) {
        DM_free(uri);
        dm_log_err(DM_UTILS_LOG_CM_SEND_MESSAGE_FAILED);
        return FAIL_RETURN;
    }

    DM_free(uri);
    return SUCCESS_RETURN;
}

int dm_mgr_upstream_ntp_request(void)
{
    int res = 0;
    const char *ntp_request_fmt = "{\"deviceSendTime\":\"1234\"}";
    char /* *cloud_payload = NULL, */ *uri = NULL;
    dm_msg_request_t request;

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_EXT_NTP_PREFIX;
    request.service_name = DM_DISP_NTP_REQUEST;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Request URI */
    res = dm_utils_service_name(request.service_prefix, request.service_name,
                                request.product_key, request.device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

#if 0
    cloud_payload = DM_malloc(payload_len + 1);
    if (cloud_payload == NULL) {
        DM_free(uri);
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(cloud_payload, 0, payload_len + 1);
    memcpy(cloud_payload, payload, payload_len);
#endif

    res = dm_cmw_send_to_cloud(uri, (char *)ntp_request_fmt, NULL);
    if (res != SUCCESS_RETURN) {
        DM_free(uri);//DM_free(cloud_payload);
        dm_log_err(DM_UTILS_LOG_CM_SEND_MESSAGE_FAILED);
        return FAIL_RETURN;
    }

    DM_free(uri);//DM_free(cloud_payload);
    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_upstream_thing_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    dm_mgr_dev_node_t *node = NULL;
    char *msgid_str = NULL, *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid < 0 || identifier == NULL || identifier_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Response Msg ID */
    res = dm_utils_itoa(msgid, &msgid_str);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    request.id.value = msgid_str;
    request.id.value_length = strlen(msgid_str);

    /* Service Name */
    service_name_len = strlen(DM_DISP_THING_SERVICE_RESPONSE) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        DM_free(msgid_str);
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_DISP_THING_SERVICE_RESPONSE, identifier_len, identifier);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = service_name;
    memcpy(response.product_key, node->product_key, strlen(node->product_key));
    memcpy(response.device_name, node->device_name, strlen(node->device_name));
    response.code = code;

    dm_log_debug("Current Service Name: %s", service_name);
    dm_msg_response_with_data(&request, &response, payload, payload_len);

    DM_free(msgid_str);
    DM_free(service_name);
    return SUCCESS_RETURN;
}

int dm_mgr_upstream_thing_service_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    dm_mgr_dev_node_t *node = NULL;
    char *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid == NULL || msgid_len <= 0 || identifier == NULL || identifier_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    request.id.value = msgid;
    request.id.value_length = msgid_len;

    /* Service Name */
    service_name_len = strlen(DM_DISP_THING_SERVICE_RESPONSE) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_DISP_THING_SERVICE_RESPONSE, identifier_len, identifier);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = service_name;
    memcpy(response.product_key, node->product_key, strlen(node->product_key));
    memcpy(response.device_name, node->device_name, strlen(node->device_name));
    response.code = code;

    dm_log_debug("Current Service Name: %s", service_name);
    dm_msg_response_with_data(&request, &response, payload, payload_len);

    DM_free(service_name);
    return SUCCESS_RETURN;
}

int dm_mgr_upstream_thing_property_get_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *payload, _IN_ int payload_len, _IN_ void *ctx)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    if (devid < 0 || msgid == NULL || msgid_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Response Msg ID */
    request.id.value = msgid;
    request.id.value_length = msgid_len;

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_SERVICE_PROPERTY_GET;
    memcpy(response.product_key, node->product_key, strlen(node->product_key));
    memcpy(response.device_name, node->device_name, strlen(node->device_name));
    response.code = code;

    dm_log_debug("Current Service Name: %s", DM_DISP_THING_SERVICE_PROPERTY_GET);

    /* Send Property Get Response Message To Local */
    dm_msg_response_local_with_data(&request, &response, payload, payload_len, ctx);
    return SUCCESS_RETURN;
}

int dm_mgr_upstream_rrpc_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                                  _IN_ char *rrpcid, _IN_ int rrpcid_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    const char *rrpc_response_service_name = "rrpc/response/%.*s";
    dm_mgr_dev_node_t *node = NULL;
    char *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid == NULL || msgid_len <= 0 ||
        rrpcid == NULL || rrpcid_len <= 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Response Msg ID */
    request.id.value = msgid;
    request.id.value_length = msgid_len;

    /* Service Name */
    service_name_len = strlen(rrpc_response_service_name) + rrpcid_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, rrpc_response_service_name, rrpcid_len, rrpcid);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = service_name;
    memcpy(response.product_key, node->product_key, strlen(node->product_key));
    memcpy(response.device_name, node->device_name, strlen(node->device_name));
    response.code = code;

    dm_log_debug("Current Service Name: %s", service_name);
    dm_msg_response_with_data(&request, &response, payload, payload_len);

    DM_free(service_name);

    return SUCCESS_RETURN;
}

int dm_mgr_upstream_thing_lan_prefix_get(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_DISP_SYS_PREFIX;
    request.service_name = DM_DISP_THING_LAN_PREFIX_GET;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_lan_prefix_get(&request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = dm_msg_get_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Send Message To Cloud */
    res = dm_msg_request_cloud(&request);
    if (res == SUCCESS_RETURN) {
        res = request.msgid;
    }

    DM_free(request.params);

    return res;
}

#ifdef DEPRECATED_LINKKIT
int dm_mgr_deprecated_set_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t tsl_source)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || tsl_source < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->tsl_source = tsl_source;

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t *tsl_source)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || tsl_source == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *tsl_source = node->tsl_source;

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_shadow(_IN_ int devid, void **shadow)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || shadow == NULL || *shadow != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *shadow = node->dev_shadow;

    return SUCCESS_RETURN;
}

static int dm_mgr_deprecated_search_devid_by_node(_IN_ dm_mgr_dev_node_t *node, _OU_ int *devid)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_node == node) {
            dm_log_debug("Device Found, node: %p", node);
            if (devid) {
                *devid = search_node->devid;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, node: %p", node);
    return FAIL_RETURN;
}

int dm_mgr_deprecated_search_devid_by_device_node(_IN_ void *node, _OU_ int *devid)
{
    int res = 0;

    if (node == NULL || devid == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_search_devid_by_node((dm_mgr_dev_node_t *)node, devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_tsl(int devid, iotx_dm_tsl_type_t tsl_type, const char *tsl, int tsl_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (tsl == NULL || tsl_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_DMGR_DEVICE_NOT_FOUND, devid);
        return FAIL_RETURN;
    }

    res = dm_shw_create(tsl_type, tsl, tsl_len, &node->dev_shadow);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_product_key(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN])
{
    int res  = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || product_key == NULL || strlen(product_key) >= PRODUCT_KEY_MAXLEN) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memcpy(product_key, node->product_key, strlen(node->product_key));

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_device_name(_IN_ int devid, _OU_ char device_name[DEVICE_NAME_MAXLEN])
{
    int res  = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || device_name == NULL || strlen(device_name) >= DEVICE_NAME_MAXLEN) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memcpy(device_name, node->device_name, strlen(node->device_name));

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_property_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_property_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_input_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_output_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_event_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_event_output_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_data_type(_IN_ void *data, _OU_ dm_shw_data_type_e *type)
{
    if (data == NULL || type == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return dm_shw_get_data_type(data, type);
}

int dm_mgr_deprecated_get_property_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_property_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_service_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_event_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_property_by_index(_IN_ int devid, _IN_ int index, _OU_ void **property)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_property_by_index(node->dev_shadow, index, property);
}

int dm_mgr_deprecated_get_service_by_index(_IN_ int devid, _IN_ int index, _OU_ void **service)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_by_index(node->dev_shadow, index, service);
}

int dm_mgr_deprecated_get_event_by_index(_IN_ int devid, _IN_ int index, _OU_ void **event)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_by_index(node->dev_shadow, index, event);
}

int dm_mgr_deprecated_get_service_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **service)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || service == NULL || *service != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_by_identifier(node->dev_shadow, identifier, service);
}

int dm_mgr_deprecated_get_event_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **event)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || event == NULL || *event != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_by_identifier(node->dev_shadow, identifier, event);
}

int dm_mgr_deprecated_get_property_identifier(_IN_ void *property, _OU_ char **identifier)
{
    if (property == NULL || identifier == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return dm_shw_get_property_identifier(property, identifier);
}

int dm_mgr_deprecated_get_service_method(_IN_ void *service, _OU_ char **method)
{
    if (service == NULL || method == NULL || *method != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return dm_shw_get_service_method(service, method);
}

int dm_mgr_deprecated_get_event_method(_IN_ void *event, _OU_ char **method)
{
    if (event == NULL || method == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return dm_shw_get_event_method(event, method);
}

int dm_mgr_deprecated_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_property_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_property_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_event_output_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_event_output_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_service_input_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_service_output_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_output_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_property(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
                                        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_property(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_event_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_event_output(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_service_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_service_output(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif