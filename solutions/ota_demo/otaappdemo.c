/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netmgr.h"
#include "ulog/ulog.h"
#include "ota_agent.h"
#include "core_mqtt.h"
#include "netmgr_wifi.h"
#include "aiot_mqtt_api.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include <uservice/eventid.h>
#include <uservice/uservice.h>

#define MY_APP_VER     "app-5.0.0"

static char mqtt_started = 0;
static ota_service_t ctx = {0};

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

static uint8_t g_mqtt_process_thread_running = 0;
static uint8_t g_mqtt_recv_thread_running = 0;

static int ota_upgrade_cb(ota_service_t *pctx, char *ver, char *url)
{
    int ret = -1;
    void *thread = NULL;
    printf("ota version:%s is coming, begin to upgrade...\r\n", ver);
    if ((pctx == NULL) || (ver == NULL) || (url == NULL)) {
        printf("ota upgrade cb param err\r\n");
        return ret;
    }
    // 版本定义和版本比较用户根据需要自定义，此处仅作参考
    if (strncmp(ver, MY_APP_VER, strlen(ver)) <= 0) {
        ret = OTA_TRANSPORT_VER_FAIL;
        printf("ota kernel version too old!\r\n");
        return ret;
    }
    ret = ota_thread_create(&thread, (void *)ota_service_start, (void *)pctx, NULL, 1024 * 6);
    if (ret < 0) {
        printf("ota thread err:%d\r\n", ret);
    }
    return ret;
}

static int user_submodule_upgrade_cb(ota_service_t *pctx, char *ver, char *url)
{
    int ret = -1;
    void *thread = NULL;
    char current_ver[64];
    // 需要用户根据需求自己添加
/*
    memset(current_ver, 0x00, sizeof(current_ver));
    // todo: 子模块版本号填入current_ver
    if((pctx != NULL) && (ver != NULL) && (url != NULL)) {
        ret = 0;
        if(strncmp(ver, current_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "submodule version too old!");
        }
        else {
            ret = ota_thread_create(&thread, (void *)ota_service_submodule_start, (void *)pctx, NULL, 1024 * 6);
            if (ret < 0) {
                printf("ota submodule thread err:%d\r\n", ret);
            }
        }
    }
*/
    printf("sorry, sumodule download function is nothing!\r\n");
    return ret;
}

/* 日志回调函数, SDK的日志会从这里输出 */
int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

static void mqtt_connected_cb(void *handle)
{ 
    int ret = -1;
    char *mqtt_attr = NULL;
    memset(ctx.pk, 0, sizeof(ctx.pk));
    memset(ctx.dn, 0, sizeof(ctx.dn));
    mqtt_attr = core_mqtt_get_product_key(handle);
    if (mqtt_attr != NULL) {
        strcpy(ctx.pk, mqtt_attr);
        mqtt_attr = core_mqtt_get_device_name(handle);
        if(mqtt_attr == NULL) {
            printf("get dn failed\r\n");
        }
    } else {
        printf("get pk failed\r\n");
    }
    if (mqtt_attr != NULL) {
        strcpy(ctx.dn, mqtt_attr);
        ctx.mqtt_client = handle;
        ota_register_cb(&ctx, OTA_CB_ID_UPGRADE, (void *)ota_upgrade_cb);
        ota_register_cb(&ctx, OTA_CB_ID_MODULE_UPGRADE, (void *)user_submodule_upgrade_cb);
        memset(ctx.module_name, 0x00, sizeof(ctx.module_name));
        ret = ota_service_init(&ctx);
        if (ret < 0) {
            LOG("OTA init failed\n");
        } else {
            LOG("OTA init successfully\n");
        }
        // 上报app的版本号，对应云端的default模块
        ret = ota_transport_inform(&ctx, NULL, MY_APP_VER);
        if (ret < STATE_SUCCESS) {
            printf("user app report ver failed!\r\n");
        }
        // 多模块情况下，USER_MODE_NAME 为用户的模块名， USER_MODULE_VER为用户模块的版本号，此用例只有一个模块
/*
        ret = ota_transport_inform(&ctx, USER_MODE_NAME, USER_MODULE_VER);
        if(ret < STATE_SUCCESS) {
            printf("user module ver report failed!\r\n");
        }
*/
    }
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            printf("AIOT_MQTTEVT_CONNECT\n");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
            mqtt_connected_cb(handle);
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            printf("AIOT_MQTTEVT_RECONNECT\n");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            printf("AIOT_MQTTEVT_DISCONNECT: %s\n", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        default: {

        }
    }
}

