/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <assert.h>
#include <string.h>

#include "iotx_cm_connectivity.h"
#ifdef CONFIG_CM_VIA_CLOUD_CONN
    #include "iotx_cm_cloud_conn.h"
#endif /* CM_SUPPORT_CLOUD_CONN */
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    #include "iotx_cm_local_conn.h"

    extern void _alcs_free_alcs_transfer_context(void *cntx);
#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */


#ifdef CM_PROCESS_NODE_USE_POOL
    static iotx_cm_process_list_node_t g_cm_cloud_process_node_list[CM_SUPPORT_MAX_PROCESS_NODE_SIZE];
#endif

#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    #ifdef CM_PROCESS_NODE_USE_POOL
        static iotx_cm_process_list_node_t g_cm_local_process_node_list[CM_SUPPORT_MAX_PROCESS_NODE_SIZE];
    #endif
#endif

iotx_cm_connectivity_status_t iotx_cm_get_connectivity_status(iotx_cm_connectivity_t *connectivity)
{
    return connectivity ? connectivity->status : IOTX_CM_CONNECTIVITY_STATUS_UNCREATED;
}


iotx_cm_connectivity_types_t iotx_cm_get_connectivity_type(iotx_cm_connectivity_t *connectivity)
{
    return connectivity ? connectivity->type : IOTX_CM_CONNECTIVITY_TYPE_MAX;
}


int iotx_cm_set_connectivity_status(iotx_cm_connectivity_t *connectivity, iotx_cm_connectivity_status_t status)
{
    if (connectivity) {
        connectivity->status = status;
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}


iotx_cm_connectivity_t  *iotx_cm_create_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_param_t *param)
{
    iotx_cm_connectivity_t *connectivity = NULL;

    switch (param->connectivity_type) {
        case IOTX_CM_CONNECTIVITY_TYPE_CLOUD:
            connectivity = iotx_cm_cloud_conn_init(cm_ctx, param->cloud_param);
            break;

        case IOTX_CM_CONNECTIVITY_TYPE_LOCAL:
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
            connectivity = iotx_cm_local_conn_init(cm_ctx, param->alcs_param);
#endif
            break;

        case IOTX_CM_CONNECTIVITY_TYPE_UNDEF:
            break;

        default:
            break;
    }

    if (connectivity) {
        connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_CREATED;

        if (FAIL_RETURN == iotx_cm_add_connectivity(cm_ctx, connectivity)) {
            connectivity->deinit_func(cm_ctx, connectivity);
            return NULL;
        }
    }

    return connectivity;
}


int iotx_cm_add_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t *connectivity)
{
    linked_list_t *list = NULL;

    assert(cm_ctx && connectivity);
    list = cm_ctx->list_connectivity;

    assert(list);
    linked_list_insert(list, connectivity);

    connectivity->status = IOTX_CM_CONNECTIVITY_STATUS_INITED;

    CM_ERR("Add Connectivity Success, Type: %d", connectivity->type);
    return SUCCESS_RETURN;
}


int iotx_cm_connect_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t *connectivity)
{
    int ret = connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);

    if (SUCCESS_RETURN == ret) {
        iotx_cm_set_connectivity_status(connectivity, IOTX_CM_CONNECTIVITY_STATUS_CONNECTED);
    }

    return ret;
}


int iotx_cm_destory_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_t **connectivity)
{
    if ((*connectivity) && (*connectivity)->deinit_func) {
        if (FAIL_RETURN == (*connectivity)->deinit_func(cm_ctx, *connectivity)) {
            CM_ERR(cm_log_error_fail);
        }
        (*connectivity)->status = IOTX_CM_CONNECTIVITY_STATUS_UNCREATED;

#if (CONFIG_SDK_THREAD_COST == 1)
        (*connectivity)->is_try_connect = 0;
        HAL_SleepMs(200);
#endif
        linked_list_remove(cm_ctx->list_connectivity, (*connectivity));
        LITE_free((*connectivity));
    }

    (*connectivity) = NULL;

    return SUCCESS_RETURN;
}


