/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOTX_MQTT_H__
#define __IOTX_MQTT_H__

#include "iotx_mqtt_config.h"
#include "iotx_utils.h"
#ifdef BUILD_AOS
#include "MQTTPacket/MQTTPacket.h"
#else
#include "MQTTPacket.h"
#endif

typedef enum {
    IOTX_MC_CONNECTION_ACCEPTED = 0,
    IOTX_MC_CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    IOTX_MC_CONNECTION_REFUSED_IDENTIFIER_REJECTED = 2,
    IOTX_MC_CONNECTION_REFUSED_SERVER_UNAVAILABLE = 3,
    IOTX_MC_CONNECTION_REFUSED_BAD_USERDATA = 4,
    IOTX_MC_CONNECTION_REFUSED_NOT_AUTHORIZED = 5
} iotx_mc_connect_ack_code_t;

/* State of MQTT client */
typedef enum {
    IOTX_MC_STATE_INVALID = 0,                    /* MQTT in invalid state */
    IOTX_MC_STATE_INITIALIZED = 1,                /* MQTT in initializing state */
    IOTX_MC_STATE_CONNECTED = 2,                  /* MQTT in connected state */
    IOTX_MC_STATE_DISCONNECTED = 3,               /* MQTT in disconnected state */
    IOTX_MC_STATE_DISCONNECTED_RECONNECTING = 4,  /* MQTT in reconnecting state */
} iotx_mc_state_t;

typedef enum MQTT_NODE_STATE {
    IOTX_MC_NODE_STATE_NORMANL = 0,
    IOTX_MC_NODE_STATE_INVALID,
} iotx_mc_node_t;

typedef enum {
    TOPIC_NAME_TYPE = 0,
    TOPIC_FILTER_TYPE
} iotx_mc_topic_type_t;

/* Handle structure of subscribed topic */
typedef struct iotx_mc_topic_handle_s {
    const char *topic_filter;
    iotx_mc_topic_type_t topic_type;
    iotx_mqtt_event_handle_t handle;
    struct iotx_mc_topic_handle_s *next;
} iotx_mc_topic_handle_t;

/* Handle structure of subscribed topic */
typedef struct  {
    char *topic_filter;
    iotx_mqtt_event_handle_func_fpt handle;
    void *user_data;
    iotx_mqtt_qos_t qos;
    struct list_head linked_list;
} iotx_mc_offline_subs_t;

/* Information structure of subscribed topic */
typedef struct SUBSCRIBE_INFO {
    enum msgTypes               type;               /* type, (sub or unsub) */
    uint16_t                    msg_id;             /* packet id of subscribe(unsubcribe) */
    iotx_time_t                 sub_start_time;     /* start time of subscribe request */
    iotx_mc_node_t              node_state;         /* state of this node */
    iotx_mc_topic_handle_t     *handler;            /* handle of topic subscribed(unsubcribed) */
    uint16_t                    len;                /* length of subscribe message */
    unsigned char              *buf;                /* subscribe message */
    struct list_head            linked_list;
} iotx_mc_subsribe_info_t, *iotx_mc_subsribe_info_pt;

#if !WITH_MQTT_ONLY_QOS0
/* Information structure of published topic */
typedef struct REPUBLISH_INFO {
    iotx_time_t                 pub_start_time;     /* start time of publish request */
    iotx_mc_node_t              node_state;         /* state of this node */
    uint16_t                    msg_id;             /* packet id of publish */
    uint32_t                    len;                /* length of publish message */
    unsigned char              *buf;                /* publish message */
    struct list_head            linked_list;
} iotx_mc_pub_info_t, *iotx_mc_pub_info_pt;
#endif
/* Reconnected parameter of MQTT client */
typedef struct {
    iotx_time_t         reconnect_next_time;        /* the next time point of reconnect */
    uint32_t            reconnect_time_interval_ms; /* time interval of this reconnect */
} iotx_mc_reconnect_param_t;

/* structure of MQTT client */
typedef struct Client {
    void                           *lock_generic;                               /* generic lock */
    uint32_t                        packet_id;                                  /* packet id */
    uint32_t                        request_timeout_ms;                         /* request timeout in millisecond */
    uint32_t                        buf_size_send;                              /* send buffer size in byte */
#if WITH_MQTT_DYN_BUF
    uint32_t                        buf_size_send_max;                          /* send buffer size max limit in byte */
    uint32_t                        buf_size_read_max;                          /* recv buffer size max limit in byte */
#endif
    uint32_t                        buf_size_read;                              /* read buffer size in byte */
    uint8_t                         keepalive_probes;                           /* keepalive probes */
    char                           *buf_send;                                   /* pointer of send buffer */
    char                           *buf_read;                                   /* pointer of read buffer */
    iotx_mc_topic_handle_t         *first_sub_handle;                           /* list of subscribe handle */
    utils_network_pt                ipstack;                                    /* network parameter */
    iotx_time_t                     next_ping_time;                             /* next ping time */
    int                             ping_mark;                                  /* flag of ping */
    iotx_mc_state_t                 client_state;                               /* state of MQTT client */
    iotx_mc_reconnect_param_t       reconnect_param;                            /* reconnect parameter */
    MQTTPacket_connectData          connect_data;                               /* connection parameter */
#if !WITH_MQTT_ONLY_QOS0
    struct list_head                list_pub_wait_ack;                          /* list of wait publish ack */
#endif
    struct list_head                list_sub_wait_ack;                          /* list of subscribe or unsubscribe ack */
    void                           *lock_list_pub;                              /* lock for list of QoS1 pub */
    void                           *lock_list_sub;                              /* lock for list of sub/unsub */
    void                           *lock_write_buf;                             /* lock of write */
    void                           *lock_read_buf;                             /* lock of write */
    void                           *lock_yield;
    iotx_mqtt_event_handle_t        handle_event;                               /* event handle */
} iotx_mc_client_t, *iotx_mc_client_pt;

/* Information structure of mutli-subscribe */
typedef struct {
    const char                                    *topicFilter;
    iotx_mqtt_qos_t                                qos;
    iotx_mqtt_event_handle_func_fpt                messageHandler;
} iotx_mutli_sub_info_t, *iotx_mutli_sub_info_pt;

typedef struct {
    struct list_head offline_sub_list;
    void *mutex;
} offline_sub_list_t;

int iotx_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams);
int iotx_mc_connect(iotx_mc_client_t *pClient);
int iotx_mc_disconnect(iotx_mc_client_t *pClient);
int iotx_mc_attempt_reconnect(iotx_mc_client_t *pClient);
int iotx_mc_handle_reconnect(iotx_mc_client_t *pClient);

int iotx_mc_unsubscribe(iotx_mc_client_t *c, const char *topicFilter);
int iotx_mc_subscribe(iotx_mc_client_t *c,
                      const char *topicFilter,
                      iotx_mqtt_qos_t qos,
                      iotx_mqtt_event_handle_func_fpt topic_handle_func,
                      void *pcontext);
int iotx_mc_publish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg);

#endif  /* __IOTX_MQTT_H__ */
