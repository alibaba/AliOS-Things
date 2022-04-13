#ifndef _CORE_MQTT_H_
#define _CORE_MQTT_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "core_list.h"
#include "core_string.h"
#include "core_log.h"
#include "core_auth.h"
#include "core_global.h"
#include "core_diag.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"

/**
 *
 * MQTT3.1 Fixed Header
 * | Bit       |          7 ~ 4           |            3 ~ 0            |
 * | byte 1    | MQTT Control Packet Type | Flags specific to each Type |
 * | byte 2... |             Remaining Length(1 ~ 4 bytes)              |
 *
 * MQTT3.1 UTF-8 Encoded Strings
 * | Bit       |                         7 ~ 0                          |
 * | byte 1    |                   String Length MSB                    |
 * | byte 2    |                   String Length LSB                    |
 * | byte 3... |              UTF-8 Encoded Character Data              |
 *
 */

#define CORE_MQTT_FIXED_HEADER_LEN                  (1)
#define CORE_MQTT_REMAINLEN_MAXLEN                  (4)
#define CORE_MQTT_UTF8_STR_EXTRA_LEN                (2)
#define CORE_MQTT_PACKETID_LEN                      (2)
#define CORE_MQTT_REQUEST_QOS_LEN                   (1)
#define CORE_MQTT_QOS0                              (0x00)
#define CORE_MQTT_QOS1                              (0x01)
#define CORE_MQTT_QOS_MAX                           (1)
#define CORE_MQTT_TOPIC_MAXLEN                      (128)
#define CORE_MQTT_PAYLOAD_MAXLEN                    (1024 * 1024 + 1)

/* MQTT 3.1 Connect Packet */
#define CORE_MQTT_CONN_PKT_TYPE                     (0x10)
#define CORE_MQTT_CONN_FIXED_HEADER_LEN             (CORE_MQTT_FIXED_HEADER_LEN) /* value: 0x10 */
#define CORE_MQTT_CONN_REMAINLEN_MAXLEN             (CORE_MQTT_REMAINLEN_MAXLEN)
#define CORE_MQTT_CONN_PROTOCOL_NAME_LEN            (6) /* value: 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54 */
#define CORE_MQTT_CONN_PROTOCOL_LEVEL_LEN           (1) /* value: 0x04 */
#define CORE_MQTT_CONN_CONNECT_FLAG_LEN             (1) /* value: 0xC0 */
#define CORE_MQTT_CONN_KEEP_ALIVE_LEN               (2) /* value: CORE_MQTT_conn_pkt_t.keep_alive */
#define CORE_MQTT_CONN_FIXED_HEADER_TOTAL_LEN       (CORE_MQTT_CONN_FIXED_HEADER_LEN + \
        CORE_MQTT_CONN_REMAINLEN_MAXLEN + \
        CORE_MQTT_CONN_PROTOCOL_NAME_LEN + \
        CORE_MQTT_CONN_PROTOCOL_LEVEL_LEN + \
        CORE_MQTT_CONN_CONNECT_FLAG_LEN + \
        CORE_MQTT_CONN_KEEP_ALIVE_LEN)
#define CORE_MQTT_CONN_REMAINLEN_FIXED_LEN          (CORE_MQTT_CONN_PROTOCOL_NAME_LEN + \
        CORE_MQTT_CONN_PROTOCOL_LEVEL_LEN + \
        CORE_MQTT_CONN_CONNECT_FLAG_LEN + \
        CORE_MQTT_CONN_KEEP_ALIVE_LEN)

/* MQTT 3.1 Connect ACK Packet */
#define CORE_MQTT_CONNACK_PKT_TYPE                  (0x20)
#define CORE_MQTT_CONNACK_FIXED_HEADER_LEN          (CORE_MQTT_FIXED_HEADER_LEN) /* value: 0x20 */
#define CORE_MQTT_CONNACK_REMAINLEN_MAXLEN          (1) /* value: 0x02 */
#define CORE_MQTT_CONNACK_FLAGS_LEN                 (1) /* value: 0x00 */
#define CORE_MQTT_CONNACK_RETURN_CODE_LEN           (1)
#define CORE_MQTT_CONNACK_FIXED_HEADER_TOTAL_LEN    (CORE_MQTT_CONNACK_FIXED_HEADER_LEN + \
        CORE_MQTT_CONNACK_REMAINLEN_MAXLEN + \
        CORE_MQTT_CONNACK_FLAGS_LEN + \
        CORE_MQTT_CONNACK_RETURN_CODE_LEN)

