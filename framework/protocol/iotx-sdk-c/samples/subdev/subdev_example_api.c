
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "lite-utils.h"
#include "subdev_example_api.h"

iotx_thing_masterlave_pt g_thing_masterlave_t = NULL;

#define GET_JSON_VALUE(node, param, buf) \
    do { \
        (node) = LITE_json_value_of((param), (buf)); \
        if((node) == NULL) { \
            printf("get json value error! \n"); \
            return FAIL_RETURN; \
        } \
    } while(0)


#define GET_JSON_VALUE_WHIH_FREE(node, param, buf) \
    do { \
        LITE_free(node); \
        (node) = LITE_json_value_of((param), (buf)); \
        if((node) == NULL) { \
            printf("get json value error! \n"); \
            return FAIL_RETURN; \
        } \
    } while(0)

#define MALLOC_MEMORY_WITH_RESULT(buffer, length, result) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            printf("Not enough memory \n"); \
            return (result); \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

typedef struct iotx_thing_subscribe_topic_st {
    char* format;
    char* params_1;
    char* params_2;
    char* params_3;
}iotx_thing_subscribe_topic_t, *iotx_thing_subscribe_topic_pt;

extern void iotx_thing_event_handle(void *pcontext, void *pclient, void* msg);

extern iotx_device_info_pt iotx_device_info_get(void);

