/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN
#include "canfestival.h"
#include "aos/errno.h"
#include "board.h"

#define MAX_NODE_ID  127
#define MAX_INDEX    0xFFFF
#define MAX_SUBINDEX 0xFF
#define CO_BAUD_125K 125000
#define CO_BAUD_250K 250000
#define CO_BAUD_500K 500000
#define CO_BAUD_1M   1000000

typedef enum canopen_status {
    CO_SUCCESS = 0u,
    CO_ODENTRY_REGISTER_ERROR,
    CO_DRIVER_INIT_FAILED,
} co_status_t;

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

int canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port, uint32_t baud, CO_Data* d);
int canopen_od_init(CO_Data* d, co_register_function_callback_t* fun_callback);
int canopen_od_handler(CO_Data* d, uint16_t index, uint8_t sub_index, co_objectdict_cb* od_callback);
int init_nodes(CO_Data* d, uint8_t id);

#endif    /* AOS_CANOPEN */