#define CORE_MQTT_CONNACK_RCODE_ACCEPTED                            (0x00)
#define CORE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION       (0x01)
#define CORE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE                  (0x02)
#define CORE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD               (0x03)
#define CORE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED                      (0x04)

#define CORE_MQTT_DYNREG_CONNACK_RCODE_ACCEPTED                     (0x00)
#define CORE_MQTT_DYNREG_CONNACK_RCODE_IDENTIFIER_REJECTED          (0x02)
#define CORE_MQTT_DYNREG_CONNACK_RCODE_SERVER_UNAVAILABLE           (0x03)
#define CORE_MQTT_DYNREG_CONNACK_RCODE_BAD_USERNAME_PASSWORD        (0x04)

/* MQTT 3.1 Disconnect Packet */
#define CORE_MQTT_DISCONNECT_PKT_TYPE               (0xE0)

/* MQTT 3.1 Ping Request Packet */
#define CORE_MQTT_PINGREQ_PKT_TYPE                  (0xC0)

/* MQTT 3.1 Ping Response Packet */
#define CORE_MQTT_PINGRESP_PKT_TYPE                 (0xD0)
#define CORE_MQTT_PINGRESP_FIXED_HEADER_LEN         (CORE_MQTT_FIXED_HEADER_LEN) /* value: 0xD0 */
#define CORE_MQTT_PINGRESP_REMAINLEN_MAXLEN         (1) /* value: 0x00 */

/* MQTT 3.1 Publish Packet */
#define CORE_MQTT_PUBLISH_PKT_TYPE                  (0x30)
#define CORE_MQTT_PUBLISH_TOPICLEN_LEN              (2)

/* MQTT 3.1 Publish ACK Packet */
#define CORE_MQTT_PUBACK_PKT_TYPE                   (0x40)

/* MQTT 3.1 Subscribe Packet */
#define CORE_MQTT_SUB_PKT_TYPE                      (0x80)
#define CORE_MQTT_SUB_PKT_RESERVE                   (0x02)

/* MQTT 3.1 Subscribe ACK Packet */
#define CORE_MQTT_SUBACK_PKT_TYPE                   (0x90)

#define CORE_MQTT_SUBACK_RCODE_MAXQOS0              (0x00)
#define CORE_MQTT_SUBACK_RCODE_MAXQOS1              (0x01)
#define CORE_MQTT_SUBACK_RCODE_MAXQOS2              (0x02)
#define CORE_MQTT_SUBACK_RCODE_FAILURE              (0x80)

/* MQTT 3.1 Unsubscribe Packet */
#define CORE_MQTT_UNSUB_PKT_TYPE                    (0xA0)

/* MQTT 3.1 Unsubscribe ACK Packet */
#define CORE_MQTT_UNSUBACK_PKT_TYPE                 (0xB0)

/* MQTT 3.1 unimplemented Packet */
#define CORE_MQTT_PUBREC_PKT_TYPE                   (0x50)
#define CORE_MQTT_PUBREL_PKT_TYPE                   (0x60)
#define CORE_MQTT_PUBCOMP_PKT_TYPE                  (0x70)

typedef struct {
    uint8_t    *buffer;
    uint32_t    len;
} core_mqtt_buff_t;

typedef struct {
    aiot_mqtt_recv_handler_t handler;
    void *userdata;
    struct core_list_head linked_node;
} core_mqtt_sub_handler_node_t;

typedef struct {
    char *topic;
    struct core_list_head linked_node;
    struct core_list_head handle_list;
} core_mqtt_sub_node_t;

typedef struct {
    uint16_t packet_id;
    uint8_t *packet;
    uint32_t len;
    uint64_t last_send_time;
    struct core_list_head linked_node;
} core_mqtt_pub_node_t;

typedef enum {
    CORE_MQTTEVT_DEINIT
} core_mqtt_event_type_t;

typedef struct {
    core_mqtt_event_type_t type;
} core_mqtt_event_t;

typedef void (*core_mqtt_process_handler_t)(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event);

typedef struct {
    core_mqtt_process_handler_t handler;
    void *context;
} core_mqtt_process_data_t;

typedef struct {
    core_mqtt_process_data_t process_data;
    struct core_list_head linked_node;
} core_mqtt_process_data_node_t;

