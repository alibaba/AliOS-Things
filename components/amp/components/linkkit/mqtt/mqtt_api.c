#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_list.h"
#include "infra_report.h"
#include "infra_sha256.h"
#include "infra_compat.h"
#include "infra_state.h"
#include "mqtt_wrapper.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"

#ifdef PLATFORM_HAS_DYNMEM
    #ifdef INFRA_MEM_STATS
        #include "infra_mem_stats.h"
        #define mqtt_api_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt-api")
        #define mqtt_api_free(ptr)               LITE_free(ptr)
    #else
        #define mqtt_api_malloc(size)            HAL_Malloc(size)
        #define mqtt_api_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
    #endif

#else
    static iotx_mqtt_param_t g_iotx_mqtt_param;
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define mqtt_emerg(...)             log_emerg("MQTT", __VA_ARGS__)
    #define mqtt_crit(...)              log_crit("MQTT", __VA_ARGS__)
    #define mqtt_err(...)               log_err("MQTT", __VA_ARGS__)
    #define mqtt_warning(...)           log_warning("MQTT", __VA_ARGS__)
    #define mqtt_info(...)              log_info("MQTT", __VA_ARGS__)
    #define mqtt_debug(...)             log_debug("MQTT", __VA_ARGS__)
#else
    #define mqtt_emerg(fmt, ...)        LOGF("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_crit(fmt, ...)         LOGF("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_err(fmt, ...)          LOGE("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_warning(fmt, ...)      LOGW("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_info(fmt, ...)         LOGI("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_debug(fmt, ...)        LOGD("MQTT", fmt, ##__VA_ARGS__)
#endif

static void        *g_mqtt_client = NULL;
iotx_sign_mqtt_t    g_default_sign, g_internal_sign;
static char         iotx_ca_crt_itls[IOTX_PRODUCT_KEY_LEN + IOTX_PRODUCT_SECRET_LEN + 2] = {0};

/* Handle structure of subscribed topic */
extern int iotx_report_ext_msg_internal(info_report_func_pt info_report_func, void *handle, iotx_dev_meta_info_t meta_info);
static void iotx_mqtt_report_funcs_interal(void *pclient, iotx_dev_meta_info_t meta_info)
{
    /* report firmware version */
#if !defined(ATHOST_MQTT_REPORT_DISBALED) && !defined(MUTE_VERSION_REPORT)
    int err;
    iotx_set_report_func(IOT_MQTT_Publish_Simple);
    err = iotx_report_ext_msg_internal(IOT_MQTT_Publish_Simple, pclient, meta_info);
    if (err < STATE_SUCCESS) {
    }
#endif
}

/* Handle structure of subscribed topic */
static void iotx_mqtt_report_funcs(void *pclient)
{
    /* report firmware version */
#if !defined(ATHOST_MQTT_REPORT_DISBALED) && !defined(MUTE_VERSION_REPORT)
    int err;
    iotx_set_report_func(IOT_MQTT_Publish_Simple);
    err = iotx_report_firmware_version(pclient);
    if (err < STATE_SUCCESS) {
    }
#endif
}

#define AMP_INTERNAL_DEVICESECRET   "_amp_internal_devicesecret"

#include "dynreg_api.h"
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);

#define DYNAMIC_REG_KV_PREFIX       "DYNAMIC_REG_"
#define DYNAMIC_REG_KV_PREFIX_LEN   12

static int _iotx_dynamic_register_interal(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta_info)
{
    char device_secret[IOTX_DEVICE_SECRET_LEN] = {0};
    char kv_key[IOTX_DEVICE_NAME_LEN + DYNAMIC_REG_KV_PREFIX_LEN] = DYNAMIC_REG_KV_PREFIX;
    int res = -1;
    int buffer_len = IOTX_DEVICE_SECRET_LEN;

    memcpy(kv_key + strlen(kv_key), meta_info->device_name, strlen(meta_info->device_name));

    /* Check if Device Secret exist in KV */
    // IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, device_secret);
    res = HAL_Kv_Get(AMP_INTERNAL_DEVICESECRET, device_secret, &buffer_len);
    if (strlen(device_secret) == 0) {
        char product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};
        void *callback = NULL;

        /* KV not exit, goto dynamic register */
        mqtt_info("DeviceSecret KV not exist, Now We Need Dynamic Register...");

        res = IOT_Dynamic_Register(region, meta_info);
        if (res < STATE_SUCCESS) {
            return res;
        }

        // IOT_Ioctl(IOTX_IOCTL_SET_DEVICE_SECRET, meta_info->device_secret);
        HAL_Kv_Set(AMP_INTERNAL_DEVICESECRET, meta_info->device_secret, IOTX_DEVICE_SECRET_LEN, 1);
        HAL_Kv_Get(AMP_INTERNAL_DEVICESECRET, device_secret, &buffer_len);
        callback = iotx_event_callback(ITE_DYNREG_DEVICE_SECRET);
        if (callback) {
            ((int (*)(const char *))callback)(meta_info->device_secret);
        }
    }

    return STATE_SUCCESS;
}

