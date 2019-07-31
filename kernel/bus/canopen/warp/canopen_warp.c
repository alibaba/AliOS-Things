/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN

#include "canopen_warp.h"

int canopen_od_init(CO_Data *d, co_register_function_callback_t *fun_callback)
{
    if (d == NULL || fun_callback == NULL) {
        return -EINVAL;
    }

    d->initialisation   = fun_callback->initialisation;
    d->preOperational   = fun_callback->preOperational;
    d->operational      = fun_callback->operational;
    d->stopped          = fun_callback->stopped;
    d->heartbeatError   = fun_callback->heartbeatError;
    d->nodeguardError   = fun_callback->nodeGuardError;
    d->post_sync        = fun_callback->post_sync;
    d->post_TPDO        = fun_callback->post_TPDO;
    d->post_emcy        = fun_callback->post_emcy;
    d->post_SlaveBootup = fun_callback->post_Slave_Bootup;
    d->storeODSubIndex  = fun_callback->store_OD_SubIndex;

    return CO_SUCCESS;
}

int canopen_od_handler(CO_Data *d, uint16_t index, uint8_t sub_index, co_objectdict_cb *od_callback)
{
    uint32_t ret;

    if (d == NULL || od_callback == NULL) {
        return -EINVAL;
    }

    if (!(index >= 0 && index < MAX_INDEX)) {
        return -EOVERFLOW;
    }

    if (!(sub_index >= 0 && sub_index < MAX_SUBINDEX)) {
        return -EOVERFLOW;
    }

    ret = RegisterSetODentryCallBack(d, index, sub_index, *od_callback);
    if (ret != 0) {
        return -CO_ODENTRY_REGISTER_ERROR;
    }

    return CO_SUCCESS;
}

int init_nodes(CO_Data *d, uint8_t id)
{
    if (d == NULL) {
        return -EINVAL;
    }

    if (!(id >= 0 && id <= MAX_NODE_ID)) {
        return -ENODATA;
    }
    /* set node id */
    setNodeId(d, id);
    /* set the node to Initialisation state */
    setState(d, Initialisation);

    return CO_SUCCESS;
}

int canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port, uint32_t baud, CO_Data *d)
{
    int ret;

    if (!(can_port >=0 && can_port < PORT_CAN_INVALID)) {
        return -ENODEV;
    }

    if (!(timer_port >= 0 && timer_port < PORT_TIMER_INVALID)) {
        return -ENODEV;
    }

    if (!(baud == CO_BAUD_125K || baud == CO_BAUD_250K || baud == CO_BAUD_500K || baud == CO_BAUD_1M)) {
        return -EINVAL;
    }

    if (d == NULL) {
        return -EINVAL;
    }
    /* init can driver add can port,can baud and OD */
    ret = can_init(can_port, baud, d);
    if (ret != 0) {
        return -CO_DRIVER_INIT_FAILED;
    }
    /* init timer driver add timer port */
    ret = timer_init(timer_port);
    if (ret != 0) {
        return -CO_DRIVER_INIT_FAILED;
    }

    return CO_SUCCESS;
}

#endif /* AOS_CANOPEN */

