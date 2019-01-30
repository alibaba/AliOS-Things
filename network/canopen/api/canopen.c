/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN
#include "canopenapi.h"

void canopen_od_init(CO_Data* d, co_register_function_callback_t fun_callback)
{
    d->initialisation = fun_callback.initialisation;
    d->preOperational = fun_callback.preOperational;
    d->operational = fun_callback.operational;
    d->stopped = fun_callback.stopped;
    d->heartbeatError = fun_callback.heartbeatError;
    d->nodeguardError = fun_callback.nodeGuardError;
    d->post_sync = fun_callback.post_sync;
    d->post_TPDO = fun_callback.post_TPDO;
    d->post_emcy = fun_callback.post_emcy;
    d->post_SlaveBootup = fun_callback.post_Slave_Bootup;
    d->storeODSubIndex = fun_callback.store_OD_SubIndex;
}

uint32_t canopen_od_handler(CO_Data* d, uint16_t index, uint8_t sub_index, co_objectdict_cb od_callback)
{
    uint32_t ret = -1;

    ret = RegisterSetODentryCallBack(d, index, sub_index, od_callback);
    return ret;
}

void init_nodes(CO_Data* d, uint32_t id)
{
    /* set node id */
    setNodeId(d, id);
    /* set the node to Initialisation state */
    setState(d, Initialisation);
}

void canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port, uint32_t baud, CO_Data* d)
{
    /* init can driver add can port,can baud and OD */
    can_init(can_port, baud, d);
    /* init timer driver add timer port */
    timer_init(timer_port);
}
#endif    /* AOS_CANOPEN */