#ifdef DYNAMIC_REGISTER
#include "dynreg_api.h"
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);

#define DYNAMIC_REG_KV_PREFIX       "DYNAMIC_REG_"
#define DYNAMIC_REG_KV_PREFIX_LEN   12

static int _iotx_dynamic_register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta_info)
{
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    char kv_key[IOTX_DEVICE_NAME_LEN + DYNAMIC_REG_KV_PREFIX_LEN] = DYNAMIC_REG_KV_PREFIX;
    int res = -1;

    memcpy(kv_key + strlen(kv_key), meta_info->device_name, strlen(meta_info->device_name));

    /* Check if Device Secret exist in KV */
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, device_secret);
    if (strlen(device_secret) == 0) {
        char product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};
        void *callback = NULL;

        /* KV not exit, goto dynamic register */
        mqtt_info("DeviceSecret KV not exist, Now We Need Dynamic Register...");

        res = IOT_Dynamic_Register(region, meta_info);
        if (res < STATE_SUCCESS) {
            return res;
        }

        IOT_Ioctl(IOTX_IOCTL_SET_DEVICE_SECRET, meta_info->device_secret);
        callback = iotx_event_callback(ITE_DYNREG_DEVICE_SECRET);
        if (callback) {
            ((int (*)(const char *))callback)(meta_info->device_secret);
        }
    }

    return STATE_SUCCESS;
}
#endif /* #ifdef DYNAMIC_REGISTER */

#ifdef MQTT_PRE_AUTH
#include "infra_preauth.h"
extern int _iotx_generate_sign_string(const char *device_id, const char *device_name, const char *product_key,
                                      const char *device_secret, char *sign_string);

static int _iotx_preauth(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *meta,
                         iotx_sign_mqtt_t *preauth_out)
{
    uint16_t length = 0;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    int res;

    memset(preauth_out, 0, sizeof(iotx_sign_mqtt_t));

    /* setup device_id */
    memcpy(device_id, meta->product_key, strlen(meta->product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta->device_name, strlen(meta->device_name));

    return preauth_get_connection_info(region, meta, device_id, preauth_out);
}
#endif /* #ifdef MQTT_PRE_AUTH */

extern int _sign_get_clientid(char *clientid_string, const char *device_id, const char *custom_kv, uint8_t enable_itls);

/************************  Public Interface ************************/
void *IOT_MQTT_Construct_Interal(iotx_mqtt_param_t *pInitParams, iotx_dev_meta_info_t meta_info)
{
    uint64_t time_now;
    void *pclient;
    iotx_dev_meta_info_t meta_info_interal;
    iotx_mqtt_param_t mqtt_params;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    iotx_mqtt_region_types_t region = IOTX_CLOUD_REGION_SHANGHAI;
    int dynamic = 1;
    uint8_t enable_itls = 0;
    int ret;
    void *callback;

    /* get region */
    IOT_Ioctl(IOTX_IOCTL_GET_REGION, (void *)&region);

    /* init g_internal_sign */
    memset(&g_internal_sign, 0, sizeof(iotx_sign_mqtt_t));

    /* get meta_info from hal */
    memset(&meta_info_interal, 0, sizeof(iotx_dev_meta_info_t));
    memcpy(&meta_info_interal, &meta_info, sizeof(iotx_dev_meta_info_t));

    if (meta_info_interal.product_key[IOTX_PRODUCT_KEY_LEN] != '\0') {
        iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_PK, "invalid product key");
        return NULL;
    }
    if (meta_info_interal.device_name[IOTX_DEVICE_NAME_LEN] != '\0') {
        iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_DN, "invalid device name");
        return NULL;
    }

    int buffer_len = IOTX_DEVICE_SECRET_LEN;
    HAL_Kv_Get(AMP_INTERNAL_DEVICESECRET, meta_info_interal.device_secret, &buffer_len);
    if (meta_info_interal.device_secret[0] == '\0' || meta_info_interal.device_secret[IOTX_DEVICE_SECRET_LEN] != '\0') {

        if (meta_info_interal.product_secret[0] == '\0' || meta_info_interal.product_secret[IOTX_PRODUCT_SECRET_LEN] != '\0') {
            iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_PS, "invalid product secret");
            return NULL;
        }

        ret = _iotx_dynamic_register_interal(region, &meta_info_interal);
        if (ret < STATE_SUCCESS) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "dynamic register fail");
            return NULL;
        } 
    }

