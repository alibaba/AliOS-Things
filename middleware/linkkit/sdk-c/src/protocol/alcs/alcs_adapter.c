/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stddef.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "linked_list.h"
#include "alcs_api.h"
#include "alcs_coap.h"
#include "alcs_mqtt.h"
#include "CoAPInternal.h"
#include "CoAPExport.h"
#include "CoAPServer.h"
#include "alcs_adapter.h"
#include "alcs_mqtt.h"
#include "CoAPPlatform.h"

static iotx_alcs_adapter_t g_alcs_adapter;

extern void on_client_auth_timer(CoAPContext *);
extern void on_svr_auth_timer(CoAPContext *);

static void alcs_heartbeat(void *handle);

static iotx_alcs_adapter_t *__iotx_alcs_get_ctx(void)
{
    return &g_alcs_adapter;
}

static char *iotx_alcs_topic_parse_pk(char *topic, uint16_t *length)
{
    if (topic == NULL || length == NULL) {
        COAP_ERR("Invalid Parameter");
        return NULL;
    }

    char *pos = NULL;
    uint8_t slash_count = 0;
    uint16_t idx = 0;
    uint16_t topic_len = strlen(topic);

    while (idx < topic_len) {
        if (topic[idx] == '/') {
            slash_count++;
            if (slash_count == 2) {
                pos = topic + idx + 1;
            }
            if (slash_count == 3) {
                *length = topic + idx - pos;
            }
        }
        idx++;
    }

    return pos;
}

static char *iotx_alcs_topic_parse_dn(char *topic, uint16_t *length)
{
    if (topic == NULL || length == NULL) {
        COAP_ERR("Invalid Parameter");
        return NULL;
    }

    char *pos = NULL;
    uint8_t slash_count = 0;
    uint16_t idx = 0;
    uint16_t topic_len = strlen(topic);

    while (idx < topic_len) {
        if (topic[idx] == '/') {
            slash_count++;
            if (slash_count == 3) {
                pos = topic + idx + 1;
            }
            if (slash_count == 4) {
                *length = topic + idx - pos;
            }
        }
        idx++;
    }

    return pos;
}

int iotx_alcs_coap_ack_send(CoAPContext *context, NetworkAddr *remote, unsigned short msgid)
{
    int ret   = COAP_SUCCESS;
    CoAPMessage message;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    CoAPMessage_init(&message);
    CoAPMessageId_set(&message, msgid);
    COAP_INFO("Send Ack Response Message: %d", msgid);
    ret = CoAPMessage_send(ctx, remote, &message);
    CoAPMessage_destory(&message);
    return ret;
}

void iotx_alcs_coap_adapter_send_msg_handle(CoAPContext *context,
        CoAPReqResult result,
        void *userdata,
        NetworkAddr *remote,
        CoAPMessage *message)
{
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)userdata;
    iotx_alcs_event_msg_t event;
    memset(&event, 0, sizeof(iotx_alcs_event_msg_t));

    switch (result) {
        case COAP_REQUEST_SUCCESS: {
            iotx_alcs_transfer_msg_t transfer_msg;
            iotx_alcs_send_msg_t *send_msg = NULL;

            memset(&transfer_msg, 0, sizeof(iotx_alcs_transfer_msg_t));

            transfer_msg.ip = (char *)remote->addr;
            transfer_msg.port = remote->port;
            linked_list_iterator(adapter->alcs_send_list,
                                 iotx_alcs_send_list_handle,
                                 &send_msg,
                                 message->token);
            if (send_msg == NULL) {
                return;
            }

            transfer_msg.uri = send_msg->uri;
            transfer_msg.token_len = send_msg->token_len;
            transfer_msg.token = send_msg->token;
            transfer_msg.payload_len = message->payloadlen;
            transfer_msg.payload = message->payload;

            event.event_type = IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_SUCCESS;
            event.msg = &transfer_msg;

            adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext, (void *)adapter, &event);

            linked_list_remove(adapter->alcs_send_list, send_msg);
            LITE_free(send_msg->token);
            LITE_free(send_msg->uri);
            LITE_free(send_msg);
        }
        break;
        case COAP_RECV_RESP_TIMEOUT: {
            iotx_alcs_transfer_msg_t transfer_msg;
            iotx_alcs_send_msg_t *send_msg = NULL;

            memset(&transfer_msg, 0, sizeof(iotx_alcs_transfer_msg_t));

            transfer_msg.ip = (char *)remote->addr;
            transfer_msg.port = remote->port;
            linked_list_iterator(adapter->alcs_send_list,
                                 iotx_alcs_send_list_handle,
                                 &send_msg,
                                 message->token);

            if (send_msg == NULL) {
                return;
            }

            transfer_msg.uri = send_msg->uri;
            transfer_msg.token_len = send_msg->token_len;
            transfer_msg.token = send_msg->token;
            transfer_msg.payload_len = 0;
            transfer_msg.payload = NULL;

            event.event_type = IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_RESP_TIMEOUT;
            event.msg = &transfer_msg;

            adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext, (void *)adapter, &event);

            linked_list_remove(adapter->alcs_send_list, send_msg);
            LITE_free(send_msg->token);
            LITE_free(send_msg->uri);
            LITE_free(send_msg);
        }
        break;
        default:
            COAP_WRN("Unknown Coap Request Result: %d", result);
            break;
    }
}

