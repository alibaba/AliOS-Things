/* 这个例程演示了用SDK配置Bootstrap会话实例的参数, 并发起请求和接收应答, 之后
 *
 * + 如果接收应答失败了, 销毁实例, 回收资源, 结束程序退出
 * + 如果接收应答成功, 在`demo_bootstrap_recv_handler()`的应答处理回调函数中, 演示解析获取服务端应答的内容
 *
 * 需要用户关注或修改的部分, 已用 `TODO` 在注释中标明
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_bootstrap_api.h"
#include "aiot_mqtt_api.h"

typedef struct {
    uint32_t code;
    char *host;
    uint16_t port;
} demo_info_t;

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

static pthread_t g_mqtt_process_thread;
static pthread_t g_mqtt_recv_thread;
static uint8_t g_mqtt_process_thread_running = 0;
static uint8_t g_mqtt_recv_thread_running = 0;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1580829318.855][LK-040B] > POST /auth/bootstrap HTTP/1.1
 *
 * 上面这条日志的code就是040B(十六进制), code值的定义见components/bootstrap/aiot_bootstrap_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            printf("AIOT_MQTTEVT_CONNECT\n");
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            printf("AIOT_MQTTEVT_RECONNECT\n");
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            printf("AIOT_MQTTEVT_DISCONNECT: %s\n", cause);
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
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            printf("suback, res: -0x%04X, packet id: %d, max qos: %d\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            printf("pub, qos: %d, topic: %.*s\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            printf("pub, payload: %.*s\n", packet->data.pub.payload_len, packet->data.pub.payload);
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            printf("puback, packet id: %d\n", packet->data.pub_ack.packet_id);
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
        sleep(1);
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
            sleep(1);
        }
    }
    return NULL;
}

int32_t demo_mqtt_start(void *mqtt_handle, char *product_key, char *device_name, char *device_secret, char *host, uint16_t port)
{
    int32_t     res = STATE_SUCCESS;
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验MQTT服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证MQTT服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证MQTT服务端的RSA根证书长度 */

    /* TODO: 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */
    /*
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    */

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
    res = pthread_create(&g_mqtt_process_thread, NULL, demo_mqtt_process_thread, mqtt_handle);
    if (res < 0) {
        printf("pthread_create demo_mqtt_process_thread failed: %d\n", res);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = pthread_create(&g_mqtt_recv_thread, NULL, demo_mqtt_recv_thread, mqtt_handle);
    if (res < 0) {
        printf("pthread_create demo_mqtt_recv_thread failed: %d\n", res);
        g_mqtt_process_thread_running = 0;
        pthread_join(g_mqtt_process_thread, NULL);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    return 0;
}

int32_t demo_mqtt_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_mqtt_process_thread_running = 0;
    g_mqtt_recv_thread_running = 0;
    pthread_join(g_mqtt_process_thread, NULL);
    pthread_join(g_mqtt_recv_thread, NULL);

    /* 断开MQTT连接 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        printf("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 销毁MQTT实例 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_mqtt_deinit failed: -0x%04X\n", -res);
        return -1;
    }

    return 0;
}

/* Bootstrap事件回调函数, 当SDK发生内部事件时被调用 */
void demo_bootstrap_event_handler(void *handle, const aiot_bootstrap_event_t *event, void *userdata)
{
    switch (event->type) {
        case AIOT_BOOTSTRAPEVT_INVALID_RESPONSE: {
            printf("AIOT_BOOTSTRAPEVT_INVALID_RESPONSE\n");
        }
        break;
        case AIOT_BOOTSTRAPEVT_INVALID_CMD: {
            printf("AIOT_BOOTSTRAPEVT_INVALID_CMD\n");
        }
        break;
        default: {

        }
        break;
    }
}

/* Bootstrap接收消息处理回调, 当SDK从服务器收到Bootstrap消息时被调用 */
void demo_bootstrap_recv_handler(void *handle, const aiot_bootstrap_recv_t *packet, void *userdata)
{
    demo_info_t *demo_info = (demo_info_t *)userdata;

    switch (packet->type) {
        case AIOT_BOOTSTRAPRECV_STATUS_CODE: {
            demo_info->code = packet->data.status_code.code;
        }
        break;
        case AIOT_BOOTSTRAPRECV_CONNECTION_INFO: {
            demo_info->host = malloc(strlen(packet->data.connection_info.host) + 1);
            if (demo_info->host != NULL) {
                memset(demo_info->host, 0, strlen(packet->data.connection_info.host) + 1);
                /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
                memcpy(demo_info->host, packet->data.connection_info.host, strlen(packet->data.connection_info.host));
                demo_info->port = packet->data.connection_info.port;
            }
        }
        break;
        case AIOT_BOOTSTRAPRECV_NOTIFY: {
            printf("AIOT_BOOTSTRAPRECV_NOTIFY, cmd: %d\n", packet->data.notify.cmd);

        }
        default: {

        }
        break;
    }
}

int demo_main(int argc, char *argv[])
{
    int32_t res = STATE_SUCCESS;
    void *bootstrap_handle = NULL, *mqtt_handle = NULL;
    aiot_sysdep_network_cred_t cred;
    demo_info_t demo_info;

    /* TODO: 替换为自己设备的productKey和deviceName */
    char *product_key       = "a13FN5TplKq";
    char *device_name       = "bootstrap_posix_demo";
    char *device_secret     = "pZeRIoLApMRp2i1LAD4D5FREp3yPVBwy";

    memset(&demo_info, 0, sizeof(demo_info_t));

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

    /* 创建1个Bootstrap会话实例并内部初始化默认参数 */
    bootstrap_handle =  aiot_bootstrap_init();
    if (bootstrap_handle == NULL) {
        printf("aiot_bootstrap_init failed\n");
        return -1;
    }

    /* 配置Bootstrap会话实例
     *
     * 配置项详细说明可参考: http://gaic.alicdn.com/ztms/linkkit/html/aiot__bootstrap__api_8h.html#a9cc177610fa0842f8287ed4fa438d56d
     *
     */
    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_PRODUCT_KEY, (void *)product_key);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_PRODUCT_KEY failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_DEVICE_NAME, (void *)device_name);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_DEVICE_NAME failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_NETWORK_CRED, (void *)&cred);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_NETWORK_CRED failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_RECV_HANDLER, (void *)demo_bootstrap_recv_handler);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_RECV_HANDLER failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_EVENT_HANDLER, (void *)demo_bootstrap_event_handler);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_EVENT_HANDLER failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_USERDATA, (void *)&demo_info);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_USERDATA failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        return -1;
    }

    /* 发送Bootstrap服务请求 */
    res = aiot_bootstrap_send_request(bootstrap_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_send_request failed, res: -0x%04X\n", -res);
        return -1;
    }

    /* 接收Bootstrap服务应答 */
    res = aiot_bootstrap_recv(bootstrap_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_recv failed, res: -0x%04X\n", -res);
        return -1;
    }

    printf("status code: %d\n", demo_info.code);

    /* 把服务应答中的MQTT域名地址和端口号打印出来 */
    if (demo_info.host != NULL) {
        printf("host: %s, port: %d\n", demo_info.host, demo_info.port);
    }else{
        return -1;
    }

    /* 创建1个MQTT客户端实例并内部初始化默认参数 */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        printf("aiot_mqtt_init failed\n");
        return -1;
    }

    /* 建立MQTT连接, 并开启保活线程和接收线程 */
    res = demo_mqtt_start(mqtt_handle, product_key, device_name, device_secret, demo_info.host, demo_info.port);
    free(demo_info.host);
    if (res < 0) {
        printf("demo_mqtt_start failed\n");
        return -1;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_MQTT_HANDLE failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        demo_mqtt_stop(&mqtt_handle);
        return -1;
    }

    while(1) {
        sleep(1);
    }

    /* 销毁Bootstrap会话实例, 一般不会运行到这里 */
    res = aiot_bootstrap_deinit(&bootstrap_handle);
    if (res < 0) {
        printf("demo_start_stop failed\n");
        return -1;
    }

    /* 销毁MQTT实例, 退出线程, 一般不会运行到这里 */
    res = demo_mqtt_stop(&mqtt_handle);
    if (res < 0) {
        printf("demo_start_stop failed\n");
        return -1;
    }

    return 0;
}