#ifdef MQTT_PRE_AUTH /* preauth mode through https */
    ret = _iotx_preauth(region, &meta_info, (iotx_sign_mqtt_t *)&g_internal_sign); /* type convert */
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "invalid device secret");
        mqtt_err("ret = _iotx_preauth() = %d, abort", ret);
        return NULL;
    }

    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)g_internal_sign.hostname);

    {
        iotx_sign_mqtt_t tmp_sign;
        memset(&tmp_sign, 0, sizeof(iotx_sign_mqtt_t));

        ret = IOT_Sign_MQTT(region, &meta_info_interal, &tmp_sign);
        if (ret < STATE_SUCCESS) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
            return NULL;
        }

        memcpy(g_internal_sign.username, tmp_sign.username, strlen(tmp_sign.username));
        memcpy(g_internal_sign.password, tmp_sign.password, strlen(tmp_sign.password));
        memcpy(g_internal_sign.clientid, tmp_sign.clientid, strlen(tmp_sign.clientid));
    }
#else /* direct mode */
    ret = IOT_Sign_MQTT(region, &meta_info_interal, &g_internal_sign);
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
        return NULL;
    }
#endif /* #ifdef MQTT_PRE_AUTH */

    /* setup device_id */
    memcpy(device_id, meta_info_interal.product_key, strlen(meta_info_interal.product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta_info_interal.device_name, strlen(meta_info_interal.device_name));

    /* reconfig clientid, append custome clientKV and itls switch flag */
    if (pInitParams != NULL && pInitParams->customize_info != NULL) {
        if (strstr(pInitParams->customize_info, "authtype=id2") != NULL) {
            enable_itls = 1;
        } else {
            enable_itls = 0;
        }
    }

    ret = _sign_get_clientid(g_internal_sign.clientid, device_id, (pInitParams != NULL) ? pInitParams->customize_info : NULL,
                             enable_itls);
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
        return NULL;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

    {
        extern const char *iotx_ca_crt;
        if (enable_itls == 0) {
            mqtt_params.pub_key = iotx_ca_crt;
        }
    }

    mqtt_params.request_timeout_ms    = CONFIG_MQTT_REQUEST_TIMEOUT;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000;
    mqtt_params.read_buf_size         = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.write_buf_size        = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.handle_event.h_fp     = NULL;
    mqtt_params.handle_event.pcontext = NULL;

    /* optional configuration */
    if (pInitParams != NULL) {
        if (pInitParams->host && strlen(pInitParams->host)) {
            mqtt_params.host = pInitParams->host;
        } else {
            mqtt_warning("Using default hostname: '%s'", g_internal_sign.hostname);
            mqtt_params.host = g_internal_sign.hostname;
        }

        if (pInitParams->port) {
            mqtt_params.port = pInitParams->port;
        } else {
            mqtt_warning("Using default port: [%d]", g_internal_sign.port);
            mqtt_params.port = g_internal_sign.port;
        }

        if (pInitParams->client_id && strlen(pInitParams->client_id)) {
            mqtt_params.client_id = pInitParams->client_id;
        } else {
            mqtt_warning("Using default client_id: %s", g_internal_sign.clientid);
            mqtt_params.client_id = g_internal_sign.clientid;
        }

        if (pInitParams->username && strlen(pInitParams->username)) {
            mqtt_params.username = pInitParams->username;
        } else {
            mqtt_warning("Using default username: %s", g_internal_sign.username);
            mqtt_params.username = g_internal_sign.username;
        }

        if (pInitParams->password && strlen(pInitParams->password)) {
            mqtt_params.password = pInitParams->password;
        } else {
#if 1
            mqtt_warning("Using default password: %s", "******");
#else
            mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
            mqtt_params.password = g_internal_sign.password;
        }

        if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN ||
            pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {
            mqtt_warning("Using default request_timeout_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.request_timeout_ms,
                         pInitParams->request_timeout_ms,
                         CONFIG_MQTT_REQ_TIMEOUT_MIN,
                         CONFIG_MQTT_REQ_TIMEOUT_MAX);
        } else {
            mqtt_params.request_timeout_ms = pInitParams->request_timeout_ms;
        }

        if (pInitParams->clean_session == 0 || pInitParams->clean_session == 1) {
            mqtt_params.clean_session = pInitParams->clean_session;
        }

        if (pInitParams->keepalive_interval_ms < CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000 ||
            pInitParams->keepalive_interval_ms > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000) {
            mqtt_warning("Using default keepalive_interval_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.keepalive_interval_ms,
                         pInitParams->keepalive_interval_ms,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000);
        } else {
            mqtt_params.keepalive_interval_ms = pInitParams->keepalive_interval_ms;
        }

        if (!pInitParams->read_buf_size) {
            mqtt_warning("Using default read_buf_size: %d", mqtt_params.read_buf_size);
        } else {
            mqtt_params.read_buf_size = pInitParams->read_buf_size;
        }

        if (!pInitParams->write_buf_size) {
            mqtt_warning("Using default write_buf_size: %d", mqtt_params.write_buf_size);
        } else {
            mqtt_params.write_buf_size = pInitParams->write_buf_size;
        }

        if (pInitParams->handle_event.h_fp != NULL) {
            mqtt_params.handle_event.h_fp = pInitParams->handle_event.h_fp;
        }

        if (pInitParams->handle_event.pcontext != NULL) {
            mqtt_params.handle_event.pcontext = pInitParams->handle_event.pcontext;
        }
    } else {
        mqtt_warning("Using default port: [%d]", g_internal_sign.port);
        mqtt_params.port = g_internal_sign.port;

        mqtt_warning("Using default hostname: '%s'", g_internal_sign.hostname);
        mqtt_params.host = g_internal_sign.hostname;

        mqtt_warning("Using default client_id: %s", g_internal_sign.clientid);
        mqtt_params.client_id = g_internal_sign.clientid;

        mqtt_warning("Using default username: %s", g_internal_sign.username);
        mqtt_params.username = g_internal_sign.username;

#if 1
        mqtt_warning("Using default password: %s", "******");
#else
        mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
        mqtt_params.password = g_internal_sign.password;
    }

    pclient = wrapper_mqtt_init_internal(&mqtt_params);
    if (pclient == NULL) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_WRAPPER_INIT_FAIL, "mqtt wrapper init fail");
        return NULL;
    }

    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.host);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%d", (int)mqtt_params.port);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", strstr(mqtt_params.client_id, ",") + 1);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.username);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.password);

    time_now = HAL_UptimeMs();
    ret = wrapper_mqtt_connect(pclient);
    if (ret < STATE_SUCCESS) {
        if (MQTT_CONNECT_BLOCK != ret) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt connect failed - ret = %d", ret);
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SUCCESS, "mqtt connected in %d ms", (int)(HAL_UptimeMs() - time_now));

