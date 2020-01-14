#ifndef _MAL_INTERNAL_H_
#define _MAL_INTERNAL_H_

/* Enum type of topic */
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

/* Structure type of mqtt client */
typedef struct Client {
    uint32_t                        packet_id;                                  /* packet id */
    void                           *lock_generic;                               /* generic lock */
    void                           *lock_yield;
    iotx_mc_topic_handle_t         *first_sub_handle;                           /* list of subscribe handle */
    void                           *lock_list_sub;                              /* lock for list of sub/unsub */
    iotx_mc_state_t                 client_state;                               /* state of MQTT client */
    iotx_mqtt_event_handle_t        handle_event;                               /* event handle */
} iotx_mc_client_t, *iotx_mc_client_pt;

#endif
