/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "GUI_Private.h"
#include "stdio.h"

static  kmutex_t disp_mutex;
static  ksem_t   event_sem;

static  ksem_t   key_sem;
static  int      key_pressed;
static  char     key_is_inited;

GUI_TIMER_TIME  GUI_X_GetTime (void) 
{
    return ((GUI_TIMER_TIME)krhino_sys_time_get());
}


void  GUI_X_Delay (int period) 
{
    krhino_task_sleep(krhino_ms_to_ticks(period));
}

void GUI_X_ExecIdle (void) 
{
    GUI_X_Delay(1);
}

void  GUI_X_InitOS (void)
{ 
    krhino_mutex_create(&disp_mutex, "mutex");
    krhino_sem_create(&event_sem, "sem", 0);
}

void  GUI_X_Lock (void)
{ 
    krhino_mutex_lock(&disp_mutex, RHINO_WAIT_FOREVER);
}

void  GUI_X_Unlock (void)
{ 
    krhino_mutex_unlock(&disp_mutex);
}


U32 GUI_X_GetTaskId (void) 
{ 
    return ((U32)krhino_cur_task_get());
}

void GUI_X_WaitEvent (void) 
{
    krhino_sem_take(&event_sem, RHINO_WAIT_FOREVER);
}

void GUI_X_SignalEvent (void) 
{
    krhino_sem_give(&event_sem);
}

static  void  CheckInit (void) 
{
    if (key_is_inited == 0) {
        key_is_inited = 1;
        GUI_X_Init();
    }
}


void GUI_X_Init (void) 
{
    krhino_sem_create(&key_sem, "sem", 0);
}


int  GUI_X_GetKey (void) 
{
    int r;

    r = key_pressed;
    CheckInit();
    key_pressed = 0;
    return (r);
}

int  GUI_X_WaitKey (void) 
{
    int    r;

    CheckInit();
    if (key_pressed == 0) {
        krhino_sem_take(&key_sem, RHINO_WAIT_FOREVER);
    }
    r          = key_pressed;
    key_pressed = 0;
    return (r);
}

void  GUI_X_StoreKey (int k) 
{
    key_pressed = k;
    krhino_sem_give(&key_sem);
}

