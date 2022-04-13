/* 这个例程演示了用SDK配置动态注册会话实例的参数, 并发起请求和接收应答, 之后
 *
 * + 如果接收应答失败了, 销毁实例, 回收资源, 结束程序退出
 * + 如果接收应答成功, 在`demo_dynregmq_recv_handler()`的应答处理回调函数中, 演示解析获取服务端应答的内容
 *
 * 需要用户关注或修改的部分, 已用 `TODO` 在注释中标明
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_dynregmq_api.h"

/* 白名单模式下用于保存deviceSecret的结构体定义 */
typedef struct {
    char device_secret[64];
} demo_devinfo_wl_t;

/* 免白名单模式下用于保存mqtt建连信息clientid, username和password的结构体定义 */
typedef struct {
    char conn_clientid[128];
    char conn_username[128];
    char conn_password[64];
} demo_devinfo_nwl_t;

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

/* 用户保存白名单模式动态注册, 服务器返回的deviceSecret */
static demo_devinfo_wl_t demo_devinfo_wl;

/* 用户保存免白名单模式动态注册, 服务器返回的mqtt建连信息 */
static demo_devinfo_nwl_t demo_devinfo_nwl;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1580995015.811][LK-040B] > POST /auth/register/device HTTP/1.1
 *
 * 上面这条日志的code就是040B(十六进制), code值的定义见components/dynregmq/aiot_dynregmq_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

/* 数据处理回调, 当SDK从网络上收到dynregmq消息时被调用 */
void demo_dynregmq_recv_handler(void *handle, const aiot_dynregmq_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
        case AIOT_DYNREGMQRECV_DEVICEINFO_WL: {
            if (strlen(packet->data.deviceinfo_wl.device_secret) >= sizeof(demo_devinfo_wl.device_secret)) {
                break;
            }

            /* 白名单模式, 用户务必要对device_secret进行持久化保存 */
            memset(&demo_devinfo_wl, 0, sizeof(demo_devinfo_wl_t));
            memcpy(demo_devinfo_wl.device_secret, packet->data.deviceinfo_wl.device_secret,
                   strlen(packet->data.deviceinfo_wl.device_secret));
        }
        break;
        /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
        case AIOT_DYNREGMQRECV_DEVICEINFO_NWL: {
            if (strlen(packet->data.deviceinfo_nwl.clientid) >= sizeof(demo_devinfo_nwl.conn_clientid) ||
                strlen(packet->data.deviceinfo_nwl.username) >= sizeof(demo_devinfo_nwl.conn_username) ||
                strlen(packet->data.deviceinfo_nwl.password) >= sizeof(demo_devinfo_nwl.conn_password)) {
                break;
            }

            /* 免白名单模式, 用户务必要对MQTT的建连信息clientid, username和password进行持久化保存 */
            memset(&demo_devinfo_nwl, 0, sizeof(demo_devinfo_nwl_t));
            memcpy(demo_devinfo_nwl.conn_clientid, packet->data.deviceinfo_nwl.clientid,
                   strlen(packet->data.deviceinfo_nwl.clientid));
            memcpy(demo_devinfo_nwl.conn_username, packet->data.deviceinfo_nwl.username,
                   strlen(packet->data.deviceinfo_nwl.username));
            memcpy(demo_devinfo_nwl.conn_password, packet->data.deviceinfo_nwl.password,
                   strlen(packet->data.deviceinfo_nwl.password));
        }
        break;
        default: {

        }
        break;
    }
}

int demo_main(int argc, char *argv[])
{
    int32_t     res = STATE_SUCCESS;
    void       *dynregmq_handle = NULL;
    char       *host =
                "a18wPzZJzNG.iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台动态注册国内站点, TODO: 前缀为设备的productKey */
    uint16_t    port = 443;            /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t
    cred;    /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */
    uint8_t     nowhitelist = 0;        /* 是否使用免白名单功能 */

    /* TODO: 替换为自己设备的productKey, productSecret和deviceName */
    char *product_key       = "a18wPzZJzNG";
    char *product_secret    = "xxxxxxxxxxxxxxxx";
    char *device_name       = "dynreg_device_01";

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验DYNREGMQ服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证服务端的RSA根证书长度 */

    /* 创建1个dynregmq客户端实例并内部初始化默认参数 */
    dynregmq_handle = aiot_dynregmq_init();
    if (dynregmq_handle == NULL) {
        printf("aiot_dynregmq_init failed\n");
        return -1;
    }

    /* 配置连接的服务器地址 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_HOST, (void *)host);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_HOST failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置连接的服务器端口 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PORT, (void *)&port);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PORT failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置设备productKey */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PRODUCT_KEY, (void *)product_key);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PRODUCT_KEY failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置设备productSecret */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PRODUCT_SECRET, (void *)product_secret);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PRODUCT_SECRET failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置设备deviceName */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_DEVICE_NAME, (void *)device_name);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_DEVICE_NAME failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_NETWORK_CRED, (void *)&cred);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_NETWORK_CRED failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置DYNREGMQ默认消息接收回调函数 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_RECV_HANDLER, (void *)demo_dynregmq_recv_handler);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_RECV_HANDLER failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 配置DYNREGMQ动态注册模式,
    1. 配置为0则为白名单模式, 用户必须提前在控制台录入deviceName, 动态注册完成后服务会返回deviceSecret, 用户可通过
       AIOT_DYNREGMQRECV_DEVICEINFO_WL类型数据回调获取到deviceSecret.
    2. 配置为1则为免白名单模式, 用户无需提前在控制台录入deviceName, 动态注册完成后服务会返回MQTT建连信息, 用户可通过
       AIOT_DYNREGMQRECV_DEVICEINFO_NWL类型数据回调获取到clientid, username, password. 用户需要将这三个参数通过
       aiot_mqtt_setopt接口以AIOT_MQTTOPT_CLIENTID, AIOT_MQTTOPT_USERNAME, AIOT_MQTTOPT_PASSWORD配置选项
       配置到MQTT句柄中。
    */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_NO_WHITELIST, (void *)&nowhitelist);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_NO_WHITELIST failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 发送动态注册请求 */
    res = aiot_dynregmq_send_request(dynregmq_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_send_request failed: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 接收动态注册请求 */
    res = aiot_dynregmq_recv(dynregmq_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_recv failed: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return -1;
    }

    /* 把服务应答中的信息打印出来 */
    if (nowhitelist == 0) {
        printf("device secret: %s\n", demo_devinfo_wl.device_secret);
    } else {
        printf("clientid: %s\n", demo_devinfo_nwl.conn_clientid);
        printf("username: %s\n", demo_devinfo_nwl.conn_username);
        printf("password: %s\n", demo_devinfo_nwl.conn_password);
    }

    /* 销毁动态注册会话实例 */
    res = aiot_dynregmq_deinit(&dynregmq_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynregmq_deinit failed: -0x%04X\n", -res);
        return -1;
    }

    return 0;
}

