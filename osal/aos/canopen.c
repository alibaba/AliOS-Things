/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/canopen.h"

int aos_canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port,
                     uint32_t baud, CO_Data *d)
{
    return canopen_init(can_port, timer_port, baud, d);
}

int aos_canopen_od_init(CO_Data *d, co_register_function_callback_t *fun_callback)
{
    return canopen_od_init(d, fun_callback);
}

int aos_canopen_od_handler(CO_Data *d, uint16_t index, uint8_t sub_index,
                           co_objectdict_cb *od_callback)
{
    return canopen_od_handler(d, index, sub_index, od_callback);
}

int aos_init_nodes(CO_Data *d, uint8_t id)
{
    return init_nodes(d, id);
}
