/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#ifndef _ALI_CRYPTO_DEBUG_H_
#define _ALI_CRYPTO_DEBUG_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#if CONFIG_DBG_CRYPT
#ifndef PLATFORM_ANDROID
#define ALI_CRYPTO_DBG_E(_f, ...) \
    printf("E %s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ALI_CRYPTO_DBG_I(_f, ...) \
    printf("I %s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else /* PLATFORM_ANDROID */
#include <android/log.h>
#define __LOG_TAG__             "ICRYPTO_LOG"

#define LOG_INF(...)            __android_log_print(        \
                                        ANDROID_LOG_INFO,   \
                                        __LOG_TAG__,        \
                                        __VA_ARGS__)

#define ALI_CRYPTO_DBG_E(_f, ...)  \
            LOG_INF("E %s %d: "_f,  __FUNCTION__, __LINE__, ##_VA_ARGS__)
#define ALI_CRYPTO_DBG_I(_f, ...)  \
            LOG_INF("I %s %d: "_f,  __FUNCTION__, __LINE__, ##_VA_ARGS__)
#endif /* PLATFORM_ANDROID */

#else /* CONFIG_DBG_CRYPT */
#define ALI_CRYPTO_DBG_E(_f, _a...)
#define ALI_CRYPTO_DBG_I(_f, _a...)
#endif  /* CONFIG_DBG_CRYPT */

#define PRINT_RET(_ret, _f, ...)        \
    do {                                \
        ALI_CRYPTO_DBG_E(_f, ##__VA_ARGS__);  \
        return (ali_crypto_result)_ret; \
    } while (0);

#define GO_RET(_ret, _f, ...)             \
    do {                                  \
        ALI_CRYPTO_DBG_E(_f, ##__VA_ARGS__);    \
        result = (ali_crypto_result)_ret; \
        goto _OUT;                        \
    } while (0);



#endif /* _ALI_CRYPTO_DEBUG_H_ */
