/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "../internal/mb.h"
#include "../internal/mbport.h"

static kevent_t mb_event;

bool mb_event_init( void )
{
    kstat_t  ret;
    
    ret = krhino_event_create(&mb_event, "modbus", 0);
    if(ret == RHINO_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

bool mb_event_post( mb_event_t event )
{
    kstat_t  ret;
    
    ret = krhino_event_set(&mb_event, event, RHINO_OR);
    if(ret == RHINO_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

bool mb_event_get( mb_event_t * event )
{
    kstat_t  ret;
    uint32_t actl_flags=0;

    ret = krhino_event_get(&mb_event,EV_READY | EV_PDUDATA_READY | EV_FRAME_SENT | EV_FRAME_RECEIVED |EV_FRAME_TIMEOUT | EV_REQU_FINISHED, 
                           RHINO_OR_CLEAR, &actl_flags, RHINO_WAIT_FOREVER);
    if(ret != RHINO_SUCCESS) {
        return false;
    }

    switch(actl_flags) {
        case EV_READY:
            *event = EV_READY;
            break;
        case EV_PDUDATA_READY:
            *event = EV_PDUDATA_READY;
            break;
        case EV_FRAME_SENT:
            *event = EV_FRAME_SENT;
            break;
        case EV_FRAME_RECEIVED:
            *event = EV_FRAME_RECEIVED;
            break;
        case EV_FRAME_TIMEOUT:
            *event = EV_FRAME_TIMEOUT;
            break;
        case EV_REQU_FINISHED:
            *event = EV_REQU_FINISHED;
            break;
        default:
            printf("[mb_event_get] received more than one event 0x%x\n",actl_flags);
            return false;
    }

    return true;
}

mb_exception_t mb_wait_slave_respond()
{

    mb_exception_t ret;
    uint32_t actl_flags=0;

    ret = krhino_event_get(&mb_event, EV_RESPOND_ERROR | EV_RESPOND_TIMEOUT | EV_RESPOND_PROCESS, 
                           RHINO_OR_CLEAR, &actl_flags, RHINO_WAIT_FOREVER);
    if(ret != RHINO_SUCCESS) {
        return false;
    }

    mb_log(MB_LOG_DEBUG, "rev event 0x%x\n", actl_flags);

    switch(actl_flags) {
        case EV_RESPOND_PROCESS:
            ret = MB_EX_NONE;
            break;
        case EV_RESPOND_TIMEOUT:
            ret = MB_EX_RESPOND_TIMEOUT;
            break;
        case EV_RESPOND_ERROR:
            ret = MB_EX_RESPOND_ERROR;
            break;
        default:
            printf("[mb_wait_slave_respond] received more than one event 0x%x\n",actl_flags);
            return MB_EX_UNCERTAIN_REASON;
    }
    return ret;
}


