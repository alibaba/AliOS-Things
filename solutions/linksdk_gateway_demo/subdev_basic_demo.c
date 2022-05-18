/*
 * 这个例程演示了用SDK代理子设备，之后创建2个线程
 *
 * + 一个线程用于保活长连接
 * + 一个线程用于接收消息, 并在有消息到达时进入默认的数据回调, 在连接状态变化时进入事件回调
 *
 * 接着演示了在MQTT连接上进行属性上报, 事件上报, 以及处理收到的属性设置, 服务调用, 取消这些代码段落的注释即可观察运行效果
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <aos/kernel.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_subdev_api.h"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

static uint8_t g_mqtt_process_thread_running = 0;
static uint8_t g_mqtt_recv_thread_running = 0;

aiot_subdev_dev_t g_subdev[] = {
    {
        "a13FN5TplKq",
        "subdev_basic_demo_01",
        "768XBgQwgOakz3K4uhOiLeeh9xjJQx6h",
        "y7GSILD480lBSsP8"
    },
    {
        "a13FN5TplKq",
        "subdev_basic_demo_02",
        "iwTZrbjbgNVChfuJkihjE5asekoyKoYv",
        "y7GSILD480lBSsP8"
    },
    {
        "a13FN5TplKq",
        "subdev_basic_demo_03",
        "fdutq35iKMYdcWWBuIINY26hsNhgFXWE",
        "y7GSILD480lBSsP8"
    },
    {
        "a13FN5TplKq",
        "subdev_basic_demo_04",
        "HCKv50YqgwdKhy5cE0Vz4aydmK2ojPvr",
        "y7GSILD480lBSsP8"
    }
};

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1577589489.033][LK-0317] subdev_basic_demo&a13FN5TplKq
 *
 * 上面这条日志的code就是0317(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
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
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
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

int32_t demo_mqtt_start(void **handle)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a1tmc66UyDK";
    char *device_name       = "subdev_basic_demo";
    char *device_secret     = "awAlkVEzZm40nk4EbMfcUVRwDnnClVVu";

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

    /* TODO: 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */
    /*
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    */

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
    // res = pthread_create(&g_mqtt_process_thread, NULL, demo_mqtt_process_thread, mqtt_handle);
    if (res != 0) {
        printf("create demo_mqtt_process_thread failed: %d\n", res);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = aos_task_new("demo_mqtt_process", demo_mqtt_recv_thread, mqtt_handle, 4096);
    // res = pthread_create(&g_mqtt_recv_thread, NULL, demo_mqtt_recv_thread, mqtt_handle);
    if (res != 0) {
        printf("create demo_mqtt_recv_thread failed: %d\n", res);
         g_mqtt_process_thread_running = 0;
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    *handle = mqtt_handle;

    return 0;
}

int32_t demo_mqtt_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_mqtt_process_thread_running = 0;
    g_mqtt_recv_thread_running = 0;

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

void demo_subdev_recv_handler(void *handle, const aiot_subdev_recv_t *packet, void *user_data)
{
    switch (packet->type) {
        case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
        case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
        case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
        case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
        case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
        case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
        case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY: {
            printf("msgid        : %d\n", packet->data.generic_reply.msg_id);
            printf("code         : %d\n", packet->data.generic_reply.code);
            printf("product key  : %s\n", packet->data.generic_reply.product_key);
            printf("device name  : %s\n", packet->data.generic_reply.device_name);
            printf("message      : %s\n", (packet->data.generic_reply.message == NULL) ? ("NULL") : (packet->data.generic_reply.message));
            printf("data         : %s\n", packet->data.generic_reply.data);
        }
        break;
        case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY: {
            printf("msgid        : %d\n", packet->data.generic_notify.msg_id);
            printf("product key  : %s\n", packet->data.generic_notify.product_key);
            printf("device name  : %s\n", packet->data.generic_notify.device_name);
            printf("params       : %s\n", packet->data.generic_notify.params);
        }
        break;
        default: {

        }
    }
}

int demo_main(int argc, char *argv[])
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL, *subdev_handle = NULL;

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    res = demo_mqtt_start(&mqtt_handle);
    if (res < 0) {
        printf("demo_mqtt_start failed\n");
        return -1;
    }

    subdev_handle = aiot_subdev_init();
    if (subdev_handle == NULL) {
        printf("aiot_subdev_init failed\n");
        demo_mqtt_stop(&mqtt_handle);
        return -1;
    }

    aiot_subdev_setopt(subdev_handle, AIOT_SUBDEVOPT_MQTT_HANDLE, mqtt_handle);
    aiot_subdev_setopt(subdev_handle, AIOT_SUBDEVOPT_RECV_HANDLER, demo_subdev_recv_handler);

    res = aiot_subdev_send_topo_add(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    if (res < STATE_SUCCESS) {
        printf("aiot_subdev_send_topo_add failed, res: -0x%04X\n", -res);
        aiot_subdev_deinit(&subdev_handle);
        demo_mqtt_stop(&mqtt_handle);
        return -1;
    }

    aos_msleep(1000);

    // aiot_subdev_send_topo_delete(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    // if (res < STATE_SUCCESS) {
    //     printf("aiot_subdev_send_topo_delete failed, res: -0x%04X\n", -res);
    //     aiot_subdev_deinit(&subdev_handle);
    //     demo_mqtt_stop(&mqtt_handle);
    //     return -1;
    // }

    // aos_msleep(1000);

    // aiot_subdev_send_topo_get(subdev_handle);
    // if (res < STATE_SUCCESS) {
    //     printf("aiot_subdev_send_topo_get failed, res: -0x%04X\n", -res);
    //     aiot_subdev_deinit(&subdev_handle);
    //     demo_mqtt_stop(&mqtt_handle);
    //     return -1;
    // }

    // aos_msleep(1000);

    // aiot_subdev_send_sub_register(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    // if (res < STATE_SUCCESS) {
    //     printf("aiot_subdev_send_sub_register failed, res: -0x%04X\n", -res);
    //     aiot_subdev_deinit(&subdev_handle);
    //     demo_mqtt_stop(&mqtt_handle);
    //     return -1;
    // }

    // aos_msleep(1000);

    // aiot_subdev_send_product_register(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    // if (res < STATE_SUCCESS) {
    //     printf("aiot_subdev_send_product_register failed, res: -0x%04X\n", -res);
    //     aiot_subdev_deinit(&subdev_handle);
    //     demo_mqtt_stop(&mqtt_handle);
    //     return -1;
    // }

    // aos_msleep(1000);

    aiot_subdev_send_batch_login(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    if (res < STATE_SUCCESS) {
        printf("aiot_subdev_send_batch_login failed, res: -0x%04X\n", -res);
        aiot_subdev_deinit(&subdev_handle);
        demo_mqtt_stop(&mqtt_handle);
        return -1;
    }

    aos_msleep(1000);

    // aiot_subdev_send_batch_logout(subdev_handle, g_subdev, sizeof(g_subdev) / sizeof(aiot_subdev_dev_t));
    // if (res < STATE_SUCCESS) {
    //     printf("aiot_subdev_send_batch_logout failed, res: -0x%04X\n", -res);
    //     aiot_subdev_deinit(&subdev_handle);
    //     demo_mqtt_stop(&mqtt_handle);
    //     return -1;
    // }

    while (1)
        aos_msleep(1000);

    res = aiot_subdev_deinit(&subdev_handle);
    if (res < STATE_SUCCESS)
        printf("aiot_subdev_deinit failed: -0x%04X\n", res);

    res = demo_mqtt_stop(&mqtt_handle);
    if (res < 0) {
        printf("demo_start_stop failed\n");
        return -1;
    }

    return 0;
}

