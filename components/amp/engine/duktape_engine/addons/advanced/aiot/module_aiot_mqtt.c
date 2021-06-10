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
#include "aiot_dm_api.h"
#include "aiot_subdev_api.h"
#include "be_inl.h"
#include "module_aiot.h"
#ifdef AOS_COMP_UAGENT
#include "uagent.h"
#endif
#define MOD_STR "AIOT_MQTT"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

uint8_t g_app_mqtt_process_thread_running = 0;
uint8_t g_app_mqtt_recv_thread_running = 0;

static char *__amp_strdup(char *src, int len)
{
    char   *dst;

    if (src == NULL) {
        return NULL;
    }

    dst = aos_malloc(len+1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
void *aiot_app_mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_app_mqtt_process_thread_running) {
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
void *aiot_app_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_app_mqtt_recv_thread_running) {
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
void aiot_app_mqtt_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
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
            //amp_debug(MOD_STR, "pub, qos: %d, topic: %.*s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            //amp_debug(MOD_STR, "pub, payload: %.*s", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
            iot_mqtt_userdata_t *udata = (iot_mqtt_userdata_t *)userdata;
            iot_mqtt_message_t message;
            memset(&message, 0, sizeof(iot_mqtt_message_t));
            if (udata && udata->callback) {
                message.option = AIOT_MQTTOPT_RECV_HANDLER;
                message.recv.type = packet->type;
                message.recv.code = AIOT_MQTT_MESSAGE;
                message.recv.topic = __amp_strdup(packet->data.pub.topic, packet->data.pub.topic_len);
                message.recv.payload = __amp_strdup(packet->data.pub.payload, packet->data.pub.payload_len);
                message.recv.topic_len = packet->data.pub.topic_len;
                message.recv.payload_len = packet->data.pub.payload_len;
                udata->callback(&message, udata);
                aos_free(message.recv.topic);
                aos_free(message.recv.payload);
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
void aiot_app_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    iot_mqtt_userdata_t *udata = (iot_mqtt_userdata_t *)userdata;
    iot_mqtt_message_t message;

    memset(&message, 0, sizeof(iot_mqtt_message_t));
    message.option = AIOT_MQTTOPT_EVENT_HANDLER;
    message.event.type = event->type;

    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_CONNECT");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AIOT_MQTT_CONNECT;
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            amp_debug(MOD_STR, "AIOT_MQTTEVT_RECONNECT");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AIOT_MQTT_RECONNECT;
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            amp_debug(MOD_STR, "AIOT_MQTTEVT_DISCONNECT: %s", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
            message.event.code = AIOT_MQTT_DISCONNECT;
        }
        break;

        default: {
        return;
        }
    }

    if (udata && udata->callback)
        udata->callback(&message, udata);
}

/* 属性上报函数演示 */
int32_t aiot_app_send_property_post(void *dm_handle, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_PROPERTY_POST;
    msg.data.property_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}

/* 事件上报函数演示 */
int32_t aiot_app_send_event_post(void *dm_handle, char *event_id, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_EVENT_POST;
    msg.data.event_post.event_id = event_id;
    msg.data.event_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}

int32_t aiot_mqtt_client_start(void **handle, int keepaliveSec, iot_mqtt_userdata_t *userdata)
{
    int32_t res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* get device tripple info */
    char product_key[IOTX_PRODUCT_KEY_LEN] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN] = {0};

    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;
    int devicesecret_len = IOTX_DEVICE_SECRET_LEN;

    aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, product_key, &productkey_len);
    aos_kv_get(AMP_CUSTOMER_DEVICENAME, device_name, &devicename_len);
    aos_kv_get(AMP_CUSTOMER_DEVICESECRET, device_secret, &devicesecret_len);
    /* end get device tripple info */

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
        return -1;
    }

    /* TODO: 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }

    snprintf(host, 100, "%s.%s", product_key, url);
    /* 配置MQTT服务器地址 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    /* 配置MQTT服务器端口 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
    /* 配置设备productKey */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PRODUCT_KEY, (void *)product_key);
    /* 配置设备deviceName */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_NAME, (void *)device_name);
    /* 配置设备deviceSecret */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_SECRET, (void *)device_secret);
    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED, (void *)&cred);
    /* 配置MQTT心跳间隔 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_KEEPALIVE_SEC, (void *)&keepaliveSec);
    /* 配置回调参数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_USERDATA, userdata);
    /* 配置MQTT默认消息接收回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)aiot_app_mqtt_recv_handler);
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)aiot_app_mqtt_event_handler);

    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS)
    {
        /* 尝试建立连接失败, 销毁MQTT实例, 回收资源 */
        aiot_mqtt_deinit(&mqtt_handle);
        amp_debug(MOD_STR, "aiot_mqtt_connect failed: -0x%04X", -res);
        aos_task_exit(0);
        return -1;
    }

    /* 创建一个单独的线程, 专用于执行aiot_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    g_app_mqtt_process_thread_running = 1;

    aos_task_t mqtt_process_task;

    if (aos_task_new_ext(&mqtt_process_task, "mqtt_process", aiot_app_mqtt_process_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_debug(MOD_STR, "management mqtt process task create failed!");
        aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return -1;
    }
    amp_debug(MOD_STR, "app mqtt process start");

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_app_mqtt_recv_thread_running = 1;

    aos_task_t mqtt_rec_task;

    if (aos_task_new_ext(&mqtt_rec_task, "mqtt_rec", aiot_app_mqtt_recv_thread, mqtt_handle, 1024 * 4, AOS_DEFAULT_APP_PRI) != 0) {
        amp_debug(MOD_STR, "management mqtt rec task create failed!");
        aiot_mqtt_deinit(&mqtt_handle);
        aos_task_exit(0);
        return -1;
    }
    amp_debug(MOD_STR, "app mqtt rec start");

    *handle = mqtt_handle;
 #ifdef AOS_COMP_UAGENT
    res = uagent_mqtt_client_set(mqtt_handle);
    if (res != 0) {
        amp_debug(MOD_STR, "uAgent mqtt handle set failed ret = %d\n", res);
    }

    res = uagent_ext_comm_start(product_key, device_name);
    if (res != 0) {
        amp_debug(MOD_STR, "uAgent ext comm  start failed ret = %d\n", res);
    }
#endif   

    return 0;
}

/* mqtt stop */
int32_t aiot_mqtt_client_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_app_mqtt_process_thread_running = 0;
    g_app_mqtt_recv_thread_running = 0;

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