void iotx_alcs_coap_adapter_recv_msg_handle(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();
    iotx_alcs_transfer_msg_t transfer_msg;
    iotx_alcs_event_msg_t event;

    memset(&transfer_msg, 0, sizeof(iotx_alcs_transfer_msg_t));
    memset(&event, 0, sizeof(iotx_alcs_event_msg_t));

    transfer_msg.ip = (char *)remote->addr;
    transfer_msg.port = remote->port;
    transfer_msg.uri = (char *)paths;
    transfer_msg.token_len = message->header.tokenlen;
    transfer_msg.token = message->token;
    transfer_msg.payload_len = message->payloadlen;
    transfer_msg.payload = message->payload;

    event.event_type = IOTX_ALCS_EVENT_MSG_RECV_MESSAGE;
    event.msg = (void *)&transfer_msg;

    iotx_alcs_coap_ack_send(context, remote, message->header.msgid);

    adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext, (void *)adapter, &event);
}

void iotx_alcs_coap_adapter_event_notifier(unsigned int event, NetworkAddr *remote, void *message)
{
    COAP_INFO("ALCS Coap Event: %d, Remote Device Address: %s, Remote Device Port: %d",
              event, remote->addr, remote->port);
}

void iotx_alcs_send_list_handle(void *list_node, va_list *params)
{
    iotx_alcs_send_msg_t *send_msg = (iotx_alcs_send_msg_t *)list_node;

    iotx_alcs_send_msg_t **match_msg = NULL;
    unsigned char *token = NULL;

    match_msg = va_arg(*params, iotx_alcs_send_msg_t **);
    token = va_arg(*params, unsigned char *);

    if (memcmp(send_msg->token, token, send_msg->token_len) == 0) {
        *match_msg = send_msg;
    }
}

