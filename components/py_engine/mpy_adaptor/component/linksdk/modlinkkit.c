#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"


#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;


static uint8_t g_mqtt_process_thread_running = 0;
static uint8_t g_mqtt_recv_thread_running = 0;


void       *dm_handle = NULL;
void       *mqtt_handle = NULL;

#define EXAMPLE_MASTER_DEVID            (0)

#define EXAMPLE_YIELD_TIMEOUT_MS        (200)

#define RETRY_NUM  5

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

typedef enum {
    ON_CONNECT    = 1,
    ON_DISCONNECT     = 3,
    ON_SERVICE = 5,
    ON_PROPS     = 7,
    ON_CLOSE   = 2,
    ON_ERROR    = 4,
} lk_callback_func_t;


typedef enum{

    ITE_CONNECT_SUCC =1 ,
    ITE_DISCONNECTED =3,
    ITE_SERVICE_REQUEST =5,
    ITE_PROPERTY_SET =7 ,
    ITE_REPORT_REPLY =2,
    ITE_TRIGGER_EVENT_REPLY =4,
}lk_callback_reg_func_t;

static user_example_ctx_t g_user_example_ctx;

static mp_obj_t on_connected ;
static mp_obj_t on_disconnect ;
static mp_obj_t on_report_reply ;
static mp_obj_t on_event_reply ;
static mp_obj_t on_property_set ;
static mp_obj_t on_async_service ;


/* 日志回调函数, SDK的日志会从这里输出 */
int32_t py_demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}


static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    // // int add_result = 0;
    // int ret = -1;
    // printf("*********** %s ********* \n",__FUNCTION__);
    // if(mp_obj_is_fun(on_service_request)){
    //     mp_call_function_2(on_service_request,mp_obj_new_str(request,request_len),mp_obj_new_str(response,response_len));
    // }else{
    //     printf("Error : on_connected : is not function  \n");
    // }
     return 0;
}

/** recv property post response message from cloud **/
static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    // if(mp_obj_is_fun(on_report_reply)){
    //     mp_call_function_2(on_report_reply,mp_obj_new_int(msgid),mp_obj_new_int(code));
    // }else{
    //     printf("Error : on_connected : is not function  \n");
    // }
     return 0;
}

/** recv event post response message from cloud **/
static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
//     if(mp_obj_is_fun(on_event_reply)){
//         mp_call_function_2(on_event_reply,mp_obj_new_int(msgid),mp_obj_new_int(code));
//     }else{
//         printf("Error : on_connected : is not function  \n");
//     }
    return 0;
}


static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    // int res = 0;

    // res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
    //                          (unsigned char *)request, request_len);

    // if(mp_obj_is_fun(on_property_set)){
    //     mp_call_function_1(on_property_set,mp_obj_new_str(request,request_len));
    // }else{
    //     printf("Error : on_connected : is not function  \n");
    // }

    // return 0;
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            printf("AIOT_MQTTEVT_CONNECT\n");
            if(mp_obj_is_fun(on_connected)){
                mp_call_function_0(on_connected);
            }

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


    printf("********aiot_mqtt_process******\r\n");

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

/* 用户数据接收处理回调函数 */
static void demo_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    printf("demo_dm_recv_handler, type = %d\r\n", recv->type);

    void *old_state = mp_thread_get_state();
    mp_state_thread_t ts;
    mp_thread_set_state(&ts);
    mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
    mp_stack_set_limit(1024*20);

    mp_locals_set(mp_locals_get());
    mp_globals_set(mp_globals_get());
    // mp_sched_lock();




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

            if(mp_obj_is_fun(on_property_set)){
                mp_call_function_1(on_property_set,mp_obj_new_str(recv->data.property_set.params,recv->data.property_set.params_len));
            }else{
                printf("Error : on_connected : is not function  \n");
            }

            /* TODO: 以下代码演示如何对来自云平台的属性设置指令进行应答, 用户可取消注释查看演示效果 */

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

        }
        break;

        /* 异步服务调用 */
        case AIOT_DMRECV_ASYNC_SERVICE_INVOKE: {
            printf("msg_id = %ld, service_id = %s, params = %.*s\r\n",
                   (unsigned long)recv->data.async_service_invoke.msg_id,
                   recv->data.async_service_invoke.service_id,
                   recv->data.async_service_invoke.params_len,
                   recv->data.async_service_invoke.params);

            if(mp_obj_is_fun(on_async_service)){
                mp_call_function_2(on_async_service,
                  mp_obj_new_str(recv->data.async_service_invoke.service_id,strlen(recv->data.async_service_invoke.service_id)),
                  mp_obj_new_str(recv->data.async_service_invoke.params,recv->data.async_service_invoke.params_len));
            }else{
                printf("Error : on_async_service : is not function  \n");
            }

            /* TODO: 以下代码演示如何对来自云平台的异步服务调用进行应答, 用户可取消注释查看演示效果
             *
             * 注意: 如果用户在回调函数外进行应答, 需要自行保存msg_id, 因为回调函数入参在退出回调函数后将被SDK销毁, 不可以再访问到
             */

            /*
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
            */
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
    // mp_sched_unlock();
    mp_thread_set_state(old_state);
}


/* 属性上报函数演示 */
int32_t demo_send_property_post(void *dm_handle, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_PROPERTY_POST;
    msg.data.property_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}


/* 事件上报函数演示 */
int32_t demo_send_event_post(void *dm_handle, char *event_id, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_EVENT_POST;
    msg.data.event_post.event_id = event_id;
    msg.data.event_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}





