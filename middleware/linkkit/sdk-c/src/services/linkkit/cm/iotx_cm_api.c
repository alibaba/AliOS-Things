/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <assert.h>
#include <string.h>

#include "iot_import.h"

#include "iotx_utils.h"
#include "iotx_system.h"
#include "iot_export.h"
#include "iotx_cm_common.h"
#include "iotx_cm_connectivity.h"
#include "iotx_cm_connection.h"
#include "iotx_cm_conn_mqtt.h"

static iotx_cm_conntext_t *g_cm_ctx = NULL;

#ifdef CONFIG_CM_VIA_CLOUD_CONN
    extern iotx_cm_send_peer_t *iotx_cm_cloud_conn_get_target();
#endif /* CONFIG_CM_VIA_CLOUD_CONN */


static void cm_trigger_connected_event_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_init_param_t *init_param;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    init_param = va_arg(*params, iotx_cm_init_param_t *);

    assert(cm_ctx && init_param);

    connectivity->trigger_connected_func(cm_ctx, connectivity, init_param->event_func, init_param->user_data);
}


/**
 * @brief CM init
 *        This function initialize the CM structures, establish network connection
 *        If CM has been initialized, this function will return success directly.
 *
 * @param pparam, specify the cm and event handler.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_init(iotx_cm_init_param_t *init_param, void *option)
{
    iotx_conn_info_t *pconn_info;
    iotx_cm_event_cb_usr_ctx_t *cb_usr_ctx = NULL;

    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};
    char device_id[DEVICE_ID_LEN + 1] = {0};

    CM_INFO(cm_log_info_version, "0.3");

    if (NULL == init_param || init_param->event_func == NULL) {
        return FAIL_RETURN;
    }
    // Setup domain type
    iotx_guider_set_domain_type(init_param->domain_type);

    if (NULL == g_cm_ctx) {
        HAL_GetProductKey(product_key);
        HAL_GetDeviceName(device_name);
        HAL_GetDeviceID(device_id);
        HAL_GetDeviceSecret(device_secret);

        ARGUMENT_SANITY_CHECK(strlen(device_name), FAIL_RETURN);
        ARGUMENT_SANITY_CHECK(strlen(product_key), FAIL_RETURN);
        ARGUMENT_SANITY_CHECK(strlen(device_id), FAIL_RETURN);

        assert(strlen(device_name) && strlen(product_key) && strlen(device_id));

        /* Device AUTH */
        if (0 != IOT_SetupConnInfo(product_key, device_name, device_secret, (void **)&pconn_info)) {
            CM_ERR(cm_log_error_auth);
            return FAIL_RETURN;
        }

        CM_INFO(cm_log_info_init);
        if (NULL == (g_cm_ctx = CM_malloc(sizeof(iotx_cm_conntext_t)))) {
            CM_ERR(cm_log_error_memory);
            return FAIL_RETURN;
        }
        memset(g_cm_ctx, 0x0, sizeof(iotx_cm_conntext_t));

        g_cm_ctx->response_func = iotx_cm_response_func;
#if (CONFIG_SDK_THREAD_COST == 1)
        g_cm_ctx->action_lock = HAL_MutexCreate();
#endif
        g_cm_ctx->list_event_callback = linked_list_create("cm event_cb list", 0);
        g_cm_ctx->list_connectivity = linked_list_create("cm connectivity list", 0);
    }

    cb_usr_ctx = CM_malloc(sizeof(iotx_cm_event_cb_usr_ctx_t));
    if (NULL == cb_usr_ctx) {
        CM_ERR(cm_log_error_memory);
        goto exit;
    }

    cb_usr_ctx->event_func = init_param->event_func;
    cb_usr_ctx->user_data = init_param->user_data;

    if (g_cm_ctx->list_event_callback) {
        linked_list_insert(g_cm_ctx->list_event_callback, cb_usr_ctx);
    } else {
        LITE_free(cb_usr_ctx);
    }

    if (g_cm_ctx->inited == 0) {
        g_cm_ctx->inited = 1;
    } else {
        linked_list_iterator(g_cm_ctx->list_connectivity, cm_trigger_connected_event_handler, g_cm_ctx, init_param);
    }

    return SUCCESS_RETURN;

