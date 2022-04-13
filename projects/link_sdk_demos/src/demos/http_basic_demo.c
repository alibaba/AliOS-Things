/*
 * 这个例程适用于`Linux`这类支持pthread的POSIX设备, 它演示了用SDK配置HTTP参数并成功鉴权, 然后
 *
 * + 如果放开被注释掉的 demo_http_post_lightswitch() 函数调用, 会向平台上报1条HTTP消息
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aiot_http_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1578127395.399][LK-0409] > POST /auth HTTP/1.1
 *
 * 上面这条日志的code就是0409(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

/* HTTP事件回调函数, 当内部Token变化时被触发, 事件定义见core/aiot_http_api.h */
void demo_http_event_handler(void *handle, const aiot_http_event_t *event, void *user_data)
{
    int32_t res;

    /* token失效事件处理 */
    if (event->type == AIOT_HTTPEVT_TOKEN_INVALID) {
        printf("token invalid, invoke iot_http_auth to get new token\n");
        res = aiot_http_auth(handle);
        printf("aiot_http_auth in callback, res = -0x%04x\r\n", -res);
    }
}

/* HTTP事件回调函数, 当SDK读取到网络报文时被触发, 报文描述类型见core/aiot_http_api.h */
void demo_http_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP状态码, 如404, 200, 302等 */
            /* printf("status code: %d\n", packet->data.status_code.code); */
        }
        break;

        case AIOT_HTTPRECV_HEADER: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP首部, 如Content-Length等 */
            /* printf("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value); */
        }
        break;

        /* TODO: 如果需要处理云平台的HTTP回应报文, 修改这里, 现在只是将回应打印出来 */
        case AIOT_HTTPRECV_BODY: {
            printf("%.*s\r\n", packet->data.body.len, packet->data.body.buffer);
        }
        break;

        default: {
        }
        break;

    }
}

/* 用HTTP通道上报业务数据给云平台, 例如: 灯已关闭 */
int32_t demo_http_post_lightswitch(void *handle)
{
    /* TODO: 如果要修改上报的数据内容, 就修改 data[] 数组 */
    char data[] = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
    int32_t res;

    /* 用HTTP通道向云平台上报1条业务数据, 接口是aiot_http_send() */
    res = aiot_http_send(handle,
                         "/sys/a13FN5TplKq/http_basic_demo/thing/event/property/post",
                         (uint8_t *)data,
                         strlen(data));
    if (res < 0) {
        printf("aiot_http_send res = -0x%04X\r\n", -res);
        return res;
    }

    res = aiot_http_recv(handle);
    if (res >= 0) {
        /* 成功接收到服务器应答, 且业务应答码为0, 说明数据上报成功 */
        return 0;
    } else {
        printf("aiot_http_recv res = -0x%04X\r\n", -res);
        return -1;
    }
}

int demo_main(int argc, char *argv[])
{
    void       *http_handle = NULL;
    int32_t     res;
    char       *host = "iot-as-http.cn-shanghai.aliyuncs.com";
    uint16_t    port = 443;
    aiot_sysdep_network_cred_t cred;

    /* TODO: 替换为自己设备的三元组 */
    char *product_key = "a13FN5TplKq";
    char *device_name = "http_basic_demo";
    char *device_secret = "TGijLt2jVngB53pwQVdGvdGXYR5pDH4y";

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
    cred.max_tls_fragment = 16384;
    cred.x509_server_cert = ali_ca_cert;
    cred.x509_server_cert_len = strlen(ali_ca_cert);

    /* 创建1个HTTP客户端实例并内部初始化默认参数 */
    http_handle = aiot_http_init();

    /* 配置服务器域名 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
    /* 配置服务器端口 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
    /* 配置设备安全凭证 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
    /* 配置设备三元组 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_PRODUCT_KEY, product_key);
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_DEVICE_NAME, device_name);
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_DEVICE_SECRET, device_secret);
    /* 配置数据到达时, SDK应调用的用户回调函数 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_RECV_HANDLER, demo_http_recv_handler);
    /* 配置内部状态变化时, SDK应调用的用户回调函数 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_EVENT_HANDLER, demo_http_event_handler);

    /*
     * TODO: 以下代码如果不被注释, 会演示用TCP(而不是TLS)连接云平台
     */

    /*

    {
        int32_t     tcp_port = 80;
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
        aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
        aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, &tcp_port);
    }

    */

    /* 设备认证, 获取token */
    res = aiot_http_auth(http_handle);
    if (res == 0) {
        printf("aiot_http_auth succeed\r\n\r\n");
    } else {
        /* 如果认证失败, 就销毁实例, 回收资源, 结束程序退出 */
        printf("aiot_http_auth failed, res = -0x%04X\r\n\r\n", -res);
        aiot_http_deinit(&http_handle);
        return -1;
    }

    /* TODO: 以下代码如果不被注释, 会演示上报 1 次数据到物联网平台 */
    /*
    demo_http_post_lightswitch(http_handle);
    printf("\r\n");
     */

    /* 销毁HTTP实例 */
    aiot_http_deinit(&http_handle);
    printf("program exit as normal return\r\n");
    printf("\r\n");

    return 0;
}

