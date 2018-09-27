/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AT_MQTT_ICA_CLIENT_H_
#define _AT_MQTT_ICA_CLIENT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AT_MQTT_ICA_MQTTMODE        "AT+IMQTTMODE"
#define AT_MQTT_ICA_MQTTOPEN        "AT+IMQTTOPEN"
#define AT_MQTT_ICA_MQTTAUTH        "AT+IMQTTAUTH"
#define AT_MQTT_ICA_MQTTPARA        "AT+IMQTTPARA"
#define AT_MQTT_ICA_MQTTCONN        "AT+IMQTTCONN"
#define AT_MQTT_ICA_MQTTPUB         "AT+IMQTTPUB"
#define AT_MQTT_ICA_MQTTPUBIN       "AT+IMQTTPUBIN"
#define AT_MQTT_ICA_MQTTSUB         "AT+IMQTTSUB"
#define AT_MQTT_ICA_MQTTUNSUB       "AT+IMQTTUNSUB"
#define AT_MQTT_ICA_MQTTSTATE       "AT+IMQTTSTATE"
#define AT_MQTT_ICA_MQTTDISCONN     "AT+IMQTTDISCONN"
#define AT_MQTT_ICA_MQTTDBG         "AT+IMQTTDBG"

#define AT_MQTT_ICA_MQTTRCV         "+IMQTT"
#define AT_MQTT_ICA_MQTTRCVPUB      "+IMQTTRCVPUB"
#define AT_MQTT_ICA_MQTTRCVPUBIN    "+IMQTTRCVPUBIN"
#define AT_MQTT_ICA_MQTTPINGRSP     "+IMQTTPINGRSP"
#define AT_MQTT_ICA_MQTTAUTHRSP     "+IMQTTAUTH"
#define AT_MQTT_ICA_MQTTPUBRSP      "+IMQTTPUB"
#define AT_MQTT_ICA_MQTTSUBRSP      "+IMQTTSUB"
#define AT_MQTT_ICA_MQTTUNSUBRSP    "+IMQTTUNSUB"
#define AT_MQTT_ICA_MQTTSTATERSP    "+IMQTTSTATE"

#define AT_MQTT_ICA_POSTFIX         "\r\n"

#define AT_MQTT_CONNECT_NETWORK     "AT+WJAP=Yuemewifi-3766,aos12345"
#define AT_MQTT_CONNECT_GOTIP       "+WEVENT:STATION_UP"


typedef struct mqtt_state_s {
    uint8_t  auto_report_flag;
    uint8_t  mqtt_state;
} mqtt_state_t;


typedef enum {
    AT_MQTT_STATE_IDLE      = 0,
    AT_MQTT_STATE_JOIN_WIFI,
    AT_MQTT_STATE_GOTIP,
    AT_MQTT_STATE_AUTH,
    AT_MQTT_STATE_AUTH_FINISH,
    AT_MQTT_STATE_PUBLISH,
    AT_MQTT_STATE_PUBLISH_FINISH,
    AT_MQTT_STATE_SUBSCRIBE,
    AT_MQTT_STATE_SUBSCRIBE_FINISH,
    AT_MQTT_STATE_UNSUBSCRIBE,
    AT_MQTT_STATE_UNSUBSCRIBE_FINISH,
} at_mqtt_state_type_t;


int at_mqtt_ica_client_init(void);
int at_mqtt_ica_client_deinit(void);


#ifdef __cplusplus
}
#endif

#endif