#ifndef ASYNC_PROTOCOL_STACK
    iotx_mqtt_report_funcs_interal(pclient, meta_info_interal);
#endif

    // g_mqtt_client = pclient;

    /* Mqtt Connect Callback */
    callback = iotx_event_callback(ITE_MQTT_CONNECT_SUCC);
    if (callback) {
        ((int (*)(void))callback)();
    }

    return pclient;
}

void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    uint64_t time_now;
    void *pclient;
    iotx_dev_meta_info_t meta_info;
    iotx_mqtt_param_t mqtt_params;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    iotx_mqtt_region_types_t region = IOTX_CLOUD_REGION_SHANGHAI;
    int dynamic = 0;
    uint8_t enable_itls = 0;
    int ret;
    void *callback;

    if (g_mqtt_client != NULL) {
        return g_mqtt_client;
    }

    /* get region */
    IOT_Ioctl(IOTX_IOCTL_GET_REGION, (void *)&region);

    /* get dynamic option */
    IOT_Ioctl(IOTX_IOCTL_GET_DYNAMIC_REGISTER, (void *)&dynamic);

    /* get meta_info from hal */
    memset(&meta_info, 0, sizeof(iotx_dev_meta_info_t));
    IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, meta_info.product_key);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, meta_info.device_name);

    if (meta_info.product_key[IOTX_PRODUCT_KEY_LEN] != '\0') {
        iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_PK, "invalid product key");
        return NULL;
    }
    if (meta_info.device_name[IOTX_DEVICE_NAME_LEN] != '\0') {
        iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_DN, "invalid device name");
        return NULL;
    }

