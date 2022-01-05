
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_LOG_H
#define HAAS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define LOG_ENABLE_D
#endif

#define LOG_ENABLE_I
#define LOG_ENABLE_W
#define LOG_ENABLE_E

#ifdef LOG_ENABLE_D
#define LOG_D(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_D(fmt, args...)
#endif

#ifdef LOG_ENABLE_I
#define LOG_I(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_I(fmt, args...)
#endif

#ifdef LOG_ENABLE_W
#define LOG_W(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_W(fmt, args...)
#endif

#ifdef LOG_ENABLE_E
#define LOG_E(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_E(fmt, args...)
#endif

#ifdef __cplusplus
}
#endif

#endif