char* iotx_thing_splice_common_event_packet(const char* params,
        const char* event_id,
        int32_t* msg_id)
{
#define EVENT_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":%s,\"method\":\"thing.event.%s.post\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    if (params == NULL || msg_id == NULL) {
        printf("input params error!\n");
        return NULL;
    }

    /* sum the string length */
    len = strlen(EVENT_PACKET_FMT) + strlen(params) + strlen(event_id) + 12;

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
               len,
               EVENT_PACKET_FMT,
               id,
               params,
               event_id);

    if (ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

char* iotx_thing_splice_common_deviceinfo_packet(const char* deviceinfo,
        const char* method,
        int32_t* msg_id)
{
#define DEVICEINFO_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":%s,\"method\":\"thing.deviceinfo.%s\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    if (deviceinfo == NULL || method == NULL || msg_id == NULL) {
        printf("input params error!\n");
        return NULL;
    }

    /* sum the string length */
    len = strlen(EVENT_PACKET_FMT) + strlen(deviceinfo) + strlen(method) + 12;

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
               len,
               DEVICEINFO_PACKET_FMT,
               id,
               deviceinfo,
               method);

    if (ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

char *iotx_thing_splice_default_reply_get_packet(int32_t msg_id,
        uint32_t code,
        const char* data)
{
#define SET_DEFAULT_REPLY_GET_PACKET_FMT     "{\"id\":%d,\"code\":%d,\"data\":{%s}}"

    int len, ret;
    char* msg = NULL;

    PARAMETER_NULL_CHECK_WITH_RESULT(data, NULL);

    /* sum the string length */
    len = strlen(SET_DEFAULT_REPLY_GET_PACKET_FMT) + 12 + 4 + strlen(data);

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    ret = HAL_Snprintf(msg,
               len,
               SET_DEFAULT_REPLY_GET_PACKET_FMT,
               msg_id,
               code,
               data);

    if (ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    return msg;
}


char *iotx_thing_splice_default_reply_packet(int32_t msg_id,  uint32_t code)
{
#define SET_DEFAULT_REPLY_PACKET_FMT     "{\"id\":%d,\"code\":%d,\"data\":{}}"

    int len, ret;
    char* msg = NULL;

    /* sum the string length */
    len = strlen(SET_DEFAULT_REPLY_PACKET_FMT) + 12 + 4;

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    ret = HAL_Snprintf(msg,
               len,
               SET_DEFAULT_REPLY_PACKET_FMT,
               msg_id,
               code);

    if (ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    return msg;
}


char *iotx_thing_splice_topo_add_packet(const char* product_key,
        const char* device_name,
        const char* sign,
        const char* sign_method,
        const char* timestamp,
        const char* client_id,
        int32_t* msg_id)
{
#define TOPO_ADD_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":[{\"deviceName\":\"%s\",\"productKey\":\"%s\",\"sign\":\"%s\",\"signMethod\":\"%s\",\"timestamp\":\"%s\",\"clientId\":\"%s\"}],\"method\":\"thing.topo.add\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    if (device_name == NULL || product_key == NULL || sign == NULL || sign_method == NULL || timestamp == NULL ||  client_id == NULL || msg_id == NULL) {
        printf("input params error!\n");
        return NULL;
    }

    /* sum the string length */
    len = strlen(TOPO_ADD_PACKET_FMT) + strlen(device_name) + strlen(product_key) + strlen(sign)
                    + strlen(sign_method) + strlen(timestamp) + strlen(client_id) + 12;

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                       len,
                       TOPO_ADD_PACKET_FMT,
                       id,
                       device_name,
                       product_key,
                       sign,
                       sign_method,
                       timestamp,
                       client_id);

    if(ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}


char* iotx_thing_splice_common_packet(const char* product_key,
        const char* device_name,
        int32_t* msg_id,
        const char* param1,
        const char* param2,
        int flag)
{
#define COMMON_PACKET_FMT_1    "{\"id\":%d,\"version\":\"1.0\",\"params\":{},\"method\":\"thing.%s.%s\"}"
#define COMMON_PACKET_FMT_2    "{\"id\":%d,\"version\":\"1.0\",\"params\":[{\"deviceName\":\"%s\",\"productKey\":\"%s\"}],\"method\":\"thing.%s.%s\"}"

    int len, ret = -1;
    char* msg = NULL;
    int32_t id = 0;

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param1, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param2, NULL);
    PARAMETER_NULL_CHECK_WITH_RESULT(msg_id, NULL);

    /* sum the string length */
    len = strlen(COMMON_PACKET_FMT_2) + strlen(device_name) +
                strlen(product_key) + 12 + strlen("unregister") + strlen("dsltemplate");

    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    if (flag == 0) {
        ret = HAL_Snprintf(msg,
                   len,
                   COMMON_PACKET_FMT_1,
                   id,
                   param1,
                   param2);
    } else {
        ret = HAL_Snprintf(msg,
                   len,
                   COMMON_PACKET_FMT_2,
                   id,
                   device_name,
                   product_key,
                   param1,
                   param2);
    }
    if(ret < 0) {
        printf("splice packet error!\n");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

int iotx_thing_publish_common_packet(void* handle,
        const char* topic_product_key,
        const char* topic_device_name,
        const char* packet_product_key,
        const char* packet_device_name,
        const char* param1,
        const char* param2,
        const char* param3,
        iotx_thing_reply_type_t reply_type)
{
    int rc = 0;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    int32_t msg_id = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char* packet = NULL;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    PARAMETER_Thing_CHECK(thing_t);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic_product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic_device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(packet_product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(packet_device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param1, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param2, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param3, FAIL_RETURN);

    /* 组装topic */
    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            topic_product_key,
            topic_device_name,
            param1,
            param2);

    memset(thing_t->thing_data_t->replys[reply_type].topic, 0x0, GATEWAY_TOPIC_LEN_MAX);
    HAL_Snprintf(thing_t->thing_data_t->replys[reply_type].topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            topic_product_key,
            topic_device_name,
            param1,
            param3);

    /* 组装packet */
    if (reply_type == IOTX_Thing_REPLY_DSL_GET) {
        packet = iotx_thing_splice_common_packet(packet_product_key, packet_device_name, &msg_id, param1, param2, 0);
    } else {
        packet = iotx_thing_splice_common_packet(packet_product_key, packet_device_name, &msg_id, param1, param2, 1);
    }

    /* 组装mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)packet;
    topic_msg.payload_len = strlen(packet);
    topic_msg.packet_id = 0;

    /* 发送packet */
    if (SUCCESS_RETURN != (rc = iotx_thing_publish_topic_sync(thing_t,
                            msg_id,
                            topic,
                            &topic_msg,
                            reply_type))) {
        LITE_free(packet);
        return rc;
    }

    LITE_free(packet);

    if (200 == thing_t->thing_data_t->replys[reply_type].code) {
        printf("common publish successfully [%s]\n", thing_t->thing_data_t->replys[reply_type].topic);
    } else {
        printf("common publish error [%s]\n code[%d]\n", thing_t->thing_data_t->replys[reply_type].topic, thing_t->thing_data_t->replys[reply_type].code);
        if (thing_t->thing_data_t->replys[reply_type].data)
            LITE_free(thing_t->thing_data_t->replys[reply_type].data);

        return (~thing_t->thing_data_t->replys[reply_type].code + 1);
    }

    return SUCCESS_RETURN;
}

int iotx_thing_publish_common_event_packet(void* handle,
        const char* product_key,
        const char* device_name,
        const char* topic_params,
        const char* packet_params,
        iotx_thing_reply_type_t reply_type)
{
    int rc = 0;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    int32_t msg_id = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char* packet = NULL;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    PARAMETER_Thing_CHECK(thing_t);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic_params, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(packet_params, FAIL_RETURN);

    /* 组装topic */
    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_EXT_FMT,
            product_key,
            device_name,
            "event",
            topic_params,
            "post");

    memset(thing_t->thing_data_t->replys[reply_type].topic, 0x0, GATEWAY_TOPIC_LEN_MAX);
    HAL_Snprintf(thing_t->thing_data_t->replys[reply_type].topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_EXT_FMT,
            product_key,
            device_name,
            "event",
            topic_params,
            "post_reply");

    /* 组装packet */
    packet = iotx_thing_splice_common_event_packet(packet_params, topic_params, &msg_id);

    /* 组装mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)packet;
    topic_msg.payload_len = strlen(packet);
    topic_msg.packet_id = 0;

    /* 发送 packet */
    if (SUCCESS_RETURN != (rc = iotx_thing_publish_topic_sync(thing_t,
                            msg_id,
                            topic,
                            &topic_msg,
                            reply_type))) {
        LITE_free(packet);
        return rc;
    }

    LITE_free(packet);

    if (200 == thing_t->thing_data_t->replys[reply_type].code) {
        printf("common publish successfully [%s]\n", thing_t->thing_data_t->replys[reply_type].topic);
    } else {
        printf("common publish error [%s]\n code[%d]\n", thing_t->thing_data_t->replys[reply_type].topic, thing_t->thing_data_t->replys[reply_type].code);
        if (thing_t->thing_data_t->replys[reply_type].data)
            LITE_free(thing_t->thing_data_t->replys[reply_type].data);
        return(~thing_t->thing_data_t->replys[reply_type].code + 1);
    }

    return SUCCESS_RETURN;
}


int iotx_thing_publish_common_deviceinfo_packet(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo,
        const char* param1,
        const char* param2,
        iotx_thing_reply_type_t reply_type)
{
    int rc = 0;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    int32_t msg_id = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char* packet = NULL;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    PARAMETER_Thing_CHECK(thing_t);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(deviceinfo, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param1, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(param2, FAIL_RETURN);

    /* 组装topic */
    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "deviceinfo",
            param1);

    memset(thing_t->thing_data_t->replys[reply_type].topic, 0x0, GATEWAY_TOPIC_LEN_MAX);
    HAL_Snprintf(thing_t->thing_data_t->replys[reply_type].topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "deviceinfo",
            param2);

    /* 组装packet */
    packet = iotx_thing_splice_common_deviceinfo_packet(deviceinfo, param1, &msg_id);

    /* 组装mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)packet;
    topic_msg.payload_len = strlen(packet);
    topic_msg.packet_id = 0;

    /* 发送packet */
    if (SUCCESS_RETURN != (rc = iotx_thing_publish_topic_sync(thing_t,
                            msg_id,
                            topic,
                            &topic_msg,
                            reply_type))) {
        LITE_free(packet);
        return rc;
    }

    LITE_free(packet);

    if (200 == thing_t->thing_data_t->replys[reply_type].code) {
        printf("common publish successfully [%s]\n", thing_t->thing_data_t->replys[reply_type].topic);
    } else {
        printf("common publish error [%s]\n code[%d]\n", thing_t->thing_data_t->replys[reply_type].topic, thing_t->thing_data_t->replys[reply_type].code);
        if (thing_t->thing_data_t->replys[reply_type].data)
            LITE_free(thing_t->thing_data_t->replys[reply_type].data);
        return (~thing_t->thing_data_t->replys[reply_type].code + 1);
    }

    return SUCCESS_RETURN;
}


int iotx_thing_publish_topic_sync(iotx_thing_masterlave_pt thing_t,
        int32_t msg_id,
        const char* topic,
        iotx_mqtt_topic_info_pt topic_msg,
        iotx_thing_reply_type_t reply)
{
    int rc = 0;
    int yiled_count = 0;

    PARAMETER_Thing_CHECK(thing_t);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(topic_msg, FAIL_RETURN);

    printf("topic: [%s] \n", topic);

    thing_t->thing_data_t->replys[reply].id = msg_id;

    if ((rc = IOT_Gateway_Publish(thing_t->gateway, topic, topic_msg)) < 0) {
        printf("Gateway Publish error!\n");
        return rc;
    }

    while (msg_id == thing_t->thing_data_t->replys[reply].id) {
        if (yiled_count > IOT_GATEWAY_YIELD_MAX_COUNT) {
            printf("yiled max count, time out\n");
            return FAIL_RETURN;
        }
        IOT_Thing_Yield(thing_t, 200);
        yiled_count++;
    }

    if (thing_t->thing_data_t->replys[reply].id == 0)
        return SUCCESS_RETURN;

    if (thing_t->thing_data_t->replys[reply].data)
        LITE_free(thing_t->thing_data_t->replys[reply].data);

    return FAIL_RETURN;
}


int iotx_thing_subscribe_unsubscribe_topic(iotx_thing_masterlave_pt thing_t,
        const char* topic,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe)
{
    int ret = 0;
    int yiled_count = 0;

    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);

    if (IOTX_Thing_SUBSCRIBE_TYPE == is_subscribe) {
        ret = IOT_Gateway_Subscribe(thing_t->gateway,
                 topic,
                 IOTX_MQTT_QOS0,
                 iotx_thing_event_handle,
                 thing_t);
    } else {
        ret = IOT_Gateway_Unsubscribe(thing_t->gateway, topic);
    }
    thing_t->thing_data_t->sync_status = ret;

    while (ret == thing_t->thing_data_t->sync_status) {
        if (yiled_count > IOT_GATEWAY_YIELD_MAX_COUNT) {
            printf("yiled max count, time out\n");
            return FAIL_RETURN;
        }

        IOT_Thing_Yield(thing_t, 200);
        yiled_count++;
    }

    if (0 == thing_t->thing_data_t->sync_status) {
        printf(" %s successfully\n", topic);
    } else {
        printf(" %s error!\n\n", topic);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int iotx_thing_subscribe_unsubscribe_basic(iotx_thing_masterlave_pt thing_t,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe)
{
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    int i = 0;
    iotx_device_info_pt pdevice_info = iotx_device_info_get();
    iotx_thing_subscribe_topic_t basic_subscribe_topic[7] = {
        {TOPIC_Thing_COMMON_FMT, "sub", "register_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "sub", "unregister_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "topo", "add_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "topo", "delete_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "topo", "get_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "config", "get_reply", ""},
        {TOPIC_Thing_COMMON_FMT, "list", "found_reply", ""},
    };

    PARAMETER_Thing_CHECK(thing_t);

    for (i = 0; i < 4; i++) {
        memset(topic, 0X0, GATEWAY_TOPIC_LEN_MAX);
        HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                basic_subscribe_topic[i].format,
                pdevice_info->product_key,
                pdevice_info->device_name,
                basic_subscribe_topic[i].params_1,
                basic_subscribe_topic[i].params_2,
                basic_subscribe_topic[i].params_3);
        if (SUCCESS_RETURN != iotx_thing_subscribe_unsubscribe_topic(thing_t, topic, is_subscribe))
            return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_thing_subscribe_unsubscribe_enhance(iotx_thing_masterlave_pt thing_t,
        const char* product_key,
        const char* device_name,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe)
{
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    int i = 0;
    iotx_thing_subscribe_topic_t enhance_subscribe_topic[6] = {
        {TOPIC_Thing_COMMON_FMT, "service", "+", ""},
        {TOPIC_Thing_COMMON_EXT_FMT, "event", "+", "post_reply"},
        {TOPIC_Thing_COMMON_EXT_FMT, "event", "property", "post_reply"},
        {TOPIC_Thing_COMMON_EXT_FMT, "service", "property", "set"},
        {TOPIC_Thing_COMMON_EXT_FMT, "service", "property", "get"},
        {TOPIC_Thing_COMMON_EXT_FMT, "topo", "add", "notify"}
    };

    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    for (i = 0; i < 6; i++) {
        memset(topic, 0X0, GATEWAY_TOPIC_LEN_MAX);
        HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                enhance_subscribe_topic[i].format,
                product_key,
                device_name,
                enhance_subscribe_topic[i].params_1,
                enhance_subscribe_topic[i].params_2,
                enhance_subscribe_topic[i].params_3);
        if (SUCCESS_RETURN != iotx_thing_subscribe_unsubscribe_topic(thing_t, topic, is_subscribe))
            return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


/* parse common_reply result */
static int iotx_thing_common_reply_proc(iotx_thing_masterlave_pt thing_t,
        const char* topic,
        char* payload,
        iotx_thing_reply_type_t type)
{
    char* node = NULL;

    /* check parameter */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(payload, FAIL_RETURN);

    /* parse result */
    /* there is no "id" in UP_RAW */
    if (IOTX_Thing_REPLY_UP_RAW == type) {
        printf("recv up raw reply\n");
        thing_t->thing_data_t->replys[type].id = 0;  /* raw data */
        GET_JSON_VALUE(node, "code", payload);
        thing_t->thing_data_t->replys[type].code = atoi(node);
    } else {
        GET_JSON_VALUE(node, "id", payload);
        if (thing_t->thing_data_t->replys[type].id == atoi(node)) {
            thing_t->thing_data_t->replys[type].id = 0;
        }
        GET_JSON_VALUE_WHIH_FREE(node, "code", payload);
        thing_t->thing_data_t->replys[type].code = atoi(node);
    }

    GET_JSON_VALUE_WHIH_FREE(node, "data", payload);
    MALLOC_MEMORY_WITH_FREE_AND_RESULT(thing_t->thing_data_t->replys[type].data, strlen(node) + 1, node, FAIL_RETURN);
    strncpy(thing_t->thing_data_t->replys[type].data, node, strlen(node));

    LITE_free(node);
    node = NULL;

    return SUCCESS_RETURN;
}

/* parse product_key and device_name form topic */
static int iotx_thing_parse_topic(char* topic, char* product_key, char* device_name)
{
    char* temp = NULL;
    char* start = NULL;

    /* check parameter */
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    if (0 == strncmp(topic, "/sys/", strlen("/sys/"))) {
        /* productKey */
        start = topic + strlen("/sys/");
        temp = strchr(start, '/');
        if (temp == NULL || temp - start > PRODUCT_KEY_LEN) {
            printf("productKey parse error\n");
            return FAIL_RETURN;
        }
        strncpy(product_key, start, temp - start);
        product_key[temp - start] = '\0';

        /* deviceName */
        start = temp + 1;
        temp = strchr(start, '/');
        if (temp == NULL || temp - start > DEVICE_NAME_LEN) {
            printf("deviceName parse error\n");
            return FAIL_RETURN;
        }
        strncpy(device_name, start, temp - start);
        device_name[temp - start] = '\0';

        return SUCCESS_RETURN;
    } else {
        printf("topic error\n\n");
        return FAIL_RETURN;
    }
}

/* handle service request */
static int iotx_thing_service_request_proc(iotx_thing_masterlave_pt thing_t,
        char* topic,
        char* payload)
{
    char product_key[PRODUCT_KEY_LEN] = {0};
    char device_name[DEVICE_NAME_LEN] = {0};
    iotx_thing_service_type_t service_type = IOTX_Thing_SERVICE_TYPE_PROPERTY_GET;
    char* service_id = NULL, *temp = NULL, *node = NULL, *params = NULL;
    int message_id = 0;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(payload, FAIL_RETURN);

    printf("receive service request\n");

    /* parse   */
    if (SUCCESS_RETURN == iotx_thing_parse_topic(topic, product_key, device_name) &&
        (NULL != thing_t->service_callback)) {
        temp = strstr(topic, "/thing/service");
        if (temp == NULL) {
            printf("parse error\n");
            return FAIL_RETURN;
        }

        /* service type */
        temp = strstr(topic, "/thing/service/property");
        if (temp) {
            temp = temp + strlen("/thing/service/property");
            if (0 == strncmp(temp, "/set", strlen("/set"))) {
                service_type = IOTX_Thing_SERVICE_TYPE_PROPERTY_SET;
            } else if(0 == strncmp(temp, "/get", strlen("/get"))) {
                service_type = IOTX_Thing_SERVICE_TYPE_PROPERTY_GET;
            }else {
                printf("service topic error [%s]\n", topic);
                return FAIL_RETURN;
            }
        } else {
            temp = strstr(topic, "/thing/service");
            temp = temp + strlen("/thing/service/");
            service_type = IOTX_Thing_SERVICE_TYPE_UNDEFINED;
            service_id = temp;
        }

        /* parse payload */
        GET_JSON_VALUE(node, "id", payload);
        message_id = atoi(node);

        GET_JSON_VALUE_WHIH_FREE(node, "params", payload);
        MALLOC_MEMORY_WITH_RESULT(params, strlen(node) + 1, FAIL_RETURN);
        strncpy(params, node, strlen(node));

        LITE_free(node);
        node = NULL;

        thing_t->service_callback(thing_t,
                product_key,
                device_name,
                service_type,
                message_id,
                params,
                strlen(params),
                service_id);

        LITE_free(params);

        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}

/* handle service request */
static int iotx_thing_topo_update_proc(iotx_thing_masterlave_pt thing_t,
        char* topic,
        char* payload)
{
    char product_key[PRODUCT_KEY_LEN] = {0};
    char device_name[DEVICE_NAME_LEN] = {0};
    iotx_thing_service_type_t service_type = IOTX_Thing_SERVICE_TYPE_PROPERTY_GET;
    char* service_id = NULL, *temp = NULL, *node = NULL, *params = NULL;
    int message_id = 0;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(payload, FAIL_RETURN);

    printf("receive topo update\n");

    /* parse   */
    if (SUCCESS_RETURN == iotx_thing_parse_topic(topic, product_key, device_name) &&
        (NULL != thing_t->service_callback)) { // reuse service callback
        temp = strstr(topic, "/thing/topo");
        if (temp == NULL) {
            printf("parse error\n");
            return FAIL_RETURN;
        }

        /* service type */
        temp = strstr(topic, "/thing/topo/add/notify");
        if (temp) {
            service_type = IOTX_Thing_SERVICE_TYPE_TOPO_UPDATE;
            service_id = "add/notify";
        } else {
            printf("topo topic error [%s]\n", topic);
            return FAIL_RETURN;
        }

        /* parse payload */
        GET_JSON_VALUE(node, "id", payload);
        message_id = atoi(node);

        GET_JSON_VALUE_WHIH_FREE(node, "params", payload);
        MALLOC_MEMORY_WITH_RESULT(params, strlen(node) + 1, FAIL_RETURN);
        strncpy(params, node, strlen(node));

        LITE_free(node);
        node = NULL;

        thing_t->service_callback(thing_t,
                product_key,
                device_name,
                service_type,
                message_id,
                params,
                strlen(params),
                service_id);

        LITE_free(params);

        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}



/* handle down_raw */
static int iotx_thing_down_raw_proc(iotx_thing_masterlave_pt thing_t,
        char* topic,
        char* payload)
{
    char product_key[PRODUCT_KEY_LEN] = {0};
    char device_name[DEVICE_NAME_LEN] = {0};

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(payload, FAIL_RETURN);

    printf("receive down raw\n");

    if (SUCCESS_RETURN == iotx_thing_parse_topic(topic, product_key, device_name)) {
        if (thing_t->raw_callback) {
            thing_t->raw_callback(thing_t,
                        product_key,
                        device_name,
                        payload,
                        strlen(payload));
            /* pulish a down_raw_reply packet */
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

/* handle thing control request */
static int iotx_thing_proc(iotx_thing_masterlave_pt thing_t,
        char* topic,
        char* payload,
        iotx_thing_control_type_t control_type)
{
    char product_key[PRODUCT_KEY_LEN] = {0};
    char device_name[DEVICE_NAME_LEN] = {0};
    int message_id = 0;
    char *node = NULL;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(payload, FAIL_RETURN);

    printf("receive thing control\n");
    GET_JSON_VALUE(node, "id", payload);
    message_id = atoi(node);

    if (SUCCESS_RETURN == iotx_thing_parse_topic(topic, product_key, device_name)) {
        if (thing_t->thing_callback) {
            thing_t->thing_callback(thing_t, product_key, device_name, message_id, control_type);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}


#ifdef SUBDEV_VIA_CLOUD_CONN
static int iotx_thing_receive_date(iotx_thing_masterlave_pt thing_t,
    iotx_cloud_connection_msg_rsp_pt msg_rsp)
{
    iotx_thing_reply_type_t reply_type = IOTX_Thing_REPLY_REGISTER;
    char* temp = NULL;
    char* publish_topic = NULL;
    char* publish_payload = NULL;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(msg_rsp, FAIL_RETURN);

    printf("recv publish message\n");

    MALLOC_MEMORY_WITH_RESULT(publish_topic, msg_rsp->URI_length, FAIL_RETURN);
    MALLOC_MEMORY_WITH_FREE_AND_RESULT(publish_payload, msg_rsp->payload_length, publish_topic, FAIL_RETURN);

    strncpy(publish_topic, msg_rsp->URI, msg_rsp->URI_length);
    strncpy(publish_payload, msg_rsp->payload, msg_rsp->payload_length);

    /* reply topic array */
    for (; reply_type < IOTX_Thing_REPLY_MAX; reply_type++) {
        if (msg_rsp->URI_length == strlen(thing_t->thing_data_t->replys[reply_type].topic) &&
            (0 == strncmp(msg_rsp->URI, thing_t->thing_data_t->replys[reply_type].topic, msg_rsp->URI_length))) {
            iotx_thing_common_reply_proc(thing_t, publish_topic, publish_payload, reply_type);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }
    }

    /* sys topic */
    if (0 == strncmp(msg_rsp->URI, "/sys/", strlen("/sys/"))) {
        /* service   */
        temp = strstr(msg_rsp->URI, "/thing/service/");
        if (temp != NULL) {
            iotx_thing_service_request_proc(thing_t, publish_topic, publish_payload);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* raw data   */
        temp = strstr(msg_rsp->URI, "/thing/model/down_raw");
        if (temp != NULL) {
            iotx_thing_down_raw_proc(thing_t, publish_topic, publish_payload);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - disable */
        temp = strstr(msg_rsp->URI, "/thing/disable");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_DISABLE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - delete */
        temp = strstr(msg_rsp->URI, "/thing/delete");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_DELETE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - enable */
        temp = strstr(msg_rsp->URI, "/thing/enable");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_ENABLE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }
    }

    printf("can not find the topic!\n");

    LITE_free(publish_topic);
    LITE_free(publish_payload);
    return FAIL_RETURN;
}

#else

/* publish callback */
static int iotx_thing_recv_publish_callback(iotx_thing_masterlave_pt thing_t,
        iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    iotx_thing_reply_type_t reply_type = IOTX_Thing_REPLY_REGISTER;
    char* temp = NULL;
    char* publish_topic = NULL;
    char* publish_payload = NULL;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(topic_info, FAIL_RETURN);

    printf("recv publish message\n");

    MALLOC_MEMORY_WITH_RESULT(publish_topic, topic_info->topic_len + 1, FAIL_RETURN);
    MALLOC_MEMORY_WITH_FREE_AND_RESULT(publish_payload, topic_info->payload_len + 1, publish_topic, FAIL_RETURN);

    strncpy(publish_topic, topic_info->ptopic, topic_info->topic_len);
    strncpy(publish_payload, topic_info->payload, topic_info->payload_len);

    /* reply topic array */
    for (; reply_type < IOTX_Thing_REPLY_MAX; reply_type++) {
        if (topic_info->topic_len == strlen(thing_t->thing_data_t->replys[reply_type].topic) &&
            (0 == strncmp(topic_info->ptopic, thing_t->thing_data_t->replys[reply_type].topic, topic_info->topic_len))) {
            iotx_thing_common_reply_proc(thing_t, publish_topic, publish_payload, reply_type);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }
    }

    /* sys topic */
    if (0 == strncmp(topic_info->ptopic, "/sys/", strlen("/sys/"))) {
        /* service   */
        temp = strstr(topic_info->ptopic, "/thing/service/");
        if (temp != NULL) {
            iotx_thing_service_request_proc(thing_t, publish_topic, publish_payload);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* raw data   */
        temp = strstr(topic_info->ptopic, "/thing/model/down_raw");
        if (temp != NULL) {
            iotx_thing_down_raw_proc(thing_t, publish_topic, publish_payload);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - disable */
        temp = strstr(topic_info->ptopic, "/thing/disable");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_DISABLE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - delete */
        temp = strstr(topic_info->ptopic, "/thing/delete");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_DELETE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* thing control - enable */
        temp = strstr(topic_info->ptopic, "/thing/enable");
        if (temp != NULL) {
            iotx_thing_proc(thing_t, publish_topic, publish_payload, IOTX_Thing_CONTROL_TYPE_ENABLE);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }

        /* topo update */
        temp = strstr(topic_info->ptopic, "/thing/topo");
        if (temp != NULL) {
            iotx_thing_topo_update_proc(thing_t, publish_topic, publish_payload);
            LITE_free(publish_topic);
            LITE_free(publish_payload);
            return SUCCESS_RETURN;
        }
    }

    printf("can not find the topic!\n");

    LITE_free(publish_topic);
    LITE_free(publish_payload);
    return FAIL_RETURN;
}
#endif


/* event   callback */
void iotx_thing_event_handle(void *pcontext, void *pclient, void* msg_pt)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)pcontext;
#ifdef SUBDEV_VIA_CLOUD_CONN
    iotx_cloud_connection_msg_rsp_pt msg = (iotx_cloud_connection_msg_rsp_pt)msg_pt;
#else
    iotx_mqtt_event_msg_pt msg = (iotx_mqtt_event_msg_pt)msg_pt;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)(msg->msg);
    uintptr_t packet_id = (uintptr_t)msg->msg;
#endif

    /* parameter check */
    if (thing_t == NULL) {
        printf("param error\n");
        return;
    }

    if (thing_t->gateway == NULL) {
        printf("param error\n");
        return;
    }

    if (thing_t->thing_data_t == NULL) {
        printf("param error\n");
        return;
    }

#ifdef SUBDEV_VIA_CLOUD_CONN
    printf("event type %d\n", msg->rsp_type);

    switch (msg->rsp_type)
    {
        case IOTX_CLOUD_CONNECTION_RESPONSE_SUBSCRIBE_SUCCESS:
        case IOTX_CLOUD_CONNECTION_RESPONSE_UNSUBSCRIBE_SUCCESS:
            thing_t->thing_data_t->sync_status = 0;
            break;

        case IOTX_CLOUD_CONNECTION_RESPONSE_SUBSCRIBE_FAIL:
        case IOTX_CLOUD_CONNECTION_RESPONSE_UNSUBSCRIBE_FAIL:
            thing_t->thing_data_t->sync_status = -1;
            break;

        case IOTX_CLOUD_CONNECTION_RESPONSE_SEND_SUCCESS:
        case IOTX_CLOUD_CONNECTION_RESPONSE_NEW_DATA:
            iotx_thing_receive_date(thing_t, msg);
            break;

        case IOTX_CLOUD_CONNECTION_RESPONSE_SEND_FAIL:
            break;

        default:
            break;
    }
#else
    PARAMETER_NULL_CHECK(topic_info);

    printf("event type %d\n", msg->event_type);

    switch (msg->event_type) {
        /* success */
        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            if (thing_t->thing_data_t->sync_status == packet_id) {
                thing_t->thing_data_t->sync_status = 0;
            }
            break;

        /* fail */
        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            if (thing_t->thing_data_t->sync_status == packet_id) {
                thing_t->thing_data_t->sync_status = -1;
            }
            break;

        /* publish */
        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            iotx_thing_recv_publish_callback(thing_t, msg);
            break;

        /* reconnect */
        case IOTX_MQTT_EVENT_RECONNECT:
            /* CMP will handle the reconnect event */
            printf("mqtt reconnect\n");
            break;

        default:
            printf("unknown event\n");
            break;
    }
#endif
    return;
}

/* constuct Thing instanct, create gateway and MQTT, subscribe gateway topic */
void* IOT_Thing_Construct(iotx_thing_param_pt param)
{
    iotx_gateway_param_t gateway_param;
    iotx_thing_masterlave_pt thing_t = NULL;
    void* gateway = NULL;
    iotx_device_info_pt pdevice_info = iotx_device_info_get();

    /* parameter check */
    PARAMETER_NULL_CHECK_WITH_RESULT(param, NULL);
    PARAMETER_NULL_CHECK_WITH_RESULT(param->mqtt, NULL);

    if (g_thing_masterlave_t != NULL) {
        printf("thing have been construct\n");
        return NULL;
    }

    MALLOC_MEMORY_WITH_RESULT(thing_t, sizeof(iotx_thing_masterlave_t), NULL);
    MALLOC_MEMORY_WITH_FREE_AND_RESULT(thing_t->thing_data_t, sizeof(iotx_thing_masterlave_data_t), thing_t, NULL);

    memset(&gateway_param, 0x0, sizeof(iotx_gateway_param_t));
    gateway_param.mqtt = param->mqtt;
    gateway_param.event_handler = iotx_thing_event_handle;    /* set MQTT event callback */
    gateway_param.event_pcontext = thing_t;                   /* callback userdata */

    /* create gateway */
    if (NULL == (gateway = IOT_Gateway_Construct(&gateway_param))) {
        printf("gateway construct fail\n");
        LITE_free(thing_t->thing_data_t);
        LITE_free(thing_t);
        return NULL;
    }

    thing_t->gateway = gateway;

    g_thing_masterlave_t = thing_t;

    /* subscribe register and topo topic
    if (FAIL_RETURN == iotx_thing_subscribe_unsubscribe_basic(thing_t, IOTX_Thing_SUBSCRIBE_TYPE)) {
        if (FAIL_RETURN == IOT_Thing_Destroy((void**)&thing_t)) {
            LITE_free(thing_t->thing_data_t);
            LITE_free(thing_t);
        }
        return NULL;
    } */

    /* subscribe thing template topic */
    if (SUCCESS_RETURN != iotx_thing_subscribe_unsubscribe_enhance(thing_t,
                                pdevice_info->product_key,
                                pdevice_info->device_name,
                                IOTX_Thing_SUBSCRIBE_TYPE)) {
        if (SUCCESS_RETURN != IOT_Thing_Destroy((void**)&thing_t)) {
            LITE_free(thing_t->thing_data_t);
            LITE_free(thing_t);
        }
        return NULL;
    }

    return thing_t;
}

/* destory Thing: MQTT disconnect, free memory */
int IOT_Thing_Destroy(void** handle)
{
    iotx_thing_masterlave_pt thing_t = NULL;
    iotx_device_info_pt pdevice_info = iotx_device_info_get();

    /* parameter chcek */
    PARAMETER_NULL_CHECK_WITH_RESULT(handle, FAIL_RETURN);

    thing_t = (iotx_thing_masterlave_pt)(*handle);
    PARAMETER_Thing_CHECK(thing_t);

    /* unsubscribe register and topo topic
    if (FAIL_RETURN == iotx_thing_subscribe_unsubscribe_basic(thing_t, IOTX_Thing_UNSUBSCRIBE_TYPE)) {
        printf("unsubscribe basic topic fail");
    }*/

    /* unsubscribe thing template topic */
    if (SUCCESS_RETURN != iotx_thing_subscribe_unsubscribe_enhance(thing_t,
                                pdevice_info->product_key,
                                pdevice_info->device_name,
                                IOTX_Thing_UNSUBSCRIBE_TYPE)){
        printf("unsubscribe enhance topic fail\n");
    }

    /* gateway destory */
    if (SUCCESS_RETURN != IOT_Gateway_Destroy((void**)&(thing_t->gateway))) {
        printf("Gateway destory fail\n");
        return FAIL_RETURN;
    }

    LITE_free(thing_t->thing_data_t);
    LITE_free(thing_t);
    *handle = NULL;
    g_thing_masterlave_t = NULL;

    return SUCCESS_RETURN;
}


/* Register: static and dynamic */
int IOT_Thing_Register(void* handle,
        iotx_thing_register_types_t type,
        const char* product_key,
        const char* device_name,
        char* timestamp,
        char* client_id,
        char* sign,
        iotx_thing_sign_method_types_t sign_type)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    return IOT_Subdevice_Register(thing_t->gateway, type, product_key, device_name, timestamp, client_id, sign, sign_type);
}

/* unregister: topo delete first, then unregister */
int IOT_Thing_Unregister(void* handle,
        const char* product_key,
        const char* device_name)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    return IOT_Subdevice_Unregister(thing_t->gateway, product_key, device_name);
}


/* login */
int IOT_Thing_Login(void* handle,
        const char* product_key,
        const char* device_name,
        char* timestamp,
        char* client_id,
        char* sign,
        iotx_thing_sign_method_types_t sign_method,
        iotx_thing_clean_session_types_t clean_session)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    /*  subdevice login */
    if (SUCCESS_RETURN != (rc = IOT_Subdevice_Login(thing_t->gateway,
                            product_key,
                            device_name,
                            timestamp,
                            client_id,
                            sign,
                            sign_method,
                            clean_session))) {
        printf("subdevice login fail\n");
        return rc;
    }

    /* subscribe thing template topic */
    if (SUCCESS_RETURN != (rc = iotx_thing_subscribe_unsubscribe_enhance(thing_t,
                            product_key,
                            device_name,
                            IOTX_Thing_SUBSCRIBE_TYPE))) {
        IOT_Subdevice_Logout(thing_t->gateway, product_key, device_name);
        printf("subscribe topic fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}

/* logout */
int IOT_Thing_Logout(void* handle,
        const char* product_key,
        const char* device_name)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    /*  subdevice logout */
    if (SUCCESS_RETURN != (rc = IOT_Subdevice_Logout(thing_t->gateway, product_key, device_name))) {
        printf("subdevice logout fail\n");
        return rc;
    }

    /* unsubscribe thing template topic */
    if (SUCCESS_RETURN != (rc = iotx_thing_subscribe_unsubscribe_enhance(thing_t,
                        product_key,
                        device_name,
                        IOTX_Thing_UNSUBSCRIBE_TYPE))) {
        printf("unsubscribe topic fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}


int IOT_Thing_Get_TOPO(void* handle,
        char* get_toop_reply,
        uint32_t* length)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(get_toop_reply, FAIL_RETURN);

    /*  get topo */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Get_TOPO(thing_t->gateway, get_toop_reply, length))) {
        printf("get topo fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}


int IOT_Thing_Get_Config(void* handle,
        char* get_config_reply,
        uint32_t* length)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(get_config_reply, FAIL_RETURN);

    /*  get config */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Get_Config(thing_t->gateway, get_config_reply, length))) {
        printf("get config fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}


int IOT_Thing_Publish_Found_List(void* handle, const char* product_key,
    const char* device_name)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);

    /*  publish new subdevice found list */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Publish_Found_List(thing_t->gateway, product_key, device_name))) {
        printf("publish new dev found list fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}


/* get dsl template */
int IOT_Thing_Get_Dsl_Template(void* handle,
        const char* product_key,
        const char* device_name,
        char* dsl_template,
        uint32_t* length)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(dsl_template, FAIL_RETURN);

    if (SUCCESS_RETURN != (rc = iotx_thing_publish_common_packet(thing_t,
                            product_key,
                            device_name,
                            product_key,
                            device_name,
                            "dsltemplate",
                            "get",
                            "get_reply",
                            IOTX_Thing_REPLY_DSL_GET))) {
        printf("publish common packet error\n");
        return rc;
    }

    /* check length */
    if (*length < strlen(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DSL_GET].data)) {
        printf("dsltemplate length is too small\n");
        LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DSL_GET].data);
        return FAIL_RETURN;
    }

    memset(dsl_template, 0x0, *length);
    strncpy(dsl_template,
        thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DSL_GET].data,
        strlen(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DSL_GET].data));
    (*length) = strlen(dsl_template);
    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DSL_GET].data);

    return SUCCESS_RETURN;
}

/* post property */
int IOT_Thing_Post_Property(void* handle,
        const char* product_key,
        const char* device_name,
        const char* property)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(property, FAIL_RETURN);

    if (SUCCESS_RETURN != (rc = iotx_thing_publish_common_event_packet(thing_t,
                            product_key,
                            device_name,
                            "property",
                            property,
                            IOTX_Thing_REPLY_PROPERTY_POST))) {
        printf("publish common event packet error\n");
        return rc;
    }

    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_PROPERTY_POST].data);

    return SUCCESS_RETURN;
}

/* update device info */
int IOT_Thing_Update_Deviceinfo(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(deviceinfo, FAIL_RETURN);

    if (SUCCESS_RETURN != (rc = iotx_thing_publish_common_deviceinfo_packet(thing_t,
                            product_key,
                            device_name,
                            deviceinfo,
                            "update",
                            "update_reply",
                            IOTX_Thing_REPLY_DEVICEINFO_UPDATE))) {
        printf("publish common packet error\n");
        return rc;
    }

    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DEVICEINFO_UPDATE].data);

    return SUCCESS_RETURN;
}

/* delete device info */
int IOT_Thing_Delete_Deviceinfo(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(deviceinfo, FAIL_RETURN);

    if (SUCCESS_RETURN != (rc = iotx_thing_publish_common_deviceinfo_packet(thing_t,
                            product_key,
                            device_name,
                            deviceinfo,
                            "delete",
                            "delete_reply",
                            IOTX_Thing_REPLY_DEVICEINFO_DELETE))) {
        printf("publish common packet error\n");
        return rc;
    }

    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_DEVICEINFO_DELETE].data);
    return SUCCESS_RETURN;
}