/** cloud connected event callback */
static int user_connected_event_handler(void)
{
    if(mp_obj_is_fun(on_connected)){
        mp_call_function_0(on_connected);
    }else{
        printf("Error : on_connected : is not function  \n");
    }
    g_user_example_ctx.cloud_connected = 1;
    return 0;
}

/** cloud disconnected event callback */
static int user_disconnected_event_handler(void)
{
    if(mp_obj_is_fun(on_connected)){
        mp_call_function_0(on_disconnect);
    }else{
        printf("Error:on_connected : is not function  \n");
    }
    g_user_example_ctx.cloud_connected = 0;
    return 0;
}



STATIC mp_obj_t linkkit_init(size_t n_args, const mp_obj_t *args) {
    int32_t     res = STATE_SUCCESS;

    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    char *product_key       = "a18wPzZJzNG";
    char *device_name       = "data_model_basic_demo";
    char *device_secret     = "uwMTmVAMnGGHaAkqmeDY6cHxxBMTP3TD";

    int ret = -1;
    if (n_args < 5)
    {
        printf("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

   // iot_host_name = mp_obj_str_get_str(args[0]);
    product_key = mp_obj_str_get_str(args[1]);
    device_name = mp_obj_str_get_str(args[2]);
    device_secret = mp_obj_str_get_str(args[3]);
  //  iot_product_sec = mp_obj_str_get_str(args[4]);


 /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(py_demo_state_logcb);

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

    /* 创建DATA-MODEL实例 */
    dm_handle = aiot_dm_init();
    if (dm_handle == NULL) {
        printf("aiot_dm_init failed");
        return -1;
    }
    /* 配置MQTT实例句柄 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_MQTT_HANDLE, mqtt_handle);
    /* 配置消息接收处理回调函数 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_RECV_HANDLER, (void *)demo_dm_recv_handler);

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
        printf("create demo_mqtt_recv_thread failed: %d\n", res);
        return mp_const_false;
    }

      /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_thread_running = 1;
    res = aos_task_new("demo_mqtt_process", demo_mqtt_recv_thread, mqtt_handle, 4096);
    // res = pthread_create(&g_mqtt_recv_thread, NULL, demo_mqtt_recv_thread, mqtt_handle);
    if (res != 0) {
        printf("create demo_mqtt_recv_thread failed: %d\n", res);
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_link_init, 5, linkkit_init);




STATIC mp_obj_t register_dyn_dev(void) {
    return mp_const_true ;
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_obj_register_dyn_dev, register_dyn_dev);

STATIC mp_obj_t connect_lp(void) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_obj_connect_lp, connect_lp);

STATIC mp_obj_t register_call_back(mp_obj_t id,mp_obj_t func) {

    int callback_id =  mp_obj_get_int(id);
    switch(callback_id){
        case ITE_CONNECT_SUCC:
            on_connected = func ;
            break;
        case ITE_DISCONNECTED:
            break;
        case ITE_SERVICE_REQUEST:
            on_async_service = func  ;
            break;
        case ITE_PROPERTY_SET:
            on_property_set = func ;
            break;
        case ITE_REPORT_REPLY:
            on_report_reply = func ;
            break;
        case ITE_TRIGGER_EVENT_REPLY:
            on_event_reply = func ;
            break;

        default:
            break;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_register_call_back, register_call_back);


STATIC mp_obj_t post_property(mp_obj_t data) {
    size_t len ;
    const char *payload = mp_obj_str_get_data(data, &len);
    size_t ret = demo_send_property_post(dm_handle, payload);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_obj_post_property,post_property);


STATIC mp_obj_t post_event(mp_obj_t eventid,mp_obj_t data) {
    size_t payload_len ;
    size_t id_len ;
    const char *event_id = mp_obj_str_get_data(eventid, &id_len);
    const char *payload = mp_obj_str_get_data(data, &payload_len);
    size_t ret =  demo_send_event_post(dm_handle, event_id, payload);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_post_event,post_event);


STATIC mp_obj_t linkkit_yield(mp_obj_t time) {
    return mp_const_none ;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_obj_linkkit_yield,linkkit_yield);


STATIC mp_obj_t linkkit_close( ) {
    int res = aiot_mqtt_disconnect(mqtt_handle);
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
    return mp_const_none ;
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_obj_linkkit_close,linkkit_close);

STATIC const mp_rom_map_elem_t linkkit_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__linkkit)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_obj_link_init)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_register_dyn_dev), MP_ROM_PTR(&mp_obj_register_dyn_dev)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&mp_obj_connect_lp)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_register_call_back), MP_ROM_PTR(&mp_obj_register_call_back)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_post_property), MP_ROM_PTR(&mp_obj_post_property)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_post_event), MP_ROM_PTR(&mp_obj_post_event)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_do_yield), MP_ROM_PTR(&mp_obj_linkkit_yield)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_obj_linkkit_close)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_CONNECT), MP_ROM_INT(ON_CONNECT)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_DISCONNECT), MP_ROM_INT(ON_DISCONNECT)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_SERVICE), MP_ROM_INT(ON_SERVICE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_PROPS), MP_ROM_INT(ON_PROPS)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_CLOSE), MP_ROM_INT(ON_CLOSE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_ERROR), MP_ROM_INT(ON_ERROR)},
};

STATIC MP_DEFINE_CONST_DICT(linkkit_module_globals, linkkit_module_globals_table);

const mp_obj_module_t linkkit_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&linkkit_module_globals,
};