exit:
    CM_ERR(cm_log_error_fail);
    iotx_cm_deinit(g_cm_ctx);

    return FAIL_RETURN;
}


/**
 * @brief Create connectivity
 *        This function use to create one connectivity with protocol and url.
 *
 * @param connectivity_param, connectivity type and protocol, url, port, psk.
 * @param option, reserve.
 *
 * @return Connectivity handler.
 */
void *iotx_cm_open(iotx_cm_connectivity_param_t *connectivity_param, void *option)
{
    if (NULL == connectivity_param) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    if (NULL == g_cm_ctx) {
        CM_ERR(cm_log_error_status);
        return NULL;
    }

    /* create thread */
    return iotx_cm_create_connectivity(g_cm_ctx, connectivity_param);
}

/**
 * @brief Get protocol connection handle from connectivity
 *        This function use to get protocol connection handler from connectivity.
 *        ex: mqtt handler returned by IOT_MQTT_Construct
 *            alcs handler returned by iotx_alcs_construct
 * @param connectivity, the return by iotx_cm_open.
 *
 * @return Connectivity handler.
 */
void *iotx_cm_get_protocol_handle(void *connectivity)
{
    iotx_cm_connectivity_t *connectivity_ptr = (iotx_cm_connectivity_t *)connectivity;
    iotx_cm_connection_t *connection_ptr = NULL;

    if (connectivity_ptr == NULL) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    connection_ptr = (iotx_cm_connection_t *)connectivity_ptr->context;
    if (connection_ptr == NULL) {
        return NULL;
    }

    switch (connectivity_ptr->type) {
        case IOTX_CM_CONNECTIVITY_TYPE_CLOUD: {
            iotx_cloud_conn_mqtt_t *mqtt_ctx = NULL;

            mqtt_ctx = (iotx_cloud_conn_mqtt_t *)connection_ptr->context;
            if (mqtt_ctx == NULL) {
                return NULL;
            }

            return mqtt_ctx->mqtt_handler;
        }
        break;
        case IOTX_CM_CONNECTIVITY_TYPE_LOCAL: {
            return connection_ptr->context;
        }
        break;
        default:
            break;
    }

    return NULL;
}

/**
 * @brief Connect connectivity
 *        This function use to connect one connectivity with create by iotx_cm_open.
 *
 * @param connectivity, the return by iotx_cm_open.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_connect(void *connectivity, void *option)
{
    if (NULL == g_cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == connectivity) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (IOTX_CM_CONNECTIVITY_STATUS_INITED != iotx_cm_get_connectivity_status(connectivity)) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 1)
    {
        /* send message to itself thread */
        iotx_cm_process_list_node_t *node = NULL;

        if (NULL == (node = iotx_cm_get_list_node(g_cm_ctx, iotx_cm_get_connectivity_type(connectivity)))) {
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_CONNECT;

        if (FAIL_RETURN == iotx_cm_process_list_push(g_cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
            iotx_cm_free_list_node(g_cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
            return FAIL_RETURN;
        }
        return SUCCESS_RETURN;
    }
#else /* CONFIG_SDK_THREAD_COST */
    return iotx_cm_connect_connectivity(g_cm_ctx, connectivity);
#endif /* CONFIG_SDK_THREAD_COST */
}


/**
 * @brief Destroy connectivity
 *        This function use to free one connectivity with create by iotx_cm_open.
 *
 * @param connectivity, the return by iotx_cm_open.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_close(void **connectivity, void *option)
{
    if (NULL == (*connectivity)) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (NULL == g_cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }
#if (CONFIG_SDK_THREAD_COST == 1)
    {
        iotx_cm_connectivity_t *connectivity_node = (iotx_cm_connectivity_t *)(*connectivity);
        connectivity_node->deinit_func(g_cm_ctx, connectivity_node);
        /* send message to itself thread */
        /* iotx_cm_process_list_node_t *node = NULL;

        if (NULL == (node = iotx_cm_get_list_node(g_cm_ctx, iotx_cm_get_connectivity_type(*connectivity)))) {
            return FAIL_RETURN;
        }

        node->type = IOTX_CM_PROCESS_DISCONNECT;

        if (FAIL_RETURN == iotx_cm_process_list_push(g_cm_ctx, iotx_cm_get_connectivity_type((*connectivity)), node)) {
            iotx_cm_free_list_node(g_cm_ctx, iotx_cm_get_connectivity_type(*connectivity), node);
            return FAIL_RETURN;
        }
        CM_ERR("1. Close Connectivity Node: %p",node); */
        HAL_SleepMs(1000);
        return SUCCESS_RETURN;
    }