/* response for set property        */
int IOT_Thing_Set_Property_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t msg_id,
        uint32_t code)
{
    int rc = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char * property_packet = NULL;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    printf("IOT_Thing_Set_Property_Response\n");

    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_EXT_FMT,
            product_key,
            device_name,
            "service",
            "property",
            "set_reply");

    property_packet = iotx_thing_splice_default_reply_packet(msg_id, code);
    /* mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)property_packet;
    topic_msg.payload_len = strlen(property_packet);
    topic_msg.packet_id = 0;

    if ((rc = IOT_Gateway_Publish(thing_t->gateway, topic, &topic_msg)) < 0) {
        printf("Gateway Publish error!\n");
        LITE_free(property_packet);
        return rc;
    }

    LITE_free(property_packet);

    return SUCCESS_RETURN;
}

/* response for get property        */
int IOT_Thing_Get_Property_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t msg_id,
        const char* property_data,
        uint32_t code)
{
    int rc = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    char * property_packet = NULL;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(property_data, FAIL_RETURN);

    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_EXT_FMT,
            product_key,
            device_name,
            "service",
            "property",
            "get_reply");

    property_packet = iotx_thing_splice_default_reply_get_packet(msg_id, code, property_data);
    /* 组装mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)property_packet;
    topic_msg.payload_len = strlen(property_packet);
    topic_msg.packet_id = 0;

    if ((rc = IOT_Gateway_Publish(thing_t->gateway, topic, &topic_msg)) < 0) {
        printf("Gateway Publish error!\n");
        LITE_free(property_packet);
        return rc;
    }

    LITE_free(property_packet);

    return SUCCESS_RETURN;
}

/* register callback for service */
int IOT_Thing_Service_Register(void* handle,
        service_request_callback service_callback)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(service_callback, FAIL_RETURN);

    if (thing_t->service_callback != NULL) {
        printf("service_callback have been set, can not set again\n");
        return FAIL_RETURN;
    }

    thing_t->service_callback = service_callback;

    return SUCCESS_RETURN;
}