static void iotx_cm_find_connectivity_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_connectivity_t **target_connectivity = NULL;
    iotx_cm_send_peer_t *target;
    void *conn_ctx;

    target_connectivity = va_arg(*params, iotx_cm_connectivity_t **);
    target = va_arg(*params, void *);
    conn_ctx = va_arg(*params, void *);

    assert(target_connectivity);

    /* TODO */
    if (conn_ctx && IOTX_CM_CONNECTIVITY_TYPE_LOCAL == connectivity->type) {
        *target_connectivity = connectivity;
        return;
    }

    if (connectivity && target && connectivity->check_target_func) {
        if (SUCCESS_RETURN == connectivity->check_target_func(target)) {
            *target_connectivity = connectivity;
        }
    }
}


iotx_cm_connectivity_t *iotx_cm_find_connectivity(iotx_cm_conntext_t *cm_ctx, iotx_cm_send_peer_t *target,
        void *conn_ctx)
{
    linked_list_t *list = NULL;
    iotx_cm_connectivity_t *target_connectivity = NULL;

    assert(cm_ctx);
    list = cm_ctx->list_connectivity;

    assert(list);

    linked_list_iterator(list, iotx_cm_find_connectivity_handler, &target_connectivity, target, conn_ctx);

    return target_connectivity;
}


static void iotx_cm_find_connectivity_by_type_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_connectivity_t **search_connectivity = NULL;
    int type;

    search_connectivity = va_arg(*params, iotx_cm_connectivity_t **);
    type = va_arg(*params, int);

    if (type == (int)connectivity->type) {
        *search_connectivity = connectivity;
    }
}


iotx_cm_connectivity_t *iotx_cm_find_connectivity_by_type(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type)
{
    linked_list_t *list = NULL;
    iotx_cm_connectivity_t *target_connectivity = NULL;

    assert(cm_ctx);
    list = cm_ctx->list_connectivity;

    assert(list);

    linked_list_iterator(list, iotx_cm_find_connectivity_by_type_handler, &target_connectivity, (int)type);

    return target_connectivity;
}


static void cm_trigger_connected_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);

    assert(cm_ctx);

    iotx_cm_connect_connectivity(cm_ctx, connectivity);
}


int iotx_cm_connect_connectivity_all(iotx_cm_conntext_t *cm_ctx)
{
    linked_list_iterator(cm_ctx->list_connectivity, cm_trigger_connected_handler, cm_ctx);
    return SUCCESS_RETURN;
}


int iotx_cm_get_connectivity_id()
{
    static int _connectivity_id = 0;
    _connectivity_id++;
    return _connectivity_id;
}



#if (CONFIG_SDK_THREAD_COST == 1)

iotx_cm_process_list_node_t *iotx_cm_get_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type)
{
#ifdef CM_PROCESS_NODE_USE_POOL
    int i = 0;
    iotx_cm_process_list_node_t *list = NULL;

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        list = g_cm_cloud_process_node_list;
    }
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        list = g_cm_local_process_node_list;
    }
#endif
    else {
        return NULL;
    }

    for (i = 0; i < CM_SUPPORT_MAX_PROCESS_NODE_SIZE; i++) {
        if (list[i].is_used == 0) {
            list[i].is_used = 1;
            return &list[i];
        }
    }
    return NULL;
#else
    iotx_cm_process_list_node_t *node = NULL;
    node = CM_malloc(sizeof(iotx_cm_process_list_node_t));
    if (NULL == node) {
        return NULL;
    }

    memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
    return node;
#endif
}


int iotx_cm_free_list_node(iotx_cm_conntext_t *cm_ctx, iotx_cm_connectivity_types_t type,
                           iotx_cm_process_list_node_t *node)
{
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == node || cm_ctx == NULL) {
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_TYPE_CLOUD == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_cloud_conn_get_target(), NULL);
        if (NULL == connectivity) {
            CM_WARNING("iotx_cm_find_connectivity() returns NULL");
            return FAIL_RETURN;
        }

    }
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    else if (IOTX_CM_CONNECTIVITY_TYPE_LOCAL == type) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, iotx_cm_local_conn_get_target(), NULL);
        if (NULL == connectivity) {
            CM_WARNING("iotx_cm_find_connectivity() returns NULL");
            return FAIL_RETURN;
        }
    }
#endif

    HAL_MutexLock(connectivity->process_lock);

