/* 这个例程演示了用SDK配置动态注册会话实例的参数, 并发起请求和接收应答, 之后
 *
 * + 如果接收应答失败了, 销毁实例, 回收资源, 结束程序退出
 * + 如果接收应答成功, 在`demo_dynreg_recv_handler()`的应答处理回调函数中, 演示解析获取服务端应答的内容
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
#include "aiot_dynreg_api.h"

typedef struct {
    uint32_t code;
    char *device_secret;
} demo_info_t;

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1580995015.811][LK-040B] > POST /auth/register/device HTTP/1.1
 *
 * 上面这条日志的code就是040B(十六进制), code值的定义见components/dynreg/aiot_dynreg_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

/* 数据处理回调, 当SDK从网络上收到dynreg消息时被调用 */
void demo_dynreg_recv_handler(void *handle, const aiot_dynreg_recv_t *packet, void *userdata)
{
    demo_info_t *demo_info = (demo_info_t *)userdata;

    switch (packet->type) {
        case AIOT_DYNREGRECV_STATUS_CODE: {
            demo_info->code = packet->data.status_code.code;
        }
        break;
        /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
        case AIOT_DYNREGRECV_DEVICE_INFO: {
            demo_info->device_secret = malloc(strlen(packet->data.device_info.device_secret) + 1);
            if (demo_info->device_secret != NULL) {
                memset(demo_info->device_secret, 0, strlen(packet->data.device_info.device_secret) + 1);
                memcpy(demo_info->device_secret, packet->data.device_info.device_secret,
                       strlen(packet->data.device_info.device_secret));
            }
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
    void       *dynreg_handle = NULL;
    char       *host = "iot-auth.cn-shanghai.aliyuncs.com"; /* 阿里云平台动态注册国内站点 */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */
    demo_info_t demo_info;

    /* TODO: 替换为自己设备的productKey, productSecret和deviceName */
    char *product_key       = "a13FN5TplKq";
    char *product_secret    = "y7GSILD480lBSsP8";
    char *device_name       = "dynreg_basic_demo";

    memset(&demo_info, 0, sizeof(demo_info_t));

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验DYNREG服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证服务端的RSA根证书长度 */

    /* 创建1个dynreg客户端实例并内部初始化默认参数 */
    dynreg_handle = aiot_dynreg_init();
    if (dynreg_handle == NULL) {
        printf("aiot_dynreg_init failed\n");
        return -1;
    }

    /* 配置连接的服务器地址 */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_HOST, (void *)host);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_HOST failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置连接的服务器端口 */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PORT, (void *)&port);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_PORT failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置设备productKey */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PRODUCT_KEY, (void *)product_key);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_PRODUCT_KEY failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置设备productSecret */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PRODUCT_SECRET, (void *)product_secret);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_PRODUCT_SECRET failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置设备deviceName */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_DEVICE_NAME, (void *)device_name);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_DEVICE_NAME failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_NETWORK_CRED, (void *)&cred);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_NETWORK_CRED failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 配置DYNREG默认消息接收回调函数 */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_RECV_HANDLER, (void *)demo_dynreg_recv_handler);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_RECV_HANDLER failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 设置用户上下文，该上下文会在 demo_dynreg_recv_handler 被调用时传回 */
    res = aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_USERDATA, (void *)&demo_info);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_setopt AIOT_DYNREGOPT_USERDATA failed, res: -0x%04X\n", -res);
        aiot_dynreg_deinit(&dynreg_handle);
        return -1;
    }

    /* 发送动态注册请求 */
    res = aiot_dynreg_send_request(dynreg_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_send_request failed: -0x%04X\n", -res);
        return -1;
    }

    /* 接收动态注册请求 */
    res = aiot_dynreg_recv(dynreg_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_recv failed: -0x%04X\n", -res);
        return -1;
    }

    printf("status code: %d\n", demo_info.code);

    /* 把服务应答中的 deviceSecret 打印出来 */
    if (demo_info.device_secret != NULL) {
        printf("device secret: %s\n", demo_info.device_secret);
        free(demo_info.device_secret);
    }

    /* 销毁动态注册会话实例 */
    res = aiot_dynreg_deinit(&dynreg_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dynreg_deinit failed: -0x%04X\n", -res);
        return -1;
    }

    return 0;
}