/* register callback for raw data */
int IOT_Thing_Down_Raw_Register(void* handle, down_raw_callback raw_callback)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(raw_callback, FAIL_RETURN);

    if (thing_t->raw_callback != NULL) {
        printf("raw_callback have been set, can not set again\n");
        return FAIL_RETURN;
    }

    thing_t->raw_callback = raw_callback;

    return SUCCESS_RETURN;
}


/* reply a down_raw_reply to server */
int IOT_Tmp_Down_Raw_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* response)
{
    int rc = 0;
    iotx_mqtt_topic_info_t topic_msg;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(response, FAIL_RETURN);

    /* topic */
    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "model",
            "down_raw_reply");

    /* mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)response;
    topic_msg.payload_len = strlen(response);
    topic_msg.packet_id = 0;

    /* publish mqtt packet */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Publish(thing_t->gateway, topic, &topic_msg))) {
        printf("Gateway publish fail\n");
        return rc;
    }

    return SUCCESS_RETURN;
}


/* register callback for thing control */
int IOT_Thing_Control_Register(void* handle,
        thing_control_callback thing_callback)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_NULL_CHECK_WITH_RESULT(thing_callback, FAIL_RETURN);

    thing_t->thing_callback = thing_callback;

    return SUCCESS_RETURN;
}