#ifdef CM_PROCESS_NODE_USE_POOL
    if (node->is_used == 1) {
        /* CM_ERR("Free Node: %p, func: %s, line: %d",node); */
        node->is_used = 0;
        memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
        HAL_MutexUnlock(connectivity->process_lock);
        return SUCCESS_RETURN;
    }

    HAL_MutexUnlock(connectivity->process_lock);
    return FAIL_RETURN;
#else
    LITE_free(node);
    HAL_MutexUnlock(connectivity->process_lock);
    return SUCCESS_RETURN;
#endif
}


void iotx_cm_free_process_list_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_process_list_t *node_list = connectivity->process_list;

    if (NULL == node_list) {
        return;
    }

    iotx_cm_process_list_node_t *node = node_list->header;
    while (node) {
        iotx_cm_process_list_node_t *next_node = node;
        switch (node->type) {
            case IOTX_CM_PROCESS_REGISTER: {
                int i = 0;
                iotx_cm_process_register_t *_register = (iotx_cm_process_register_t *)node->msg;
                iotx_cm_register_param_t **register_param = (iotx_cm_register_param_t **)_register->register_param;
                for (; i < _register->count; i++) {
                    LITE_free(register_param[i]->URI);
                    LITE_free(register_param[i]);
                }
                LITE_free(register_param);
                LITE_free(_register);
            }
            break;

            case IOTX_CM_PROCESS_UNREGISTER: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_ADD_SERVICE: {
                iotx_cm_process_service_t *_service = (iotx_cm_process_service_t *)node->msg;
                LITE_free(_service->URI);
                LITE_free(_service);
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_SERVICE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_ADD_SUBDIVCE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_SUBDIVCE: {
                LITE_free(node->msg);
            }
            break;

            case IOTX_CM_PROCESS_CLOUD_SEND: {
                iotx_cm_process_send_t *send = (iotx_cm_process_send_t *)node->msg;
                if (send->URI) {
                    LITE_free(send->URI);
                }
                if (send->payload) {
                    LITE_free(send->payload);
                }
                if (send->target) {
                    LITE_free(send->target);
                }
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
                if (send->conn_ctx) {
                    _alcs_free_alcs_transfer_context(send->conn_ctx);
                }
#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */
                if (send->sem) {
                    HAL_SemaphorePost(send->sem);
                }
                LITE_free(send);
            }
            break;

            case IOTX_CM_PROCESS_REGISTER_RESULT:
            case IOTX_CM_PROCESS_UNREGISTER_RESULT: {
                iotx_cm_process_register_result_t *_register = (iotx_cm_process_register_result_t *)node->msg;
                if (_register->URI) {
                    LITE_free(_register->URI);
                }
                LITE_free(_register);
            }
            break;

            case IOTX_CM_PROCESS_CLOUD_NEW_DATA: {
                iotx_cm_message_info_t *message_info = (iotx_cm_message_info_t *)node->msg;
                if (message_info->URI) {
                    LITE_free(message_info->URI);
                }
                if (message_info->payload) {
                    LITE_free(message_info->payload);
                }
                LITE_free(message_info);
            }
            break;

            case IOTX_CM_PROCESS_ADD_DEVICE: {
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
                iotx_cm_local_device_t *device = (iotx_cm_local_device_t *)node->msg;
                if (device->addr) {
                    LITE_free(device->addr);
                }
                if (device->device_info) {
                    LITE_free(device->device_info);
                }
                if (device) {
                    LITE_free(device);
                }
#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */
            }
            break;

            case IOTX_CM_PROCESS_REMOVE_DEVICE: {
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
                iotx_cm_local_device_t *device = (iotx_cm_local_device_t *)node->msg;
                if (device->addr) {
                    LITE_free(device->addr);
                }
                if (device->device_info) {
                    LITE_free(device->device_info);
                }
                if (device) {
                    LITE_free(device);
                }
#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */
            }
            break;

            case IOTX_CM_PROCESS_ADD_SERVICE_RESULT:
            case IOTX_CM_PROCESS_REMOVE_SERVICE_RESULT: {
            }
            break;

            case IOTX_CM_PROCESS_LOCAL_NEW_DATA: {
                iotx_cm_message_info_t *message_info = (iotx_cm_message_info_t *)node->msg;
                if (message_info->URI) {
                    LITE_free(message_info->URI);
                }
                if (message_info->payload) {
                    LITE_free(message_info->payload);
                }
                LITE_free(message_info);
            }
            break;

            case IOTX_CM_PROCESS_LOCAL_SEND: {
                iotx_cm_process_send_t *send = (iotx_cm_process_send_t *)node->msg;
                if (send->URI) {
                    LITE_free(send->URI);
                }
                if (send->payload) {
                    LITE_free(send->payload);
                }
                if (send->target) {
                    LITE_free(send->target);
                }
                LITE_free(send);
                break;
            }
            case IOTX_CM_PROCESS_LOCAL_CLOUD_INIT: {

            }
            break;
            default:
                break;
        }
        next_node = node->next;

#ifdef CM_PROCESS_NODE_USE_POOL
        if (node->is_used == 1) {
            node->is_used = 0;
            memset(node, 0x0, sizeof(iotx_cm_process_list_node_t));
        }
#else
        LITE_free(node);
#endif
        node = next_node;
    }

    LITE_free(connectivity->process_list);
}