#ifdef DYNAMIC_REGISTER /* get device secret through https dynamic register */
    if (dynamic) {
        IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_SECRET, meta_info.product_secret);
        if (meta_info.product_secret[0] == '\0' || meta_info.product_secret[IOTX_PRODUCT_SECRET_LEN] != '\0') {
            iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_PS, "invalid product secret");
            return NULL;
        }

        ret = _iotx_dynamic_register(region, &meta_info);
        if (ret < STATE_SUCCESS) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "dynamic register fail");
            return NULL;
        }
    } else {
        IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, meta_info.device_secret);
        if (meta_info.device_secret[0] == '\0' || meta_info.device_secret[IOTX_DEVICE_SECRET_LEN] != '\0') {
            iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_DS, "invalid device secret");
            return NULL;
        }
    }
#else /* get device secret from hal */
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, meta_info.device_secret);
    if (meta_info.device_secret[IOTX_DEVICE_SECRET_LEN] != '\0') {
        iotx_state_event(ITE_STATE_USER_INPUT, STATE_USER_INPUT_DS, "invalid device secret");
        return NULL;
    }
#endif /* #ifdef DYNAMIC_REGISTER */

#ifdef MQTT_PRE_AUTH /* preauth mode through https */
    ret = _iotx_preauth(region, &meta_info, (iotx_sign_mqtt_t *)&g_default_sign); /* type convert */
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "invalid device secret");
        mqtt_err("ret = _iotx_preauth() = %d, abort", ret);
        return NULL;
    }

    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)g_default_sign.hostname);

    {
        iotx_sign_mqtt_t tmp_sign;
        memset(&tmp_sign, 0, sizeof(iotx_sign_mqtt_t));

        ret = IOT_Sign_MQTT(region, &meta_info, &tmp_sign);
        if (ret < STATE_SUCCESS) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
            return NULL;
        }

        memcpy(g_default_sign.username, tmp_sign.username, strlen(tmp_sign.username));
        memcpy(g_default_sign.password, tmp_sign.password, strlen(tmp_sign.password));
        memcpy(g_default_sign.clientid, tmp_sign.clientid, strlen(tmp_sign.clientid));
    }
#else /* direct mode */
    ret = IOT_Sign_MQTT(region, &meta_info, &g_default_sign);
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
        return NULL;
    }
#endif /* #ifdef MQTT_PRE_AUTH */

    /* setup device_id */
    memcpy(device_id, meta_info.product_key, strlen(meta_info.product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta_info.device_name, strlen(meta_info.device_name));

    /* reconfig clientid, append custome clientKV and itls switch flag */
    if (pInitParams != NULL && pInitParams->customize_info != NULL) {
        if (strstr(pInitParams->customize_info, "authtype=id2") != NULL) {
            enable_itls = 1;
        } else {
            enable_itls = 0;
        }
    }

    ret = _sign_get_clientid(g_default_sign.clientid, device_id, (pInitParams != NULL) ? pInitParams->customize_info : NULL,
                             enable_itls);
    if (ret < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt sign fail");
        return NULL;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

#ifdef SUPPORT_TLS
    {
        extern const char *iotx_ca_crt;
        if (enable_itls == 0) {
            mqtt_params.pub_key = iotx_ca_crt;
        } else {
            memset(iotx_ca_crt_itls, 0, sizeof(iotx_ca_crt_itls));
            IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, iotx_ca_crt_itls);
            IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_SECRET, iotx_ca_crt_itls + strlen(iotx_ca_crt_itls) + 1);
            mqtt_params.pub_key = iotx_ca_crt_itls;
        }
    }
