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
#include "be_inl.h"
#include "module_mqtt.h"

#define MOD_STR "MQTT"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

uint8_t mqtt_process_thread_running = 0;
uint8_t mqtt_recv_thread_running = 0;

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
            amp_debug(MOD_STR, "pub, qos: %d, topic: %.*s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            amp_debug(MOD_STR, "pub, payload: %.*s", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
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
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_CONNECT");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
                int js_cb_ref = (int *)userdata;
                amp_debug(MOD_STR, "js cb ref is: %d", js_cb_ref);
                duk_context *ctx = be_get_context();
                be_push_ref(ctx, js_cb_ref);
                duk_push_int(ctx, 9);
                if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                    amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
                }
                duk_pop(ctx);

                duk_gc(ctx, 0);
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_RECONNECT");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            amp_debug(MOD_STR, "AIOT_MQTTEVT_DISCONNECT: %s", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        default: {

        }
    }
}

int32_t mqtt_client_start(void **handle, amp_mqtt_params_t *mqtt_params)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *host = mqtt_params->host;
    uint16_t    port = mqtt_params->port;
    char       *clientid = mqtt_params->clientid;
    char       *username = mqtt_params->username;
    char       *password = mqtt_params->password;
    uint16_t    keepaliveSec = mqtt_params->keepaliveSec;
    int         js_cb_ref = mqtt_params->js_cb_ref;
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
        amp_debug(MOD_STR, "aiot_mqtt_init failed");
        aos_free(mqtt_handle);
        return NULL;
    }

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
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_USERDATA, js_cb_ref);
    /* 配置MQTT默认消息接收回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)mqtt_recv_handler);
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)mqtt_event_handler);

    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS)
    {
        /* 尝试建立连接失败, 销毁MQTT实例, 回收资源 */
        aiot_mqtt_deinit(&mqtt_handle);
        amp_debug(MOD_STR, "aiot_mqtt_connect failed: -0x%04X", -res);
        aos_task_exit(0);
        return NULL;
    }

    /* 创建一个单独的线程, 专用于执行aiot_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    mqtt_process_thread_running = 1;

    aos_task_t mqtt_process_task;

    if (aos_task_new_ext(&mqtt_process_task, "mqtt_process", mqtt_process_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_debug(MOD_STR, "management mqtt process task create failed!");
		aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return NULL;
    }
    amp_debug(MOD_STR, "app mqtt process start");

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    mqtt_recv_thread_running = 1;

    aos_task_t mqtt_rec_task;

    if (aos_task_new_ext(&mqtt_rec_task, "mqtt_rec", mqtt_recv_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_debug(MOD_STR, "management mqtt rec task create failed!");
		aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return NULL;
    }
    amp_debug(MOD_STR, "app mqtt rec start");

    *handle = mqtt_handle;

    return res;
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
        amp_debug(MOD_STR, "aiot_mqtt_disconnect failed: -0x%04X", -res);
        return -1;
    }

    /* 销毁MQTT实例 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_mqtt_deinit failed: -0x%04X", -res);
        return -1;
    }

    return res;
}
