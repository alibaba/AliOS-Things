/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_UTILS_H
#define JSE_UTILS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

unsigned char hex2num(unsigned char ch);
char itoch(int val);
void num2hex(unsigned char ch, unsigned char *hex);
int end_with(char *str1, char *str2);
void hexdump(const char *title, const void *buff, const int len);

#ifdef SUPPORT_NODE_MODELES
char *getFilePath(char *name);
char *getClearPath(char *path);
char *getNodeModulePath(char *path);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* JSE_UTILS_H */