#else /* CONFIG_SDK_THREAD_COST */
    return iotx_cm_destory_connectivity(g_cm_ctx, (iotx_cm_connectivity_t **)connectivity);
#endif /* CONFIG_SDK_THREAD_COST */
}


#if (CONFIG_SDK_THREAD_COST == 1)
static int cm_connectivity_register(void *handler, void *connectivity, iotx_cm_register_param_t *register_param,
                                    int count)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_process_register_t *msg = NULL;
    iotx_cm_register_param_t **param = NULL;
    char *URI;
    int length = 0, i = 0, fail = 0;

    CM_ERR("Current Type: %d", iotx_cm_get_connectivity_type(connectivity));

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    node->type = IOTX_CM_PROCESS_REGISTER;
    msg = node->msg = CM_malloc(sizeof(iotx_cm_process_register_t));
    if (NULL == node->msg) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    param = CM_malloc(sizeof(iotx_cm_register_param_t *) * count);
    if (NULL == param) {
        CM_ERR(cm_log_error_memory);
        return FAIL_RETURN;
    }

    for (; i < count; i++) {
        length = strlen(register_param[i].URI);
        URI = CM_malloc(length + 1);
        if (NULL == URI) {
            CM_ERR(cm_log_error_memory);
            iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
            fail = 1;
            break;
        }
        strcpy(URI, register_param[i].URI);

        param[i] = CM_malloc(sizeof(iotx_cm_register_param_t));
        if (NULL == param[i]) {
            CM_ERR(cm_log_error_memory);
            LITE_free(URI);
            LITE_free(msg);
            iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
            fail = 1;
            break;
        }
        param[i]->URI = URI;
        param[i]->register_func = register_param[i].register_func;
        param[i]->user_data = register_param[i].user_data;
        param[i]->mail_box = register_param[i].mail_box;
    }

    msg->register_param = param;
    msg->count = count;

    if (fail == 1) {
        for (; i >= 0; i--) {
            LITE_free(param[i]->URI);
            LITE_free(param[i]);
        }
        LITE_free(param);
        LITE_free(msg);
        return FAIL_RETURN;
    }

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        for (i = 0; i < count; i++) {
            LITE_free(param[i]->URI);
            LITE_free(param[i]);
        }
        LITE_free(param);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


static int cm_connectivity_unregister(void *handler, void *connectivity, iotx_cm_unregister_param_t *unregister_param)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;
    char *URI;
    int length = 0;

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    length = strlen(unregister_param->URI);
    URI = CM_malloc(length + 1);
    if (NULL == URI) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }
    strcpy(URI, unregister_param->URI);

    node->type = IOTX_CM_PROCESS_UNREGISTER;
    node->msg = URI;

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        LITE_free(URI);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


static int cm_connectivity_add_service(void *handler, void *connectivity,
                                       iotx_cm_add_service_param_t *add_service_param)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_process_service_t *msg = NULL;
    char *URI;
    int length = 0;

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    length = strlen(add_service_param->URI);
    URI = CM_malloc(length + 1);
    if (NULL == URI) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }
    strcpy(URI, add_service_param->URI);

    node->type = IOTX_CM_PROCESS_ADD_SERVICE;
    node->msg = CM_malloc(sizeof(iotx_cm_process_service_t));
    if (NULL == node->msg) {
        CM_ERR(cm_log_error_memory);
        LITE_free(URI);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }
    msg = node->msg;
    msg->URI = URI;
    msg->auth_type = add_service_param->auth_type;
    msg->register_func = add_service_param->service_func;
    msg->user_data = add_service_param->user_data;
    msg->mail_box = add_service_param->mail_box;

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        LITE_free(URI);
        LITE_free(node->msg);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


