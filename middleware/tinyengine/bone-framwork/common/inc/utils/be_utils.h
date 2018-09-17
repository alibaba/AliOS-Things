/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _be_utils_h
#define _be_utils_h

#if defined(__cplusplus)
 extern "C" {
#endif

#include <stddef.h>

int be_utils_timestr(char* buf, size_t buf_len);
unsigned char hex2num(unsigned char ch);
inline char itoch(int val);
void num2hex(unsigned char ch, unsigned char* hex);

#if defined(__cplusplus)
}
#endif

#endif /* be_utils_h */
