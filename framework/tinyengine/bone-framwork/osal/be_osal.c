/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_ringbuf.h>
#include <k_obj.h>
#include <k_sem.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_stats.h>
#include <k_time.h>
#include <k_task.h>
#ifdef BE_OS_AOS
#include <aos/aos.h>
#endif
#include "be_osal.h"

void be_osal_reboot() { aos_reboot(); }

void be_osal_delay(uint32_t ms) { aos_msleep(ms); }

long long be_osal_time_now(void) { return aos_now(); }

void *be_osal_new_semaphore(void) { return (NULL); }

void be_osal_del_semaphore(void *sem) { return; }

int32_t be_osal_wait_semaphore(void *sem, uint32_t ms) { return (0); }

void be_osal_post_semaphore(void *sem) { return; }

void *be_osal_new_mutex(void) { return (NULL); }

int32_t be_osal_del_mutex(void *mutex) { return (0); }

int32_t be_osal_lock_mutex(void *mutex, int32_t ms) { return (0); }

int32_t be_osal_unlock_mutex(void *mutex) { return (0); }

int32_t  be_osal_new_task(const char * name,be_osal_task task,void * data,int32_t size,void ** task_handle)
{
	int8_t ret = -1;
    
	if(NULL != task_handle){
		aos_task_t *new_handle = calloc(1,sizeof(*new_handle));
		if(NULL== new_handle)return(-1);
		ret = aos_task_new_ext(new_handle,name,task,data,size,1);
		if(0 != ret){
			free(new_handle);
			new_handle = NULL;
			*task_handle = NULL;
			return(-1);
		}
		*task_handle = new_handle;
		return 0;
	}
    
	return aos_task_new(name,task,data,size);
}

int32_t  be_osal_free_task(void * handle)
{
	aos_task_t *task_handle = (aos_task_t*)handle;
	if(NULL == task_handle){

		krhino_task_dyn_del(NULL);
		return(0);
	}
	krhino_task_dyn_del((ktask_t*)task_handle->hdl);
	free(task_handle);
	return(0);		
}

void be_osal_exit_task(int code)
{
    aos_task_exit(code);
}

const char *be_osal_task_name()
{
    return aos_task_name();

}

int32_t  be_osal_schedule_call(be_osal_call_cb action, void *data)
{
	return aos_schedule_call(action, data);	
}


int32_t  be_osal_post_event(uint16_t type, uint16_t code, size_t value)
{
	return aos_post_event(type, code, value);	
}


int32_t  be_osal_register_event_filter(uint16_t type, be_osal_event_cb cb, void *data)
{
	return aos_register_event_filter(type, (aos_event_cb)cb, data);	
}

int32_t be_osal_unregister_event_filter(uint16_t type, be_osal_event_cb cb, void *priv)
{
	return aos_unregister_event_filter(type, (aos_event_cb)cb, priv);
}


int32_t	be_osal_init_yloop(void)
{
	return(0);	
}


int32_t	be_osal_start_yloop(void)
{
	aos_loop_run();
	return(0);
}


int be_osal_poll_read_fd(int fd, be_osal_fd_cb_t cb, void* arg)
{
    return aos_poll_read_fd(fd, cb, arg);
}

void be_osal_cancel_poll_read_fd(int fd, be_osal_fd_cb_t cb, void *arg)
{
    aos_cancel_poll_read_fd(fd, cb, arg);
}


int32_t be_osal_post_delayed_action(int32_t ms, be_osal_call_cb action, void *arg)
{
    return aos_post_delayed_action(ms, action, arg);
}

void be_osal_cancel_delayed_action(int ms, be_osal_call_cb action, void *arg)
{   
    return NULL;
}