static int cm_connectivity_remove_service(void *handler, void *connectivity,
        iotx_cm_remove_service_param_t *remove_service_param)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;
    char *URI;
    int length = 0;

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    length = strlen(remove_service_param->URI);
    URI = CM_malloc(length + 1);
    if (NULL == URI) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }
    strcpy(URI, remove_service_param->URI);

    node->type = IOTX_CM_PROCESS_REMOVE_SERVICE;
    node->msg = URI;

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        LITE_free(URI);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


static int cm_connectivity_sub_device(void *handler, void *connectivity, const char *PK, const char *DN, int is_add)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;
    iotx_cm_process_subdevice_t *subdevice = NULL;

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    node->type = is_add ? IOTX_CM_PROCESS_ADD_SUBDIVCE : IOTX_CM_PROCESS_REMOVE_SUBDIVCE;
    node->msg = CM_malloc(sizeof(iotx_cm_process_subdevice_t));
    if (NULL == node->msg) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    subdevice = node->msg;
    strncpy(subdevice->pk, PK, strlen(PK));
    strncpy(subdevice->dn, DN, strlen(DN));

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        LITE_free(node->msg);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int cm_connectivity_init_second(void *handler, void *connectivity)
{
    iotx_cm_conntext_t *cm_ctx = (iotx_cm_conntext_t *)handler;
    iotx_cm_process_list_node_t *node = NULL;

    node = iotx_cm_get_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity));
    if (NULL == node) {
        CM_ERR(cm_log_error_get_node);
        return FAIL_RETURN;
    }

    node->type = IOTX_CM_PROCESS_LOCAL_CLOUD_INIT;

    if (FAIL_RETURN == iotx_cm_process_list_push(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node)) {
        CM_ERR(cm_log_error_push_node);
        iotx_cm_free_list_node(cm_ctx, iotx_cm_get_connectivity_type(connectivity), node);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


static void cm_multi_iterator_action_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    cm_iterator_action_t action;
    int *ret;
    void *arg3 = NULL;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    action = va_arg(*params, int);
    ret = va_arg(*params, int *);
    arg3 = va_arg(*params, void *);

    assert(cm_ctx && action < cm_iterator_action_max);

    HAL_MutexLock(cm_ctx->action_lock);

    if (connectivity/* && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)*/) {
        switch (action) {
            case cm_iterator_action_register: {
                int arg4 = 0;
                arg4 = va_arg(*params, int);
                (*ret) = cm_connectivity_register(cm_ctx, connectivity, (iotx_cm_register_param_t *)arg3, arg4);
            }
            break;

            case cm_iterator_action_unregister: {
                (*ret) = cm_connectivity_unregister(cm_ctx, connectivity, (iotx_cm_unregister_param_t *)arg3);
            }
            break;

            case cm_iterator_action_add_service: {
                (*ret) = cm_connectivity_add_service(cm_ctx, connectivity, (iotx_cm_add_service_param_t *)arg3);
            }
            break;

            case cm_iterator_action_remove_service: {
                (*ret) = cm_connectivity_remove_service(cm_ctx, connectivity, (iotx_cm_remove_service_param_t *)arg3);
            }
            break;

            case cm_iterator_action_add_subdevice: {
                void *arg4 = NULL;
                arg4 = va_arg(*params, void *);
                (*ret) = cm_connectivity_sub_device(cm_ctx, connectivity, (const char *)arg3, (const char *)arg4, 1);
            }
            break;

            case cm_iterator_action_remove_subdevice: {
                void *arg4 = NULL;
                arg4 = va_arg(*params, void *);
                (*ret) = cm_connectivity_sub_device(cm_ctx, connectivity, (const char *)arg3, (const char *)arg4, 0);
            }
            break;

            case cm_iterator_action_init_second: {
                *ret = cm_connectivity_init_second(cm_ctx, connectivity);
            }
            break;
            default:
                break;
        }
    }
    HAL_MutexUnlock(cm_ctx->action_lock);
}
#endif