int iotx_alcs_adapter_list_init(iotx_alcs_adapter_t *adapter)
{
    //initialze send list
    adapter->alcs_send_list = linked_list_create("alcs send list", 1);
    if (adapter->alcs_send_list == NULL) {
        return FAIL_RETURN;
    }

    adapter->alcs_subdev_list = linked_list_create("alcs subdev list", 1);
    if (adapter->alcs_subdev_list == NULL) {
        linked_list_destroy(adapter->alcs_send_list);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_alcs_adapter_deinit(void)
{
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    if (adapter->alcs_send_list) {
        linked_list_destroy(adapter->alcs_send_list);
        adapter->alcs_send_list = NULL;
    }

    if (adapter->alcs_subdev_list) {
        linked_list_destroy(adapter->alcs_subdev_list);
        adapter->alcs_subdev_list = NULL;
    }

    if (adapter->alcs_event_handle) {
        LITE_free(adapter->alcs_event_handle);
    }

    if (adapter->mutex) {
        HAL_MutexDestroy(adapter->mutex);
    }

    alcs_mqtt_deinit(adapter->coap_ctx, product_key, device_name);

    //if (adapter->coap_ctx) CoAPContext_free(adapter->coap_ctx);

    alcs_context_deinit();
    alcs_deinit();
    alcs_auth_deinit();

    return SUCCESS_RETURN;
}

int iotx_alcs_adapter_init(iotx_alcs_adapter_t *adapter, iotx_alcs_param_t *param)
{
    COAP_INFO("iotx_alcs_adapter_init");

    int res;
    CoAPInitParam coap_param;
    CoAPContext *coap_ctx = NULL;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    memset(&coap_param, 0, sizeof(CoAPInitParam));

    adapter->mutex = HAL_MutexCreate();
    if (adapter->mutex == NULL) {
        COAP_ERR("Mutex Init Failed");
        return FAIL_RETURN;
    }

    coap_param.send_maxcount = param->send_maxcount;
    coap_param.obs_maxcount = param->obs_maxcount;
    coap_param.port = param->port;
    coap_param.group = param->group;
    coap_param.waittime = param->waittime;
    coap_param.res_maxcount = param->res_maxcount;
    coap_param.appdata = NULL;
    coap_param.notifier = iotx_alcs_coap_adapter_event_notifier;

    coap_ctx = alcs_context_init(&coap_param);
    if (coap_ctx == NULL) {
        COAP_ERR("Coap Context Init Failed");
        HAL_MutexDestroy(adapter->mutex);
        return FAIL_RETURN;
    }
    adapter->coap_ctx = coap_ctx;

    res = HAL_GetProductKey(product_key);
    if (res <= 0 || res > PRODUCT_KEY_MAXLEN - 1) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("Get Product Key Failed");
        return FAIL_RETURN;
    }

    res = HAL_GetDeviceName(device_name);
    if (res <= 0 || res > DEVICE_NAME_MAXLEN - 1) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("Get Device Name Failed");
        return FAIL_RETURN;
    }

    alcs_init();

    res = alcs_auth_init(coap_ctx, product_key, device_name, param->role);
    if (res != COAP_SUCCESS) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("ALCS Auth Init Failed");
        return FAIL_RETURN;
    }
    adapter->role = param->role;

    if (adapter->role & IOTX_ALCS_ROLE_SERVER) {
        adapter->alcs_server_auth_timer_func = on_svr_auth_timer;
    }

    if (adapter->role & IOTX_ALCS_ROLE_CLIENT) {
        adapter->alcs_client_auth_timer_func = on_client_auth_timer;
    }

    adapter->alcs_event_handle = (iotx_alcs_event_handle_t *)ALCS_ADAPTER_malloc(sizeof(iotx_alcs_event_handle_t));
    if (adapter->alcs_event_handle == NULL) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("ALCS Event Handle Init Failed");
        return FAIL_RETURN;
    }
    memcpy(adapter->alcs_event_handle, param->handle_event, sizeof(iotx_alcs_event_handle_t));

    if (iotx_alcs_adapter_list_init(adapter) != SUCCESS_RETURN) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("ALCS Linked List Init Failed");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

void iotx_alcs_subdev_search_iterator(void *list_node, va_list *params)
{
    iotx_alcs_subdev_item_t *item_node = (iotx_alcs_subdev_item_t *)list_node;

    if (item_node == NULL) {
        COAP_ERR("Not Found Item Node");
        return;
    }

    const char *pk = va_arg(*params, const char *);
    const char *dn = va_arg(*params, const char *);
    iotx_alcs_subdev_item_t **subdev_item = va_arg(*params, iotx_alcs_subdev_item_t **);

    if (memcmp(item_node->product_key, pk, strlen(pk)) == 0 &&
        memcmp(item_node->device_name, dn, strlen(dn)) == 0) {
        *subdev_item = item_node;
    }
}

