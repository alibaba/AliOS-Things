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

/* MQTT 5.0 conack error code */
#define CORE_MQTT_V5_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION    (0x84)
#define CORE_MQTT_V5_CONNACK_RCODE_BAD_USERNAME_PASSWORD            (0x86)
#define CORE_MQTT_V5_CONNACK_RCODE_SERVER_UNAVAILABLE               (0x88)
#define CORE_MQTT_V5_CONNACK_RCODE_NOT_AUTHORIZED                   (0x87)

/* MQTT 5.0 conack error code */
#define CORE_MQTT_V5_PROPERTY_ID_LEN                                 (1)
#define CORE_MQTT_V5_USER_PROPERTY_KEY_LEN                           (2)
#define CORE_MQTT_V5_USER_PROPERTY_VALUE_LEN                         (2)
#define CORE_MQTT_V5_TOPIC_ALIAS_LEN                                 (2)
#define CORE_MQTT_V5_TOPIC_ALIAS_MAX_LEN                             (2)
#define CORE_MQTT_V5_RECEIVE_MAX_LEN                                 (2)
#define CORE_MQTT_V5_RESPONSE_TOPIC_LEN                              (2)
#define CORE_MQTT_V5_CORELATION_DATA_LEN                             (2)
#define CORE_MQTT_V5_REASON_STRING_LEN                               (2)
#define CORE_MQTT_V5_DISCONNECT_REASON_CODE_LEN                      (1)

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
#define CORE_MQTT_UNSUB_PKT_RESERVE                 (0x02)

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

/* MQTT 5.0 implemented Packet */
#define CORE_MQTT_SERVER_DISCONNECT_PKT_TYPE        (0xE0)

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
    uint8_t network_type;       /* 0: TCP, 1: TLS */
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

    /* mqtt protovol version */
    uint8_t protocol_version;

    /* mqtt 5.0 specific*/
    void *topic_alias_mutex;
    struct core_list_head rx_topic_alias_list;
    struct core_list_head tx_topic_alias_list;
    uint8_t *pre_connect_property;
    uint8_t use_assigned_clientid;
    uint32_t tx_packet_max_size;
    uint32_t tx_topic_alias_max;
    uint32_t tx_topic_alias;
    uint16_t server_receive_max;
    uint8_t flow_control_enabled;
} core_mqtt_handle_t;

/* topic alias node. mqtt 5.0 specific*/
typedef struct {
    char *topic;
    uint16_t topic_alias;
    struct core_list_head linked_node;
} core_mqtt_topic_alias_node_t;

/**
 * @brief MQTT5.0 属性的枚举
 *
 * @details
 *
 * 传入@ref conn_property_t 等数据结构的MQTT报文类型
 *
 */
typedef enum {
    CORE_MQTTPROP_UNRESOLVED = 0x00,
    CORE_MQTTPROP_PAYLOAD_FORMAT_INDICATOR = 0x01,
    CORE_MQTTPROP_PUBLICATION_EXPIRY_INTERVAL = 0x2,
    CORE_MQTTPROP_CONTENT_TYPE = 0x3,
    CORE_MQTTPROP_RESPONSE_TOPIC = 0x8,
    CORE_MQTTPROP_CORRELATION_DATA = 0x9,
    CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER = 0xB,
    CORE_MQTTPROP_SESSION_EXPIRY_INTERVAL = 0x11,
    CORE_MQTTPROP_ASSIGNED_CLIENT_IDENTIFIER = 0x12,
    CORE_MQTTPROP_SERVER_KEEP_ALIVE = 0x13,
    CORE_MQTTPROP_AUTHENTICATION_METHOD = 0x15,
    CORE_MQTTPROP_AUTHENTICATION_DATA = 0x16,
    CORE_MQTTPROP_REQUEST_PROBLEM_INFORMATION = 0x17,
    CORE_MQTTPROP_WILL_DELAY_INTERVAL = 0x18,
    CORE_MQTTPROP_REQUEST_RESPONSE_INFORMATION = 0x19,
    CORE_MQTTPROP_RESPONSE_INFORMATION = 0x1A,
    CORE_MQTTPROP_SERVER_REFERENCE = 0x1C,
    CORE_MQTTPROP_REASON_STRING = 0x1F,
    CORE_MQTTPROP_RECEIVE_MAXIMUM = 0x21,
    CORE_MQTTPROP_TOPIC_ALIAS_MAX = 0x22,
    CORE_MQTTPROP_TOPIC_ALIAS = 0x23,
    CORE_MQTTPROP_MAX_QOS = 0x24,
    CORE_MQTTPROP_RETAIN_AVAILABLE = 0x25,
    CORE_MQTTPROP_USER_PROPERTY = 0x26,
    CORE_MQTTPROP_MAX_PACK_SIZE = 0x27,
    CORE_MQTTPROP_WILDCARD_SUBSCRIPTION_AVAILABLE = 0x28,
    CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER_AVAILABLE = 0x29,
    CORE_MQTTPROP_SHARED_SUBSCRIPTION_AVAILABLE = 0x2A,
    CORE_MQTTPROP_MAX,
} core_mqtt_property_t;

/* tlv structure. mqtt 5.0 specific */
typedef struct {
    core_mqtt_property_t type;
    uint16_t len;
    uint8_t  *value;
} type_len_value_t;

/**
 * @brief MQTT 5.0协议中, 上下行报文中所能携带属性
 */
typedef struct {
    uint8_t max_qos;
    uint16_t topic_alias_max;           /* 服务端/设备端能够支持的topic alias最大值 */
    uint8_t *assigned_clientid;
    uint32_t max_packet_size;
    uint8_t wildcard_subscription_available;
    uint8_t subscription_identifier_available;
    uint8_t shared_subscription_available;
    uint32_t message_expire_interval;
    uint16_t topic_alias;
    len_value_t response_topic;          /*response topic */
    len_value_t correlation_data;        /* 关联数据 */
    uint32_t subscription_identifier;
    user_property_t
    *user_property[USER_PROPERTY_MAX];   /* 用户属性的列表 todo 改成**的结构 */
    uint16_t server_receive_max;         /* 服务端的qos1流控值 */
    uint16_t client_receive_max;         /* 设备端的qos1流控值 */
    len_value_t *reason_string;
} general_property_t;

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

/* default settings for mqtt 5.0 */
#define CORE_TX_PKT_MAX_LENGTH                      (1024 * 256)
#define CORE_DEFAULT_SERVER_RECEIVE_MAX             (10)
#define CORE_MQTT_USER_PROPERTY_KEY_MAX_LEN         (128)  /* max key length for user property */
#define CORE_MQTT_USER_PROPERTY_VALUE_MAX_LEN       (128)  /* max value length for user property */

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