int iotx_cm_postinit(void *_connectivity)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx,
                             cm_iterator_action_init_second, &ret);
    } else {
        ret = cm_connectivity_init_second(cm_ctx, connectivity);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_init_second(cm_ctx, _connectivity);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}

/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to register.
 * @param register_param, register parameter, include URI and register callback.
 * @param option, reserve.
 *
 * @return success or fail.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 */
int iotx_cm_serv_reg(void *_connectivity, iotx_cm_register_param_t *register_param, int count, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && register_param && register_param->URI);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx, cm_iterator_action_register,
                             &ret, register_param, count);
    } else {
        ret = cm_connectivity_register(cm_ctx, connectivity, register_param, count);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_register(cm_ctx, _connectivity, register_param, count);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


/**
 * @brief Unregister service.
 *        This function used to unregister some service by different URI
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to unregister.
 * @param unregister_param, unregister parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_serv_unreg(void *_connectivity, iotx_cm_unregister_param_t *unregister_param, void *option)
{
    int ret = 0;

    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && unregister_param && unregister_param->URI);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx, cm_iterator_action_unregister,
                             &ret, unregister_param);
    } else {
        ret = cm_connectivity_unregister(cm_ctx, connectivity, unregister_param);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_unregister(cm_ctx, _connectivity, unregister_param->URI);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


/**
 * @brief Register service.
 *        This function used to register some service by different URI, set the URI's callback.
 *        If it is received a request, will callback the register_cb.
 *        If there is no match register_cb (user have not register the service set callback), the request will be discard.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to add service.
 * @param service_param, register parameter, include URI and register callback.
 * @param option, reserve.
 *          This API not support one URI register twice, if the URI have been register, it will return fail.
 *
 * @return success or fail.
 */
int iotx_cm_serv_add(void *_connectivity, iotx_cm_add_service_param_t *service_param, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && service_param && service_param->URI);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx,
                             cm_iterator_action_add_service, &ret, service_param);
    } else {
        ret = cm_connectivity_add_service(cm_ctx, connectivity, service_param);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_add_service(cm_ctx, _connectivity, service_param->URI, service_param->auth_type,
                              service_param->service_func, service_param->user_data, service_param->mail_box);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


/**
 * @brief Remove service.
 *        This function used to remove some service by different URI
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to remove service.
 * @param pparam, remove parameter, include URI.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_serv_del(void *_connectivity, iotx_cm_remove_service_param_t *service_param, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && service_param && service_param->URI);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx,
                             cm_iterator_action_remove_service, &ret, service_param);
    } else {
        ret = cm_connectivity_remove_service(cm_ctx, connectivity, service_param);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_remove_service(cm_ctx, _connectivity, service_param->URI);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


/**
 * @brief add sub-device.
 *        This function used to add sub-device with sub-device's pk and dn.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to add sub-device.
 * @param PK, product_key.
 * @param DN, device_name.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_subdev_add(void *_connectivity, const char *PK, const char *DN, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && PK && DN);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx,
                             cm_iterator_action_add_subdevice, &ret, PK, DN);
    } else {
        ret = cm_connectivity_sub_device(cm_ctx, connectivity, PK, DN, 1);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_add_subdevice(cm_ctx, _connectivity, PK, DN);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


/**
 * @brief remove sub-device.
 *        This function used to remove sub-device with sub-device's pk and dn.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to remove sub-device.
 * @param PK, product_key.
 * @param DN, device_name.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_subdev_del(void *_connectivity, const char *PK, const char *DN, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
#if (CONFIG_SDK_THREAD_COST == 1)
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
#endif

    assert(cm_ctx && PK && DN);

#if (CONFIG_SDK_THREAD_COST == 1)
    if (NULL == connectivity) {
        linked_list_iterator(cm_ctx->list_connectivity, cm_multi_iterator_action_handler, cm_ctx,
                             cm_iterator_action_remove_subdevice, &ret, PK, DN);
    } else {
        ret = cm_connectivity_sub_device(cm_ctx, connectivity, PK, DN, 0);
    }
#else /* CONFIG_SDK_THREAD_COST */
    ret = iotx_cm_remove_subdevice(cm_ctx, _connectivity, PK, DN);
#endif /* CONFIG_SDK_THREAD_COST */

    return ret;
}


