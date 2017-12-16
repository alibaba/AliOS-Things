/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_SRV_H_
#define _TEE_SRV_H_

#include "tee_types.h"
#include "tee_tos.h"

#define IS_TEE_SRV_VALID(_srv)  (('T' == (_srv)->magic[0]) && \
                                ('s' == (_srv)->magic[1]) && \
                                ('R' == (_srv)->magic[2]) && \
                                ('v' == (_srv)->magic[3]))

typedef struct _tee_srv_intf_t
{
    TEE_Result (*TA_CreateEntryPoint)(void);
    void (*TA_DestroyEntryPoint)(void);
    TEE_Result (*TA_OpenSessionEntryPoint)(
            uint32_t paramTypes,
            TEE_Param params[4],
            void **sessionContext);
    void (*TA_CloseSessionEntryPoint)(void *sessionContext);
    TEE_Result (*TA_InvokeCommandEntryPoint)(
            void *sessionContext,
            uint32_t commandID,
            uint32_t paramTypes,
            TEE_Param param[4]);
} tee_srv_intf_t;

typedef enum _tee_srv_prop_type_t
{
    TEE_SRV_PROP_TYPE_INVALID,
    TEE_SRV_PROP_TYPE_STR,
    TEE_SRV_PROP_TYPE_BOOL,
    TEE_SRV_PROP_TYPE_INT,
    TEE_SRV_PROP_TYPE_BINARY,
    TEE_SRV_PROP_TYPE_UUID,
    TEE_SRV_PROP_TYPE_IDENTITY,
    TEE_SRV_PROP_TYPE_MAX,
} tee_srv_prop_type_t;

typedef struct _tee_srv_prop_t
{
    int8_t *name;
    tee_srv_prop_type_t type;
    union
    {
        int8_t *str;
        bool boolean;
        uint32_t u32;
        struct
        {
            void *buf;
            uint32_t size;
        } bin;
        TEE_UUID uuid;
        TEE_Identity id;
    } data;
} tee_srv_prop_t;

typedef struct _tee_srv_data_t
{
    tee_srv_intf_t *intf;
    tee_srv_prop_t (*prop)[];
} tee_srv_data_t;

typedef struct _tee_srv_head_t
{
    uint8_t         magic[4];
    uint8_t         ver;
    uint8_t         rsvd[3];
    uint32_t        tee_fix;    /* rsvd for smart computing */
    uint32_t        data_link_start;
    uint32_t        data_link_end;
    uint32_t        data_offset;
    uint32_t        bss_link_start;
    uint32_t        bss_link_end;
    uint32_t        bss_offset;
    tee_srv_data_t  srv_data;
} tee_srv_head_t;

#define TEE_SRV_DATA_START(create, destroy, open_ss, close_ss, invoke_cmd)  \
    static tee_srv_intf_t _g_$$intf$$ __attribute__((__used__)) = {         \
            create, destroy, open_ss, close_ss, invoke_cmd                  \
    };                                                                      \
    static tee_srv_prop_t _g_$$prop$$[] __attribute__((__used__)) = {

#define TEE_SRV_UUID_PROPERTY(n, val)       \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_UUID, \
            .data.uuid = val,               \
        },

#define TEE_SRV_STR_PROPERTY(n, val)        \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_STR,  \
            .data.str = val,                \
        },

#define TEE_SRV_INT_PROPERTY(n, val)        \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_INT,  \
            .data.u32 = val,                \
        },

#define TEE_SRV_BOOL_PROPERTY(n, val)       \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_BOOL, \
            .data.boolean = val,            \
        },

#define TEE_SRV_DATA_END                                \
        {                                               \
            .name = NULL,                               \
            .type = TEE_SRV_PROP_TYPE_INVALID,          \
            .data.u32 = 0,                              \
        },                                              \
    };                                                  \
                                                        \
volatile static tee_srv_data_t _g_$$srv$$_$$data$$      \
        __attribute__((__used__))                       \
        __attribute__((__section__(".srv.data"))) =     \
        { &_g_$$intf$$, &_g_$$prop$$ };

#endif /* _TEE_SRV_H_ */
