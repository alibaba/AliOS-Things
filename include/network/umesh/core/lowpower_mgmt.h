/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_LOWPOWER_MGMT_H
#define UMESH_LOWPOWER_MGMT_H

#include "core/topology.h"

typedef enum {
    LOWPOWER_NONE_SCHEDULE = 0,
    LOWPOWER_PARENT_SCHEDULE = 1,
    LOWPOWER_ATTACHING_SCHEDULE = 2,
    LOWPOWER_ATTACHED_SCHEDULE = 3,
} schedule_type_t;

typedef struct time_slot_s {
    uint8_t slot_num;
    uint32_t offset;
} time_slot_t;

void lowpower_init(void);
void lowpower_update_info(neighbor_t *nbr, uint8_t *tlvs, uint16_t length);
uint16_t lowpower_set_info(uint8_t type, uint8_t *data, void *context);
bool lowpower_is_radio_up(void);
uint32_t lowpower_get_sleep_time(void);

typedef void (* lowpower_radio_down_t)(schedule_type_t type);
typedef void (* lowpower_radio_up_t)(schedule_type_t type);

typedef struct lowpower_events_handler_s {
    slist_t next;
    lowpower_radio_down_t radio_down;
    lowpower_radio_up_t radio_up;
} lowpower_events_handler_t;

void lowpower_register_callback(lowpower_events_handler_t *handler);

#endif
