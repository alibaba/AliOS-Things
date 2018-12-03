/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef IOTX_MQTT_CONFIG_H__
#define IOTX_MQTT_CONFIG_H__

#ifndef WITH_MQTT_JSON_FLOW
    #define WITH_MQTT_JSON_FLOW                 (0)
#endif

#ifndef WITH_MQTT_DYN_BUF
    #define WITH_MQTT_DYN_BUF                   (0)
#endif

#ifndef WITH_MQTT_QOS2_PACKET
    #define WITH_MQTT_QOS2_PACKET               (0)
#endif

#ifndef WITH_MQTT_FLOW_CTRL
    #define WITH_MQTT_FLOW_CTRL                 (0)
#endif

#ifndef WITH_MQTT_ONLY_QOS0
    #define WITH_MQTT_ONLY_QOS0                 (0)
#endif

#ifndef WITH_MQTT_DYN_CONNINFO
    #define WITH_MQTT_DYN_CONNINFO              (0)
#endif

/* maximum republish elements in list */
#define IOTX_MC_REPUB_NUM_MAX                   (20)

/* MQTT client version number */
#define IOTX_MC_MQTT_VERSION                    (4)

/* maximum length of topic name in byte */
#define IOTX_MC_TOPIC_NAME_MAX_LEN              (128)

/* maximum MQTT packet-id */
#define IOTX_MC_PACKET_ID_MAX                   (65535)

/* maximum number of simultaneously invoke subscribe request */
#define IOTX_MC_SUB_REQUEST_NUM_MAX             (256)

/* Minimum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MIN_MS       (1000)

/* Maximum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MAX_MS       (60000)

/* Minimum timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_MIN_MS          (500)

/* Maximum timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_MAX_MS          (5000)

/* Default timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_DEFAULT_MS      (2000)

/* Max times of keepalive which has been send and did not received response package */
#define IOTX_MC_KEEPALIVE_PROBE_MAX             (3)


#endif  /* IOTX_MQTT_CONFIG_H__ */