/* register callback for RRPC */
int IOT_Thing_RRPC_Register(        void* handle,
        const char* product_key,
        const char* device_name,
        rrpc_request_callback rrpc_callback)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(rrpc_callback, FAIL_RETURN);

    return IOT_Gateway_RRPC_Register(thing_t->gateway,
                product_key,
                device_name,
                rrpc_callback);
}

/* response for RRPC */
int IOT_Thing_RRPC_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* message_id,
        const char* response)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(message_id, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(response, FAIL_RETURN);

    return IOT_Gateway_RRPC_Response(thing_t->gateway,
                product_key,
                device_name,
                message_id,
                response);
}

/* response for service */
int IOT_Thing_Service_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* service_id,
        uint32_t message_id,
        uint32_t code)
{
    int rc = 0;
    char* response_packet = NULL;
    char response_reply_topic[20] = {0};
    iotx_mqtt_topic_info_t topic_msg;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(service_id, FAIL_RETURN);

    HAL_Snprintf(response_reply_topic, 20, "%s_reply", service_id);

    /* topic */
    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "service",
            response_reply_topic);

    /* packet */
    response_packet = iotx_thing_splice_default_reply_packet(message_id, code);

    /* mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)response_packet;
    topic_msg.payload_len = strlen(response_packet);
    topic_msg.packet_id = 0;

    /* publish mqtt packet */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Publish(thing_t->gateway, topic, &topic_msg))) {
        printf("Gateway publish fail\n");
        LITE_free(response_packet);
        return rc;
    }

    LITE_free(response_packet);

    return SUCCESS_RETURN;
}

