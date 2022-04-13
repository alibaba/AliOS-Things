/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _BUG_H_
#define _BUG_H_
#include <stdio.h>

#define BUG() do {} while(1)
#define panic(...) BUG_ON_MSG(1, ##__VA_ARGS__)
#define BUG_ON(condition) do { \
    if (unlikely(condition)) { \
        printf("BUG at %s:%d!\r\n", __func__, __LINE__); \
		BUG(); \
	} \
} while (0)

#define BUG_ON_MSG(condition, fmt, ...) do { \
	if(condition) { \
		printf("BUG at %s:%d! "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__); \
	} \
} while (0)

#define WARN_ON(condition) ({\
	int __ret = !!(condition); \
	if(__ret) { \
		printf("WARN at %s:%d!\r\n", __func__, __LINE__); \
	} \
	__ret;})


#define WARN_ON_ONCE(condition) ({\
	static int __ret = 0; \
	__ret = !!(condition); \
	if(__ret) { \
		printf("WARN at %s:%d!\r\n", __func__, __LINE__); \
	} \
	__ret;})

#define WARN(condition, fmt, ...) ({\
	static int __ret = 0; \
	__ret = !!(condition); \
	if(__ret) { \
		printf("WARN at %s:%d "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__); \
	} \
	__ret;})

#define WARN_ON_MSG(condition, fmt, ...) do { \
	if(condition) { \
		printf("WARN at %s:%d! "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__); \
	} \
} while (0)

#define BUILD_BUG_ON(condition) BUG_ON(condition)

#endif //_BUG_H_
