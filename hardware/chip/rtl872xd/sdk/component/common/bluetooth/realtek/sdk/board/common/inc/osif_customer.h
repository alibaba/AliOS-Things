/*
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _OSIF_H_
#define _OSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef false
	#define false 0
#endif

#ifndef true
	#define true (!false)
#endif

#ifndef bool
	#define bool unsigned char
#endif

#ifndef CONFIG_PLATFORM_8721D
#define CONFIG_PLATFORM_8721D
#endif

#include <mem_types.h>

void *os_mem_alloc_intern(RAM_TYPE ram_type, size_t size, 
						  const char *p_func, uint32_t file_line);

void *os_mem_zalloc_intern(RAM_TYPE ram_type, size_t size, 
						   const char *p_func, uint32_t file_line);

void os_mem_free(void *p_block);

size_t os_mem_peek(RAM_TYPE ram_type);

bool os_msg_queue_create_intern(void **pp_handle, uint32_t msg_num, uint32_t msg_size, 
								const char *p_func, uint32_t file_line);

bool os_msg_queue_delete_intern(void *p_handle, const char *p_func, uint32_t file_line);

bool os_msg_send_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line);

bool os_msg_recv_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line);

bool os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
					void *p_param, uint16_t stack_size, uint16_t priority);

bool os_task_delete(void *p_handle);

void os_delay(uint32_t ms);

bool os_sem_create(void **pp_handle, uint32_t init_count, uint32_t max_count);

bool os_sem_delete(void *p_handle);

bool os_sem_take(void *p_handle, uint32_t wait_ms);

bool os_sem_give(void *p_handle);

#define os_mem_alloc(ram_type, size)    \
    os_mem_alloc_intern(ram_type, size, __func__, __LINE__)

#define os_mem_zalloc(ram_type, size)   \
    os_mem_zalloc_intern(ram_type, size, __func__, __LINE__)

#define os_msg_queue_create(pp_handle, msg_num, msg_size)    \
    os_msg_queue_create_intern(pp_handle, msg_num, msg_size, __func__, __LINE__)

#define os_msg_queue_delete(p_handle) \
    os_msg_queue_delete_intern(p_handle, __func__, __LINE__)

#define os_msg_send(p_handle, p_msg, wait_ms) \
    os_msg_send_intern(p_handle, p_msg, wait_ms, __func__, __LINE__)

#define os_msg_recv(p_handle, p_msg, wait_ms) \
    os_msg_recv_intern(p_handle, p_msg, wait_ms, __func__, __LINE__)

#endif /* _OSIF_H_ */
