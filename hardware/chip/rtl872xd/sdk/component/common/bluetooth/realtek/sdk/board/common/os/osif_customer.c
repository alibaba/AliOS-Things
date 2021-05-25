/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include <mem_types.h>
#include "aos/kernel.h"
#include <aos/errno.h>
#include "k_api.h"



#define HCI_OS_DBG

void *os_mem_alloc_intern(RAM_TYPE ram_type, size_t size,
                        const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	
	return aos_malloc(size);
}

void *os_mem_zalloc_intern(RAM_TYPE ram_type, size_t size,
						const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	void *ptr = aos_zalloc(size);

    if (ptr == NULL) {
        printf("os_mem_alloc_intern failed\n");
    }

    return ptr;
}

void os_mem_free(void *p_block)
{
	aos_free(p_block);
}

size_t os_mem_peek(RAM_TYPE ram_type)
{
	extern k_mm_head *g_kmm_head;
	return g_kmm_head->free_size;
}

typedef struct xqueue_desc_t {
    aos_queue_t queue_hdl;
    int message_size;
    char *buf;
} xqueue_desc;

bool os_msg_queue_create_intern(void **pp_handle, uint32_t msg_num, uint32_t msg_size,
                                const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	char *buf = aos_malloc(msg_size * msg_num);

    if (buf == NULL) {
        return false;
    }

    xqueue_desc *desc = aos_malloc(sizeof(xqueue_desc));
    desc->message_size = msg_size;
    desc->buf = buf;

    if (aos_queue_new(&desc->queue_hdl, buf, msg_size * msg_num, msg_size) != 0) {
        aos_free(desc);
        aos_free(buf);
        return false;
    }

    *pp_handle = desc;
	return true;
}

bool os_msg_queue_delete_intern(void *p_handle, const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	xqueue_desc *desc = (xqueue_desc *)p_handle;

    aos_queue_free(&desc->queue_hdl);
    aos_free(desc->buf);
    aos_free(desc);

	return true;
}

bool os_msg_send_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	 xqueue_desc *desc = (xqueue_desc *)p_handle;

    if (aos_queue_send(&desc->queue_hdl, p_msg, desc->message_size) == 0)
        return true;	
	else
		return false;
}

bool os_msg_recv_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line)
{
	HCI_OS_DBG("%s %d: %s\r\n",p_func, file_line, __func__);
	xqueue_desc *desc = (xqueue_desc *)p_handle;

    if (wait_ms == 0xFFFFFFFF)
        wait_ms = 1000 * 1000 * 1000;

    if (aos_queue_recv(&desc->queue_hdl, wait_ms, p_msg, (uint32_t*)&desc->message_size) == 0)
        return true;
	else
		return false;
}

bool os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
					void *p_param, uint16_t stack_size, uint16_t priority)
{
	int ret = 0;
    ret = aos_task_new_ext((aos_task_t*)pp_handle, p_name, p_routine, p_param, stack_size * 4, AOS_DEFAULT_APP_PRI - priority + 8);

    if (ret != 0) {
        printf("Create Task \"%s\" Failed! ret=%d\n", p_name, ret);
    }

    HCI_OS_DBG("Create Task \"%s\"\n", p_name);

    if (ret == 0) {
        return true;
    } else {
        return false;
    }
}

bool os_task_delete(void *p_handle)
{
	int ret = 0;
	ret = aos_task_delete(&p_handle);
	if (ret == 0) {
        return true;
    } else {
        return false;
    }
}

void os_delay(uint32_t ms)
{
	aos_msleep(ms);
}

bool os_sem_create(void **pp_handle, uint32_t init_count, uint32_t max_count)
{
	aos_sem_t *aos_sema = aos_malloc(sizeof(aos_sem_t));
    if (aos_sem_new(aos_sema, init_count) == 0) {
	    *pp_handle = aos_sema;
		return true;
    } else
		return false;
}

bool os_sem_delete(void *p_handle)
{
	if (p_handle != NULL) {
        aos_sem_free(p_handle);
        aos_free(p_handle);
    }

    p_handle = NULL;
	return true;
}

bool os_sem_take(void *p_handle, uint32_t wait_ms)
{
	if(wait_ms == 0xFFFFFFFF) {
		wait_ms = AOS_WAIT_FOREVER;
	}
	
	if (aos_sem_wait(p_handle, wait_ms) == 0)
    	return true;
	else
		return false;
}

bool os_sem_give(void *p_handle)
{
	aos_sem_signal(p_handle);
	return true;
}
