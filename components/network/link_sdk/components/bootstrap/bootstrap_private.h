#ifndef _BOOTSTRAP_PRIVATE_H_
#define _BOOTSTRAP_PRIVATE_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "core_string.h"
#include "core_log.h"
#include "core_http.h"
#include "core_global.h"
#include "core_mqtt.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_bootstrap_api.h"

typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *http_handle;
    void *mqtt_handle;
    core_http_response_t response;
    uint32_t response_body_len;
    char *host;
    uint16_t port;
    char *product_key;
    char *device_name;
    char *region_id;
    aiot_sysdep_network_cred_t *cred;
    uint8_t exec_enabled;
    uint32_t exec_count;
    uint32_t send_timeout_ms;
    uint32_t recv_timeout_ms;
    uint32_t timeout_ms;
    uint32_t deinit_timeout_ms;
    void *data_mutex;
    aiot_bootstrap_recv_handler_t recv_handler;
    aiot_bootstrap_event_handler_t event_handler;
    void *userdata;
} bootstrap_handle_t;

#define BOOTSTRAP_MODULE_NAME                      "Bootstrap"

#define BOOTSTRAP_DEFAULT_HOST                     "iot-auth-global.aliyuncs.com"
#define BOOTSTRAP_DEFAULT_PORT                     (443)
#define BOOTSTRAP_DEFAULT_TIMEOUT_MS               (5 * 1000)
#define BOOTSTRAP_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)
#define BOOTSTRAP_DEFAULT_RECV_TIMEOUT             (5 * 1000)
#define BOOTSTRAP_DEFAULT_SEND_TIMEOUT             (5 * 1000)

#define BOOTSTRAP_PATH                             "/auth/bootstrap"
#define BOOTSTRAP_NOTIFY_TOPIC_FMT                 "/sys/%s/%s/thing/bootstrap/notify"
#define BOOTSTRAP_NOTIFY_REPLY_TOPIC_FMT           "/sys/%s/%s/thing/bootstrap/notify_reply"

#define BOOTSTRAP_DEINIT_INTERVAL_MS               (100)
#define BOOTSTRAP_RESPONSE_BODY_LEN                (192)

#if defined(__cplusplus)
}
#endif

#endif

