/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "jse_common.h"
#include "mqtt_task.h"
#include "mqtt_instance.h"

#include "linkkit/infra/infra_compat.h"
#include "linkkit/mqtt_api.h"
#include "linkkit/wrappers/wrappers.h"

#ifndef CONFIG_MQTT_STACK_SIZE
#define CONFIG_MQTT_STACK_SIZE 16384
#endif

static void* mqttMutexHandle        = NULL;
static void* mqttTskHandle          = NULL;
static osMessageQId mqttQueueHandle = NULL;

#define MAX_MQTT_MSG_SIZE 2048

typedef enum {
    POST_ERROR = 1,
    POST_ONLINE,
    POST_SUBSCRIBE,
    POST_UNSUBSCRIBE,
} JSE_TSK_CMD;

typedef struct {
    int cmd;
    be_jse_symbol_t* func;
    char* topic;
    char* payload;
} JSE_TSK_MSG_s;

static void jse_task_cb(void* ctx)
{
    JSE_TSK_MSG_s* pmsg = (JSE_TSK_MSG_s*)ctx;
    be_jse_symbol_t** params;
    be_jse_symbol_t* func = pmsg->func;

    switch (pmsg->cmd) {
        case POST_ERROR:
            params = (be_jse_symbol_t**)jse_calloc(1, sizeof(be_jse_symbol_t*) * 1);
            if (params) {
                params[0] = new_str_symbol(pmsg->topic);
                be_jse_post_async(func, params, 1);
            } else {
                symbol_unlock(func);
            }
            break;
        case POST_ONLINE:
            be_jse_post_async(func, NULL, 0);
            break;
        case POST_SUBSCRIBE:
            params = (be_jse_symbol_t**)jse_calloc(1, sizeof(be_jse_symbol_t*) * 2);
            /* maybe call subscribe callback function multiple times! */
            symbol_relock(func);
            if (params) {
                params[0] =
                    new_json_symbol(pmsg->payload, strlen(pmsg->payload));
                params[1] = new_str_symbol(pmsg->topic);
                be_jse_post_async(func, params, 2);
            } else {
                symbol_unlock(func);
            }
            break;
        case POST_UNSUBSCRIBE:
            symbol_unlock(func);
            break;
    }
    jse_free(pmsg->topic);
    jse_free(pmsg->payload);
    jse_free(pmsg);
}

static void mqtt_subscribe_cb(char* topic, int topic_len, void* payload_data,
                              int payload_len, void* ctx)
{
    char ch;
    be_jse_symbol_t* func = (be_jse_symbol_t*)ctx;
    JSE_TSK_MSG_s* pmsg;

    pmsg       = jse_calloc(1, sizeof(JSE_TSK_MSG_s));
    pmsg->cmd  = POST_SUBSCRIBE;
    pmsg->func = func;

    ch               = topic[topic_len];
    topic[topic_len] = 0;
    pmsg->topic      = strdup(topic);
    topic[topic_len] = ch;

    char* payload        = (char*)payload_data;
    ch                   = payload[payload_len];
    payload[payload_len] = 0;
    pmsg->payload        = strdup(payload);
    payload[payload_len] = ch;

    be_jse_task_schedule_call(jse_task_cb, pmsg);
}

