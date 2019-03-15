/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host_conf.h"
#include "usb_host_os.h"

static usbh_os_event_t g_usbh_queue_buf[USBH_MAX_MESSAGE_COUNT];
static usbh_os_stack_t g_usbh_task_stack[USBH_TASK_STACK_SIZE];

int usbh_os_queue_create(usbh_os_queue_t *pqueue)
{
    return krhino_fix_buf_queue_create(pqueue, "usbh_queue", 
                                       g_usbh_queue_buf,
                                       sizeof(usbh_os_event_t), 
                                       USBH_MAX_MESSAGE_COUNT);
}

int usbh_os_queue_recv(usbh_os_queue_t *pqueue, void *msg, uint32_t size, 
                       uint32_t timeout)
{
    return krhino_buf_queue_recv(pqueue, timeout, msg, &size);
}

int usbh_os_queue_send(usbh_os_queue_t *pqueue, void *msg, uint32_t size)
{
    return krhino_buf_queue_send(pqueue, msg, size);
}

int usbh_os_queue_del(usbh_os_queue_t *pqueue)
{
    return krhino_buf_queue_del(pqueue);
}

int usbh_os_start_task(usbh_os_task_t *ptask, void (*fn)(void *), void *arg)
{
    return krhino_task_create(ptask, "usbh_main", arg, USBH_TASK_PRIO, 0,
                              g_usbh_task_stack, USBH_TASK_STACK_SIZE, fn, 1);
}

int usbh_os_lock_create(usbh_os_lock_t *plock)
{
    return krhino_mutex_create(plock, "usbh_lock");
}

int usbh_os_lock(usbh_os_lock_t *plock)
{
    return krhino_mutex_lock(plock, RHINO_WAIT_FOREVER);
}

int usbh_os_unlock(usbh_os_lock_t *plock)
{
    return krhino_mutex_unlock(plock);
}

int usbh_os_lock_del(usbh_os_lock_t *plock)
{
    return krhino_mutex_del(plock);
}

void usbh_os_delay(int ms)
{
    krhino_task_sleep(krhino_ms_to_ticks(ms));
}