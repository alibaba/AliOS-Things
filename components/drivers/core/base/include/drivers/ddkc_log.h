/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _DDKC_LOG_H_
#define _DDKC_LOG_H_

#include <unistd.h>
#include <sys/types.h>

/*!!!!!!!!!! ONLY loglevel and log format should be modified !!!!!!!!!*/
extern int g_ddkc_log_level;
extern char ddkc_level_tag[];

/*!!!!!!!!!! NEVER TRY to modify the following log defination !!!!!!!!!*/

#define DDKC_LOG_LOUD 0
#define DDKC_LOG_DEBUG 1
#define DDKC_LOG_INFO  2
#define DDKC_LOG_WARN  3
#define DDKC_LOG_ERROR 4

#define ddkc_log(level, format, ...) do { \
	if (level >= g_ddkc_log_level) \
		printf("<%c><%d> [%s:%d]" format, ddkc_level_tag[level], getpid(), __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)

#define ddkc_loud(format, ...)  ddkc_log(DDKC_LOG_LOUD, format, ##__VA_ARGS__)
#define ddkc_dbg(format, ...)   ddkc_log(DDKC_LOG_DEBUG, format, ##__VA_ARGS__)
#define ddkc_info(format, ...)  ddkc_log(DDKC_LOG_INFO, format, ##__VA_ARGS__)
#define ddkc_warn(format, ...)  ddkc_log(DDKC_LOG_WARN, format, ##__VA_ARGS__)
#define ddkc_err(format, ...)   ddkc_log(DDKC_LOG_ERROR, format, ##__VA_ARGS__)
#define ddkc_assert(cond, str, ...)          \
    do { if (!(cond)) { printf("[ASSERT] %s/" str, __FUNCTION__, __VA_ARGS__); while (1); } } while (0)

#endif //_DDKC_LOG_H_
