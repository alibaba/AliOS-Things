/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN
#include "canfestival.h"
#include "board.h"

typedef uint32_t (*co_objectdict_cb)(CO_Data*, const indextable *, uint8_t);

typedef struct od_subindex
{
    uint8_t          access_type;
    uint8_t          data_type;
    uint32_t         size;
    void*            object;
    co_objectdict_cb callback;
}od_subindex_t;

/** Struct for creating entries in the communictaion profile
 */
typedef struct indextable
{
    od_subindex_t* sub_index;
    uint8_t        sub_count;
    uint16_t       index;
}indextable_t;

typedef void (*co_heartbeatError_t)(CO_Data*, uint8_t);
typedef void (*co_nodeguardError_t)(CO_Data*, uint8_t);
typedef void (*co_initialisation_t)(CO_Data*);
typedef void (*co_preOperational_t)(CO_Data*);
typedef void (*co_operational_t)(CO_Data*);
typedef void (*co_stopped_t)(CO_Data*);
typedef void (*co_post_sync_t)(CO_Data*);
typedef void (*co_post_TPDO_t)(CO_Data*);
typedef void (*co_post_SlaveBootup_t)(CO_Data*, uint8_t);
typedef void (*co_post_emcy_t)(CO_Data*, uint8_t, uint16_t, uint8_t, const uint8_t);
typedef uint32_t (* co_storeODSubIndex_t)(CO_Data*, uint16_t, uint8_t);

typedef struct co_register_function_callback
{
    co_initialisation_t   initialisation;
    co_preOperational_t   preOperational;
    co_operational_t      operational;
    co_stopped_t          stopped;
    co_post_sync_t        post_sync;
    co_post_TPDO_t        post_TPDO;
    co_post_SlaveBootup_t post_Slave_Bootup;
    co_post_emcy_t        post_emcy;
    co_heartbeatError_t   heartbeatError;
    co_nodeguardError_t   nodeGuardError;
    co_storeODSubIndex_t  store_OD_SubIndex;
}co_register_function_callback_t;

void canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port, uint32_t baud, CO_Data* d);
void canopen_od_init(CO_Data* d, co_register_function_callback_t fun_callback);
uint32_t canopen_od_handler(CO_Data* d, uint16_t index, uint8_t sub_index, co_objectdict_cb od_callback);
void init_nodes(CO_Data* d, uint32_t id);

#endif    /* AOS_CANOPEN */