#if (CONFIG_SDK_THREAD_COST == 1)
static void iotx_cm_connectivity_add_send_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    iotx_cm_send_peer_t *send_peer;
    iotx_cm_message_info_t *message_info;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    send_peer = va_arg(*params, iotx_cm_send_peer_t *);
    message_info = va_arg(*params, iotx_cm_message_info_t *);

    assert(cm_ctx);

    if (connectivity && connectivity->add_send_func) {
        connectivity->add_send_func(cm_ctx, send_peer, message_info, NULL);
    }
}
#endif


/**
 * @brief Send data.
 *        This function used to send data to target.
 *        If the connectivity and target is NULL, the data will broadcast to all the reachable places.
 *        If the connectivity is not NULL, target is NULL, the data will broadcast to the connectivity's all the reachable places.
 *        If the connectivity is NULL, target's product_key and device_name is itself, the data will send to cloud.
 *        If the connectivity and target is not NULL, the data will send to target only.
 *
 * @param connectivity
 *        return by iotx_cm_open
 *        If it is NULL, call all connectivity, alse, via connectivity to send.
 * @param target. the send peer.
 *        if NULL, will send to all target. [alcs will have more targets in one connectivity]
 * @param message_info.
 * @param option, reserve.
 *
 * @return success or fail.
 *
 */
int iotx_cm_send(void *_connectivity, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info, void *option)
{
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == message_info || NULL == message_info->URI || NULL == message_info->payload) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (NULL == connectivity) {
        connectivity = iotx_cm_find_connectivity(cm_ctx, target, message_info->conn_ctx);
    }

#if (CONFIG_SDK_THREAD_COST == 1)
    {
        if (connectivity) {
            ret = connectivity->add_send_func(g_cm_ctx, target, message_info, NULL);
        } else {
            linked_list_iterator(g_cm_ctx->list_connectivity, iotx_cm_connectivity_add_send_handler, g_cm_ctx, target,
                                 message_info);
        }
    }
#else /* CONFIG_SDK_THREAD_COST */

    ret = iotx_cm_send_data(cm_ctx, connectivity, target, message_info->URI, message_info->ack_type, message_info->payload,
                            message_info->payload_length, message_info->conn_ctx, NULL);
#endif /* CONFIG_SDK_THREAD_COST */
    return ret;
}


/**
 * @brief Send data.
 *        This function used to send data to target.
 *        If the connectivity and target is NULL, the data will broadcast to all the reachable places.
 *        If the connectivity is not NULL, target is NULL, the data will broadcast to the connectivity's all the reachable places.
 *        If the connectivity is NULL, target's product_key and device_name is itself, the data will send to cloud.
 *        If the connectivity and target is not NULL, the data will send to target only.
 *
 * @note This function only working in CONFIG_SDK_THREAD_COST.
 *       This function only working in _connectivity is not NULL.
 *
 * @param connectivity. the handler of iotx_cm_open.
 *        if NULL, will send to all connectivity.
 * @param target. the send peer.
 *        if NULL, will send to all target. [alcs will have more targets in one connectivity]
 * @param message_info [in/out].
 * @param option, reserve.
 *
 * @return success or fail.
 *
 */
int iotx_cm_send_sync(void *_connectivity, iotx_cm_send_peer_t *target, iotx_cm_message_info_t *message_info,
                      void *option)
{
#if (CONFIG_SDK_THREAD_COST == 1)
    int ret = 0;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)_connectivity;
    void *sem = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }

    if (NULL == message_info || NULL == message_info->URI || NULL == message_info->payload) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (NULL == connectivity) {
        CM_ERR(cm_log_error_param_3);
        return FAIL_RETURN;
    }

    sem = HAL_SemaphoreCreate();
    //    if (NULL == sem) {
    //        CM_ERR(cm_log_error_fail);
    //        return FAIL_RETURN;
    //    }

    ret = connectivity->add_send_func(g_cm_ctx, target, message_info, sem);

    HAL_SemaphoreWait(sem, 2000);
    HAL_SemaphoreDestroy(sem);

    return ret;
