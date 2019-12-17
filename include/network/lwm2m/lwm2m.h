/**
 * @file lwm2m.h
 * lwm2m interface header file
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LWM2M_API_H
#define LWM2M_API_H

/** @addtogroup aos_lwm2m lwm2m
 *  Interfaces for lwm2m client.
 *
 *  @{
 */

#include "liblwm2m.h"

/**
 * lwm2m data type
 */
typedef enum {
    LWM2M_CODEC_TYPE_UNDEFINED = 0,
    LWM2M_CODEC_TYPE_OBJECT,
    LWM2M_CODEC_TYPE_OBJECT_INSTANCE,
    LWM2M_CODEC_TYPE_MULTIPLE_RESOURCE,

    LWM2M_CODEC_TYPE_STRING,
    LWM2M_CODEC_TYPE_NSTRING,
    LWM2M_CODEC_TYPE_OPAQUE,
    LWM2M_CODEC_TYPE_INTEGER,
    LWM2M_CODEC_TYPE_FLOAT,
    LWM2M_CODEC_TYPE_BOOLEAN,

    LWM2M_CODEC_TYPE_OBJECT_LINK
} lwm2m_data_codec_type_t;


/**
 * Allocate lwm2m data array
 *
 * @param[in]   size   the number of lwm2m data to allocate.
 *
 * @return none-NULL on succsss, NULL on failure.
 */
lwm2m_data_t *lwm2m_data_new(int size);

/**
 * Free lwm2m data array
 *
 * @param[in]   size   the size of lwm2m data array.
 * @param[in]   dataP  the address of lwm2m data array.
 *
 * @return none
 */
void lwm2m_data_free(int size, lwm2m_data_t *dataP);

/**
 * Fill lwm2m data with value according to data type
 *
 * @param[in]   type    lwm2m data type.
 * @param[in]   valueP  the address of the value
 * @param[in]   dataP   the address of lwm2m data
 *
 * @return none
 */
void lwm2m_data_encode(lwm2m_data_codec_type_t type, const void *valueP, lwm2m_data_t *dataP);

/**
 * Fill lwm2m data with value according to data type
 *
 * @param[in]   type    lwm2m data type.
 * @param[in]   dataP   the address of lwm2m data.
 * @param[out]  dataP   the address of the value.
 *
 * @return 0 on succsss, -1 on failure
 */
int lwm2m_data_decode(lwm2m_data_codec_type_t type, const lwm2m_data_t *dataP, void *value);

/**
 * Parse an URI in LWM2M format and fill the lwm2m_uri_t.
 *
 * @param[in]   buffer       the address of the string buffer
 * @param[in]   buffer_len   the size of the string buffer
 * @param[out]  uriP         the address of lwm2m uri.
 *
 * @return the number of characters read from buffer, positive on success, 0 on failure
 */
int lwm2m_string_to_uri(const char *buffer, size_t buffer_len, lwm2m_uri_t *uriP);

/**
 * Initialize a lwm2m context.
 *
 * @param[in]  userData   the address of user data.
 *
 * @return none-NULL on succsss, NULL on failure.
 */
void *lwm2m_init(void *userData);

/**
 * Deinitialize a lwm2m context
 *
 * @param[in]  handler    the address of lwm2m context.
 *
 * @return none.
 */
void lwm2m_deinit(void *handler);

/**
 * Perform any required pending operation and adjust timeoutP to the maximal time interval to wait in seconds.
 *
 * @param[in]  handler    the address of lwm2m context.
 * @param[in]  timeoutP   the address of timeout
 *
 * @return 0 on success, none-zero on failure. See COAP error code in liblwm2m.h.
 */
int lwm2m_step(void *handler, time_t *timeoutP);

/**
 * Dispatch received data
 *
 * @param[in]  handler          the address of lwm2m context.
 * @param[in]  buffer           the address of packet buffer.
 * @param[in]  length           the length of packcet.
 * @param[in]  fromSessionH     the session of CoAP.
 *
 * @return none
 */
void lwm2m_handle_packet(void *handler, uint8_t *buffer, int length, void *fromSessionH);

#ifdef LWM2M_CLIENT_MODE

/**
 * Configure lwm2m client context
 *
 * @note: configure the client side with the Endpoint Name, binding, MSISDN (can be nil), alternative path
 * for objects (can be nil) and a list of objects.
 * LWM2M Security Object (ID 0) must be present with either a bootstrap server or a LWM2M server and
 * its matching LWM2M Server Object (ID 1) instance
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  endpoint         the address of endpoint name.
 * @param[in]  msisdn           the identity of mobile station.
 * @param[in]  altPath          the alternative path.
 * @param[in]  numObject        the number of objects support.
 * @param[in]  objectList       the array of objects
 *
 * @return 0 on success, none-zero on failure. See COAP error code in liblwm2m.h.
 */
int lwm2m_configure(void *handler, const char *endpointName, const char *msisdn,
                    const char *altPath, uint16_t numObject, lwm2m_object_t *objectList[]);

/**
 * Add a lwm2m object
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  objectP          the address of object.
 *
 * @return 0 on success, none-zero on failure. See COAP error code in liblwm2m.h.
 */
int lwm2m_add_object(void *handler, lwm2m_object_t *objectP);

/**
 * Remove lwm2m object
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  id               the object id.
 *
 * @return 0 on success, none-zero on failure. See COAP error code in liblwm2m.h.
 */
int lwm2m_remove_object(void *handler, uint16_t id);

/**
 * Update lwm2m client registeration
 *
 * @note: send a registration update to the server specified by the server short identifier
 * or all if the ID is 0.
 * If withObjects is true, the registration update contains the object list.
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  shortServerID    the address of endpoint name.
 * @param[in]  withObjects      whether update with object.
 *
 * @return 0 on success, none-zero on failure. See COAP error code in liblwm2m.h.
 */
int lwm2m_update_registration(void *handler, uint16_t shortServerID, bool withObjects);

/**
 * Action on lwm2m resource value changed
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  uriP             the address of the resource
 *
 * @return none
 */
void lwm2m_resource_value_changed(void *contextP, lwm2m_uri_t *uriP);
#endif /* LWM2M_CLIENT_MODE */

/**
 * Get lwm2m server list
 *
 * @param[in]  handler          the address of lwm2m client context.
 * @param[in]  is_bootstrap     whether is bootstrap server.
 *
 * @return none-NULL on success, NULL on failure.
*/
lwm2m_object_t *lwm2m_get_serverlist(void *handler, bool is_bootstrap);

/**
 * Get lwm2m object list
 *
 * @param[in]  handler          the address of lwm2m client context.
 *
 * @return none-NULL on success, NULL on failure.
 */
lwm2m_object_t *lwm2m_get_objectlist(void *handler);

/**
 * Get lwm2m observerd resources list
 *
 * @param[in]  handler          the address of lwm2m client context.
 *
 * @return none-NULL on success, NULL on failure.
 */
lwm2m_observed_t *lwm2m_get_observedlist(void *handler);

/**
 * Get lwm2m client state
 *
 * @param[in]  handler          the address of lwm2m client context.
 *
 * @return none-NULL on success, NULL on failure.
 */
lwm2m_client_state_t *lwm2m_get_client_state(void *handler);

/** @} */

#endif /* LWM2M_HDR_H */