int iotx_cm_free_list_node_all(iotx_cm_conntext_t *cm_ctx)
{
    assert(cm_ctx);

    if (NULL == cm_ctx->list_connectivity) {
        return FAIL_RETURN;
    }

    linked_list_iterator(cm_ctx->list_connectivity, iotx_cm_free_process_list_handler, cm_ctx);

    return SUCCESS_RETURN;
}


/* node is in */
int iotx_cm_process_list_push(iotx_cm_conntext_t *cm_ctx,
                              iotx_cm_connectivity_types_t type,
                              iotx_cm_process_list_node_t *node)
{
    iotx_cm_process_list_t *list = NULL;
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx || NULL == node) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, type);
    if (NULL == connectivity) {
        CM_ERR("Current Connectivity Count: %d", linked_list_get_size((const linked_list_t *)cm_ctx->list_connectivity));
        CM_ERR("Cannot Found Connectivity,Type: %d", type);
        return FAIL_RETURN;
    }

    HAL_MutexLock(connectivity->process_lock);

    list = connectivity->process_list;

    if (NULL == list) {
        list = CM_malloc(sizeof(iotx_cm_process_list_t));

        if (NULL == list) {
            HAL_MutexUnlock(connectivity->process_lock);
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }

        list->header = list->tailer = NULL;
        list->size = 0;

        connectivity->process_list = list;
    }

    if (list->size == 0) {
        list->header = list->tailer = node;
        list->size = 1;
    } else if (list->size == CM_SUPPORT_MAX_PROCESS_NODE_SIZE) {
        HAL_MutexUnlock(connectivity->process_lock);
        CM_ERR(cm_log_error_process);
        return FAIL_RETURN;
    } else {
        node->pre = list->tailer;
        list->tailer->next = node;
        list->tailer = node;
        list->size++;
    }

    HAL_MutexUnlock(connectivity->process_lock);

    return SUCCESS_RETURN;
}


iotx_cm_process_list_node_t *iotx_cm_process_list_pop(iotx_cm_conntext_t *cm_ctx,
        iotx_cm_connectivity_types_t type)
{
    iotx_cm_process_list_t *list = NULL;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, type);
    if (NULL == connectivity) {
        return NULL;
    }

    HAL_MutexLock(connectivity->process_lock);

    list = connectivity->process_list;

    if (NULL == list) {
        CM_ERR(cm_log_error_status);
        return NULL;
    }

    if (list->size == 1) {
        node = list->header;
        list->header = list->tailer = NULL;
        list->size = 0;
        LITE_free(list);
        list = NULL;
        connectivity->process_list = NULL;
    } else {
        node = list->header;
        list->header = list->header->next;
        list->header->pre = NULL;
        list->size--;
    }

    HAL_MutexUnlock(connectivity->process_lock);
    return node;
}


int iotx_cm_process_list_get_size(iotx_cm_conntext_t *cm_ctx,
                                  iotx_cm_connectivity_types_t type)
{
    iotx_cm_connectivity_t *connectivity = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    connectivity = iotx_cm_find_connectivity_by_type(cm_ctx, type);
    if (NULL == connectivity) {
        return FAIL_RETURN;
    }

    return (connectivity && connectivity->process_list) ? connectivity->process_list->size : 0;
}
#endif /* CONFIG_SDK_THREAD_COST */