#else /* CONFIG_SDK_THREAD_COST */
    CM_ERR(cm_log_error_param_2);
    return FAIL_RETURN;
#endif /* CONFIG_SDK_THREAD_COST */
}


#if (CONFIG_SDK_THREAD_COST == 0)
static void cm_connectivity_yield_handler(void *list_node, va_list *params)
{
    iotx_cm_connectivity_t *connectivity = (iotx_cm_connectivity_t *)list_node;
    iotx_cm_conntext_t *cm_ctx;
    int timeout_ms;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);
    timeout_ms = va_arg(*params, int);

    assert(cm_ctx);

    if (connectivity && connectivity->yield_func) {
        if (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED != iotx_cm_get_connectivity_status(connectivity)) {
            connectivity->connect_func(cm_ctx, connectivity);
            connectivity->trigger_connected_func(cm_ctx, connectivity, NULL, NULL);
        } else {
            connectivity->yield_func(cm_ctx, connectivity, timeout_ms);
        }
    }
}

/**
 * @brief Yield.
 *        This function used to yield when want to received data.
 *        This function just need used in CONFIG_SDK_THREAD_COST = n.
 *        If the CONFIG_SDK_THREAD_COST = y, this function is no need.
 *
 * @param target.
 * @param message_info.
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_yield(int timeout_ms, void *option)
{
    linked_list_t *list = NULL;
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;

    assert(cm_ctx);

    list = cm_ctx->list_connectivity;

    linked_list_iterator(list, cm_connectivity_yield_handler, cm_ctx, timeout_ms / list->_size);

    return SUCCESS_RETURN;
}
#endif /* CONFIG_SDK_THREAD_COST */


static void free_list_event_callback(void *_node, va_list *params)
{
    void *node = _node;

    assert(node);

    LITE_free(node);
}


#if (CONFIG_SDK_THREAD_COST == 1)
static void connectivity_deinit(void *_node, va_list *params)
{
    /* iotx_cm_close(&_node, NULL); */
}
#endif


static void free_list_connectivity(void *_node, va_list *params)
{
    iotx_cm_connectivity_t *node = (iotx_cm_connectivity_t *)_node;
    iotx_cm_conntext_t *cm_ctx;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t *);

    assert(node && cm_ctx);

    /* please note the multi-thread case */
#if (CONFIG_SDK_THREAD_COST == 0)
    node->deinit_func(cm_ctx, node);
#endif

    LITE_free(node);
}


/**
 * @brief deinit
 *
 * @param option, reserve.
 *
 * @return success or fail.
 */
int iotx_cm_deinit(void *option)
{
    iotx_cm_conntext_t *cm_ctx = g_cm_ctx;
    linked_list_t *list;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_status);
        return FAIL_RETURN;
    }
    list = cm_ctx->list_connectivity;

    /* remove mapping */
    iotx_cm_remove_mapping_all(cm_ctx);

#if (CONFIG_SDK_THREAD_COST == 1)
    HAL_MutexDestroy(g_cm_ctx->action_lock);
    linked_list_iterator(list, connectivity_deinit, cm_ctx);

    HAL_SleepMs(2000);
    iotx_cm_free_list_node_all(g_cm_ctx);
#endif /* CONFIG_SDK_THREAD_COST */

    /* remove connectivity */
    linked_list_iterator(list, free_list_connectivity, cm_ctx);
    linked_list_clear(list);
    linked_list_destroy(list);

    /* remove event_callback */
    list = cm_ctx->list_event_callback;
    linked_list_iterator(list, free_list_event_callback, cm_ctx);

    linked_list_clear(list);
    linked_list_destroy(list);

    LITE_free(cm_ctx);
    g_cm_ctx = 0;

    return SUCCESS_RETURN;
}