/* MQTT默认消息处理回调, 当SDK从服务器收到MQTT消息时, 且无对应用户回调处理时被调用 */
void demo_mqtt_default_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            printf("heartbeat response\n");
            /* TODO: 处理服务器对心跳的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            printf("suback, res: -0x%04X, packet id: %d, max qos: %d\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
            /* TODO: 处理服务器对订阅请求的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            printf("pub, qos: %d, topic: %.*s\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            printf("pub, payload: %.*s\n", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            printf("puback, packet id: %d\n", packet->data.pub_ack.packet_id);
            /* TODO: 处理服务器对QoS1上报消息的回应, 一般不处理 */
        }
        break;

        default: {

        }
    }
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
void *demo_mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_mqtt_process_thread_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        aos_msleep(1000);
    }
    return NULL;
}

/* 执行aiot_mqtt_recv的线程, 包含网络自动重连和从服务器收取MQTT消息 */
void *demo_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_mqtt_recv_thread_running) {
        res = aiot_mqtt_recv(args);
        if (res < STATE_SUCCESS) {
            if (res == STATE_USER_INPUT_EXEC_DISABLED) {
                break;
            }
            aos_msleep(1000);
        }
    }
    return NULL;
}


int mqtt_main(void *param)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a13FN5TplKq";
    char *device_name       = "mqtt_basic_demo";
    char *device_secret     = "jA0K15GobTDa5wgOtJPzdtcZPc4X7NYQ";

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

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
        printf("aiot_mqtt_init failed\n");
        return -1;
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
    /* 配置MQTT默认消息接收回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)demo_mqtt_default_recv_handler);
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)demo_mqtt_event_handler);
    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        /* 尝试建立连接失败, 销毁MQTT实例, 回收资源 */
        aiot_mqtt_deinit(&mqtt_handle);
        printf("aiot_mqtt_connect failed: -0x%04X\n", -res);
        return -1;
    }
    /* 创建一个单独的线程, 专用于执行aiot_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    g_mqtt_process_thread_running = 1;
    res = aos_task_new("demo_mqtt_process", demo_mqtt_process_thread, mqtt_handle, 4096);
    if (res != 0) {
        printf("create demo_mqtt_process_thread failed: %d\n", res);
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = aos_task_new("demo_mqtt_recv", demo_mqtt_recv_thread, mqtt_handle, 4096);
    if (res != 0) {
        printf("create demo_mqtt_recv_thread failed: %d\n", res);
        return -1;
    }
    /* 主循环进入休眠 */
    while (1) {
        aos_msleep(1000);
    }

    /* 断开MQTT连接, 一般不会运行到这里 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        printf("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 销毁MQTT实例, 一般不会运行到这里 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_mqtt_deinit failed: -0x%04X\n", -res);
        return -1;
    }

    g_mqtt_process_thread_running = 0;
    g_mqtt_recv_thread_running = 0;

    return 0;
}

static void wifi_service_event(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_NETMGR_DHCP_SUCCESS) {
        return;
    }
    if (!mqtt_started) {
        aos_task_new("iotx_example", mqtt_main, (void *)NULL, 1024 * 8);
        mqtt_started = 1;
    }
}

int application_start(int argc, char *argv[])
{
    ulog_init();
    event_service_init(NULL);
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_service_init(NULL);
    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_service_event, NULL);
    while (1) {
        aos_msleep(1000);
    }
    return 0;
}
