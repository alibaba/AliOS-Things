/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_ICA_AT_CLIENT_H_
#define _MDAL_ICA_AT_CLIENT_H_

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

#define AT_MQTT_MAX_MSG_LEN     1024
#define AT_MQTT_MAX_TOPIC_LEN   256
#define AT_MQTT_WAIT_FOREVER 0xffffffffu

#define AT_MQTT_CMD_MAX_LEN             1024
#define AT_MQTT_CMD_SUCCESS_RSP         "OK"
#define AT_MQTT_CMD_FAIL_RSP            "FAIL"
#define AT_MQTT_CMD_ERROR_RSP           "ERROR"
#define AT_MQTT_SUBSCRIBE_FAIL          128
#define AT_MQTT_RSP_MAX_LEN             1500

#define AT_MQTT_WAIT_TIMEOUT            10*1000

#define mdal_err(...)               log_err("MAL", __VA_ARGS__)


typedef struct mqtt_state_s {
    uint8_t  auto_report_flag;
    uint8_t  mqtt_state;
} mqtt_state_t;

#ifdef __cplusplus
}
#endif

#endif