/* response for thing control */
int IOT_Thing_Control_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t message_id,
        uint32_t code,
        iotx_thing_control_type_t control_type)
{
    int rc = 0;
    char* response_packet = NULL;
    iotx_mqtt_topic_info_t topic_msg;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    if (control_type == IOTX_Thing_CONTROL_TYPE_ENABLE) {
        /* topic */
        HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                TOPIC_Thing_COMMON_EXT_EXT_FMT,
                product_key,
                device_name,
                "enable_reply");
    } else if (control_type == IOTX_Thing_CONTROL_TYPE_DISABLE) {
        /* topic */
        HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                TOPIC_Thing_COMMON_EXT_EXT_FMT,
                product_key,
                device_name,
                "disable_reply");
    } else if (control_type == IOTX_Thing_CONTROL_TYPE_DELETE) {
        /* topic */
        HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                TOPIC_Thing_COMMON_EXT_EXT_FMT,
                product_key,
                device_name,
                "delete_reply");
    }

    /* packet */
    response_packet = iotx_thing_splice_default_reply_packet(message_id, code);

    /* mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)response_packet;
    topic_msg.payload_len = strlen(response_packet);
    topic_msg.packet_id = 0;

    /* publish mqtt packet */
    if (SUCCESS_RETURN != (rc = IOT_Gateway_Publish(thing_t->gateway, topic, &topic_msg))) {
        printf("Gateway publish fail\n");
        LITE_free(response_packet);
        return FAIL_RETURN;
    }

    LITE_free(response_packet);

    return SUCCESS_RETURN;
}

