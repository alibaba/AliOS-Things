/*!
 * @file mal.h
 *
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _MAL_H_
#define _MAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "linkkit/mqtt_api.h"

/** @addtogroup aos_mal API
 *  MQTT Adapter Layer, which provides MQTT API service
 *  implemented on top of AT modules.
 *
 *  @{
 */

/**
 *  MAL provide MQTT API defined in linkkit/mqtt_api.h
 *   IOT_MQTT_Construct
 *   IOT_MQTT_Destroy
 *   IOT_MQTT_Yield
 *   IOT_MQTT_Subscribe
 *   IOT_MQTT_Unsubscribe
 *   IOT_MQTT_Publish
 * /

/* State of MQTT client */
typedef enum {
    IOTX_MC_STATE_INVALID = 0,                    /* MQTT in invalid state */
    IOTX_MC_STATE_INITIALIZED = 1,                /* MQTT in initializing state */
    IOTX_MC_STATE_CONNECTED = 2,                  /* MQTT in connected state */
    IOTX_MC_STATE_DISCONNECTED = 3,               /* MQTT in disconnected state */
    IOTX_MC_STATE_DISCONNECTED_RECONNECTING = 4,  /* MQTT in reconnecting state */
    IOTX_MC_STATE_CONNECT_BLOCK = 5               /* MQTT in connecting state when using async protocol stack */
} iotx_mc_state_t;

/**
* Add external device for MAL
*
* @param[in]  driver_name - MAL driver name.
* @param[in]  data - parameter.
*
* @return  0 - success, negative - failure
*/
int mal_add_dev(char* driver_name, void* data);


/**
* Initialization for MAL module
*
* @return  0 - success, negative - failure
*/
int mal_init(void);

/** @} */ /* end of group aos_mal */

#ifdef __cplusplus
}
#endif

#endif
