/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos/kv.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "module_mqtt.h"

#define MOD_STR "MODULE_MQTT_CLIENT"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

uint8_t mqtt_process_thread_running = 0;
uint8_t mqtt_recv_thread_running = 0;

static char *__amp_strdup(char *src, int len)
{
    char   *dst;

    if (src == NULL) {
        return NULL;
    }

    dst = amp_malloc(len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
void mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (mqtt_process_thread_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        aos_msleep(1000);
    }
    aos_task_exit(0);
    return;
}

/* 执行aiot_mqtt_recv的线程, 包含网络自动重连和从服务器收取MQTT消息 */
void mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (mqtt_recv_thread_running) {
        res = aiot_mqtt_recv(args);
        if (res < STATE_SUCCESS) {
            if (res == STATE_USER_INPUT_EXEC_DISABLED) {
                break;
            }
            aos_msleep(1000);
        }
    }
    aos_task_exit(0);
    return;
}

/* MQTT默认消息处理回调, 当SDK从服务器收到MQTT消息时, 且无对应用户回调处理时被调用 */
void mqtt_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    void (*callback)(void *userdata) = (void (*)(void *))userdata;

    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            // amp_debug(MOD_STR, "heartbeat response");
            /* TODO: 处理服务器对心跳的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            amp_debug(MOD_STR, "suback, res: -0x%04X, packet id: %d, max qos: %d",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
            /* TODO: 处理服务器对订阅请求的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            // amp_debug(MOD_STR, "pub, qos: %d, topic: %.*s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            // amp_debug(MOD_STR, "pub, payload: %.*s", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
            aos_mqtt_userdata_t *udata = (aos_mqtt_userdata_t *)userdata;
            aos_mqtt_message_t message;
            memset(&message, 0, sizeof(aos_mqtt_message_t));
            if (udata && udata->callback) {
                message.option = AIOT_MQTTOPT_RECV_HANDLER;
                message.recv.type = packet->type;
                message.recv.code = AOS_MQTT_MESSAGE;
                message.recv.topic = __amp_strdup(packet->data.pub.topic, packet->data.pub.topic_len);
                message.recv.payload = __amp_strdup(packet->data.pub.payload, packet->data.pub.payload_len);
                message.recv.topic_len = packet->data.pub.topic_len;
                message.recv.payload_len = packet->data.pub.payload_len;
                udata->callback(&message, udata);
                amp_free(message.recv.topic);
                amp_free(message.recv.payload);
            }
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            amp_debug(MOD_STR, "puback, packet id: %d", packet->data.pub_ack.packet_id);
            /* TODO: 处理服务器对QoS1上报消息的回应, 一般不处理 */
        }
        break;

        default: {

        }
    }
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    aos_mqtt_userdata_t *udata = (aos_mqtt_userdata_t *)userdata;
    aos_mqtt_message_t message;

    memset(&message, 0, sizeof(aos_mqtt_message_t));
    message.option = AIOT_MQTTOPT_EVENT_HANDLER;
    message.event.type = event->type;

    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_CONNECT");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AOS_MQTT_CONNECT;
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_RECONNECT");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AOS_MQTT_RECONNECT;
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            amp_debug(MOD_STR, "AIOT_MQTTEVT_DISCONNECT: %s", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AOS_MQTT_DISCONNECT;
        }
        break;

        default: {
        return;
        }
    }

    if (udata && udata->callback)
        udata->callback(&message, udata);
}

int32_t mqtt_client_start(void **handle, aos_mqtt_userdata_t *userdata)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    aos_mqtt_handle_t *aos_mqtt_handle = (aos_mqtt_handle_t *)userdata->handle;
    char       *host = aos_mqtt_handle->host;
    uint16_t    port = aos_mqtt_handle->port;
    char       *clientid = aos_mqtt_handle->clientid;
    char       *username = aos_mqtt_handle->username;
    char       *password = aos_mqtt_handle->password;
    uint16_t    keepaliveSec = aos_mqtt_handle->keepaliveSec;
    aiot_sysdep_network_cred_t cred;

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    // aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验MQTT服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证MQTT服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证MQTT服务端的RSA根证书长度 */

    /* 创建1个MQTT客户端实例并内部初始化默认参数 */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        amp_error(MOD_STR, "aiot_mqtt_init failed");
        amp_free(mqtt_handle);
        return -1;
    }

    aos_mqtt_handle->mqtt_handle = mqtt_handle;
    /* TODO: 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }

    /* 配置MQTT服务器地址 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    /* 配置MQTT服务器端口 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
    /* 配置设备productKey */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_CLIENTID, (void *)clientid);
    /* 配置设备deviceName */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_USERNAME, (void *)username);
    /* 配置设备deviceSecret */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PASSWORD, (void *)password);
    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED, (void *)&cred);
    /* 配置MQTT心跳间隔 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_KEEPALIVE_SEC, (void *)&keepaliveSec);
    /* 配置回调参数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_USERDATA, userdata);
    /* 配置MQTT默认消息接收回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)mqtt_recv_handler);
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)mqtt_event_handler);

    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        /* 尝试建立连接失败, 销毁MQTT实例, 回收资源 */
        aiot_mqtt_deinit(&mqtt_handle);
        amp_error(MOD_STR, "aos_mqtt_connect failed: -0x%04X", -res);
        aos_task_exit(0);
        return -1;
    }

    /* 创建一个单独的线程, 专用于执行aos_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    mqtt_process_thread_running = 1;
    *handle = mqtt_handle;

    aos_task_t user_mqtt_process_task;

    if (aos_task_new_ext(&user_mqtt_process_task, "user_mqtt_process", mqtt_process_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_error(MOD_STR, "user mqtt process task create failed!");
        aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return NULL;
    }
    amp_debug(MOD_STR, "user mqtt process start");

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    mqtt_recv_thread_running = 1;

    aos_task_t user_mqtt_rec_task;

    if (aos_task_new_ext(&user_mqtt_rec_task, "user_mqtt_recv", mqtt_recv_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_error(MOD_STR, "user mqtt rec task create failed!");
        aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return NULL;
    }
    amp_debug(MOD_STR, "user mqtt recv start");

    return 0;
}

/* mqtt stop */
int32_t mqtt_client_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    mqtt_process_thread_running = 0;
    mqtt_recv_thread_running = 0;

    /* 断开MQTT连接 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        amp_error(MOD_STR, "aiot_mqtt_disconnect failed: -0x%04X", -res);
        return -1;
    }

    /* 销毁MQTT实例 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot_mqtt_deinit failed: -0x%04X", -res);
        return -1;
    }

    return res;
}