int iotx_alcs_subdev_insert(iotx_alcs_subdev_item_t *item)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    if (item == NULL) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    linked_list_insert(adapter->alcs_subdev_list, item);

    return SUCCESS_RETURN;
}

int iotx_alcs_subdev_remove(const char *pk, const char *dn)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    if (pk == NULL || dn == NULL) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    iotx_alcs_subdev_item_t *subdev_item = NULL;
    linked_list_iterator(adapter->alcs_subdev_list, iotx_alcs_subdev_search_iterator, pk, dn, &subdev_item);

    if (subdev_item == NULL) {
        COAP_ERR("No Matched Item");
        return FAIL_RETURN;
    }

    linked_list_remove(adapter->alcs_subdev_list, (void *)subdev_item);
    LITE_free(subdev_item);

    return SUCCESS_RETURN;
}

int iotx_alcs_subdev_search(const char *pk, const char *dn, iotx_alcs_subdev_item_t **item)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    if (pk == NULL || strlen(pk) >= PRODUCT_KEY_MAXLEN ||
        dn == NULL || strlen(dn) >= DEVICE_NAME_MAXLEN ||
        item == NULL || *item != NULL) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    linked_list_iterator(adapter->alcs_subdev_list, iotx_alcs_subdev_search_iterator, pk, dn, item);

    if (*item == NULL) {
        COAP_ERR("No Matched Subdev");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_alcs_subdev_update_prefix_secret(iotx_alcs_subdev_item_t *item)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();
    iotx_alcs_subdev_item_t *subdev_item = NULL;

    if (item == NULL) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    linked_list_iterator(adapter->alcs_subdev_list, iotx_alcs_subdev_search_iterator,
                         item->product_key, item->device_name, &subdev_item);

    if (subdev_item == NULL) {
        COAP_ERR("No Matched Item");
        return FAIL_RETURN;
    }

    memcpy(subdev_item->prefix, item->prefix, ALCS_MQTT_PREFIX_MAX_LEN);
    memcpy(subdev_item->secret, item->secret, ALCS_MQTT_SECRET_MAX_LEN);

    return SUCCESS_RETURN;
}

int iotx_alcs_subdev_update_stage(iotx_alcs_subdev_item_t *item)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();
    iotx_alcs_subdev_item_t *subdev_item = NULL;

    if (item == NULL) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    linked_list_iterator(adapter->alcs_subdev_list, iotx_alcs_subdev_search_iterator,
                         item->product_key, item->device_name, &subdev_item);

    if (subdev_item == NULL) {
        COAP_ERR("No Matched Item");
        return FAIL_RETURN;
    }

    subdev_item->stage = item->stage;

    return SUCCESS_RETURN;
}

void iotx_alcs_subdev_stage_iterator(void *list_node, va_list *params)
{
    iotx_alcs_subdev_item_t *item_node = (iotx_alcs_subdev_item_t *)list_node;

    uint64_t time_now = HAL_UptimeMs();

    if (item_node->stage == IOTX_ALCS_SUBDEV_DISCONNCET_CLOUD) {
        if (((time_now > item_node->retry_ms) &&
             (time_now - item_node->retry_ms >= IOTX_ALCS_SUBDEV_RETRY_INTERVAL_MS)) ||
            ((time_now <= item_node->retry_ms) &&
             ((0xFFFFFFFFFFFFFFFF - item_node->retry_ms) + time_now >= IOTX_ALCS_SUBDEV_RETRY_INTERVAL_MS))) {
            //Get Prefix And Secret From Cloud
            alcs_mqtt_subdev_prefix_get(item_node->product_key, item_node->device_name);
            item_node->retry_ms = time_now;
        }
    }
}

void iotx_alcs_subdev_stage_check(void)
{
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    linked_list_iterator(adapter->alcs_subdev_list, iotx_alcs_subdev_stage_iterator);
}