typedef struct {
    uint32_t    interval_ms;
    uint8_t     max_lost_times;
    uint32_t    lost_times;
    uint64_t    last_send_time;
} core_mqtt_heartbeat_t;

typedef struct {
    uint8_t enabled;
    uint32_t interval_ms;
    uint64_t last_retry_time;
} core_mqtt_reconnect_t;

typedef struct {
    /* network info */
    uint8_t network_type;       // 0: TCP, 1: TLS
    uint64_t connect_timestamp;
    uint32_t connect_time_used;
    uint64_t failed_timestamp;
    int32_t failed_error_code;

    /* heartbeat rtt info */
    uint64_t rtt;
} core_mqtt_nwkstats_info_t;

typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *network_handle;
    char *host;
    uint16_t port;
    char *product_key;
    char *device_name;
    char *device_secret;
    char *username;
    char *password;
    char *clientid;
    char *extend_clientid;
    char *security_mode;
    uint16_t keep_alive_s;
    uint8_t clean_session;
    uint8_t append_requestid;
    uint32_t connect_timeout_ms;
    core_mqtt_heartbeat_t heartbeat_params;
    core_mqtt_reconnect_t reconnect_params;
    uint32_t send_timeout_ms;
    uint32_t recv_timeout_ms;
    uint32_t repub_timeout_ms;
    aiot_sysdep_network_cred_t *cred;
    uint8_t has_connected;
    uint8_t disconnected;
    uint8_t disconnect_api_called;
    uint8_t exec_enabled;
    uint32_t exec_count;
    uint32_t deinit_timeout_ms;
    uint16_t packet_id;
    void *data_mutex;
    void *send_mutex;
    void *recv_mutex;
    void *sub_mutex;
    void *pub_mutex;
    void *process_handler_mutex;
    struct core_list_head sub_list;
    struct core_list_head pub_list;
    struct core_list_head process_data_list;
    aiot_mqtt_recv_handler_t recv_handler;
    aiot_mqtt_event_handler_t event_handler;

    /* network info stats */
    core_mqtt_nwkstats_info_t nwkstats_info;

    void *userdata;
} core_mqtt_handle_t;

/* default configuration */
#define CORE_MQTT_MODULE_NAME                      "MQTT"
#define CORE_MQTT_DEINIT_INTERVAL_MS               (100)

#define CORE_MQTT_DEFAULT_KEEPALIVE_S              (1200)
#define CORE_MQTT_DEFAULT_CLEAN_SESSION            (1)
#define CORE_MQTT_DEFAULT_CONNECT_TIMEOUT_MS       (10 * 1000)
#define CORE_MQTT_DEFAULT_HEARTBEAT_INTERVAL_MS    (25 * 1000)
#define CORE_MQTT_DEFAULT_HEARTBEAT_MAX_LOST_TIMES (2)
#define CORE_MQTT_DEFAULT_SEND_TIMEOUT_MS          (5 * 1000)
#define CORE_MQTT_DEFAULT_RECV_TIMEOUT_MS          (5 * 1000)
#define CORE_MQTT_DEFAULT_REPUB_TIMEOUT_MS         (3 * 1000)
#define CORE_MQTT_DEFAULT_RECONN_ENABLED           (1)
#define CORE_MQTT_DEFAULT_RECONN_INTERVAL_MS       (2 * 1000)
#define CORE_MQTT_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)

#define CORE_MQTT_DIAG_TLV_MQTT_CONNECTION         (0x0010)
#define CORE_MQTT_DIAG_TLV_MQTT_HEARTBEAT          (0x0020)

#define CORE_MQTT_NWKSTATS_RTT_THRESHOLD           (10000)

typedef enum {
    CORE_MQTTOPT_APPEND_PROCESS_HANDLER,
    CORE_MQTTOPT_REMOVE_PROCESS_HANDLER,
    CORE_MQTTOPT_MAX
} core_mqtt_option_t;

int32_t core_mqtt_setopt(void *handle, core_mqtt_option_t option, void *data);
char *core_mqtt_get_product_key(void *handle);
char *core_mqtt_get_device_name(void *handle);
uint16_t core_mqtt_get_port(void *handle);
int32_t core_mqtt_get_nwkstats(void *handle, core_mqtt_nwkstats_info_t *nwk_stats_info);

#if defined(__cplusplus)
}
#endif

#endif

