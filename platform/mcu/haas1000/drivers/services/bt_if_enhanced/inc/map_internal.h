/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __MAP_INTERNAL_H__
#define __MAP_INTERNAL_H__

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

//------ STATE MACHINE ------//
typedef unsigned int btif_map_sm_event_t;
#define BTIF_MAP_SM_EVENT_GLOBAL_BASE 0x00000000
#define BTIF_MAP_SM_EVENT_STATE_ENTER (BTIF_MAP_SM_EVENT_GLOBAL_BASE+1)

typedef void (*btif_map_sm_event_handler_t)(void *instance, void *param);

typedef struct {
    union {
    } p;
} btif_map_sm_event_param_t;
typedef struct {
    btif_map_sm_event_t event;
    btif_map_sm_event_handler_t handler;
} btif_map_sm_state_handler_t;
typedef struct {
    btif_map_sm_state_handler_t *handlers;
} btif_map_sm_state_t;

//------ SMS ------//
#define BTIF_MAP_SM_EVENT_SMS_BASE 0x00000000

#ifdef __cplusplus
}
#endif

#endif /* __MAP_INTERNAL_H__ */