/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __INFRA_ERRNO_H__
#define __INFRA_ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ERRCODE_BASE                                (0x0000)

/* General: 0x0000 ~ 0x00FF */
#define ERRCODE_SUCCESS                             (ERRCODE_BASE - 0x0000)

/* ProductKey from SDK user is NULL or too long */
#define ERRCODE_INVALID_PK                          (ERRCODE_BASE - 0x0001)
/* ProductSecret from SDK user is NULL or too long */
#define ERRCODE_INVALID_PS                          (ERRCODE_BASE - 0x0002)
/* DeviceName from SDK user is NULL or too long */
#define ERRCODE_INVALID_DN                          (ERRCODE_BASE - 0x0003)
/* DeviceSecret from SDK user is NULL or too long */

#define ERRCODE_INVALID_DS                          (ERRCODE_BASE - 0x0004)
/* General: 0x0000 ~ 0x00FF */

/* Utils: 0x0100 ~ 0x01FF */
#define ERRCODE_UTILS_BASE                          (-0x0100)

/* Utils: 0x0100 ~ 0x01FF */

/* System: 0x0200 ~ 0x02FF */
#define ERRCODE_SYS_BASE                            (-0x0200)

/* System: 0x0200 ~ 0x02FF */

/* MQTT: 0x0300 ~ 0x03FF */
#define ERRCODE_MQTT_BASE                           (-0x0300)

/* Deserialized CONNACK from MQTT server says protocol version is unacceptable */
#define ERRCODE_MQTT_CONNACK_VERSION_UNACCEPT       (ERRCODE_MQTT_BASE - 0x0001)
/* Deserialized CONNACK from MQTT server says identifier is rejected */
#define ERRCODE_MQTT_CONNACK_IDENT_REJECT           (ERRCODE_MQTT_BASE - 0x0002)
/* Deserialized CONNACK from MQTT server says service is not available */
#define ERRCODE_MQTT_CONNACK_SERVICE_NA             (ERRCODE_MQTT_BASE - 0x0003)
/* Deserialized CONNACK from MQTT server says username/password is invalid */
#define ERRCODE_MQTT_CONNACK_BAD_USERDATA           (ERRCODE_MQTT_BASE - 0x0005)

/* MQTT: 0x0300 ~ 0x03FF */

/* WiFi Provision: 0x0400 ~ 0x04FF */
#define ERRCODE_WIFI_BASE                           (-0x0400)

/* WiFi Provision: 0x0400 ~ 0x04FF */

/* COAP: 0x0500 ~ 0x05FF */
#define ERRCODE_COAP_BASE                           (-0x0500)

/* COAP: 0x0500 ~ 0x05FF */

/* HTTP: 0x0600 ~ 0x06FF */
#define ERRCODE_HTTP_BASE                           (-0x0600)

/* HTTP: 0x0600 ~ 0x06FF */

/* OTA: 0x0700 ~ 0x07FF */
#define ERRCODE_OTA_BASE                            (-0x0700)

/* OTA: 0x0700 ~ 0x07FF */

/* Bind: 0x0800 ~ 0x08FF */
#define ERRCODE_BIND_BASE                           (-0x0800)

/* Bind: 0x0800 ~ 0x08FF */

/* Device Model: 0x0900 ~ 0x09FF */
#define ERRCODE_DEV_MODEL_BASE                      (-0x0900)

/* Device Model: 0x0900 ~ 0x09FF */

/* SubDevice Mgmt: 0x0A00 ~ 0x0AFF */
#define ERRCODE_SUBDEV_MGMT_BASE                    (-0x0A00)

/* SubDevice Mgmt: 0x0A00 ~ 0x0AFF */

#ifdef __cplusplus
}
#endif
#endif