static void mqtt_task(void* data)
{
    int ret;
    MQTT_MSG_s msg;
    uint32_t xTicksToWait = 50;
    void* handle;
    int done = false;
    be_jse_symbol_t* func;
    JSE_TSK_MSG_s* pmsg;

    while (!done) {
        ret = jse_osal_messageQ_get(mqttQueueHandle, &msg, xTicksToWait);
        if (ret == 0) {
            /* process MQTT Message */
            switch (msg.common.cmd) {
                case MQTT_START:
                    if (mqtt_get_instance()) {
                        /* 1:  mqtt instance have been init */
                        ret = 1;

                    } else {
                        ret = mqtt_init_instance(
                            msg.start.productKey, msg.start.deviceName,
                            msg.start.deviceSecret, MAX_MQTT_MSG_SIZE);
                        if (ret == 0) {
                            /* mqtt connect success. */
                            pmsg       = jse_calloc(1, sizeof(JSE_TSK_MSG_s));
                            pmsg->cmd  = POST_ONLINE;
                            pmsg->func = msg.start.func;
                            be_jse_task_schedule_call(jse_task_cb, pmsg);
                        }

                        IOT_SetLogLevel(IOT_LOG_WARNING);
                    }

                    if (ret != 0) {
                        /* notify JS App */
                        pmsg       = jse_calloc(1, sizeof(JSE_TSK_MSG_s));
                        pmsg->cmd  = POST_ERROR;
                        pmsg->func = msg.start.func;
                        if (ret == 1) {
                            pmsg->topic = strdup("mqtt instance already start");
                        } else {
                            pmsg->topic = strdup("mqtt_init_instance error");
                        }
                        be_jse_task_schedule_call(jse_task_cb, pmsg);
                    }
                    break;

                case MQTT_SUBSCRIBE:
                    ret = mqtt_subscribe(msg.subscribe.topic, mqtt_subscribe_cb,
                                         msg.subscribe.func);
                    jse_free(msg.subscribe.topic);
                    break;

                case MQTT_UNSUBSCRIBE:
                    ret        = mqtt_unsubscribe(msg.unsubscribe.topic);
                    pmsg       = jse_calloc(1, sizeof(JSE_TSK_MSG_s));
                    pmsg->cmd  = POST_UNSUBSCRIBE;
                    pmsg->func = msg.start.func;
                    be_jse_task_schedule_call(jse_task_cb, pmsg);
                    jse_free(msg.unsubscribe.topic);
                    break;

                case MQTT_PUBLISH:
                    ret = mqtt_publish(msg.publish.topic, IOTX_MQTT_QOS1,
                                       msg.publish.payload,
                                       strlen(msg.publish.payload));
                    jse_free(msg.publish.topic);
                    jse_free(msg.publish.payload);
                    break;

                case MQTT_STOP:
                    /* 退出 MQTT */
                    done = 1;
                    break;
                default:
                    break;
            }

        } else {
            handle = mqtt_get_instance();
            if (handle) {
                IOT_MQTT_Yield(handle, 50);
            }
        }
    }

    handle = mqtt_get_instance();
    if (handle) {
        mqtt_deinit_instance();
    }

    handle        = mqttTskHandle;
    mqttTskHandle = NULL;

    jse_osal_unlock_mutex(mqttMutexHandle);

    jse_osal_delete_task(handle);
}

int mqtt_tsk_start()
{
    int ret = 0;
    if (mqttMutexHandle == NULL) {
        mqttMutexHandle = jse_osal_new_mutex();
    }
    if (mqttQueueHandle == NULL)
        mqttQueueHandle = jse_osal_messageQ_create(5, sizeof(MQTT_MSG_s));
    if (mqttTskHandle == NULL) {
        /* mqtt_tsk_stop() will waiting task exit and free memory */
        jse_osal_lock_mutex(mqttMutexHandle, 0);
        ret = jse_osal_create_task("mqttTsk", mqtt_task, NULL,
                                  CONFIG_MQTT_STACK_SIZE, MQTT_TSK_PRIORITY,
                                  &mqttTskHandle);
    }
    return ret;
}

int mqtt_tsk_stop()
{
    MQTT_MSG_s msg;
    memset(&msg, 0, sizeof(MQTT_MSG_s));

    if (mqttQueueHandle) {
        msg.common.cmd = MQTT_STOP;
        mqtt_send_msg(&msg);
        /* waiting mqtt task exit */
        jse_osal_lock_mutex(mqttMutexHandle, 300);
        jse_osal_messageQ_delete(mqttQueueHandle);
        mqttQueueHandle = NULL;
    }
    return 0;
}

int mqtt_send_msg(MQTT_MSG_s* pMsg)
{
    int32_t ret;
    uint32_t xTicksToWait = 0xffffffffL;

    ret = jse_osal_messageQ_put(mqttQueueHandle, (void*)pMsg, xTicksToWait);
    if (ret == 0) return 0;
    return -1;
}
