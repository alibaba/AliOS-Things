/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _STRING_H_
#define _STRING_H_

#include "tee_types.h"

int32_t printk(const int8_t *fmt, ...);

int8_t *strcpy(int8_t *dest, const int8_t *src);
int8_t *strncpy(int8_t *dest, const int8_t *src, size_t n);
int32_t strcmp(const int8_t *s1, const int8_t *s2);
int32_t strncmp(const int8_t *s1, const int8_t *s2, size_t n);
size_t strlen(const int8_t *s);
int8_t *strrchr(const int8_t *s, int32_t c);
void *memset(void *s, int32_t c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int32_t memcmp(const void *s1, const void *s2, size_t n);

#endif /* _STRING_H_ */
