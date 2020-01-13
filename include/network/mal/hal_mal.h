/*!
 * @file hal_mal.h
 *
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _HAL_MAL_H_
#define _HAL_MAL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_mal API
 *  MQTT Adapter Layer, which provides MQTT API service
 *  implemented on top of AT modules.
 *
 *  @{
 */

#include "linkkit/mqtt_api.h"

typedef int (*mqtt_data_input_cb_t)(char *topic, unsigned int topic_len, char *message, unsigned int msg_len);

typedef struct mal_op_s {
    struct mal_op_s * next;  //<! Next mal_op_t structure

    char *version; //<! Reserved for furture use.

    char *name; //<! Drvier name

    /**
     * Add mal device .
     *
     * @param[in]  data - device parameters
     *
     * @return  0 - success, -1 - failure
     */
    int (*add_dev)(void* data);

    /**
     * Module low level init so that it's ready to setup mqtt connection.
     *
     * @param[in]  pInitParams - connect parameters which are used to setup
     *                           the MQTT connection.
     * @return  0 - success, -1 - failure
     */
    int (*init)(iotx_mqtt_param_t *pInitParams);

    /**
     * Start a socket connection via module.
     *
     * @param[in]  proKey    - product key.
     * @param[in]  devName   - device name.
     * @param[in]  devSecret - device secret.
     *
     * @note: If the module does not accept the triple, ingore these parameters
     *
     * @return  0 - success, -1 - failure
     */
    int (*connect)(char *proKey, char *devName, char *devSecret);

   /**
    * Subscribe a topic via the MQTT connection
    *
    * @param[in]  topic          - MQTT topic.
    * @param[in]  qos            - QoS used.
    * @param[out] mqtt_packet_id - MQTT packet ID.
    * @param[out] mqtt_status    - MQTT subscribe status.
    * @param[out] timeout_ms     - time out in milliseconds.
    *
    * @return  0 - success, -1 - failure
    */
    int (*subscribe)(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms);

    /**
    * Unsubscribe a topic via the MQTT connection
    *
    * @param[in]  topic          - MQTT topic.
    * @param[out] mqtt_packet_id - MQTT packet ID.
    * @param[out] mqtt_status    - MQTT unsubscribe status.
    *
    * @return  0 - success, -1 - failure
    */
    int (*unsubscribe)(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status);

   /**
    * Publish MQTT message to the topic via module.
    *
    * @param[in]  topic - MQTT topic
    * @param[in]  qos   - quality of service used
    * @param[in]  message - message to be published
    * @param[in]  msg_len - message length
    *
    * @return  0 - success, -1 - failure
    */
    int (*publish)(const char *topic, int qos, const char *message, unsigned int msg_len);

    /**
    * Query AT MQTT connection status
    *
    * @return  MQTT status value.
    *          Refer to definition of iotx_mc_state_t in mal.h.
    */
    int (*conn_state)(void);

    /**
    * Disconnect the MQTT connection via module.
    *
    * @return  0 - success, -1 - failure
    */
    int (*disconn)(void);

    /**
     * Destroy MAL or exit low level state if necessary.
     *
     * @return  0 - success, -1 - failure
     */
    int (*deinit)(void);

    /**
     * Register mqtt data input function
     * Input data from mqtt module.
     * This callback should be called when mqtt data is received from the module
     * @param[in]  topic     - topic of the received message.
     * @param[in]  topic_len - length of topic.
     * @param[in]  message   - received message.
     * @param[in]  meg_len   - length of received message.
     *
     * @return  0 - success, -1 - failure
     */
    int (*register_mqtt_data_input_cb)(mqtt_data_input_cb_t cb);
} mal_op_t;

#ifdef __cplusplus
}
#endif

#endif