void *iotx_alcs_construct(iotx_alcs_param_t *params)
{
    COAP_INFO("iotx_alcs_construct enter");

    int res = 0;
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    POINTER_SANITY_CHECK(params, NULL);
    STRING_PTR_SANITY_CHECK(params->group, NULL);

    memset(adapter, 0, sizeof(iotx_alcs_adapter_t));

    res = iotx_alcs_adapter_init(adapter, params);
    if (res != SUCCESS_RETURN) {
        COAP_ERR("Adapter Init Failed");
        return NULL;
    }

    return (void *)adapter;
}

int iotx_alcs_cloud_init(void *handle)
{
    COAP_ERR("Start ALCS Cloud Init");
    int res = 0;
    iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    if (adapter->local_cloud_inited == 1) {
        return SUCCESS_RETURN;
    }

    if (handle == NULL) {
        return FAIL_RETURN;
    }

    res = HAL_GetProductKey(product_key);
    if (res <= 0 || res > PRODUCT_KEY_MAXLEN - 1) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("Get Product Key Failed");
        return FAIL_RETURN;
    }

    res = HAL_GetDeviceName(device_name);
    if (res <= 0 || res > DEVICE_NAME_MAXLEN - 1) {
        iotx_alcs_adapter_deinit();
        COAP_ERR("Get Device Name Failed");
        return FAIL_RETURN;
    }

    if (alcs_mqtt_init(adapter->coap_ctx, product_key, device_name) != ALCS_MQTT_STATUS_SUCCESS) {
        /*solve the prpblem of hard fault when mqtt connection fails once*/
        COAP_ERR("ALCS MQTT Init Failed");
        return FAIL_RETURN;
    }

    adapter->local_cloud_inited = 1;

    return SUCCESS_RETURN;
}

int iotx_alcs_destroy(void **phandle)
{
    POINTER_SANITY_CHECK(phandle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(*phandle, NULL_VALUE_ERROR);

    iotx_alcs_adapter_deinit();

    return SUCCESS_RETURN;
}

static void alcs_heartbeat(void *handle)
{
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;

    if (adapter->role & IOTX_ALCS_ROLE_SERVER && adapter->alcs_server_auth_timer_func != NULL) {
        adapter->alcs_server_auth_timer_func(adapter->coap_ctx);
    }

    if (adapter->role & IOTX_ALCS_ROLE_CLIENT && adapter->alcs_client_auth_timer_func != NULL) {
        adapter->alcs_client_auth_timer_func(adapter->coap_ctx);
    }
}
int iotx_alcs_yield(void *handle)
{
    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;

    CoAPServer_thread_leave();
    POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);

    res = (CoAPMessage_cycle(adapter->coap_ctx) != COAP_SUCCESS) ? (FAIL_RETURN) : (SUCCESS_RETURN);

    alcs_heartbeat(handle);

    iotx_alcs_subdev_stage_check();

    return res;
}

