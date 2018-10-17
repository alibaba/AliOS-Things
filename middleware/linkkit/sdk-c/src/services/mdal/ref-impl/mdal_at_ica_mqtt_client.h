/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_ICA_AT_MQTT_CLIENT_H_
#define _MDAL_ICA_AT_MQTT_CLIENT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AT_ICA_MQTT_MQTTMODE        "AT+IMQTTMODE"
#define AT_ICA_MQTT_MQTTOPEN        "AT+IMQTTOPEN"
#define AT_ICA_MQTT_MQTTAUTH        "AT+IMQTTAUTH"
#define AT_ICA_MQTT_MQTTPARA        "AT+IMQTTPARA"
#define AT_ICA_MQTT_MQTTCONN        "AT+IMQTTCONN"
#define AT_ICA_MQTT_MQTTPUB         "AT+IMQTTPUB"
#define AT_ICA_MQTT_MQTTPUBIN       "AT+IMQTTPUBIN"
#define AT_ICA_MQTT_MQTTSUB         "AT+IMQTTSUB"
#define AT_ICA_MQTT_MQTTUNSUB       "AT+IMQTTUNSUB"
#define AT_ICA_MQTT_MQTTSTATE       "AT+IMQTTSTATE"
#define AT_ICA_MQTT_MQTTDISCONN     "AT+IMQTTDISCONN"
#define AT_ICA_MQTT_MQTTDBG         "AT+IMQTTDBG"

#define AT_ICA_MQTT_MQTTRCV         "+IMQTT"
#define AT_ICA_MQTT_MQTTERROR       "+CME"
#define AT_ICA_MQTT_MQTTOK          "OK"
#define AT_ICA_MQTT_MQTTRCVPUB      "+IMQTTRCVPUB"
#define AT_ICA_MQTT_MQTTRCVPUBIN    "+IMQTTRCVPUBIN"
#define AT_ICA_MQTT_MQTTPINGRSP     "+IMQTTPINGRSP"
#define AT_ICA_MQTT_MQTTAUTHRSP     "+IMQTTAUTH"
#define AT_ICA_MQTT_MQTTPUBRSP      "+IMQTTPUB"
#define AT_ICA_MQTT_MQTTSUBRSP      "+IMQTTSUB"
#define AT_ICA_MQTT_MQTTUNSUBRSP    "+IMQTTUNSUB"
#define AT_ICA_MQTT_MQTTSTATERSP    "+IMQTTSTATE"

#define AT_ICA_MQTT_POSTFIX         "\r\n"

typedef struct mqtt_state_s {
    uint8_t  auto_report_flag;
    uint8_t  mqtt_state;
} mqtt_state_t;

int at_ica_mqtt_client_init(void);
int at_ica_mqtt_client_deinit(void);
int at_ica_mqtt_client_disconn(void);
int at_ica_mqtt_client_auth(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
int at_ica_mqtt_client_conn(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
int at_ica_mqtt_client_subscribe(char *topic,
                                 uint8_t qos,
                                 int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms);
int at_ica_mqtt_client_unsubscribe(char *topic,
                                   int *mqtt_packet_id,
                                   int *mqtt_status);
int at_ica_mqtt_client_publish(char *topic, uint8_t qos, char *message);
int at_ica_mqtt_client_state(void);
int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif

