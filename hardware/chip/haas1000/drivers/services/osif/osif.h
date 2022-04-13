/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __OSIF_H__
#define __OSIF_H__
#include <stdint.h>

typedef uint32_t osif_timer_t;

typedef void (*osif_timer_callback)(void);

#if defined(__cplusplus)
extern "C" {
#endif

bool osif_init(void);

uint32_t osif_get_sys_time(void);

uint16_t osif_rand(void);

void osif_stop_hardware(void);

void osif_resume_hardware(void);

void osif_memcopy(uint8_t *dest, const uint8_t *source, uint32_t numBytes);

bool osif_memcmp(const uint8_t *buffer1, uint16_t len1, const uint8_t *buffer2, uint16_t len2);

void osif_memset(uint8_t *dest, uint8_t byte, uint32_t len);

uint8_t osif_strcmp(const char *Str1, const char *Str2);

uint16_t osif_strlen(const char *Str);

void osif_assert(const char *expression, const char *file, uint16_t line);

void osif_lock_stack(void);

void osif_unlock_stack(void);

void osif_notify_evm(void);

void osif_start_timer(osif_timer_t t, osif_timer_callback func);

void osif_cancel_timer(void);

uint8_t osif_lock_is_exist(void);

#if defined(__cplusplus)
}
#endif

#endif /*__OSIF_H__*/

