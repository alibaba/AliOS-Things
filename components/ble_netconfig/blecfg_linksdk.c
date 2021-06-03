/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "blecfg_pub.h"

#include "aos/kv.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"
#if (BOARD_HAAS100 == 1)
#include "led.h"
#include "cJSON.h"
#endif

/* 字符串长度限制 */
#define LIMIT_PRODUCT_KEY       32
#define LIMIT_DEVICE_NAME       64
#define LIMIT_DEVICE_SECRTE     64

typedef struct {
    BleCfg_link_cb link_cb;
    uint8_t product_key[LIMIT_PRODUCT_KEY];
    uint8_t device_name[LIMIT_DEVICE_NAME];
    uint8_t device_secret[LIMIT_DEVICE_SECRTE];
}device_info;

static device_info  s_device_info;

extern int BleCfg_linksdk_main(char *product_key, char *device_name, char *device_secret);

static void linksdk_thread(void *arg)
{
    device_info *dev_info = &s_device_info;

    printf("linksdk_thread start\r\n");
    BleCfg_linksdk_main(dev_info->product_key, dev_info->device_name, dev_info->device_secret);
}

static int32_t BleCfg_devinfo_kv_save(char *pk, char *dn, char *ds)
{
    int32_t ret;
    int len;
    char product_key[LIMIT_PRODUCT_KEY];
    char device_name[LIMIT_DEVICE_NAME];
    char device_secret[LIMIT_DEVICE_SECRTE];

    if ( pk == NULL || strlen(pk) >= LIMIT_PRODUCT_KEY
      || dn == NULL || strlen(dn) >= LIMIT_DEVICE_NAME
      || ds == NULL || strlen(ds) >= LIMIT_DEVICE_SECRTE ) {
        BLECFG_LOG_ERROR("[%s]: input para error!\r\n", __func__);
        return -1;
    }

    len = LIMIT_PRODUCT_KEY;
    ret = aos_kv_get("product_key", product_key, &len);
    if(ret){
        product_key[0] = 0;
    }

    len = LIMIT_DEVICE_NAME;
    ret = aos_kv_get("device_name", device_name, &len);
    if(ret){
        device_name[0] = 0;
    }

    len = LIMIT_DEVICE_SECRTE;
    ret = aos_kv_get("device_secret", device_secret, &len);
    if(ret){
        device_secret[0] = 0;
    }

    if ( strcmp(product_key, pk) == 0
      && strcmp(device_name, dn) == 0
      && strcmp(device_secret, ds) == 0) {
        BLECFG_LOG_INFO("[%s]: kv already saved.\r\n", __func__);
        return -1;
    }

    ret = aos_kv_set("product_key", pk, strlen(pk) + 1, 1);
    if ( ret ){
        BLECFG_LOG_ERROR("[%s]: set 'product_key' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("device_name", dn, strlen(dn) + 1, 1);
    if ( ret ) {
        BLECFG_LOG_ERROR("[%s]: set 'device_name' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("device_secret", ds, strlen(ds) + 1, 1);
    if ( ret ) {
        BLECFG_LOG_ERROR("[%s]: set 'device_secret' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int32_t BleCfg_devinfo_kv_load(char *pk, char *dn, char *ds)
{
    int32_t ret;
    int len;

    len = LIMIT_PRODUCT_KEY;
    ret = aos_kv_get("product_key", pk, &len);
    if(ret){
        //BLECFG_LOG_INFO("%s: product_key not find\r\n", __func__);
        return ret;
    }
    BLECFG_LOG_INFO("[%s]: product_key '%s'\r\n", __func__, pk);

    len = LIMIT_DEVICE_NAME;
    ret = aos_kv_get("device_name", dn, &len);
    if(ret){
        //BLECFG_LOG_INFO("%s: device_name not find\r\n", __func__);
        return ret;
    }
    BLECFG_LOG_INFO("[%s]: device_name '%s'\r\n", __func__, dn);

    len = LIMIT_DEVICE_SECRTE;
    ret = aos_kv_get("device_secret", ds, &len);
    if(ret){
        //BLECFG_LOG_INFO("%s: device_secret not find\r\n", __func__);
        return ret;
    }
    BLECFG_LOG_INFO("[%s]: device_secret '%s'\r\n", __func__, ds);

    return 0;
}

BLECFG_STATE BleCfg_dev_set(char *pk, char *dn, char *ds)
{
    device_info *dev_info = &s_device_info;

    if ( strlen(pk) + 1 >= sizeof(dev_info->product_key)
      || strlen(dn) + 1 >= sizeof(dev_info->device_name)
      || strlen(ds) + 1 >= sizeof(dev_info->device_secret)) {
        return BLECFG_COMMON_FAILED;
    }

    strncpy(dev_info->product_key, pk, sizeof(dev_info->product_key));
    strncpy(dev_info->device_name, dn, sizeof(dev_info->device_name));
    strncpy(dev_info->device_secret, ds, sizeof(dev_info->device_secret));

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_dev_connect()
{
    int32_t ret;
    aos_task_t task_main;

    BLECFG_LOG_INFO("%s: Connecting cloud...", __func__);

    /* 配网流程结束，开启linkSDK连云流程 */
    ret = aos_task_create(&task_main, "linksdk_demo", linksdk_thread,
                          NULL, NULL, 6048, AOS_DEFAULT_APP_PRI, AOS_TASK_AUTORUN);
    if (ret < 0) {
        printf("create 'linksdk_demo' task failed, ret = %ld \r\n", ret);
    }

}

BLECFG_STATE BleCfg_dev_save()
{
    int32_t ret;
    device_info *dev_info = &s_device_info;

    ret = BleCfg_devinfo_kv_save(dev_info->product_key, dev_info->device_name, dev_info->device_secret);
    if ( ret != 0 ) {
        return BLECFG_COMMON_FAILED;
    }

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_dev_restore()
{
    int32_t ret;
    device_info *dev_info = &s_device_info;

    ret = BleCfg_devinfo_kv_load(dev_info->product_key, dev_info->device_name, dev_info->device_secret);
    if ( ret != 0 ) {
        return BLECFG_COMMON_FAILED;
    }

    return BLECFG_SUCCESS;
}

/*
 * 这个例程演示了用SDK配置MQTT参数并建立连接, 之后创建2个线程
 *
 * + 一个线程用于保活长连接
 * + 一个线程用于接收消息, 并在有消息到达时进入默认的数据回调, 在连接状态变化时进入事件回调
 *
 * 接着演示了在MQTT连接上进行属性上报, 事件上报, 以及处理收到的属性设置, 服务调用, 取消这些代码段落的注释即可观察运行效果
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

static uint8_t g_mqtt_process_thread_running = 0;
static uint8_t g_mqtt_recv_thread_running = 0;

#if (BOARD_HAAS100 == 1)
static int32_t s_led_state[6];

static void BleCfg_led_switch(uint32_t len, char *str)
{
    int32_t led_id;
    int32_t led_state;
    cJSON *strCommand, *strLed;
    char *led_name[] = {NULL, "HaaS100_LED_1", "HaaS100_LED_2", "HaaS100_LED_3", "HaaS100_LED_4", "HaaS100_LED_5"};

    strCommand = cJSON_Parse(str);   //创建JSON解析对象，返回JSON格式是否正确
    if (!strCommand) {
        printf("JSON格式错误:%s\n\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return;
    }

    printf("JSON格式正确:\n%s\n\n",cJSON_Print(strCommand) );

    for (led_id = 1; led_id < 6; led_id++)
    {
        strLed = cJSON_GetObjectItem(strCommand, led_name[led_id]); //获取name键对应的值的信息
        if (strLed == NULL || strLed->type != cJSON_Number) {
            continue;
        }
        s_led_state[led_id] = strLed->valueint;
        if (strLed->valueint == 1) {
            led_switch(led_id, LED_ON);
        } else {
            led_switch(led_id, LED_OFF);
        }
        break;
    }

    cJSON_Delete(strCommand);//释放内存
}

static void BleCfg_led_init()
{
    memset(s_led_state, 0, sizeof(s_led_state));
    led_switch(1, LED_OFF);
    led_switch(2, LED_OFF);
    led_switch(3, LED_OFF);
    led_switch(4, LED_OFF);
    led_switch(5, LED_OFF);
}

static void BleCfg_led_update(void *dm_handle)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_PROPERTY_POST;

    msg.data.property_post.params = "{\"LightSwitch\": 1}";
    aiot_dm_send(dm_handle, &msg);

    msg.data.property_post.params = s_led_state[1] == 1 ? "{\"HaaS100_LED_1\": 1}" : "{\"HaaS100_LED_1\": 0}";
    aiot_dm_send(dm_handle, &msg);

    msg.data.property_post.params = s_led_state[2] == 1 ? "{\"HaaS100_LED_2\": 1}" : "{\"HaaS100_LED_2\": 0}";
    aiot_dm_send(dm_handle, &msg);

    msg.data.property_post.params = s_led_state[3] == 1 ? "{\"HaaS100_LED_3\": 1}" : "{\"HaaS100_LED_3\": 0}";
    aiot_dm_send(dm_handle, &msg);

    msg.data.property_post.params = s_led_state[4] == 1 ? "{\"HaaS100_LED_4\": 1}" : "{\"HaaS100_LED_4\": 0}";
    aiot_dm_send(dm_handle, &msg);

    msg.data.property_post.params = s_led_state[5] == 1 ? "{\"HaaS100_LED_5\": 1}" : "{\"HaaS100_LED_5\": 0}";
    aiot_dm_send(dm_handle, &msg);
}
#endif

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1577589489.033][LK-0317] mqtt_basic_demo&a13FN5TplKq
 *
 * 上面这条日志的code就是0317(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
int32_t BleCfg_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void BleCfg_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    char *product_key;
    char *device_name;
    char *device_secret;
    int32_t res;

    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            printf("AIOT_MQTTEVT_CONNECT\n");
            if ( s_device_info.link_cb != NULL ){
                s_device_info.link_cb();
            }
            break;
        }

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            printf("AIOT_MQTTEVT_RECONNECT\n");
            break;
        }

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            printf("AIOT_MQTTEVT_DISCONNECT: %s\n", cause);
            break;
        }

        default: {

        }
    }
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
void *BleCfg_mqtt_process_thread(void *args)
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
void *BleCfg_mqtt_recv_thread(void *args)
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

/* 用户数据接收处理回调函数 */
static void BleCfg_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    printf("BleCfg_dm_recv_handler, type = %d\r\n", recv->type);

    switch (recv->type) {

        /* 属性上报, 事件上报, 获取期望属性值或者删除期望属性值的应答 */
        case AIOT_DMRECV_GENERIC_REPLY: {
            printf("msg_id = %d, code = %d, data = %.*s, message = %.*s\r\n",
                   recv->data.generic_reply.msg_id,
                   recv->data.generic_reply.code,
                   recv->data.generic_reply.data_len,
                   recv->data.generic_reply.data,
                   recv->data.generic_reply.message_len,
                   recv->data.generic_reply.message);
        }
        break;

        /* 属性设置 */
        case AIOT_DMRECV_PROPERTY_SET: {
            printf("msg_id = %ld, params = %.*s\r\n",
                   (unsigned long)recv->data.property_set.msg_id,
                   recv->data.property_set.params_len,
                   recv->data.property_set.params);

#if (BOARD_HAAS100 == 1)
            BleCfg_led_switch(recv->data.property_set.params_len, recv->data.property_set.params);
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_PROPERTY_SET_REPLY;
                msg.data.property_set_reply.msg_id = recv->data.property_set.msg_id;
                msg.data.property_set_reply.code = 200;
                msg.data.property_set_reply.data = "{}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
            BleCfg_led_update(dm_handle);
#endif
        }
        break;

        /* 异步服务调用 */
        case AIOT_DMRECV_ASYNC_SERVICE_INVOKE: {
            printf("msg_id = %ld, service_id = %s, params = %.*s\r\n",
                   (unsigned long)recv->data.async_service_invoke.msg_id,
                   recv->data.async_service_invoke.service_id,
                   recv->data.async_service_invoke.params_len,
                   recv->data.async_service_invoke.params);

            /* TODO: 以下代码演示如何对来自云平台的异步服务调用进行应答, 用户可取消注释查看演示效果
             *
             * 注意: 如果用户在回调函数外进行应答, 需要自行保存msg_id, 因为回调函数入参在退出回调函数后将被SDK销毁, 不可以再访问到
             */


            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_ASYNC_SERVICE_REPLY;
                msg.data.async_service_reply.msg_id = recv->data.async_service_invoke.msg_id;
                msg.data.async_service_reply.code = 200;
                msg.data.async_service_reply.service_id = "ToggleLightSwitch";
                msg.data.async_service_reply.data = "{\"dataA\": 20}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }

        }
        break;

        /* 同步服务调用 */
        case AIOT_DMRECV_SYNC_SERVICE_INVOKE: {
            printf("msg_id = %ld, rrpc_id = %s, service_id = %s, params = %.*s\r\n",
                   (unsigned long)recv->data.sync_service_invoke.msg_id,
                   recv->data.sync_service_invoke.rrpc_id,
                   recv->data.sync_service_invoke.service_id,
                   recv->data.sync_service_invoke.params_len,
                   recv->data.sync_service_invoke.params);

            /* TODO: 以下代码演示如何对来自云平台的同步服务调用进行应答, 用户可取消注释查看演示效果
             *
             * 注意: 如果用户在回调函数外进行应答, 需要自行保存msg_id和rrpc_id字符串, 因为回调函数入参在退出回调函数后将被SDK销毁, 不可以再访问到
             */

            /*
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_SYNC_SERVICE_REPLY;
                msg.data.sync_service_reply.rrpc_id = recv->data.sync_service_invoke.rrpc_id;
                msg.data.sync_service_reply.msg_id = recv->data.sync_service_invoke.msg_id;
                msg.data.sync_service_reply.code = 200;
                msg.data.sync_service_reply.service_id = "SetLightSwitchTimer";
                msg.data.sync_service_reply.data = "{}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
            */
        }
        break;

        /* 下行二进制数据 */
        case AIOT_DMRECV_RAW_DATA: {
            printf("raw data len = %d\r\n", recv->data.raw_data.data_len);
            /* TODO: 以下代码演示如何发送二进制格式数据, 若使用需要有相应的数据透传脚本部署在云端 */
            /*
            {
                aiot_dm_msg_t msg;
                uint8_t raw_data[] = {0x01, 0x02};

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_RAW_DATA;
                msg.data.raw_data.data = raw_data;
                msg.data.raw_data.data_len = sizeof(raw_data);
                aiot_dm_send(dm_handle, &msg);
            }
            */
        }
        break;

        /* 二进制格式的同步服务调用, 比单纯的二进制数据消息多了个rrpc_id */
        case AIOT_DMRECV_RAW_SYNC_SERVICE_INVOKE: {
            printf("raw sync service rrpc_id = %s, data_len = %d\r\n",
                   recv->data.raw_service_invoke.rrpc_id,
                   recv->data.raw_service_invoke.data_len);
        }
        break;

        default:
            break;
    }
}

/* 属性上报函数演示 */
int32_t BleCfg_send_property_post(void *dm_handle, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_PROPERTY_POST;
    msg.data.property_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}

/* 事件上报函数演示 */
int32_t BleCfg_send_event_post(void *dm_handle, char *event_id, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_EVENT_POST;
    msg.data.event_post.event_id = event_id;
    msg.data.event_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}

/* 演示了获取属性LightSwitch的期望值, 用户可将此函数加入到main函数中运行演示 */
int32_t BleCfg_send_get_desred_requset(void *dm_handle)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_GET_DESIRED;
    msg.data.get_desired.params = "[\"LightSwitch\"]";

    return aiot_dm_send(dm_handle, &msg);
}

/* 演示了删除属性LightSwitch的期望值, 用户可将此函数加入到main函数中运行演示 */
int32_t BleCfg_send_delete_desred_requset(void *dm_handle)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_DELETE_DESIRED;
    msg.data.get_desired.params = "{\"LightSwitch\":{}}";

    return aiot_dm_send(dm_handle, &msg);
}

int BleCfg_linksdk_main(char *product_key, char *device_name, char *device_secret)
{
    int32_t     res = STATE_SUCCESS;
    void       *dm_handle = NULL;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(BleCfg_state_logcb);

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
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)BleCfg_mqtt_event_handler);

    /* 创建DATA-MODEL实例 */
    dm_handle = aiot_dm_init();
    if (dm_handle == NULL) {
        printf("aiot_dm_init failed");
        return -1;
    }
    /* 配置MQTT实例句柄 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_MQTT_HANDLE, mqtt_handle);
    /* 配置消息接收处理回调函数 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_RECV_HANDLER, (void *)BleCfg_dm_recv_handler);
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
    res = aos_task_new("BleCfg_mqtt_process_thread", (void (*)(void *))BleCfg_mqtt_process_thread, mqtt_handle, 4096);
    // res = pthread_create(&g_mqtt_process_thread, NULL, BleCfg_mqtt_process_thread, mqtt_handle);
    if (res != 0) {
        printf("create BleCfg_mqtt_process_thread failed: %d\n", res);
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = aos_task_new("BleCfg_mqtt_recv_thread", (void (*)(void *))BleCfg_mqtt_recv_thread, mqtt_handle, 4096);
    // res = pthread_create(&g_mqtt_recv_thread, NULL, BleCfg_mqtt_recv_thread, mqtt_handle);
    if (res != 0) {
        printf("create BleCfg_mqtt_recv_thread failed: %d\n", res);
        return -1;
    }

#if (BOARD_HAAS100 == 1)
    BleCfg_led_update(mqtt_handle);
#endif

    /* 主循环进入休眠 */
    while (1) {
        /* TODO: 以下代码演示了简单的属性上报和事件上报, 用户可取消注释观察演示效果 */
        //BleCfg_send_property_post(dm_handle, s_led_state[1] == 1 "{\"HaaS100_LED_1\": 1}" ?: "{\"HaaS100_LED_1\": 0}");
        //BleCfg_send_event_post(dm_handle, "Error", "{\"ErrorCode\": 0}");
        aos_msleep(10000);
    }

    /* 断开MQTT连接, 一般不会运行到这里 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        printf("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 销毁DATA-MODEL实例, 一般不会运行到这里 */
    res = aiot_dm_deinit(&dm_handle);
    if (res < STATE_SUCCESS) {
        printf("aiot_dm_deinit failed: -0x%04X\n", -res);
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

BLECFG_STATE BleCfg_linksdk_init(BleCfg_link_cb link_callback)
{
    memset(&s_device_info, 0, sizeof(s_device_info));

    s_device_info.link_cb = link_callback;

#if (BOARD_HAAS100 == 1)
    BleCfg_led_init();
#endif

    return BLECFG_SUCCESS;
}


