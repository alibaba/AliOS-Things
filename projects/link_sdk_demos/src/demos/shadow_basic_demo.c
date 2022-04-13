/*
 * 这个例程适用于`Linux`这类支持pthread的POSIX设备, 它演示了用SDK配置MQTT参数并建立连接, 之后创建2个线程
 *
 * + 一个线程用于保活长连接
 * + 一个线程用于接收消息, 并在有消息到达时进入默认的数据回调, 在连接状态变化时进入事件回调
 *
 * 接着演示了在MQTT连接上进行设备影子更新, 删除, 拉取等操作, 取消这些代码段落的注释即可观察运行效果
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_shadow_api.h"

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
 * 例如: [1577589489.033][LK-0317] shadow_basic_demo&a13FN5TplKq
 *
 * 上面这条日志的code就是0317(十六进制), code值的定义见core/aiot_state_api.h
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

/* 数据处理回调, 当SDK从网络上收到shadow消息时被调用 */
void demo_shadow_recv_handler(void *handle, const aiot_shadow_recv_t *recv, void *userdata)
{
    printf("demo_shadow_recv_handler, type = %d, productKey = %s, deviceName = %s\r\n",
            recv->type, recv->product_key, recv->device_name);

    switch (recv->type) {
        /* 当设备发送AIOT_SHADOWMSG_UPDATE, AIOT_SHADOWMSG_CLEAN_DESIRED或者AIOT_SHADOWMSG_DELETE_REPORTED消息后, 会收到此应答消息 */
        case AIOT_SHADOWRECV_GENERIC_REPLY: {
            const aiot_shadow_recv_generic_reply_t *generic_reply = &recv->data.generic_reply;

            printf("payload = \"%.*s\", status = %s, timestamp = %ld\r\n",
                   generic_reply->payload_len,
                   generic_reply->payload,
                   generic_reply->status,
                   (unsigned long)generic_reply->timestamp);
        }
        break;
        /* 当设备在线时, 若用户应用程序调用云端API主动更新设备影子, 设备便会收到此消息 */
        case AIOT_SHADOWRECV_CONTROL: {
            const aiot_shadow_recv_control_t *control = &recv->data.control;

            printf("payload = \"%.*s\", version = %ld\r\n",
                   control->payload_len,
                   control->payload,
                   (unsigned long)control->version);
        }
        break;
        /* 当设备发送AIOT_SHADOWMSG_GET消息主动获取设备影子时, 会收到此消息 */
        case AIOT_SHADOWRECV_GET_REPLY: {
            const aiot_shadow_recv_get_reply_t *get_reply = &recv->data.get_reply;

            printf("payload = \"%.*s\", version = %ld\r\n",
                   get_reply->payload_len,
                   get_reply->payload,
                   (unsigned long)get_reply->version);
        }
        default:
            break;
    }
}

/* 发送更新设备影子reported值的请求 */
int32_t demo_update_shadow(void *shadow_handle, char *reported_data, int64_t version)
{
    aiot_shadow_msg_t message;

    memset(&message, 0, sizeof(aiot_shadow_msg_t));
    message.type = AIOT_SHADOWMSG_UPDATE;
    message.data.update.reported = reported_data;
    message.data.update.version = version;

    return aiot_shadow_send(shadow_handle, &message);
}

/* 发送删除设备影子中特定reported值的请求 */
int32_t demo_delete_shadow_report(void *shadow_handle, char *reported, int64_t version)
{
    aiot_shadow_msg_t message;

    memset(&message, 0, sizeof(aiot_shadow_msg_t));
    message.type = AIOT_SHADOWMSG_DELETE_REPORTED;
    message.data.delete_reporte.reported = reported;
    message.data.delete_reporte.version = version;

    return aiot_shadow_send(shadow_handle, &message);
}

/* 发送清除设备影子中所有desired值的请求 */
int32_t demo_clean_shadow_desired(void *shadow_handle, int64_t version)
{
    aiot_shadow_msg_t message;

    memset(&message, 0, sizeof(aiot_shadow_msg_t));
    message.type = AIOT_SHADOWMSG_CLEAN_DESIRED;
    message.data.clean_desired.version = version;

    return aiot_shadow_send(shadow_handle, &message);
}

/* 发送获取设备影子的请求 */
int32_t demo_get_shadow(void *shadow_handle)
{
    aiot_shadow_msg_t message;

    memset(&message, 0, sizeof(aiot_shadow_msg_t));
    message.type = AIOT_SHADOWMSG_GET;

    return aiot_shadow_send(shadow_handle, &message);
}


int demo_main(int argc, char *argv[])
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    void       *shadow_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 1883;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a18wPzZJzNG";
    char *device_name       = "aiot_shadow_test_case_01";
    char *device_secret     = "4QmldLFR7XgtksiSFeEVan4BnWSfF7Y7";

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
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)demo_mqtt_event_handler);

    /* 创建1个shadow客户端实例并内部初始化默认参数 */
    shadow_handle = aiot_shadow_init();
    if (shadow_handle == NULL) {
        printf("aiot_shadow_init failed\n");
        return -1;
    }

    /* 配置MQTT实例句柄 */
    aiot_shadow_setopt(shadow_handle, AIOT_SHADOWOPT_MQTT_HANDLE, mqtt_handle);
    /* 配置SHADOW默认消息接收回调函数 */
    aiot_shadow_setopt(shadow_handle, AIOT_SHADOWOPT_RECV_HANDLER, (void *)demo_shadow_recv_handler);

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
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = pthread_create(&g_mqtt_recv_thread, NULL, demo_mqtt_recv_thread, mqtt_handle);
    if (res < 0) {
        printf("pthread_create demo_mqtt_recv_thread failed: %d\n", res);
        return -1;
    }

    /* 主循环进入休眠 */
    while (1) {
        /* TODO: 以下代码演示了发送请求拉取设备影子, 设备影子将在demo_shadow_recv_handler回调函数中返回, 用户可取消注释观察演示效果 */

        res = demo_get_shadow(shadow_handle);
        if (res < 0) {
            printf("demo_get_shadow failed, res = -0x%04x\r\n", -res);
        }
        sleep(2);

        /* TODO: 以下代码演示了如何清除设备影子中的desired值, 同时将version设置为1, 用户可取消注释观察演示效果 */

        res = demo_clean_shadow_desired(shadow_handle, 1);
        if (res < 0) {
            printf("demo_clean_shadow_desired failed, res = -0x%04x\r\n", -res);
        }
        sleep(2);

        /* TODO: 以下代码演示了如何删除设备影子中特定的reported值, 同时将version设置为2, 用户可取消注释观察演示效果 */

        res = demo_delete_shadow_report(shadow_handle, "{\"LightSwitch\":\"null\"}", 2);
        if (res < 0) {
            printf("demo_delete_shadow_report failed, res = -0x%04x\r\n", -res);
        }

        sleep(2);

        /* TODO: 以下代码演示了如何更新设备影子中的reported值, 同时将version重置为0, 用户可取消注释观察演示效果 */

        res = demo_update_shadow(shadow_handle, "{\"LightSwitch\":1}", 0);
        if (res < 0) {
            printf("demo_delete_shadow_report failed, res = -0x%04x\r\n", -res);
        }

        sleep(10);
    }

    /* 断开MQTT连接, 一般不会运行到这里 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        printf("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 销毁SHADOW实例, 一般不会运行到这里 */
    res = aiot_shadow_deinit(&shadow_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_shadow_deinit failed: -0x%04X\n", -res);
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
    pthread_join(g_mqtt_process_thread, NULL);
    pthread_join(g_mqtt_recv_thread, NULL);

    return 0;
}