#endif
    mqtt_params.request_timeout_ms    = CONFIG_MQTT_REQUEST_TIMEOUT;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000;
    mqtt_params.read_buf_size         = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.write_buf_size        = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.handle_event.h_fp     = NULL;
    mqtt_params.handle_event.pcontext = NULL;

    /* optional configuration */
    if (pInitParams != NULL) {
        if (pInitParams->host && strlen(pInitParams->host)) {
            mqtt_params.host = pInitParams->host;
        } else {
            mqtt_warning("Using default hostname: '%s'", g_default_sign.hostname);
            mqtt_params.host = g_default_sign.hostname;
        }

        if (pInitParams->port) {
            mqtt_params.port = pInitParams->port;
        } else {
            mqtt_warning("Using default port: [%d]", g_default_sign.port);
            mqtt_params.port = g_default_sign.port;
        }

        if (pInitParams->client_id && strlen(pInitParams->client_id)) {
            mqtt_params.client_id = pInitParams->client_id;
        } else {
            mqtt_warning("Using default client_id: %s", g_default_sign.clientid);
            mqtt_params.client_id = g_default_sign.clientid;
        }

        if (pInitParams->username && strlen(pInitParams->username)) {
            mqtt_params.username = pInitParams->username;
        } else {
            mqtt_warning("Using default username: %s", g_default_sign.username);
            mqtt_params.username = g_default_sign.username;
        }

        if (pInitParams->password && strlen(pInitParams->password)) {
            mqtt_params.password = pInitParams->password;
        } else {
#if 1
            mqtt_warning("Using default password: %s", "******");
#else
            mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
            mqtt_params.password = g_default_sign.password;
        }

        if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN ||
            pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {
            mqtt_warning("Using default request_timeout_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.request_timeout_ms,
                         pInitParams->request_timeout_ms,
                         CONFIG_MQTT_REQ_TIMEOUT_MIN,
                         CONFIG_MQTT_REQ_TIMEOUT_MAX);
        } else {
            mqtt_params.request_timeout_ms = pInitParams->request_timeout_ms;
        }

        if (pInitParams->clean_session == 0 || pInitParams->clean_session == 1) {
            mqtt_params.clean_session = pInitParams->clean_session;
        }

        if (pInitParams->keepalive_interval_ms < CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000 ||
            pInitParams->keepalive_interval_ms > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000) {
            mqtt_warning("Using default keepalive_interval_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.keepalive_interval_ms,
                         pInitParams->keepalive_interval_ms,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000);
        } else {
            mqtt_params.keepalive_interval_ms = pInitParams->keepalive_interval_ms;
        }

        if (!pInitParams->read_buf_size) {
            mqtt_warning("Using default read_buf_size: %d", mqtt_params.read_buf_size);
        } else {
            mqtt_params.read_buf_size = pInitParams->read_buf_size;
        }

        if (!pInitParams->write_buf_size) {
            mqtt_warning("Using default write_buf_size: %d", mqtt_params.write_buf_size);
        } else {
            mqtt_params.write_buf_size = pInitParams->write_buf_size;
        }

        if (pInitParams->handle_event.h_fp != NULL) {
            mqtt_params.handle_event.h_fp = pInitParams->handle_event.h_fp;
        }

        if (pInitParams->handle_event.pcontext != NULL) {
            mqtt_params.handle_event.pcontext = pInitParams->handle_event.pcontext;
        }
    } else {
        mqtt_warning("Using default port: [%d]", g_default_sign.port);
        mqtt_params.port = g_default_sign.port;

        mqtt_warning("Using default hostname: '%s'", g_default_sign.hostname);
        mqtt_params.host = g_default_sign.hostname;

        mqtt_warning("Using default client_id: %s", g_default_sign.clientid);
        mqtt_params.client_id = g_default_sign.clientid;

        mqtt_warning("Using default username: %s", g_default_sign.username);
        mqtt_params.username = g_default_sign.username;

#if 1
        mqtt_warning("Using default password: %s", "******");
#else
        mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
        mqtt_params.password = g_default_sign.password;
    }

    pclient = wrapper_mqtt_init(&mqtt_params);
    if (pclient == NULL) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_WRAPPER_INIT_FAIL, "mqtt wrapper init fail");
        return NULL;
    }

    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.host);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%d", (int)mqtt_params.port);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", strstr(mqtt_params.client_id, ",") + 1);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.username);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CONN_USER_INFO, "%s", mqtt_params.password);

    time_now = HAL_UptimeMs();
    ret = wrapper_mqtt_connect(pclient);
    if (ret < STATE_SUCCESS) {
        if (MQTT_CONNECT_BLOCK != ret) {
            iotx_state_event(ITE_STATE_MQTT_COMM, ret, "mqtt connect failed - ret = %d", ret);
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SUCCESS, "mqtt connected in %d ms", (int)(HAL_UptimeMs() - time_now));

#ifndef ASYNC_PROTOCOL_STACK
    iotx_mqtt_report_funcs(pclient);
#endif

    g_mqtt_client = pclient;

    /* Mqtt Connect Callback */
    callback = iotx_event_callback(ITE_MQTT_CONNECT_SUCC);
    if (callback) {
        ((int (*)(void))callback)();
    }

    return pclient;
}