int iotx_alcs_send(void *handle, iotx_alcs_msg_t *msg)
{
    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
    CoAPMessage coap_msg;
    CoAPLenString coap_payload;
    NetworkAddr network_addr;

    POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(msg, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(msg->payload, NULL_VALUE_ERROR);

    STRING_PTR_SANITY_CHECK(msg->ip, FAIL_RETURN);
    STRING_PTR_SANITY_CHECK(msg->uri, FAIL_RETURN);


    if (strlen(msg->ip) > NETWORK_ADDR_LEN) {
        COAP_ERR("Invalid Ip Address Length");
        return FAIL_RETURN;
    }

    memset(&coap_msg, 0, sizeof(CoAPMessage));
    memset(&coap_payload, 0, sizeof(CoAPLenString));

    coap_payload.len = msg->payload_len;
    coap_payload.data = msg->payload;

    alcs_msg_init(adapter->coap_ctx, &coap_msg, msg->msg_code, msg->msg_type, 0, &coap_payload, (void *)adapter);

    res = alcs_msg_setAddr(&coap_msg, msg->uri, NULL);
    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Message Set URI Failed");
        return FAIL_RETURN;
    }

    memset(&network_addr, 0, sizeof(NetworkAddr));
    memcpy(network_addr.addr, msg->ip, strlen(msg->ip));
    network_addr.port = msg->port;

    //Get Product Key And Device Name
    AlcsDeviceKey devKey;
    char productKey[PRODUCT_KEY_MAXLEN] = {0};
    char deviceName[DEVICE_NAME_MAXLEN] = {0};
    char *uri_pk = NULL;
    char *uri_dn = NULL;
    uint16_t uri_pk_len = 0;
    uint16_t uri_dn_len = 0;

    memset(&devKey, 0, sizeof(AlcsDeviceKey));
    memcpy(&devKey.addr, &network_addr, sizeof(NetworkAddr));

    uri_pk = iotx_alcs_topic_parse_pk(msg->uri, &uri_pk_len);
    uri_dn = iotx_alcs_topic_parse_dn(msg->uri, &uri_dn_len);

    if (uri_pk == NULL || uri_pk_len >= PRODUCT_KEY_MAXLEN ||
        uri_dn == NULL || uri_dn_len >= DEVICE_NAME_MAXLEN) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }
    memcpy(productKey, uri_pk, uri_pk_len);
    memcpy(deviceName, uri_dn, uri_dn_len);

    devKey.pk = productKey;
    devKey.dn = deviceName;

    res = alcs_sendmsg_secure(adapter->coap_ctx, &devKey, &coap_msg, 2, iotx_alcs_coap_adapter_send_msg_handle);
    alcs_msg_deinit(&coap_msg);

    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Message Send Message Failed");
        return FAIL_RETURN;
    }

    iotx_alcs_send_msg_t *alcs_send_msg =
                (iotx_alcs_send_msg_t *)ALCS_ADAPTER_malloc(sizeof(iotx_alcs_send_msg_t));
    if (alcs_send_msg == NULL) {
        COAP_WRN("Not Enough Memory");
        return FAIL_RETURN;
    }
    memset(alcs_send_msg, 0, sizeof(iotx_alcs_send_msg_t));

    alcs_send_msg->token = (uint8_t *)ALCS_ADAPTER_malloc(coap_msg.header.tokenlen + 1);
    if (alcs_send_msg->token == NULL) {
        LITE_free(alcs_send_msg);
        COAP_WRN("Not Enough Memory");
        return FAIL_RETURN;
    }
    alcs_send_msg->token_len = coap_msg.header.tokenlen;

    memset(alcs_send_msg->token, 0, alcs_send_msg->token_len + 1);
    memcpy(alcs_send_msg->token, coap_msg.token, alcs_send_msg->token_len);

    alcs_send_msg->uri = (char *)ALCS_ADAPTER_malloc(strlen(msg->uri) + 1);
    if (alcs_send_msg->uri == NULL) {
        LITE_free(alcs_send_msg->token);
        LITE_free(alcs_send_msg);
        COAP_WRN("ALCS Message Buffer Failed");
        return FAIL_RETURN;
    }
    memset(alcs_send_msg->uri, 0, strlen(msg->uri) + 1);
    memcpy(alcs_send_msg->uri, msg->uri, strlen(msg->uri));

    linked_list_insert(adapter->alcs_send_list, (void *)alcs_send_msg);

    return SUCCESS_RETURN;
}

