/*
 * OS specific functions for UNIX/POSIX systems
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/os.h"

//#ifdef CONFIG_WPS

#if !defined(CONFIG_PLATFORM_8195A) && !defined(CONFIG_PLATFORM_8711B)
#ifdef CONFIG_MEM_MONITOR
#if CONFIG_MEM_MONITOR & MEM_MONITOR_LEAK
_list wpa_mem_table;
int wpa_mem_used_num;
//int wpa_mem_used_size;
#endif
extern int min_free_heap_size;
u8* os_malloc(u32 sz)
{
	int free_heap_size = rtw_getFreeHeapSize();
	u8 *pbuf = _rtw_malloc(sz);
#if CONFIG_MEM_MONITOR & MEM_MONITOR_LEAK
	add_mem_usage(&wpa_mem_table, pbuf, sz, &wpa_mem_used_num, MEM_MONITOR_FLAG_WPAS);
#else
	add_mem_usage(NULL, pbuf, sz, NULL, MEM_MONITOR_FLAG_WPAS);
#endif
	if(min_free_heap_size > free_heap_size)
		min_free_heap_size = free_heap_size;
	return pbuf;
}

void os_mfree(u8 *pbuf, u32 sz)
{
	_rtw_mfree(pbuf, sz);
#if CONFIG_MEM_MONITOR & MEM_MONITOR_LEAK
	del_mem_usage(&wpa_mem_table, pbuf, &wpa_mem_used_num, MEM_MONITOR_FLAG_WPAS);	
#else
	del_mem_usage(NULL, pbuf, NULL, MEM_MONITOR_FLAG_WPAS);
#endif
}
#endif//CONFIG_MEM_MONITOR

#endif// !defined(CONFIG_PLATFORM_8195A)

#ifndef OS_NO_C_LIB_DEFINES
char *os_strdup(const char *string_copy_from) 
{
	char *string_copy_to = NULL;
	string_copy_to = os_zalloc(strlen(string_copy_from) + 1);
	os_memcpy((void *)string_copy_to, string_copy_from, strlen(string_copy_from));
	string_copy_to[strlen(string_copy_from)] = '\0';
	return string_copy_to;
}
#endif

int os_get_random(unsigned char *buf, size_t len)
{
	//TODO implement it
	rtw_get_random_bytes(buf, len);
	return 0;
}

int os_get_time(struct os_time *t){
	unsigned int tt = rtw_get_current_time();
	t->sec = (os_time_t) (tt / 1000);
	t->usec = (os_time_t) (tt % 1000)*1000;
	return 0;
}

int os_get_reltime(struct os_reltime *t){
	os_get_time((struct os_time *)t);
	return 0;
}
#if 0
void *os_xqueue_create(unsigned long uxQueueLength, unsigned long uxItemSize) 
{
	return xQueueCreate( uxQueueLength, uxItemSize );
}

int os_xqueue_receive(xqueue_handle_t xQueue, void * const pvBuffer, unsigned long xSecsToWait)
{
	return xQueueReceive((xQueueHandle)xQueue, pvBuffer, (portTickType)(xSecsToWait*configTICK_RATE_HZ));
}

void os_xqueue_delete(xqueue_handle_t xQueue )
{
	vQueueDelete((xQueueHandle)xQueue);
}

int os_xqueue_send(xqueue_handle_t xQueue, const void * const pvItemToQueue, unsigned long xSecsToWait)
{
	return xQueueSendToBack((xQueueHandle)xQueue, pvItemToQueue, (portTickType)(xSecsToWait*configTICK_RATE_HZ));
}
#else
void *os_xqueue_create(unsigned long uxQueueLength, unsigned long uxItemSize) 
{
	void* xQueue = NULL;
	rtw_init_xqueue(&xQueue, "queue", uxItemSize, uxQueueLength);
	return xQueue;
}

int os_xqueue_receive(xqueue_handle_t xQueue, void * const pvBuffer, unsigned long xSecsToWait)
{
	return rtw_pop_from_xqueue(&xQueue, pvBuffer, xSecsToWait*1000);
}

void os_xqueue_delete(xqueue_handle_t xQueue )
{
	rtw_deinit_xqueue(&xQueue);
}

int os_xqueue_send(xqueue_handle_t xQueue, const void * const pvItemToQueue, unsigned long xSecsToWait)
{
	return rtw_push_to_xqueue(&xQueue, (void*)pvItemToQueue, xSecsToWait*1000);
}
#endif
//#endif