int IOT_MQTT_Destroy(void **phandler)
{
    void *client;
    if (phandler != NULL) {
        client = *phandler;
        *phandler = NULL;
    } else {
        client = g_mqtt_client;
    }

    if (client == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    wrapper_mqtt_release(&client);
    g_mqtt_client = NULL;

    return STATE_SUCCESS;
}

int IOT_MQTT_Yield(void *handle, int timeout_ms)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    return wrapper_mqtt_yield(pClient, timeout_ms);
}

/* check whether MQTT connection is established or not */
int IOT_MQTT_CheckStateNormal(void *handle)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    if (pClient == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    return wrapper_mqtt_check_state(pClient);
}

int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext)
{
    void *client = handle ? handle : g_mqtt_client;

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_SUB_INFO, "sub - '%s'", topic_filter);
    return wrapper_mqtt_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
}

#define SUBSCRIBE_SYNC_TIMEOUT_MAX 10000
int IOT_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms)
{
    void *client = handle ? handle : g_mqtt_client;

    if (timeout_ms > SUBSCRIBE_SYNC_TIMEOUT_MAX) {
        timeout_ms = SUBSCRIBE_SYNC_TIMEOUT_MAX;
    }

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_SUB_INFO, "subs - '%s'", topic_filter);
    return wrapper_mqtt_subscribe_sync(client, topic_filter, qos, topic_handle_func, pcontext, timeout_ms);
}

int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    void *client = handle ? handle : g_mqtt_client;


    if (client == NULL || topic_filter == NULL || strlen(topic_filter) == 0) {
        return STATE_USER_INPUT_INVALID;
    }

    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_SUB_INFO, "unsub - '%s'", topic_filter);
    return wrapper_mqtt_unsubscribe(client, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    void *client = handle ? handle : g_mqtt_client;
    int                 rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return STATE_USER_INPUT_INVALID;
    }

    rc = wrapper_mqtt_publish(client, topic_name, topic_msg);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_PUB_INFO, "pub - '%s': %d", topic_name, rc);
    return rc;
}

int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;
    int rc;
    void *client = handle ? handle : g_mqtt_client;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        return STATE_USER_INPUT_INVALID;
    }

    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    rc = wrapper_mqtt_publish(client, topic_name, &mqtt_msg);
    iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_PUB_INFO, "pub - '%s': %d", topic_name, rc);
    return rc;
}

int IOT_MQTT_Nwk_Event_Handler(void *handle, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param)
{
#ifdef ASYNC_PROTOCOL_STACK
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || event >= IOTX_MQTT_SOC_MAX || param == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_nwk_event_handler(client, event, param);

    if (rc < 0) {
        return rc;
    }

    switch (event) {
        case IOTX_MQTT_SOC_CONNECTED: {
            iotx_mqtt_report_funcs(client);
        }
        break;
        default: {
        }
        break;
    }

    return rc;
#else
    return STATE_MQTT_ASYNC_STACK_NOT_SUPPORT;
#endif
}
