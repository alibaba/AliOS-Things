/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef LWM2M_HDR_H
#define LWM2M_HDR_H

#include <liblwm2m.h>

typedef enum
{
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

lwm2m_data_t * lwm2m_data_new(int size);

void lwm2m_data_free(int size, lwm2m_data_t * dataP);

void lwm2m_data_encode(lwm2m_data_codec_type_t type, const void* valueP, lwm2m_data_t* dataP);

int lwm2m_data_decode(lwm2m_data_codec_type_t type, const lwm2m_data_t* dataP, void* value);

int lwm2m_string_to_uri(const char * buffer, size_t buffer_len, lwm2m_uri_t * uriP);

void * lwm2m_init(void * userData);

void lwm2m_deinit(void * handler);

int lwm2m_step(void* handler, time_t * timeoutP);

void lwm2m_handle_packet(void* handler, uint8_t * buffer, int length, void * fromSessionH);

#ifdef LWM2M_CLIENT_MODE
int lwm2m_configure(void* handler, const char * endpointName, const char * msisdn, const char * altPath, uint16_t numObject, lwm2m_object_t * objectList[]);

int lwm2m_add_object(void * handler, lwm2m_object_t * objectP);

int lwm2m_remove_object(void * handler, uint16_t id);

int lwm2m_update_registration(void * handler, uint16_t shortServerID, bool withObjects);

void lwm2m_resource_value_changed(void * contextP, lwm2m_uri_t * uriP);
#endif /* LWM2M_CLIENT_MODE */

lwm2m_object_t * lwm2m_get_serverlist(void * handler, bool is_bootstrap);

lwm2m_object_t * lwm2m_get_objectlist(void * handler);

lwm2m_observed_t * lwm2m_get_observedlist(void * handler);

lwm2m_client_state_t * lwm2m_get_client_state(void * handler);

#endif /* LWM2M_HDR_H */
