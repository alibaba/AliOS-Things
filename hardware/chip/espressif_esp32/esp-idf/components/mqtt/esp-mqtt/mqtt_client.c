#include <stdio.h>
#include "platform.h"

#include "mqtt_client.h"
#include "mqtt_msg.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
#include "mqtt_outbox.h"
#include "mqtt_supported_features.h"

/* using uri parser */
#include "http_parser.h"

#ifdef MQTT_DISABLE_API_LOCKS
# define MQTT_API_LOCK(c)
# define MQTT_API_UNLOCK(c)
#else
# define MQTT_API_LOCK(c)          xSemaphoreTakeRecursive(c->api_lock, portMAX_DELAY)
# define MQTT_API_UNLOCK(c)        xSemaphoreGiveRecursive(c->api_lock)
#endif /* MQTT_USE_API_LOCKS */

_Static_assert(sizeof(uint64_t) == sizeof(outbox_tick_t), "mqtt-client tick type size different from outbox tick type");
#ifdef ESP_EVENT_ANY_ID
_Static_assert(MQTT_EVENT_ANY == ESP_EVENT_ANY_ID, "mqtt-client event enum does not match the global EVENT_ANY_ID");
#endif

static const char *TAG = "MQTT_CLIENT";

#ifdef MQTT_SUPPORTED_FEATURE_EVENT_LOOP
/**
 * @brief Define of MQTT Event base
 *
 */
ESP_EVENT_DEFINE_BASE(MQTT_EVENTS);
#endif

typedef struct mqtt_state
{
    mqtt_connect_info_t *connect_info;
    uint8_t *in_buffer;
    uint8_t *out_buffer;
    int in_buffer_length;
    int out_buffer_length;
    uint32_t message_length;
    uint32_t in_buffer_read_len;
    mqtt_message_t *outbound_message;
    mqtt_connection_t mqtt_connection;
    uint16_t pending_msg_id;
    int pending_msg_type;
    int pending_publish_qos;
    int pending_msg_count;
} mqtt_state_t;

typedef struct {
    mqtt_event_callback_t event_handle;
    esp_event_loop_handle_t event_loop_handle;
    int task_stack;
    int task_prio;
    char *uri;
    char *host;
    char *path;
    char *scheme;
    int port;
    bool auto_reconnect;
    void *user_context;
    int network_timeout_ms;
    int refresh_connection_after_ms;
    int reconnect_timeout_ms;
    char **alpn_protos;
    int num_alpn_protos;
    char *clientkey_password;
    int clientkey_password_len;
    bool use_global_ca_store;
    const char *cacert_buf;
    size_t cacert_bytes;
    const char *clientcert_buf;
    size_t clientcert_bytes;
    const char *clientkey_buf;
    size_t clientkey_bytes;
    const struct psk_key_hint *psk_hint_key;
    bool skip_cert_common_name_check;
    bool use_secure_element;
    void *ds_data;
} mqtt_config_storage_t;

typedef enum {
    MQTT_STATE_ERROR = -1,
    MQTT_STATE_UNKNOWN = 0,
    MQTT_STATE_INIT,
    MQTT_STATE_CONNECTED,
    MQTT_STATE_WAIT_TIMEOUT,
} mqtt_client_state_t;

/* State values for reading MQTT message header */
typedef enum {
    MQTT_HEADER_STATE_INCOMPLETE = -1,
    MQTT_HEADER_STATE_COMPLETE = 0,
} mqtt_header_state_t;

struct esp_mqtt_client {
    esp_transport_list_handle_t transport_list;
    esp_transport_handle_t transport;
    mqtt_config_storage_t *config;
    mqtt_state_t  mqtt_state;
    mqtt_connect_info_t connect_info;
    mqtt_client_state_t state;
    uint64_t refresh_connection_tick;
    uint64_t keepalive_tick;
    uint64_t reconnect_tick;
    int wait_timeout_ms;
    int auto_reconnect;
    esp_mqtt_event_t event;
    bool run;
    bool wait_for_ping_resp;
    outbox_handle_t outbox;
    EventGroupHandle_t status_bits;
    SemaphoreHandle_t  api_lock;
    TaskHandle_t       task_handle;
};

const static int STOPPED_BIT = BIT0;
const static int RECONNECT_BIT = BIT1;
const static int DISCONNECT_BIT = BIT2;

static esp_err_t esp_mqtt_dispatch_event(esp_mqtt_client_handle_t client);
static esp_err_t esp_mqtt_dispatch_event_with_msgid(esp_mqtt_client_handle_t client);
static esp_err_t esp_mqtt_destroy_config(esp_mqtt_client_handle_t client);
static esp_err_t esp_mqtt_connect(esp_mqtt_client_handle_t client, int timeout_ms);
static esp_err_t esp_mqtt_abort_connection(esp_mqtt_client_handle_t client);
static esp_err_t esp_mqtt_client_ping(esp_mqtt_client_handle_t client);
static char *create_string(const char *ptr, int len);
static int mqtt_message_receive(esp_mqtt_client_handle_t client, int read_poll_timeout_ms);
static void esp_mqtt_client_dispatch_tls_error(esp_mqtt_client_handle_t client);


#if MQTT_ENABLE_SSL
enum esp_mqtt_ssl_cert_key_api {
    MQTT_SSL_DATA_API_CA_CERT,
    MQTT_SSL_DATA_API_CLIENT_CERT,
    MQTT_SSL_DATA_API_CLIENT_KEY,
    MQTT_SSL_DATA_API_MAX,
};

