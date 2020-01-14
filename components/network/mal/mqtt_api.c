#include <string.h>

#include "linkkit/mqtt_api.h"

#include "ulog/ulog.h"
#define MQTT_TAG "MAL_MQTT"

#define mqtt_emerg(format, arg...)   LOGI(MQTT_TAG, format, ##arg)
#define mqtt_crit(format, arg...)    LOGI(MQTT_TAG, format, ##arg)
#define mqtt_err(format, arg...)     LOGE(MQTT_TAG, format, ##arg)
#define mqtt_warning(format, arg...) LOGW(MQTT_TAG, format, ##arg)
#define mqtt_info(format, arg...)    LOGI(MQTT_TAG, format, ##arg)
#define mqtt_debug(format, arg...)   LOGD(MQTT_TAG, format, ##arg)

#define mqtt_api_malloc(size)            HAL_Malloc(size)
#define mqtt_api_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}

static void        *g_mqtt_client = NULL;

/************************  Public Interface ************************/
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    void *pclient;
    iotx_mqtt_param_t mqtt_params;
    int ret;
    void *callback;

    if (g_mqtt_client != NULL) {
        mqtt_err("Already exist default MQTT connection, won't proceed another one");
        return g_mqtt_client;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

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
            mqtt_err("Hostname missing");
            return NULL;
        }

        if (pInitParams->port) {
            mqtt_params.port = pInitParams->port;
        } else {
            mqtt_err("Port missing");
            return NULL;
        }

        if (pInitParams->client_id && strlen(pInitParams->client_id)) {
            mqtt_params.client_id = pInitParams->client_id;
        } else {
            mqtt_err("Client missing");
            return NULL;
        }

        if (pInitParams->username && strlen(pInitParams->username)) {
            mqtt_params.username = pInitParams->username;
        } else {
            mqtt_err("Username missing");
            return NULL;
        }

        if (pInitParams->password && strlen(pInitParams->password)) {
            mqtt_params.password = pInitParams->password;
        } else {
            mqtt_err("Password missing");
            return NULL;
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
    }

    pclient = wrapper_mqtt_init(&mqtt_params);
    if (pclient == NULL) {
        mqtt_err("wrapper_mqtt_init error");
        return NULL;
    }

    ret = wrapper_mqtt_connect(pclient);
    if (SUCCESS_RETURN != ret) {
        if (MQTT_CONNECT_BLOCK != ret) {
            mqtt_err("wrapper_mqtt_connect failed");
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }

    g_mqtt_client = pclient;

#if 0
    /* Mqtt Connect Callback */
    callback = iotx_event_callback(ITE_MQTT_CONNECT_SUCC);
    if (callback) {
        ((int (*)(void))callback)();
    }
#endif

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
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
    }

    wrapper_mqtt_release(&client);
    g_mqtt_client = NULL;

    return SUCCESS_RETURN;
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
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
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
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

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
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

    return wrapper_mqtt_subscribe_sync(client, topic_filter, qos, topic_handle_func, pcontext, timeout_ms);
}

int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    void *client = handle ? handle : g_mqtt_client;


    if (client == NULL || topic_filter == NULL || strlen(topic_filter) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    return wrapper_mqtt_unsubscribe(client, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    void *client = handle ? handle : g_mqtt_client;
    int                 rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_publish(client, topic_name, topic_msg);
    return rc;
}

int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    rc = wrapper_mqtt_publish(client, topic_name, &mqtt_msg);

    if (rc < 0) {
        mqtt_err("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return rc;
}

int IOT_MQTT_Nwk_Event_Handler(void *handle, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param)
{
    return -1;
}
