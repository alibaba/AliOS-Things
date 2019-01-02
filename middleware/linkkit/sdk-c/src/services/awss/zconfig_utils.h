/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _SMART_UTILS_H_
#define _SMART_UTILS_H_

#include "os.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#ifndef DEBUG
    #define __zc_loglevel_printf(log_level, format, ...)	do { } while (0)
#else
    #define __zc_loglevel_printf(log_level, format, ...)	\
    do {													\
            os_printf(format, ##__VA_ARGS__);		\
    } while (0)
#endif //end of ifndef DEBUG

//for library safety, close the log output 
#define log(format, ...)						__zc_loglevel_printf(LOGLEVEL_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...)						__zc_loglevel_printf(LOGLEVEL_INFO, format, ##__VA_ARGS__)
#define warn(format, ...)						__zc_loglevel_printf(LOGLEVEL_WARN, format, ##__VA_ARGS__)
#define error(format, ...)						__zc_loglevel_printf(LOGLEVEL_ERROR, format, ##__VA_ARGS__)

#define bug_on(condition, format, ...)							\
do {											\
		if (condition) {							\
			error("########BUG ON %d!!!\r\n", __LINE__);			\
			error(format, ##__VA_ARGS__);					\
			while (1);							\
		}										\
} while (0)

#define warn_on(condition, format, ...)							\
do {											\
		if (condition) {							\
			warn("########WARNING ON %d!!!\r\n", __LINE__);			\
			warn(format, ##__VA_ARGS__);					\
		}									\
} while (0)

void dump_mac(uint8_t *src, uint8_t *dst);
void dump_hex(uint8_t *data, int len, int tab_num);
void dump_ascii(uint8_t *data, int len, int tab_num);

uint16_t zconfig_checksum_v3(uint8_t *data, uint8_t len);
char is_utf8(const char *ansi_str, int length);
void produce_random(uint8_t *random, unsigned int len);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif	// _UTILS_H_