/* trigger event */
int IOT_Thing_Trigger_Event(void* handle,
        const char* product_key,
        const char* device_name,
        const char* params,
        const char* event_id)
{
    int rc = 0;
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(params, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(event_id, FAIL_RETURN);

    if (SUCCESS_RETURN != (rc = iotx_thing_publish_common_event_packet(thing_t,
                            product_key,
                            device_name,
                            event_id,
                            params,
                            IOTX_Thing_REPLY_EVENT_POST))) {
        printf("publish common event packet error\n");
        return rc;
    }

    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_EVENT_POST].data);

    return SUCCESS_RETURN;
}

/* publish raw data */
int IOT_Thing_Publish_Rawdata(void* handle,
        const char* product_key,
        const char* device_name,
        const char* raw_data,
        uint32_t raw_data_length)
{
    int32_t msg_id = -2;   /* for raw data */
    iotx_mqtt_topic_info_t topic_msg;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(raw_data, FAIL_RETURN);

    HAL_Snprintf(topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "model",
            "up_raw");
    memset(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_UP_RAW].topic, 0x0, GATEWAY_TOPIC_LEN_MAX);
    HAL_Snprintf(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_UP_RAW].topic,
            GATEWAY_TOPIC_LEN_MAX,
            TOPIC_Thing_COMMON_FMT,
            product_key,
            device_name,
            "model",
            "up_raw_reply");

    /* mqtt packet */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)raw_data;
    topic_msg.payload_len = raw_data_length;
    topic_msg.packet_id = 0;

    /* publish packet */
    if (FAIL_RETURN == iotx_thing_publish_topic_sync(thing_t,
            msg_id,
            topic,
            &topic_msg,
            IOTX_Thing_REPLY_UP_RAW)) {
        return FAIL_RETURN;
    }

    LITE_free(thing_t->thing_data_t->replys[IOTX_Thing_REPLY_UP_RAW].data);

    if (200 == thing_t->thing_data_t->replys[IOTX_Thing_REPLY_UP_RAW].code) {
        printf("up raw success\n");
    } else {
        printf("up raw replay error!code:%d\n", thing_t->thing_data_t->replys[IOTX_Thing_REPLY_UP_RAW].code);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int IOT_Thing_Yield(void* handle, uint32_t timeout)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);

    return IOT_Gateway_Yield(thing_t->gateway, timeout);
}

int IOT_Thing_Subscribe(void* handle,
        const char *topic_filter,
        int qos,
        iotx_subdev_event_handle_func_fpt topic_handle_func,
        void *pcontext)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);

    return IOT_Gateway_Subscribe(thing_t->gateway, topic_filter, qos, topic_handle_func, pcontext);
}

int IOT_Thing_Unsubscribe(void* handle, const char *topic_filter)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);

    return IOT_Gateway_Unsubscribe(thing_t->gateway, topic_filter);
}

int IOT_Thing_Publish(void* handle,
        const char *topic_name,
        iotx_mqtt_topic_info_pt topic_msg)
{
    iotx_thing_masterlave_pt thing_t = (iotx_thing_masterlave_pt)handle;

    /* parameter check */
    PARAMETER_Thing_CHECK(thing_t);

    return IOT_Gateway_Publish(thing_t->gateway, topic_name, topic_msg);
}