int iotx_alcs_send_Response(void *handle, iotx_alcs_msg_t *msg, uint8_t token_len, uint8_t *token)
{
    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
    CoAPMessage coap_msg;
    CoAPLenString coap_payload;
    CoAPLenString token_payload;
    NetworkAddr network_addr;

    POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(msg, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(msg->payload, NULL_VALUE_ERROR);

    STRING_PTR_SANITY_CHECK(msg->ip, FAIL_RETURN);
    STRING_PTR_SANITY_CHECK(msg->uri, FAIL_RETURN);
    if (token_len == 0 || token == NULL) {
        return FAIL_RETURN;
    }


    if (strlen(msg->ip) > NETWORK_ADDR_LEN) {
        COAP_ERR("Invalid Ip Address Length");
        return FAIL_RETURN;
    }

    memset(&coap_msg, 0, sizeof(CoAPMessage));
    memset(&coap_payload, 0, sizeof(CoAPLenString));
    memset(&token_payload, 0, sizeof(CoAPLenString));

    coap_payload.len = msg->payload_len;
    coap_payload.data = msg->payload;

    alcs_msg_init(adapter->coap_ctx, &coap_msg, msg->msg_code, msg->msg_type, 0, &coap_payload, (void *)adapter);

    res = alcs_msg_setAddr(&coap_msg, msg->uri, NULL);
    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Message Set URI Failed");
        return FAIL_RETURN;
    }

    memset(&network_addr, 0, sizeof(NetworkAddr));
    memcpy(network_addr.addr, msg->ip, strlen(msg->ip));
    network_addr.port = msg->port;

    token_payload.len = token_len;
    token_payload.data = token;

    //Get Product Key And Device Name
    AlcsDeviceKey devKey;
    char productKey[PRODUCT_KEY_MAXLEN] = {0};
    char deviceName[DEVICE_NAME_MAXLEN] = {0};
    char *uri_pk = NULL;
    char *uri_dn = NULL;
    uint16_t uri_pk_len = 0;
    uint16_t uri_dn_len = 0;

    memset(&devKey, 0, sizeof(AlcsDeviceKey));
    memcpy(&devKey.addr, &network_addr, sizeof(NetworkAddr));

    uri_pk = iotx_alcs_topic_parse_pk(msg->uri, &uri_pk_len);
    uri_dn = iotx_alcs_topic_parse_dn(msg->uri, &uri_dn_len);

    if (uri_pk == NULL || uri_pk_len >= PRODUCT_KEY_MAXLEN ||
        uri_dn == NULL || uri_dn_len >= DEVICE_NAME_MAXLEN) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }
    memcpy(productKey, uri_pk, uri_pk_len);
    memcpy(deviceName, uri_dn, uri_dn_len);

    devKey.pk = productKey;
    devKey.dn = deviceName;

    if (alcs_resource_need_auth(adapter->coap_ctx, msg->uri)) {
        res = alcs_sendrsp_secure(adapter->coap_ctx, &devKey, &coap_msg, 0, 0, &token_payload);
    } else {
        res = alcs_sendrsp(adapter->coap_ctx, &network_addr, &coap_msg, 0, 0, &token_payload);
    }

    alcs_msg_deinit(&coap_msg);

    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Message Send Message Failed");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_alcs_register_resource(void *handle, iotx_alcs_res_t *resource)
{
    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
    char productKey[PRODUCT_KEY_MAXLEN] = {0};
    char deviceName[DEVICE_NAME_MAXLEN] = {0};
    char *uri_pk = NULL;
    char *uri_dn = NULL;
    uint16_t uri_pk_len = 0;
    uint16_t uri_dn_len = 0;

    POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);

    STRING_PTR_SANITY_CHECK(resource->uri, FAIL_RETURN);

    uri_pk = iotx_alcs_topic_parse_pk(resource->uri, &uri_pk_len);
    uri_dn = iotx_alcs_topic_parse_dn(resource->uri, &uri_dn_len);

    if (uri_pk == NULL || uri_pk_len >= PRODUCT_KEY_MAXLEN ||
        uri_dn == NULL || uri_dn_len >= DEVICE_NAME_MAXLEN) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }
    memcpy(productKey, uri_pk, uri_pk_len);
    memcpy(deviceName, uri_dn, uri_dn_len);

    COAP_INFO("alcs register resource, uri:%s", resource->uri);
    int needAuth = resource->need_auth; // strcmp (resource->uri, "/dev/core/service/dev");

    res = alcs_resource_register(adapter->coap_ctx,
                                 productKey,
                                 deviceName,
                                 resource->uri,
                                 resource->msg_perm,
                                 resource->msg_ct,
                                 resource->maxage,
                                 needAuth,
                                 iotx_alcs_coap_adapter_recv_msg_handle);

    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Register Resource Failed, Code: %d", res);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_alcs_observe_notify(void *handle, const char *uri, uint32_t payload_len, uint8_t *payload)
{
    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
    CoAPLenString coap_payload;

    coap_payload.len = (int32_t)payload_len;
    coap_payload.data = payload;

    res = alcs_observe_notify(adapter->coap_ctx, uri, &coap_payload);
    if (res != COAP_SUCCESS) {
        COAP_ERR("ALCS Observe Notify Failed, Code: %d", res);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_alcs_unregister_resource(void *handle, char *uri)
{
    return SUCCESS_RETURN;
}

int iotx_alcs_add_sub_device(void *handle, const char *pk, const char *dn)
{
    if (handle == NULL || pk == NULL || strlen(pk) >= PRODUCT_KEY_MAXLEN ||
        dn == NULL || strlen(dn) >= DEVICE_NAME_MAXLEN) {
        COAP_ERR("Invalid Argument");
        return FAIL_RETURN;
    }

    int res = 0;
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
    iotx_alcs_subdev_item_t *subdev_item = NULL;

    if (adapter->coap_ctx != NULL) {
        alcs_auth_subdev_init(adapter->coap_ctx, pk, dn);
    }

    //Search Subdev In Linked List
    res = iotx_alcs_subdev_search(pk, dn, &subdev_item);
    if (res == SUCCESS_RETURN) {
        COAP_INFO("This Product Key And Device Name Have Been Added");
        return SUCCESS_RETURN;
    }

    //Insert New Subdev Into Linked List
    subdev_item = (iotx_alcs_subdev_item_t *)ALCS_ADAPTER_malloc(sizeof(iotx_alcs_subdev_item_t));
    if (subdev_item == NULL) {
        COAP_ERR("No Enough Memory");
        return FAIL_RETURN;
    }
    memset(subdev_item, 0, sizeof(iotx_alcs_subdev_item_t));

    //Set Product Key And Device Name
    memcpy(subdev_item->product_key, pk, strlen(pk));
    memcpy(subdev_item->device_name, dn, strlen(dn));
    subdev_item->stage = IOTX_ALCS_SUBDEV_DISCONNCET_CLOUD;
    subdev_item->retry_ms = HAL_UptimeMs();

    res = iotx_alcs_subdev_insert(subdev_item);
    if (res != SUCCESS_RETURN) {
        COAP_ERR("Insert New Subdev Failed");
        LITE_free(subdev_item);
        return FAIL_RETURN;
    }
    //Get Prefix And Secret From KV
    char prefix[ALCS_MQTT_PREFIX_MAX_LEN] = {0};
    char secret[ALCS_MQTT_SECRET_MAX_LEN] = {0};

    res = alcs_mqtt_prefix_secret_laod(pk, strlen(pk), dn, strlen(dn), prefix, secret);
    if (res == SUCCESS_RETURN) {
        memcpy(subdev_item->prefix, prefix, strlen(prefix));
        memcpy(subdev_item->secret, secret, strlen(secret));
        alcs_mqtt_add_srv_key(prefix, secret);
    }

    //Get Prefix And Secret From Cloud
    alcs_mqtt_subdev_prefix_get(pk, dn);

    return SUCCESS_RETURN;
}

int iotx_alcs_remove_sub_device(void *handle, const char *pk, const char *dn)
{
    if (handle == NULL || pk == NULL || strlen(pk) >= PRODUCT_KEY_MAXLEN ||
        dn == NULL || strlen(dn) >= DEVICE_NAME_MAXLEN) {
        COAP_ERR("Invalid Parameter");
        return FAIL_RETURN;
    }

    int res = 0;
    res = iotx_alcs_subdev_remove(pk, dn);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    //Remove Subdev Item From KV
    alcs_mqtt_prefix_secret_del(pk, strlen(pk), dn, strlen(dn));
    return SUCCESS_RETURN;
}