static esp_err_t esp_mqtt_set_cert_key_data(esp_transport_handle_t ssl, enum esp_mqtt_ssl_cert_key_api what, const char *cert_key_data, int cert_key_len)
{
    char *data = (char *)cert_key_data;
    int ssl_transport_api_id = what;
    int len = cert_key_len;

    if (!data) {
        return ESP_OK;
    }

    if (len == 0) {
        // if length not specified, expect 0-terminated PEM string
        // and the original transport_api_id (by convention after the last api_id in the enum)
        ssl_transport_api_id += MQTT_SSL_DATA_API_MAX;
        len = strlen(data);
    }
#ifndef MQTT_SUPPORTED_FEATURE_DER_CERTIFICATES
    else {
        ESP_LOGE(TAG, "Explicit cert-/key-len is not available in IDF version %s", IDF_VER);
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    // option to force the cert/key config to null (i.e. skip validation) when existing config updates
    if (0 == strcmp(data, "NULL")) {
        data = NULL;
        len = 0;
    }

    switch (ssl_transport_api_id) {
#ifdef MQTT_SUPPORTED_FEATURE_DER_CERTIFICATES
        case MQTT_SSL_DATA_API_CA_CERT:
            esp_transport_ssl_set_cert_data_der(ssl, data, len);
            break;
        case MQTT_SSL_DATA_API_CLIENT_CERT:
            esp_transport_ssl_set_client_cert_data_der(ssl, data, len);
            break;
        case MQTT_SSL_DATA_API_CLIENT_KEY:
            esp_transport_ssl_set_client_key_data_der(ssl, data, len);
            break;
#endif
        case MQTT_SSL_DATA_API_CA_CERT + MQTT_SSL_DATA_API_MAX:
            esp_transport_ssl_set_cert_data(ssl, data, len);
            break;
        case MQTT_SSL_DATA_API_CLIENT_CERT + MQTT_SSL_DATA_API_MAX:
            esp_transport_ssl_set_client_cert_data(ssl, data, len);
            break;
        case MQTT_SSL_DATA_API_CLIENT_KEY + MQTT_SSL_DATA_API_MAX:
            esp_transport_ssl_set_client_key_data(ssl, data, len);
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t esp_mqtt_set_ssl_transport_properties(esp_transport_list_handle_t transport_list, mqtt_config_storage_t *cfg)
{
    esp_transport_handle_t ssl = esp_transport_list_get_transport(transport_list, "mqtts");

    if (cfg->use_global_ca_store == true) {
        esp_transport_ssl_enable_global_ca_store(ssl);
    } else {
        ESP_OK_CHECK(TAG, esp_mqtt_set_cert_key_data(ssl, MQTT_SSL_DATA_API_CA_CERT, cfg->cacert_buf, cfg->cacert_bytes),
                     goto esp_mqtt_set_transport_failed);
    }

    if (cfg->use_secure_element) {
#ifdef MQTT_SUPPORTED_FEATURE_SECURE_ELEMENT
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
        esp_transport_ssl_use_secure_element(ssl);
#else
        ESP_LOGE(TAG, "Secure element not enabled for esp-tls in menuconfig");
        goto esp_mqtt_set_transport_failed;
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */
#else
        ESP_LOGE(TAG, "Secure element feature is not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif /* MQTT_SUPPORTED_FEATURE_SECURE_ELEMENT */
    }

    if(cfg->ds_data != NULL) {
#ifdef MQTT_SUPPORTED_FEATURE_DIGITAL_SIGNATURE
#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
        esp_transport_ssl_set_ds_data(ssl, cfg->ds_data);
#else
        ESP_LOGE(TAG, "Digital Signature not enabled for esp-tls in menuconfig");
        goto esp_mqtt_set_transport_failed;
#endif /* CONFIG_ESP_TLS_USE_DS_PERIPHERAL */
#else
        ESP_LOGE(TAG, "Digital Signature feature is not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif
    }
    ESP_OK_CHECK(TAG, esp_mqtt_set_cert_key_data(ssl, MQTT_SSL_DATA_API_CLIENT_CERT, cfg->clientcert_buf, cfg->clientcert_bytes),
                 goto esp_mqtt_set_transport_failed);
    ESP_OK_CHECK(TAG, esp_mqtt_set_cert_key_data(ssl, MQTT_SSL_DATA_API_CLIENT_KEY, cfg->clientkey_buf, cfg->clientkey_bytes),
                 goto esp_mqtt_set_transport_failed);

    if (cfg->clientkey_password && cfg->clientkey_password_len) {
#if defined(MQTT_SUPPORTED_FEATURE_CLIENT_KEY_PASSWORD) && MQTT_ENABLE_SSL
        esp_transport_ssl_set_client_key_password(ssl,
                                                  cfg->clientkey_password,
                                                  cfg->clientkey_password_len);
#else
        ESP_LOGE(TAG, "Password protected keys are not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif
    }

    if (cfg->psk_hint_key) {
#if defined(MQTT_SUPPORTED_FEATURE_PSK_AUTHENTICATION) && MQTT_ENABLE_SSL
        esp_transport_ssl_set_psk_key_hint(ssl, cfg->psk_hint_key);
#else
        ESP_LOGE(TAG, "PSK authentication is not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif
    }

    if (cfg->alpn_protos) {
#if defined(MQTT_SUPPORTED_FEATURE_ALPN) && MQTT_ENABLE_SSL
        esp_transport_ssl_set_alpn_protocol(ssl, (const char **)cfg->alpn_protos);
#else
        ESP_LOGE(TAG, "APLN is not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif
    }


    if (cfg->skip_cert_common_name_check) {
#if defined(MQTT_SUPPORTED_FEATURE_SKIP_CRT_CMN_NAME_CHECK) && MQTT_ENABLE_SSL
        esp_transport_ssl_skip_common_name_check(ssl);
#else
        ESP_LOGE(TAG, "Skip certificate common name check is not available in IDF version %s", IDF_VER);
        goto esp_mqtt_set_transport_failed;
#endif
    }

    return ESP_OK;

    esp_mqtt_set_transport_failed:
    return ESP_FAIL;
}
#endif // MQTT_ENABLE_SSL

/* Checks if the user supplied config values are internally consistent */
static esp_err_t esp_mqtt_check_cfg_conflict(const mqtt_config_storage_t *cfg, const esp_mqtt_client_config_t *user_cfg)
{
    esp_err_t ret = ESP_OK;

    bool ssl_cfg_enabled = cfg->use_global_ca_store || cfg->cacert_buf || cfg->clientcert_buf || cfg->psk_hint_key || cfg->alpn_protos;
    bool is_ssl_scheme = false;
    if (cfg->scheme) {
        is_ssl_scheme = (strcasecmp(cfg->scheme, "mqtts") == 0) || (strcasecmp(cfg->scheme, "wss") == 0);
    }

    if (!is_ssl_scheme && ssl_cfg_enabled) {
        if (cfg->uri) {
            ESP_LOGW(TAG, "SSL related configs set, but the URI scheme specifies a non-SSL scheme, scheme = %s", cfg->scheme);
        } else {
            ESP_LOGW(TAG, "SSL related configs set, but the transport protocol is a non-SSL scheme, transport = %d", user_cfg->transport);
        }
        ret = ESP_ERR_INVALID_ARG;
    }

    if (cfg->uri && user_cfg->transport) {
        ESP_LOGW(TAG, "Transport config set, but overridden by scheme from URI: transport = %d, uri scheme = %s", user_cfg->transport, cfg->scheme);
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

esp_err_t esp_mqtt_set_config(esp_mqtt_client_handle_t client, const esp_mqtt_client_config_t *config)
{
    MQTT_API_LOCK(client);
    //Copy user configurations to client context
    esp_err_t err = ESP_OK;
    mqtt_config_storage_t *cfg;
    if (client->config) {
        cfg = client->config;
    } else {
        cfg = calloc(1, sizeof(mqtt_config_storage_t));
        ESP_MEM_CHECK(TAG, cfg, {
            MQTT_API_UNLOCK(client);
            return ESP_ERR_NO_MEM;
        });
        client->config = cfg;
    }
    if (config->task_prio) {
        cfg->task_prio = config->task_prio;
    }

    if (cfg->task_prio <= 0) {
        cfg->task_prio = MQTT_TASK_PRIORITY;
    }
    if (config->task_stack) {
        cfg->task_stack = config->task_stack;
    }
    if (cfg->task_stack == 0) {
        cfg->task_stack = MQTT_TASK_STACK;
    }
    if (config->port) {
        cfg->port = config->port;
    }

    err = ESP_ERR_NO_MEM;
    if (config->host) {
        free(cfg->host);
        cfg->host = strdup(config->host);
        ESP_MEM_CHECK(TAG, cfg->host, goto _mqtt_set_config_failed);
    }

    if (config->username) {
        free(client->connect_info.username);
        client->connect_info.username = strdup(config->username);
        ESP_MEM_CHECK(TAG, client->connect_info.username, goto _mqtt_set_config_failed);
    }

    if (config->password) {
        free(client->connect_info.password);
        client->connect_info.password = strdup(config->password);
        ESP_MEM_CHECK(TAG, client->connect_info.password, goto _mqtt_set_config_failed);
    }

    if (config->client_id) {
        free(client->connect_info.client_id);
        client->connect_info.client_id = strdup(config->client_id);
        ESP_MEM_CHECK(TAG, client->connect_info.client_id, goto _mqtt_set_config_failed);
    } else if (client->connect_info.client_id == NULL) {
        client->connect_info.client_id = platform_create_id_string();
    }

    ESP_MEM_CHECK(TAG, client->connect_info.client_id, goto _mqtt_set_config_failed);
    ESP_LOGD(TAG, "MQTT client_id=%s", client->connect_info.client_id);

    if (config->uri) {
        free(cfg->uri);
        cfg->uri = strdup(config->uri);
        ESP_MEM_CHECK(TAG, cfg->uri, goto _mqtt_set_config_failed);
    }

    if (config->lwt_topic) {
        free(client->connect_info.will_topic);
        client->connect_info.will_topic = strdup(config->lwt_topic);
        ESP_MEM_CHECK(TAG, client->connect_info.will_topic, goto _mqtt_set_config_failed);
    }

    if (config->lwt_msg_len && config->lwt_msg) {
        free(client->connect_info.will_message);
        client->connect_info.will_message = malloc(config->lwt_msg_len);
        ESP_MEM_CHECK(TAG, client->connect_info.will_message, goto _mqtt_set_config_failed);
        memcpy(client->connect_info.will_message, config->lwt_msg, config->lwt_msg_len);
        client->connect_info.will_length = config->lwt_msg_len;
    } else if (config->lwt_msg) {
        free(client->connect_info.will_message);
        client->connect_info.will_message = strdup(config->lwt_msg);
        ESP_MEM_CHECK(TAG, client->connect_info.will_message, goto _mqtt_set_config_failed);
        client->connect_info.will_length = strlen(config->lwt_msg);
    }
    if (config->lwt_qos) {
        client->connect_info.will_qos = config->lwt_qos;
    }
    if (config->lwt_retain) {
        client->connect_info.will_retain = config->lwt_retain;
    }

    if (config->disable_clean_session == client->connect_info.clean_session) {
        client->connect_info.clean_session = !config->disable_clean_session;
    }
    if (config->keepalive) {
        client->connect_info.keepalive = config->keepalive;
    }
    if (client->connect_info.keepalive == 0) {
        client->connect_info.keepalive = MQTT_KEEPALIVE_TICK;
    }

    if (config->protocol_ver) {
        client->connect_info.protocol_ver = config->protocol_ver;
    }
    if (client->connect_info.protocol_ver== MQTT_PROTOCOL_UNDEFINED) {
#ifdef MQTT_PROTOCOL_311
        client->connect_info.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
#else
        client->connect_info.protocol_ver = MQTT_PROTOCOL_V_3_1;
#endif
    }

    cfg->network_timeout_ms = MQTT_NETWORK_TIMEOUT_MS;
    if (config->user_context) {
        cfg->user_context = config->user_context;
    }

    if (config->event_handle) {
        cfg->event_handle = config->event_handle;
    }

    if (config->refresh_connection_after_ms) {
        cfg->refresh_connection_after_ms = config->refresh_connection_after_ms;
    }

    cfg->auto_reconnect = true;
    if (config->disable_auto_reconnect == cfg->auto_reconnect) {
        cfg->auto_reconnect = !config->disable_auto_reconnect;
    }

    if (config->reconnect_timeout_ms) {
        cfg->reconnect_timeout_ms = config->reconnect_timeout_ms;
    } else {
        cfg->reconnect_timeout_ms = MQTT_RECON_DEFAULT_MS;
    }

    if (config->alpn_protos) {
        for (int i = 0; i < cfg->num_alpn_protos; i++) {
            free(cfg->alpn_protos[i]);
        }
        free(cfg->alpn_protos);
        cfg->num_alpn_protos = 0;

        const char **p;

        for (p = config->alpn_protos; *p != NULL; p++ ) {
            cfg->num_alpn_protos++;
        }
        // mbedTLS expects the list to be null-terminated
        cfg->alpn_protos = calloc(cfg->num_alpn_protos + 1, sizeof(config->alpn_protos));
        ESP_MEM_CHECK(TAG, cfg->alpn_protos, goto _mqtt_set_config_failed);

        for (int i = 0; i < cfg->num_alpn_protos; i++) {
            cfg->alpn_protos[i] = strdup(config->alpn_protos[i]);
            ESP_MEM_CHECK(TAG, cfg->alpn_protos[i], goto _mqtt_set_config_failed);
        }
    }

    // configure ssl related parameters
    cfg->use_global_ca_store = config->use_global_ca_store;
    cfg->cacert_buf = config->cert_pem;
    cfg->cacert_bytes = config->cert_len;
    cfg->clientcert_buf = config->client_cert_pem;
    cfg->clientcert_bytes = config->client_cert_len;
    cfg->clientkey_buf = config->client_key_pem;
    cfg->clientkey_bytes = config->client_key_len;
    cfg->psk_hint_key = config->psk_hint_key;
    cfg->skip_cert_common_name_check = config->skip_cert_common_name_check;
    cfg->use_secure_element = config->use_secure_element;
    cfg->ds_data = config->ds_data;

    if (config->clientkey_password && config->clientkey_password_len) {
        cfg->clientkey_password_len = config->clientkey_password_len;
        cfg->clientkey_password = malloc(cfg->clientkey_password_len);
        memcpy(cfg->clientkey_password, config->clientkey_password, cfg->clientkey_password_len);
    }

    if (config->transport) {
        free(client->config->scheme);
        if (config->transport == MQTT_TRANSPORT_OVER_WS) {
            cfg->scheme = create_string("ws", 2);
            ESP_MEM_CHECK(TAG, cfg->scheme, goto _mqtt_set_config_failed);
        } else if (config->transport == MQTT_TRANSPORT_OVER_TCP) {
            cfg->scheme = create_string("mqtt", 4);
            ESP_MEM_CHECK(TAG, cfg->scheme, goto _mqtt_set_config_failed);
        } else if (config->transport == MQTT_TRANSPORT_OVER_SSL) {
            cfg->scheme = create_string("mqtts", 5);
            ESP_MEM_CHECK(TAG, cfg->scheme, goto _mqtt_set_config_failed);
        } else if (config->transport == MQTT_TRANSPORT_OVER_WSS) {
            cfg->scheme = create_string("wss", 3);
            ESP_MEM_CHECK(TAG, cfg->scheme, goto _mqtt_set_config_failed);
        }
    }

    // Set uri at the end of config to override separately configured uri elements
    if (config->uri) {
        if (esp_mqtt_client_set_uri(client, cfg->uri) != ESP_OK) {
            err = ESP_FAIL;
            goto _mqtt_set_config_failed;
        }
    }
    esp_mqtt_check_cfg_conflict(cfg, config);

    MQTT_API_UNLOCK(client);

    return ESP_OK;
_mqtt_set_config_failed:
    esp_mqtt_destroy_config(client);
    MQTT_API_UNLOCK(client);
    return err;
}

static esp_err_t esp_mqtt_destroy_config(esp_mqtt_client_handle_t client)
{
    mqtt_config_storage_t *cfg = client->config;
    if (cfg == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    free(cfg->host);
    free(cfg->uri);
    free(cfg->path);
    free(cfg->scheme);
    for (int i = 0; i < cfg->num_alpn_protos; i++) {
            free(cfg->alpn_protos[i]);
    }
    free(cfg->alpn_protos);
    free(cfg->clientkey_password);
    free(client->connect_info.will_topic);
    free(client->connect_info.will_message);
    free(client->connect_info.client_id);
    free(client->connect_info.username);
    free(client->connect_info.password);
    memset(&client->connect_info, 0, sizeof(mqtt_connect_info_t));
#ifdef MQTT_SUPPORTED_FEATURE_EVENT_LOOP
    if (client->config->event_loop_handle) {
        esp_event_loop_delete(client->config->event_loop_handle);
    }
#endif
    memset(cfg, 0, sizeof(mqtt_config_storage_t));
    free(client->config);
    client->config = NULL;
    return ESP_OK;
}

static esp_err_t esp_mqtt_connect(esp_mqtt_client_handle_t client, int timeout_ms)
{
    int write_len, read_len, connect_rsp_code;
    client->wait_for_ping_resp = false;
    client->mqtt_state.outbound_message = mqtt_msg_connect(&client->mqtt_state.mqtt_connection,
                                          client->mqtt_state.connect_info);
    if (client->mqtt_state.outbound_message->length == 0) {
        ESP_LOGE(TAG, "Connect message cannot be created");
        return ESP_FAIL;
    }

    client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
    client->mqtt_state.pending_msg_id = mqtt_get_id(client->mqtt_state.outbound_message->data,
                                        client->mqtt_state.outbound_message->length);
    ESP_LOGD(TAG, "Sending MQTT CONNECT message, type: %d, id: %04X",
             client->mqtt_state.pending_msg_type,
             client->mqtt_state.pending_msg_id);

    write_len = esp_transport_write(client->transport,
                                    (char *)client->mqtt_state.outbound_message->data,
                                    client->mqtt_state.outbound_message->length,
                                    client->config->network_timeout_ms);
    if (write_len < 0) {
        ESP_LOGE(TAG, "Writing failed, errno= %d", errno);
        return ESP_FAIL;
    }

    client->mqtt_state.in_buffer_read_len = 0;
    client->mqtt_state.message_length = 0;

    /* wait configured network timeout for broker connection response */
    uint64_t connack_recv_started = platform_tick_get_ms();
    do {
        read_len = mqtt_message_receive(client, client->config->network_timeout_ms);
    } while (read_len == 0 && platform_tick_get_ms() - connack_recv_started < client->config->network_timeout_ms);

    if (read_len <= 0) {
        ESP_LOGE(TAG, "%s: mqtt_message_receive() returned %d", __func__, read_len);
        return ESP_FAIL;
    }

    if (mqtt_get_type(client->mqtt_state.in_buffer) != MQTT_MSG_TYPE_CONNACK) {
        ESP_LOGE(TAG, "Invalid MSG_TYPE response: %d, read_len: %d", mqtt_get_type(client->mqtt_state.in_buffer), read_len);
        return ESP_FAIL;
    }
    client->mqtt_state.in_buffer_read_len = 0;
    connect_rsp_code = mqtt_get_connect_return_code(client->mqtt_state.in_buffer);
    if (connect_rsp_code == MQTT_CONNECTION_ACCEPTED) {
        ESP_LOGD(TAG, "Connected");
        return ESP_OK;
    }
    switch (connect_rsp_code) {
    case MQTT_CONNECTION_REFUSE_PROTOCOL:
        ESP_LOGW(TAG, "Connection refused, bad protocol");
        break;
    case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE:
        ESP_LOGW(TAG, "Connection refused, server unavailable");
        break;
    case MQTT_CONNECTION_REFUSE_BAD_USERNAME:
        ESP_LOGW(TAG, "Connection refused, bad username or password");
        break;
    case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED:
        ESP_LOGW(TAG, "Connection refused, not authorized");
        break;
    default:
        ESP_LOGW(TAG, "Connection refused, Unknow reason");
        break;
    }
    /* propagate event with connection refused error */
    client->event.event_id = MQTT_EVENT_ERROR;
    client->event.error_handle->error_type = MQTT_ERROR_TYPE_CONNECTION_REFUSED;
    client->event.error_handle->connect_return_code = connect_rsp_code;
    client->event.error_handle->esp_tls_stack_err = 0;
    client->event.error_handle->esp_tls_last_esp_err = 0;
    client->event.error_handle->esp_tls_cert_verify_flags = 0;
    esp_mqtt_dispatch_event_with_msgid(client);

    return ESP_FAIL;
}

static esp_err_t esp_mqtt_abort_connection(esp_mqtt_client_handle_t client)
{
    MQTT_API_LOCK(client);
    esp_transport_close(client->transport);
    client->wait_timeout_ms = client->config->reconnect_timeout_ms;
    client->reconnect_tick = platform_tick_get_ms();
    client->state = MQTT_STATE_WAIT_TIMEOUT;
    ESP_LOGD(TAG, "Reconnect after %d ms", client->wait_timeout_ms);
    client->event.event_id = MQTT_EVENT_DISCONNECTED;
    client->wait_for_ping_resp = false;
    esp_mqtt_dispatch_event_with_msgid(client);
    MQTT_API_UNLOCK(client);
    return ESP_OK;
}

esp_mqtt_client_handle_t esp_mqtt_client_init(const esp_mqtt_client_config_t *config)
{
    esp_mqtt_client_handle_t client = calloc(1, sizeof(struct esp_mqtt_client));
    ESP_MEM_CHECK(TAG, client, return NULL);
    client->event.error_handle = calloc(1, sizeof(esp_mqtt_error_codes_t));
    if (!client->event.error_handle) {
        free(client);
        return NULL;
    }
    client->api_lock = xSemaphoreCreateRecursiveMutex();
    if (!client->api_lock) {
        free(client->event.error_handle);
        free(client);
        return NULL;
    }

    client->transport_list = esp_transport_list_init();
    ESP_MEM_CHECK(TAG, client->transport_list, goto _mqtt_init_failed);

    esp_transport_handle_t tcp = esp_transport_tcp_init();
    ESP_MEM_CHECK(TAG, tcp, goto _mqtt_init_failed);
    esp_transport_set_default_port(tcp, MQTT_TCP_DEFAULT_PORT);
    esp_transport_list_add(client->transport_list, tcp, "mqtt");

#if MQTT_ENABLE_WS
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    ESP_MEM_CHECK(TAG, ws, goto _mqtt_init_failed);
    esp_transport_set_default_port(ws, MQTT_WS_DEFAULT_PORT);
#ifdef MQTT_SUPPORTED_FEATURE_WS_SUBPROTOCOL
    esp_transport_ws_set_subprotocol(ws, "mqtt");
#endif
    esp_transport_list_add(client->transport_list, ws, "ws");
#endif

#if MQTT_ENABLE_SSL
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    ESP_MEM_CHECK(TAG, ssl, goto _mqtt_init_failed);
    esp_transport_set_default_port(ssl, MQTT_SSL_DEFAULT_PORT);

    esp_transport_list_add(client->transport_list, ssl, "mqtts");
#endif

#if MQTT_ENABLE_WSS
    esp_transport_handle_t wss = esp_transport_ws_init(ssl);
    ESP_MEM_CHECK(TAG, wss, goto _mqtt_init_failed);
#ifdef MQTT_SUPPORTED_FEATURE_WS_SUBPROTOCOL
    esp_transport_ws_set_subprotocol(wss, "mqtt");
#endif
    esp_transport_set_default_port(wss, MQTT_WSS_DEFAULT_PORT);
    esp_transport_list_add(client->transport_list, wss, "wss");
#endif
    ESP_MEM_CHECK(TAG, client->transport_list, goto _mqtt_init_failed);

    if (esp_mqtt_set_config(client, config) != ESP_OK) {
        goto _mqtt_init_failed;
    }
#ifdef MQTT_SUPPORTED_FEATURE_EVENT_LOOP
    esp_event_loop_args_t no_task_loop = {
            .queue_size = 1,
            .task_name = NULL,
    };
    esp_event_loop_create(&no_task_loop, &client->config->event_loop_handle);
#endif

    client->keepalive_tick = platform_tick_get_ms();
    client->reconnect_tick = platform_tick_get_ms();
    client->refresh_connection_tick = platform_tick_get_ms();
    client->wait_for_ping_resp = false;
    int buffer_size = config->buffer_size;
    if (buffer_size <= 0) {
        buffer_size = MQTT_BUFFER_SIZE_BYTE;
    }
    // use separate value for output buffer size if configured
    int out_buffer_size = config->out_buffer_size > 0 ? config->out_buffer_size : buffer_size;

    client->mqtt_state.in_buffer = (uint8_t *)malloc(buffer_size);
    ESP_MEM_CHECK(TAG, client->mqtt_state.in_buffer, goto _mqtt_init_failed);
    client->mqtt_state.in_buffer_length = buffer_size;
    client->mqtt_state.out_buffer = (uint8_t *)malloc(out_buffer_size);
    ESP_MEM_CHECK(TAG, client->mqtt_state.out_buffer, goto _mqtt_init_failed);

    client->mqtt_state.out_buffer_length = out_buffer_size;
    client->mqtt_state.connect_info = &client->connect_info;
    client->outbox = outbox_init();
    ESP_MEM_CHECK(TAG, client->outbox, goto _mqtt_init_failed);
    client->status_bits = xEventGroupCreate();
    ESP_MEM_CHECK(TAG, client->status_bits, goto _mqtt_init_failed);

    mqtt_msg_init(&client->mqtt_state.mqtt_connection, client->mqtt_state.out_buffer,
                            client->mqtt_state.out_buffer_length);

    return client;
_mqtt_init_failed:
    esp_mqtt_client_destroy(client);
    return NULL;
}

esp_err_t esp_mqtt_client_destroy(esp_mqtt_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_mqtt_client_stop(client);
    esp_mqtt_destroy_config(client);
    if (client->transport_list) {
        esp_transport_list_destroy(client->transport_list);
    }
    if (client->outbox) {
        outbox_destroy(client->outbox);
    }
    if (client->status_bits) {
        vEventGroupDelete(client->status_bits);
    }
    free(client->mqtt_state.in_buffer);
    free(client->mqtt_state.out_buffer);
    vSemaphoreDelete(client->api_lock);
    free(client->event.error_handle);
    free(client);
    return ESP_OK;
}

static char *create_string(const char *ptr, int len)
{
    char *ret;
    if (len <= 0) {
        return NULL;
    }
    ret = calloc(1, len + 1);
    ESP_MEM_CHECK(TAG, ret, return NULL);
    memcpy(ret, ptr, len);
    return ret;
}

esp_err_t esp_mqtt_client_set_uri(esp_mqtt_client_handle_t client, const char *uri)
{
    struct http_parser_url puri;
    http_parser_url_init(&puri);
    int parser_status = http_parser_parse_url(uri, strlen(uri), 0, &puri);
    if (parser_status != 0) {
        ESP_LOGE(TAG, "Error parse uri = %s", uri);
        return ESP_FAIL;
    }

    // This API could be also executed when client is active (need to protect config fields)
    MQTT_API_LOCK(client);
    // set uri overrides actual scheme, host, path if configured previously
    free(client->config->scheme);
    free(client->config->host);
    free(client->config->path);

    client->config->scheme = create_string(uri + puri.field_data[UF_SCHEMA].off, puri.field_data[UF_SCHEMA].len);
    client->config->host = create_string(uri + puri.field_data[UF_HOST].off, puri.field_data[UF_HOST].len);
    client->config->path = NULL;

    if (puri.field_data[UF_PATH].len || puri.field_data[UF_QUERY].len) {
        if (puri.field_data[UF_QUERY].len == 0) {
            asprintf(&client->config->path, "%.*s", puri.field_data[UF_PATH].len, uri + puri.field_data[UF_PATH].off);
        } else if (puri.field_data[UF_PATH].len == 0)  {
            asprintf(&client->config->path, "/?%.*s", puri.field_data[UF_QUERY].len, uri + puri.field_data[UF_QUERY].off);
        } else {
            asprintf(&client->config->path, "%.*s?%.*s", puri.field_data[UF_PATH].len, uri + puri.field_data[UF_PATH].off,
                    puri.field_data[UF_QUERY].len, uri + puri.field_data[UF_QUERY].off);
        }
        ESP_MEM_CHECK(TAG, client->config->path, {
            MQTT_API_UNLOCK(client);
            return ESP_ERR_NO_MEM;
        });
    }

    if (client->config->path) {
        esp_transport_handle_t trans = esp_transport_list_get_transport(client->transport_list, "ws");
        if (trans) {
            esp_transport_ws_set_path(trans, client->config->path);
        }
        trans = esp_transport_list_get_transport(client->transport_list, "wss");
        if (trans) {
            esp_transport_ws_set_path(trans, client->config->path);
        }
    }

    if (puri.field_data[UF_PORT].len) {
        client->config->port = strtol((const char *)(uri + puri.field_data[UF_PORT].off), NULL, 10);
    }

    char *user_info = create_string(uri + puri.field_data[UF_USERINFO].off, puri.field_data[UF_USERINFO].len);
    if (user_info) {
        char *pass = strchr(user_info, ':');
        if (pass) {
            pass[0] = 0; //terminal username
            pass ++;
            client->connect_info.password = strdup(pass);
        }
        client->connect_info.username = strdup(user_info);

        free(user_info);
    }

    MQTT_API_UNLOCK(client);
    return ESP_OK;
}

static esp_err_t mqtt_write_data(esp_mqtt_client_handle_t client)
{
    int write_len = esp_transport_write(client->transport,
                                        (char *)client->mqtt_state.outbound_message->data,
                                        client->mqtt_state.outbound_message->length,
                                        client->config->network_timeout_ms);
    // client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
    if (write_len <= 0) {
        esp_mqtt_client_dispatch_tls_error(client);
        ESP_LOGE(TAG, "Error write data or timeout, written len = %d, errno=%d", write_len, errno);
        return ESP_FAIL;
    }
    /* we've just sent a mqtt control packet, update keepalive counter
     * [MQTT-3.1.2-23]
     */
    client->keepalive_tick = platform_tick_get_ms();
    return ESP_OK;
}

static esp_err_t esp_mqtt_dispatch_event_with_msgid(esp_mqtt_client_handle_t client)
{
    client->event.msg_id = mqtt_get_id(client->mqtt_state.in_buffer, client->mqtt_state.in_buffer_length);
    return esp_mqtt_dispatch_event(client);
}

static esp_err_t esp_mqtt_dispatch_event(esp_mqtt_client_handle_t client)
{
    client->event.user_context = client->config->user_context;
    client->event.client = client;

    if (client->config->event_handle) {
        return client->config->event_handle(&client->event);
    } else {
#ifdef MQTT_SUPPORTED_FEATURE_EVENT_LOOP
        esp_event_post_to(client->config->event_loop_handle, MQTT_EVENTS, client->event.event_id, &client->event, sizeof(client->event), portMAX_DELAY);
        return esp_event_loop_run(client->config->event_loop_handle, 0);
#else
        return ESP_FAIL;
#endif
    }
}

static esp_err_t deliver_publish(esp_mqtt_client_handle_t client)
{
    uint8_t *msg_buf = client->mqtt_state.in_buffer;
    size_t msg_read_len = client->mqtt_state.in_buffer_read_len;
    size_t msg_total_len = client->mqtt_state.message_length;
    size_t msg_topic_len = msg_read_len, msg_data_len = msg_read_len;
    size_t msg_data_offset = 0;
    char *msg_topic = NULL, *msg_data = NULL;

    // get topic
    msg_topic = mqtt_get_publish_topic(msg_buf, &msg_topic_len);
    if (msg_topic == NULL) {
        ESP_LOGE(TAG, "%s: mqtt_get_publish_topic() failed", __func__);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "%s: msg_topic_len=%u", __func__, msg_topic_len);

    // get payload
    msg_data = mqtt_get_publish_data(msg_buf, &msg_data_len);
    if (msg_data_len > 0 && msg_data == NULL) {
        ESP_LOGE(TAG, "%s: mqtt_get_publish_data() failed", __func__);
        return ESP_FAIL;
    }

    // post data event
    client->event.msg_id = mqtt_get_id(msg_buf, msg_data_len);
    client->event.total_data_len = msg_data_len + msg_total_len - msg_read_len;

post_data_event:
    ESP_LOGD(TAG, "Get data len= %d, topic len=%d, total_data: %d offset: %d", msg_data_len, msg_topic_len,
             client->event.total_data_len, msg_data_offset);
    client->event.event_id = MQTT_EVENT_DATA;
    client->event.data = msg_data_len > 0 ? msg_data : NULL;
    client->event.data_len = msg_data_len;
    client->event.current_data_offset = msg_data_offset;
    client->event.topic = msg_topic;
    client->event.topic_len = msg_topic_len;
    esp_mqtt_dispatch_event(client);

    if (msg_read_len < msg_total_len) {
        size_t buf_len = client->mqtt_state.in_buffer_length;

        msg_data = (char *)client->mqtt_state.in_buffer;
        msg_topic = NULL;
        msg_topic_len = 0;
        msg_data_offset += msg_data_len;
        msg_data_len = esp_transport_read(client-> transport, (char *)client->mqtt_state.in_buffer,
                                          msg_total_len - msg_read_len > buf_len ? buf_len : msg_total_len - msg_read_len,
                                          client->config->network_timeout_ms);
        if (msg_data_len <= 0) {
            ESP_LOGE(TAG, "Read error or timeout: len_read=%d, errno=%d", msg_data_len, errno);
            return ESP_FAIL;
        }
        msg_read_len += msg_data_len;
        goto post_data_event;
    }
    return ESP_OK;
}

static bool is_valid_mqtt_msg(esp_mqtt_client_handle_t client, int msg_type, int msg_id)
{
    ESP_LOGD(TAG, "pending_id=%d, pending_msg_count = %d", client->mqtt_state.pending_msg_id, client->mqtt_state.pending_msg_count);
    if (client->mqtt_state.pending_msg_count == 0) {
        return false;
    }
    if (outbox_delete(client->outbox, msg_id, msg_type) == ESP_OK) {
        client->mqtt_state.pending_msg_count --;
        return true;
    }

    return false;
}

static void mqtt_enqueue_oversized(esp_mqtt_client_handle_t client, uint8_t *remaining_data, int remaining_len)
{
    ESP_LOGD(TAG, "mqtt_enqueue_oversized id: %d, type=%d successful",
             client->mqtt_state.pending_msg_id, client->mqtt_state.pending_msg_type);
    //lock mutex
    outbox_message_t msg = { 0 };
    msg.data = client->mqtt_state.outbound_message->data;
    msg.len =  client->mqtt_state.outbound_message->length;
    msg.msg_id = client->mqtt_state.pending_msg_id;
    msg.msg_type = client->mqtt_state.pending_msg_type;
    msg.msg_qos = client->mqtt_state.pending_publish_qos;
    msg.remaining_data = remaining_data;
    msg.remaining_len = remaining_len;
    //Copy to queue buffer
    outbox_enqueue(client->outbox, &msg, platform_tick_get_ms());

    //unlock
}

static void mqtt_enqueue(esp_mqtt_client_handle_t client)
{
    ESP_LOGD(TAG, "mqtt_enqueue id: %d, type=%d successful",
             client->mqtt_state.pending_msg_id, client->mqtt_state.pending_msg_type);
    //lock mutex
    if (client->mqtt_state.pending_msg_count > 0) {
        outbox_message_t msg = { 0 };
        msg.data = client->mqtt_state.outbound_message->data;
        msg.len =  client->mqtt_state.outbound_message->length;
        msg.msg_id = client->mqtt_state.pending_msg_id;
        msg.msg_type = client->mqtt_state.pending_msg_type;
        msg.msg_qos = client->mqtt_state.pending_publish_qos;
        //Copy to queue buffer
        outbox_enqueue(client->outbox, &msg, platform_tick_get_ms());
    }
    //unlock
}


/*
 * Returns:
 *     -1 in case of failure
 *      0 if no message has been received
 *      1 if a message has been received and placed to client->mqtt_state:
 *           message length:  client->mqtt_state.message_length
 *           message content: client->mqtt_state.in_buffer
 */
static int mqtt_message_receive(esp_mqtt_client_handle_t client, int read_poll_timeout_ms)
{
    int read_len, total_len, fixed_header_len;
    uint8_t *buf = client->mqtt_state.in_buffer + client->mqtt_state.in_buffer_read_len;
    esp_transport_handle_t t = client->transport;

    client->mqtt_state.message_length = 0;
    if (client->mqtt_state.in_buffer_read_len == 0) {
        /*
         * Read first byte of the mqtt packet fixed header, it contains packet
         * type and flags.
         */
        read_len = esp_transport_read(t, (char *)buf, 1, read_poll_timeout_ms);
        if (read_len < 0) {
            ESP_LOGE(TAG, "%s: transport_read() error: errno=%d", __func__, errno);
            goto err;
        }
        if (read_len == 0) {
            ESP_LOGV(TAG, "%s: transport_read(): no data or EOF", __func__);
            return 0;
        }
        ESP_LOGD(TAG, "%s: first byte: 0x%x", __func__, *buf);
        /*
         * Verify the flags and act according to MQTT protocol: close connection
         * if the flags are set incorrectly.
         */
        if (!mqtt_has_valid_msg_hdr(buf, read_len)) {
            ESP_LOGE(TAG, "%s: received a message with an invalid header=0x%x", __func__, *buf);
            goto err;
        }
        buf++;
        client->mqtt_state.in_buffer_read_len++;
    }
    if ((client->mqtt_state.in_buffer_read_len == 1) ||
            ((client->mqtt_state.in_buffer_read_len < 6) && (*(buf - 1) & 0x80))) {
        do {
            /*
             * Read the "remaining length" part of mqtt packet fixed header.  It
             * starts at second byte and spans up to 4 bytes, but we accept here
             * only up to 2 bytes of remaining length, i.e. messages with
             * maximal remaining length value = 16383 (maximal total message
             * size of 16386 bytes).
             */
            read_len = esp_transport_read(t, (char *)buf, 1, read_poll_timeout_ms);
            if (read_len < 0) {
                ESP_LOGE(TAG, "%s: transport_read() error: errno=%d", __func__, errno);
                goto err;
            }
            if (read_len == 0) {
                ESP_LOGD(TAG, "%s: transport_read(): no data or EOF", __func__);
                return 0;
            }
            ESP_LOGD(TAG, "%s: read \"remaining length\" byte: 0x%x", __func__, *buf);
            buf++;
            client->mqtt_state.in_buffer_read_len++;
        } while ((client->mqtt_state.in_buffer_read_len < 6) && (*(buf - 1) & 0x80));
    }
    total_len = mqtt_get_total_length(client->mqtt_state.in_buffer, client->mqtt_state.in_buffer_read_len, &fixed_header_len);
    ESP_LOGD(TAG, "%s: total message length: %d (already read: %u)", __func__, total_len, client->mqtt_state.in_buffer_read_len);
    client->mqtt_state.message_length = total_len;
    if (client->mqtt_state.in_buffer_length < total_len) {
        if (mqtt_get_type(client->mqtt_state.in_buffer) == MQTT_MSG_TYPE_PUBLISH) {
            /*
             * In case larger publish messages, we only need to read full topic, data can be split to multiple data event.
             * Evaluate and correct total_len to read only publish message header, so data can be read separately
             */
            if (client->mqtt_state.in_buffer_read_len < fixed_header_len + 2) {
                /* read next 2 bytes - topic length to get minimum portion of publish packet */
                read_len = esp_transport_read(t, (char *)buf, client->mqtt_state.in_buffer_read_len - fixed_header_len + 2, read_poll_timeout_ms);
                ESP_LOGD(TAG, "%s: read_len=%d", __func__, read_len);
                if (read_len < 0) {
                    ESP_LOGE(TAG, "%s: transport_read() error: errno=%d", __func__, errno);
                    goto err;
                } else if (read_len == 0) {
                    ESP_LOGD(TAG, "%s: transport_read(): no data or EOF", __func__);
                    return 0;
                }
                client->mqtt_state.in_buffer_read_len += read_len;
                buf += read_len;
                if (client->mqtt_state.in_buffer_read_len < fixed_header_len + 2) {
                    ESP_LOGD(TAG, "%s: transport_read(): message reading left in progress :: total message length: %d (already read: %u)",
                             __func__, total_len, client->mqtt_state.in_buffer_read_len);
                    return 0;
                }
            }
            int topic_len = client->mqtt_state.in_buffer[fixed_header_len] << 8;
            topic_len |= client->mqtt_state.in_buffer[fixed_header_len + 1];
            total_len = fixed_header_len + topic_len + (mqtt_get_qos(client->mqtt_state.in_buffer) > 0 ? 2 : 0);
            ESP_LOGD(TAG, "%s: total len modified to %d as message longer than input buffer", __func__, total_len);
            if (client->mqtt_state.in_buffer_length < total_len) {
                ESP_LOGE(TAG, "%s: message is too big, insufficient buffer size", __func__);
                goto err;
            } else {
                total_len = client->mqtt_state.in_buffer_length;
            }
            /* free to continue with reading */
        } else {
            ESP_LOGE(TAG, "%s: message is too big, insufficient buffer size", __func__);
            goto err;
        }
    }
    if (client->mqtt_state.in_buffer_read_len < total_len) {
        /* read the rest of the mqtt message */
        read_len = esp_transport_read(t, (char *)buf, total_len - client->mqtt_state.in_buffer_read_len, read_poll_timeout_ms);
        ESP_LOGD(TAG, "%s: read_len=%d", __func__, read_len);
        if (read_len < 0) {
            ESP_LOGE(TAG, "%s: transport_read() error: errno=%d", __func__, errno);
            goto err;
        }
        if (read_len == 0) {
            ESP_LOGD(TAG, "%s: transport_read(): no data or EOF", __func__);
            return 0;
        }
        client->mqtt_state.in_buffer_read_len += read_len;
        if (client->mqtt_state.in_buffer_read_len < total_len) {
            ESP_LOGD(TAG, "%s: transport_read(): message reading left in progress :: total message length: %d (already read: %u)",
                     __func__, total_len, client->mqtt_state.in_buffer_read_len);
            return 0;
        }
    }
    ESP_LOGD(TAG, "%s: transport_read():%d %d", __func__, client->mqtt_state.in_buffer_read_len, client->mqtt_state.message_length);
    return 1;
err:
    esp_mqtt_client_dispatch_tls_error(client);
    return -1;
}

static esp_err_t mqtt_process_receive(esp_mqtt_client_handle_t client)
{
    uint8_t  msg_type;
    uint8_t  msg_qos;
    uint16_t msg_id;

    /* non-blocking receive in order not to block other tasks */
    int recv = mqtt_message_receive(client, 0);
    if (recv < 0) {
        ESP_LOGE(TAG, "%s: mqtt_message_receive() returned %d", __func__, recv);
        return ESP_FAIL;
    }
    if (recv == 0) {
        return ESP_OK;
    }
    int read_len = client->mqtt_state.message_length;

    // If the message was valid, get the type, quality of service and id of the message
    msg_type = mqtt_get_type(client->mqtt_state.in_buffer);
    msg_qos = mqtt_get_qos(client->mqtt_state.in_buffer);
    msg_id = mqtt_get_id(client->mqtt_state.in_buffer, read_len);

    ESP_LOGD(TAG, "msg_type=%d, msg_id=%d", msg_type, msg_id);

    switch (msg_type) {
    case MQTT_MSG_TYPE_SUBACK:
        if (is_valid_mqtt_msg(client, MQTT_MSG_TYPE_SUBSCRIBE, msg_id)) {
            ESP_LOGD(TAG, "Subscribe successful");
            client->event.event_id = MQTT_EVENT_SUBSCRIBED;
            esp_mqtt_dispatch_event_with_msgid(client);
        }
        break;
    case MQTT_MSG_TYPE_UNSUBACK:
        if (is_valid_mqtt_msg(client, MQTT_MSG_TYPE_UNSUBSCRIBE, msg_id)) {
            ESP_LOGD(TAG, "UnSubscribe successful");
            client->event.event_id = MQTT_EVENT_UNSUBSCRIBED;
            esp_mqtt_dispatch_event_with_msgid(client);
        }
        break;
    case MQTT_MSG_TYPE_PUBLISH:
        ESP_LOGD(TAG, "deliver_publish, message_length_read=%d, message_length=%d", client->mqtt_state.in_buffer_read_len, client->mqtt_state.message_length);
        if (deliver_publish(client) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to deliver publish message id=%d", msg_id);
            return ESP_FAIL;
        }
        if (msg_qos == 1) {
            client->mqtt_state.outbound_message = mqtt_msg_puback(&client->mqtt_state.mqtt_connection, msg_id);
        } else if (msg_qos == 2) {
            client->mqtt_state.outbound_message = mqtt_msg_pubrec(&client->mqtt_state.mqtt_connection, msg_id);
        }
        if (client->mqtt_state.outbound_message->length == 0) {
            ESP_LOGE(TAG, "Publish response message PUBACK or PUBREC cannot be created");
            return ESP_FAIL;
        }

        if (msg_qos == 1 || msg_qos == 2) {
            ESP_LOGD(TAG, "Queue response QoS: %d", msg_qos);

            if (mqtt_write_data(client) != ESP_OK) {
                ESP_LOGE(TAG, "Error write qos msg repsonse, qos = %d", msg_qos);
                return ESP_FAIL;
            }
        }
        break;
    case MQTT_MSG_TYPE_PUBACK:
        if (is_valid_mqtt_msg(client, MQTT_MSG_TYPE_PUBLISH, msg_id)) {
            ESP_LOGD(TAG, "received MQTT_MSG_TYPE_PUBACK, finish QoS1 publish");
            outbox_set_pending(client->outbox, msg_id, CONFIRMED);
            client->event.event_id = MQTT_EVENT_PUBLISHED;
            esp_mqtt_dispatch_event_with_msgid(client);
        }
        break;
    case MQTT_MSG_TYPE_PUBREC:
        ESP_LOGD(TAG, "received MQTT_MSG_TYPE_PUBREC");
        client->mqtt_state.outbound_message = mqtt_msg_pubrel(&client->mqtt_state.mqtt_connection, msg_id);
        if (client->mqtt_state.outbound_message->length == 0) {
            ESP_LOGE(TAG, "Publish response message PUBREL cannot be created");
            return ESP_FAIL;
        }

        outbox_set_pending(client->outbox, msg_id, ACKNOWLEDGED);
        mqtt_write_data(client);
        break;
    case MQTT_MSG_TYPE_PUBREL:
        ESP_LOGD(TAG, "received MQTT_MSG_TYPE_PUBREL");
        client->mqtt_state.outbound_message = mqtt_msg_pubcomp(&client->mqtt_state.mqtt_connection, msg_id);
        if (client->mqtt_state.outbound_message->length == 0) {
            ESP_LOGE(TAG, "Publish response message PUBCOMP cannot be created");
            return ESP_FAIL;
        }

        mqtt_write_data(client);
        break;
    case MQTT_MSG_TYPE_PUBCOMP:
        ESP_LOGD(TAG, "received MQTT_MSG_TYPE_PUBCOMP");
        if (is_valid_mqtt_msg(client, MQTT_MSG_TYPE_PUBLISH, msg_id)) {
            ESP_LOGD(TAG, "Receive MQTT_MSG_TYPE_PUBCOMP, finish QoS2 publish");
            outbox_set_pending(client->outbox, msg_id, CONFIRMED);
            client->event.event_id = MQTT_EVENT_PUBLISHED;
            esp_mqtt_dispatch_event_with_msgid(client);
        }
        break;
    case MQTT_MSG_TYPE_PINGRESP:
        ESP_LOGD(TAG, "MQTT_MSG_TYPE_PINGRESP");
        client->wait_for_ping_resp = false;
        break;
    }

    client->mqtt_state.in_buffer_read_len = 0;
    return ESP_OK;
}

static esp_err_t mqtt_resend_queued(esp_mqtt_client_handle_t client, outbox_item_handle_t item)
{
    // decode queued data
    client->mqtt_state.outbound_message->data = outbox_item_get_data(item, &client->mqtt_state.outbound_message->length, &client->mqtt_state.pending_msg_id,
            &client->mqtt_state.pending_msg_type, &client->mqtt_state.pending_publish_qos);
    // set duplicate flag for QoS-1 and QoS-2 messages
    if (client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_PUBLISH && client->mqtt_state.pending_publish_qos > 0) {
        mqtt_set_dup(client->mqtt_state.outbound_message->data);
    }

    // try to resend the data
    if (mqtt_write_data(client) != ESP_OK) {
        ESP_LOGE(TAG, "Error to resend data ");
        esp_mqtt_abort_connection(client);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void esp_mqtt_task(void *pv)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t) pv;
    uint64_t last_retransmit = 0;
    outbox_tick_t msg_tick = 0;
    client->run = true;

    //get transport by scheme
    client->transport = esp_transport_list_get_transport(client->transport_list, client->config->scheme);

    if (client->transport == NULL) {
        ESP_LOGE(TAG, "There are no transports valid, stop mqtt client, config scheme = %s", client->config->scheme);
        client->run = false;
    }
    //default port
    if (client->config->port == 0) {
        client->config->port = esp_transport_get_default_port(client->transport);
    }

    client->state = MQTT_STATE_INIT;
    xEventGroupClearBits(client->status_bits, STOPPED_BIT);
    while (client->run) {
        MQTT_API_LOCK(client);
        switch ((int)client->state) {
        case MQTT_STATE_INIT:
            xEventGroupClearBits(client->status_bits, RECONNECT_BIT | DISCONNECT_BIT);
            client->event.event_id = MQTT_EVENT_BEFORE_CONNECT;
            esp_mqtt_dispatch_event_with_msgid(client);

            if (client->transport == NULL) {
                ESP_LOGE(TAG, "There is no transport");
                client->run = false;
            }
#if MQTT_ENABLE_SSL
            esp_mqtt_set_ssl_transport_properties(client->transport_list, client->config);
#endif

            if (esp_transport_connect(client->transport,
                                      client->config->host,
                                      client->config->port,
                                      client->config->network_timeout_ms) < 0) {
                ESP_LOGE(TAG, "Error transport connect");
                esp_mqtt_client_dispatch_tls_error(client);
                esp_mqtt_abort_connection(client);
                break;
            }
            ESP_LOGD(TAG, "Transport connected to %s://%s:%d", client->config->scheme, client->config->host, client->config->port);
            if (esp_mqtt_connect(client, client->config->network_timeout_ms) != ESP_OK) {
                ESP_LOGE(TAG, "MQTT connect failed");
                esp_mqtt_abort_connection(client);
                break;
            }
            client->event.event_id = MQTT_EVENT_CONNECTED;
            client->event.session_present = mqtt_get_connect_session_present(client->mqtt_state.in_buffer);
            client->state = MQTT_STATE_CONNECTED;
            esp_mqtt_dispatch_event_with_msgid(client);
            client->refresh_connection_tick = platform_tick_get_ms();

            break;
        case MQTT_STATE_CONNECTED:
            // check for disconnection request
            if (xEventGroupWaitBits(client->status_bits, DISCONNECT_BIT, true, true, 0) & DISCONNECT_BIT) {
                esp_mqtt_abort_connection(client);
                break;
            }
            // receive and process data
            if (mqtt_process_receive(client) == ESP_FAIL) {
                esp_mqtt_abort_connection(client);
                break;
            }

            //Delete message after OUTBOX_EXPIRED_TIMEOUT_MS miliseconds
            int deleted = outbox_delete_expired(client->outbox, platform_tick_get_ms(), OUTBOX_EXPIRED_TIMEOUT_MS);
            client->mqtt_state.pending_msg_count -= deleted;

            if (client->mqtt_state.pending_msg_count < 0) {
                client->mqtt_state.pending_msg_count = 0;
            }

            // resend all non-transmitted messages first
            outbox_item_handle_t item = outbox_dequeue(client->outbox, QUEUED, NULL);
            if (item) {
                if (mqtt_resend_queued(client, item) == ESP_OK) {
                    outbox_set_pending(client->outbox, client->mqtt_state.pending_msg_id, TRANSMITTED);
                }
                // resend other "transmitted" messages after 1s
            } else if (platform_tick_get_ms() - last_retransmit > 1000) {
                last_retransmit = platform_tick_get_ms();
                item = outbox_dequeue(client->outbox, TRANSMITTED, &msg_tick);
                if (item && (last_retransmit - msg_tick > 1000))  {
                    mqtt_resend_queued(client, item);
                }
            }

            if (platform_tick_get_ms() - client->keepalive_tick > client->connect_info.keepalive * 1000 / 2) {
                //No ping resp from last ping => Disconnected
                if (client->wait_for_ping_resp) {
                    ESP_LOGE(TAG, "No PING_RESP, disconnected");
                    esp_mqtt_abort_connection(client);
                    client->wait_for_ping_resp = false;
                    break;
                }
                if (esp_mqtt_client_ping(client) == ESP_FAIL) {
                    ESP_LOGE(TAG, "Can't send ping, disconnected");
                    esp_mqtt_abort_connection(client);
                    break;
                } else {
                    client->wait_for_ping_resp = true;
                }
                ESP_LOGD(TAG, "PING sent");
            }

            if (client->config->refresh_connection_after_ms &&
                    platform_tick_get_ms() - client->refresh_connection_tick > client->config->refresh_connection_after_ms) {
                ESP_LOGD(TAG, "Refreshing the connection...");
                esp_mqtt_abort_connection(client);
                client->state = MQTT_STATE_INIT;
            }

            outbox_cleanup(client->outbox, OUTBOX_MAX_SIZE);
            break;
        case MQTT_STATE_WAIT_TIMEOUT:

            if (!client->config->auto_reconnect) {
                client->run = false;
                client->state = MQTT_STATE_UNKNOWN;
                break;
            }
            if (platform_tick_get_ms() - client->reconnect_tick > client->wait_timeout_ms) {
                client->state = MQTT_STATE_INIT;
                client->reconnect_tick = platform_tick_get_ms();
                ESP_LOGD(TAG, "Reconnecting...");
                break;
            }
            MQTT_API_UNLOCK(client);
            xEventGroupWaitBits(client->status_bits, RECONNECT_BIT, false, true,
                                client->wait_timeout_ms / 2 / portTICK_RATE_MS);
            // continue the while loop instead of break, as the mutex is unlocked
            continue;
        }
        MQTT_API_UNLOCK(client);
        if (MQTT_STATE_CONNECTED == client->state) {
            if (esp_transport_poll_read(client->transport, MQTT_POLL_READ_TIMEOUT_MS) < 0) {
                ESP_LOGE(TAG, "Poll read error: %d, aborting connection", errno);
                esp_mqtt_abort_connection(client);
            }
        }

    }
    esp_transport_close(client->transport);
    xEventGroupSetBits(client->status_bits, STOPPED_BIT);

    vTaskDelete(NULL);
}

esp_err_t esp_mqtt_client_start(esp_mqtt_client_handle_t client)
{
    if (!client) {
        ESP_LOGE(TAG, "Client was not initialized");
        return ESP_ERR_INVALID_ARG;
    }
    MQTT_API_LOCK(client);
    if (client->state >= MQTT_STATE_INIT) {
        ESP_LOGE(TAG, "Client has started");
        MQTT_API_UNLOCK(client);
        return ESP_FAIL;
    }
    esp_err_t err = ESP_OK;
#if MQTT_CORE_SELECTION_ENABLED
    ESP_LOGD(TAG, "Core selection enabled on %u", MQTT_TASK_CORE);
    if (xTaskCreatePinnedToCore(esp_mqtt_task, "mqtt_task", client->config->task_stack, client, client->config->task_prio, &client->task_handle, MQTT_TASK_CORE) != pdTRUE) {
        ESP_LOGE(TAG, "Error create mqtt task");
        err = ESP_FAIL;
    }
#else
    ESP_LOGD(TAG, "Core selection disabled");
    if (xTaskCreate(esp_mqtt_task, "mqtt_task", client->config->task_stack, client, client->config->task_prio, &client->task_handle) != pdTRUE) {
        ESP_LOGE(TAG, "Error create mqtt task");
        err = ESP_FAIL;
    }
#endif
    MQTT_API_UNLOCK(client);
    return err;
}

esp_err_t esp_mqtt_client_disconnect(esp_mqtt_client_handle_t client)
{
    ESP_LOGI(TAG, "Client asked to disconnect");
    xEventGroupSetBits(client->status_bits, DISCONNECT_BIT);
    return ESP_OK;
}

esp_err_t esp_mqtt_client_reconnect(esp_mqtt_client_handle_t client)
{
    ESP_LOGI(TAG, "Client force reconnect requested");

    if (client->state != MQTT_STATE_WAIT_TIMEOUT) {
        ESP_LOGD(TAG, "The client is not waiting for reconnection. Ignore the request");
        return ESP_FAIL;
    }
    client->wait_timeout_ms = 0;
    xEventGroupSetBits(client->status_bits, RECONNECT_BIT);
    return ESP_OK;
}

esp_err_t esp_mqtt_client_stop(esp_mqtt_client_handle_t client)
{
    MQTT_API_LOCK(client);
    if (client->run) {
        /* A running client cannot be stopped from the MQTT task/event handler */
        TaskHandle_t running_task = xTaskGetCurrentTaskHandle();
        if (running_task == client->task_handle) {
            ESP_LOGE(TAG, "Client cannot be stopped from MQTT task");
            return ESP_FAIL;
        }

        // Only send the disconnect message if the client is connected
        if(client->state == MQTT_STATE_CONNECTED) {
            // Notify the broker we are disconnecting
            client->mqtt_state.outbound_message = mqtt_msg_disconnect(&client->mqtt_state.mqtt_connection);
            if (client->mqtt_state.outbound_message->length == 0) {
                ESP_LOGE(TAG, "Disconnect message cannot be created");
                MQTT_API_UNLOCK(client);
                return ESP_FAIL;
            }
            if (mqtt_write_data(client) != ESP_OK) {
                ESP_LOGE(TAG, "Error sending disconnect message");
            }
        }

        client->run = false;
        client->state = MQTT_STATE_UNKNOWN;
        MQTT_API_UNLOCK(client);
        xEventGroupWaitBits(client->status_bits, STOPPED_BIT, false, true, portMAX_DELAY);
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "Client asked to stop, but was not started");
        MQTT_API_UNLOCK(client);
        return ESP_FAIL;
    }
}

static esp_err_t esp_mqtt_client_ping(esp_mqtt_client_handle_t client)
{
    client->mqtt_state.outbound_message = mqtt_msg_pingreq(&client->mqtt_state.mqtt_connection);
    if (client->mqtt_state.outbound_message->length == 0) {
        ESP_LOGE(TAG, "Ping message cannot be created");
        return ESP_FAIL;
    }

    if (mqtt_write_data(client) != ESP_OK) {
        ESP_LOGE(TAG, "Error sending ping");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "Sent PING successful");
    return ESP_OK;
}

int esp_mqtt_client_subscribe(esp_mqtt_client_handle_t client, const char *topic, int qos)
{
    MQTT_API_LOCK(client);
    if (client->state != MQTT_STATE_CONNECTED) {
        ESP_LOGE(TAG, "Client has not connected");
        MQTT_API_UNLOCK(client);
        return -1;
    }
    client->mqtt_state.outbound_message = mqtt_msg_subscribe(&client->mqtt_state.mqtt_connection,
                                          topic, qos,
                                          &client->mqtt_state.pending_msg_id);
    if (client->mqtt_state.outbound_message->length == 0) {
        ESP_LOGE(TAG, "Subscribe message cannot be created");
        MQTT_API_UNLOCK(client);
        return -1;
    }

    client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
    client->mqtt_state.pending_msg_count ++;
    mqtt_enqueue(client); //move pending msg to outbox (if have)
    outbox_set_pending(client->outbox, client->mqtt_state.pending_msg_id, TRANSMITTED);

    if (mqtt_write_data(client) != ESP_OK) {
        ESP_LOGE(TAG, "Error to subscribe topic=%s, qos=%d", topic, qos);
        MQTT_API_UNLOCK(client);
        return -1;
    }

    ESP_LOGD(TAG, "Sent subscribe topic=%s, id: %d, type=%d successful", topic, client->mqtt_state.pending_msg_id, client->mqtt_state.pending_msg_type);
    MQTT_API_UNLOCK(client);
    return client->mqtt_state.pending_msg_id;
}

int esp_mqtt_client_unsubscribe(esp_mqtt_client_handle_t client, const char *topic)
{
    MQTT_API_LOCK(client);
    if (client->state != MQTT_STATE_CONNECTED) {
        MQTT_API_UNLOCK(client);
        ESP_LOGE(TAG, "Client has not connected");
        return -1;
    }
    client->mqtt_state.outbound_message = mqtt_msg_unsubscribe(&client->mqtt_state.mqtt_connection,
                                          topic,
                                          &client->mqtt_state.pending_msg_id);
    if (client->mqtt_state.outbound_message->length == 0) {
        MQTT_API_UNLOCK(client);
        ESP_LOGE(TAG, "Unubscribe message cannot be created");
        return -1;
    }
    ESP_LOGD(TAG, "unsubscribe, topic\"%s\", id: %d", topic, client->mqtt_state.pending_msg_id);

    client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
    client->mqtt_state.pending_msg_count ++;
    mqtt_enqueue(client);
    outbox_set_pending(client->outbox, client->mqtt_state.pending_msg_id, TRANSMITTED);

    if (mqtt_write_data(client) != ESP_OK) {
        ESP_LOGE(TAG, "Error to unsubscribe topic=%s", topic);
        MQTT_API_UNLOCK(client);
        return -1;
    }

    ESP_LOGD(TAG, "Sent Unsubscribe topic=%s, id: %d, successful", topic, client->mqtt_state.pending_msg_id);
    MQTT_API_UNLOCK(client);
    return client->mqtt_state.pending_msg_id;
}

int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain)
{
    uint16_t pending_msg_id = 0;
    int ret = 0;

    /* Acceptable publish messages:
        data == NULL, len == 0: publish null message
        data valid,   len == 0: publish all data, payload len is determined from string length
        data valid,   len >  0: publish data with defined length
     */
    if (len <= 0 && data != NULL) {
        len = strlen(data);
    }

    MQTT_API_LOCK(client);
    mqtt_message_t *publish_msg = mqtt_msg_publish(&client->mqtt_state.mqtt_connection,
                                  topic, data, len,
                                  qos, retain,
                                  &pending_msg_id);

    if (publish_msg->length == 0) {
        ESP_LOGE(TAG, "Publish message cannot be created");
        MQTT_API_UNLOCK(client);
        return -1;
    }
    /* We have to set as pending all the qos>0 messages */
    client->mqtt_state.outbound_message = publish_msg;
    if (qos > 0) {
        client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
        client->mqtt_state.pending_msg_id = pending_msg_id;
        client->mqtt_state.pending_publish_qos = qos;
        client->mqtt_state.pending_msg_count ++;
        // by default store as QUEUED (not transmitted yet) only for messages which would fit outbound buffer
        if (client->mqtt_state.mqtt_connection.message.fragmented_msg_total_length == 0) {
            mqtt_enqueue(client);
        } else {
            int first_fragment = client->mqtt_state.outbound_message->length - client->mqtt_state.outbound_message->fragmented_msg_data_offset;
            mqtt_enqueue_oversized(client, ((uint8_t *)data) + first_fragment, len - first_fragment);
        }
    }

    /* Skip sending if not connected (rely on resending) */
    if (client->state != MQTT_STATE_CONNECTED) {
        ESP_LOGD(TAG, "Publish: client is not connected");
        if (qos > 0) {
            ret = pending_msg_id;
        }

        //Delete message after OUTBOX_EXPIRED_TIMEOUT_MS miliseconds
        int deleted = outbox_delete_expired(client->outbox, platform_tick_get_ms(), OUTBOX_EXPIRED_TIMEOUT_MS);
        client->mqtt_state.pending_msg_count -= deleted;

        if (client->mqtt_state.pending_msg_count < 0) {
            client->mqtt_state.pending_msg_count = 0;
        }

        goto cannot_publish;
    }

    /* Provide support for sending fragmented message if it doesn't fit buffer */
    int remaining_len = len;
    const char *current_data = data;
    bool sending = true;

    while (sending)  {

        if (mqtt_write_data(client) != ESP_OK) {
            esp_mqtt_abort_connection(client);
            ret = -1;
            goto cannot_publish;
        }

        int data_sent = client->mqtt_state.outbound_message->length - client->mqtt_state.outbound_message->fragmented_msg_data_offset;
        client->mqtt_state.outbound_message->fragmented_msg_data_offset = 0;
        client->mqtt_state.outbound_message->fragmented_msg_total_length = 0;
        remaining_len -= data_sent;
        current_data +=  data_sent;

        if (remaining_len > 0) {
            mqtt_connection_t *connection = &client->mqtt_state.mqtt_connection;
            ESP_LOGD(TAG, "Sending fragmented message, remains to send %d bytes of %d", remaining_len, len);
            if (remaining_len > connection->buffer_length) {
                // Continue with sending
                memcpy(connection->buffer, current_data, connection->buffer_length);
                connection->message.length = connection->buffer_length;
                sending = true;
            } else {
                memcpy(connection->buffer, current_data, remaining_len);
                connection->message.length = remaining_len;
                sending = true;
            }
            connection->message.data = connection->buffer;
            client->mqtt_state.outbound_message = &connection->message;
        } else {
            // Message was sent correctly
            sending = false;
        }
    }

    if (qos > 0) {
        //Tick is set after transmit to avoid retransmitting too early due slow network speed / big messages
        outbox_set_tick(client->outbox, pending_msg_id, platform_tick_get_ms());
        outbox_set_pending(client->outbox, pending_msg_id, TRANSMITTED);
    }
    MQTT_API_UNLOCK(client);
    return pending_msg_id;

cannot_publish:
    // clear out possible fragmented publish if failed or skipped
    client->mqtt_state.outbound_message->fragmented_msg_total_length = 0;
    if (qos == 0) {
        ESP_LOGW(TAG, "Publish: Losing qos0 data when client not connected");
    }
    MQTT_API_UNLOCK(client);

    return ret;
}


esp_err_t esp_mqtt_client_register_event(esp_mqtt_client_handle_t client, esp_mqtt_event_id_t event, esp_event_handler_t event_handler, void* event_handler_arg)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
#ifdef MQTT_SUPPORTED_FEATURE_EVENT_LOOP
    if (client->config->event_handle) {
        ESP_LOGW(TAG, "Registering event loop while event callback is not null, clearing callback");
        client->config->event_handle = NULL;
    }

    return esp_event_handler_register_with(client->config->event_loop_handle, MQTT_EVENTS, event, event_handler, event_handler_arg);
#else
    ESP_LOGE(TAG, "Registering event handler while event loop not available in IDF version %s", IDF_VER);
    return ESP_FAIL;
#endif
}


static void esp_mqtt_client_dispatch_tls_error(esp_mqtt_client_handle_t client)
{
        client->event.event_id = MQTT_EVENT_ERROR;
        client->event.error_handle->error_type = MQTT_ERROR_TYPE_ESP_TLS;
        client->event.error_handle->connect_return_code = 0;
#ifdef MQTT_SUPPORTED_FEATURE_TRANSPORT_ERR_REPORTING
        client->event.error_handle->esp_tls_last_esp_err = esp_tls_get_and_clear_last_error(esp_transport_get_error_handle(client->transport),
                                                                                            &client->event.error_handle->esp_tls_stack_err,
                                                                                            &client->event.error_handle->esp_tls_cert_verify_flags);
#endif
        esp_mqtt_dispatch_event_with_msgid(client);
}
