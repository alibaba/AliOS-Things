/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_MQTT_INSTANCE_H
#define BE_MQTT_INSTANCE_H

enum {
    MQTT_INSTANCE_EVENT_DISCONNECTED = 0,
    MQTT_INSTANCE_EVENT_CONNECTED    = 1,
};

/**
 * @brief   Get the mqtt singleton instance.
 *
 * @param None
 *
 * @retval NULL : failed
 * @retval NOT_NULL : The singleton instance of mqtt client.
 * @see None.
 */
void *mqtt_get_instance();

/**
 * @brief   Remove the mqtt singleton instance.
 *
 * @param None
 *
 * @retval None
 * @see None.
 */
void mqtt_remove_instance();

/**
 * @brief   Set the mqtt singleton instance.
 *
 * @param None
 *
 * @retval -1:
 * @retval  0:
 * @see None.
 */
int mqtt_set_instance(void *mqtt_t);

/**
 * @brief   Initialize the mqtt singleton instance.
 *
 * @param [in] productKey
 * @param [in] deviceName
 * @param [in] deviceSecret
 * @param [in] maxMsgSize: mqtt read/send buffer size
 *
 * @retval 1:  mqtt instance have been init
 * @retval 0:  mqtt instance init success
 *     IOT_MQTT_Construct success,  MQTT connected.
 * @retval -1: mqtt instance init fail
 * @see None.
 */
int mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret,
                       int maxMsgSize);

/**
 * @brief   Deinitialize the mqtt singleton instance.
 *
 *
 * @retval 0:  success
 * @retval -1: fail
 * @see None.
 */
int mqtt_deinit_instance();

/**
 * @brief   Set mqtt event callback.
 *
 * @param [in] event callback
 * @param [in] user data
 *
 * @retval 0:  success
 * @retval -1: fail
 * @see None.
 */
int mqtt_set_event_cb(void (*event)(int event, void *ctx), void *ctx);

/**
 * @brief   Subscribe topic.
 *
 * @param [in] topic
 * @param [in] callback
 * @param [in] user data
 *
 * @retval 0:  success
 * @retval -1: fail
 * @see None.
 */
int mqtt_subscribe(char *topic,
                   void (*cb)(char *topic, int topic_len, void *payload,
                              int payload_len, void *ctx),
                   void *ctx);

/**
 * @brief   Unsubscribe topic.
 *
 * @param [in] topic
 *
 * @retval 0:  success
 * @retval -1: fail
 * @see None.
 */
int mqtt_unsubscribe(char *topic);

/**
 * @brief   Publish packet.
 *
 * @param [in] topic
 * @param [in] qos
 * @param [in] payload data
 * @param [in] payload data length
 *
 * @retval 0:  success
 * @retval -1: fail
 * @see None.
 */
int mqtt_publish(char *topic, int qos, void *data, int len);

/**
 * @brief   set mqtt domain
 *
 */
void mqtt_set_domain(char *domain, int port);

#endif /* BE_MQTT_INSTANCE_H */
