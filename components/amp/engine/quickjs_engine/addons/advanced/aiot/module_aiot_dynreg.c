// /*
//  * Copyright (C) 2015-2019 Alibaba Group Holding Limited
//  */

// #include "amp_platform.h"
// #include "aos_system.h"
// #include "amp_defines.h"
// #include "aiot_state_api.h"
// #include "aiot_sysdep_api.h"
// #include "aiot_dynreg_api.h"
// #include "aiot_mqtt_api.h"
// #include "module_aiot.h"
// // #include "be_inl.h"

// #define MOD_STR "AIOT_DYNREG"

// /* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
// extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

// /* 位于external/ali_ca_cert.c中的服务器证书 */
// extern const char *ali_ca_cert;

// void aiot_app_dynreg_recv_handler(void *handle, const aiot_dynreg_recv_t *packet, void *userdata)
// {
//     int js_cb_ref = 0;

//     if (packet->type == AIOT_DYNREGRECV_STATUS_CODE)
//     {
//         amp_debug(MOD_STR, "dynreg rsp code = %d", packet->data.status_code.code);
//     }
//     else if (packet->type == AIOT_DYNREGRECV_DEVICE_INFO)
//     {
//         amp_debug(MOD_STR, "dynreg DS = %s", packet->data.device_info.device_secret);
//         aos_kv_set(AMP_CUSTOMER_DEVICESECRET, packet->data.device_info.device_secret, strlen(packet->data.device_info.device_secret), 1);
//         int js_cb_ref = (int *)userdata;
//         duk_context *ctx = be_get_context();
//         be_push_ref(ctx, js_cb_ref);
//         duk_push_string(ctx, packet->data.device_info.device_secret);
//         if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
//             amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
//         }
//         duk_pop(ctx);
//         duk_gc(ctx, 0);
//     }
// }

// int32_t aiot_dynreg_http(int js_cb_ref)
// {
//     int32_t res = STATE_SUCCESS;
//     char *auth_url = "iot-auth.cn-shanghai.aliyuncs.com";    /* 阿里云平台上海站点的域名后缀 */
//     char host[100] = {0};                                    /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
//     uint16_t port = 443;                                     /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
//     aiot_sysdep_network_cred_t cred;                         /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

//     /* get device tripple info */
//     char product_key[IOTX_PRODUCT_KEY_LEN] = {0};
//     char product_secret[IOTX_PRODUCT_SECRET_LEN] = {0};
//     char device_name[IOTX_DEVICE_NAME_LEN] = {0};
//     char device_secret[IOTX_DEVICE_SECRET_LEN] = {0};

//     int productkey_len = IOTX_PRODUCT_KEY_LEN;
//     int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
//     int devicename_len = IOTX_DEVICE_NAME_LEN;
//     int devicesecret_len = IOTX_DEVICE_SECRET_LEN;

//     aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, product_key, &productkey_len);
//     aos_kv_get(AMP_CUSTOMER_PRODUCTSECRET, product_secret, &productsecret_len);
//     aos_kv_get(AMP_CUSTOMER_DEVICENAME, device_name, &devicename_len);
//     amp_debug(MOD_STR, "dev info pk: %s, ps: %s, dn: %s", product_key, product_secret, device_name);
//     /* end get device tripple info */

//     /* 配置SDK的底层依赖 */
//     aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
//     /* 配置SDK的日志输出 */
//     // aiot_state_set_logcb(demo_state_logcb);

//     /* 创建SDK的安全凭据, 用于建立TLS连接 */
//     memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
//     cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA; /* 使用RSA证书校验MQTT服务端 */
//     cred.max_tls_fragment = 16384;                     /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
//     cred.sni_enabled = 1;                              /* TLS建连时, 支持Server Name Indicator */
//     cred.x509_server_cert = ali_ca_cert;               /* 用来验证MQTT服务端的RSA根证书 */
//     cred.x509_server_cert_len = strlen(ali_ca_cert);   /* 用来验证MQTT服务端的RSA根证书长度 */

//     aos_kv_get(AMP_CUSTOMER_DEVICESECRET, device_secret, &devicesecret_len);
//     if (device_secret[0] == '\0' || device_secret[IOTX_DEVICE_SECRET_LEN] != '\0')
//     {

//         if (product_secret[0] == '\0' || product_secret[IOTX_PRODUCT_SECRET_LEN] != '\0')
//         {
//             amp_error(MOD_STR, "need dynamic register, product secret is null");
//             return -1;
//         }

//         void *dynreg_handle = NULL;

//         dynreg_handle = aiot_dynreg_init();
//         if (dynreg_handle == NULL)
//         {
//             amp_error(MOD_STR, "dynreg handle is null");
//             aos_task_exit(0);
//             return NULL;
//         }

//         /* 配置网络连接的安全凭据, 上面已经创建好了 */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_NETWORK_CRED, (void *)&cred);
//         /* 配置MQTT服务器地址 */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_HOST, (void *)auth_url);
//         /* 配置MQTT服务器端口 */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PORT, (void *)&port);
//         /* 配置设备productKey */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PRODUCT_KEY, (void *)product_key);
//         /* 配置设备productSecret */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_PRODUCT_SECRET, (void *)product_secret);
//         /* 配置设备deviceName */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_DEVICE_NAME, (void *)device_name);
//         /* 配置DYNREG默认消息接收回调函数 */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_RECV_HANDLER, (void *)aiot_app_dynreg_recv_handler);
//         /* 配置DYNREG默认消息接收回调函数参数 */
//         aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_USERDATA, js_cb_ref);

//         res = aiot_dynreg_send_request(dynreg_handle);
//         if (res < STATE_SUCCESS)
//         {
//             amp_error(MOD_STR, "dynamic register send  fail res = %d\n\r", res);
//             aiot_dynreg_deinit(&dynreg_handle);
//             return -1;
//         }

//         res = aiot_dynreg_recv(dynreg_handle);
//         if (res < STATE_SUCCESS)
//         {
//             amp_error(MOD_STR, "dynamic register recv fail res = %d\n\r", res);
//             aiot_dynreg_deinit(&dynreg_handle);
//             return -1;
//         }

//         aos_kv_get(AMP_CUSTOMER_DEVICESECRET, device_secret, &devicesecret_len);

//         res = aiot_dynreg_deinit(&dynreg_handle);
//         if (res < STATE_SUCCESS)
//         {
//             amp_error(MOD_STR, "dynamic register deinit fail res = %d\n\r", res);
//             return -1;
//         }
//     } else {
//         amp_debug(MOD_STR, "device is already activated");
//         return -1;
//     }

//     return STATE_SUCCESS;
// }
