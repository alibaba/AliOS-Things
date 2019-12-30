/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_COMMON_H
#define UAI_COMMON_H

#include "stdlib.h"
#include "ulog/ulog.h"
#include "uai_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UAI_LOGMID "uai"

#define UAI_LOGF(...) LOGF(UAI_LOGMID, __VA_ARGS__)
#define UAI_LOGE(...) LOGE(UAI_LOGMID, __VA_ARGS__)
#define UAI_LOGW(...) LOGW(UAI_LOGMID, __VA_ARGS__)
#define UAI_LOGI(...) LOGI(UAI_LOGMID, __VA_ARGS__)
#define UAI_LOGD(...) LOGD(UAI_LOGMID, __VA_ARGS__)

#define UAI_MAX(x, y) ((x) > (y) ? (x) : (y))

#ifdef UAI_MEM_DBG
static inline void *uai_malloc_dbg(int size, int line)
{
    void *addr;
    addr = malloc(size);

    UAI_LOGI("alloc size %u, addr %p, line %d\n", size, addr, line);

    return addr;
}

static inline void uai_free_dbg(void *addr, int line)
{
    UAI_LOGI("free addr %p, line %d\n", addr, line);

    free(addr);
}

#define uai_malloc(size) uai_malloc_dbg(size, __LINE__)
#define uai_free(addr)   uai_free_dbg(addr, __LINE__)

#else

#define uai_malloc(size) malloc(size)
#define uai_free(addr)   free(addr)

#endif /* end of UAI_MEM_DBG */

#define UAI_NON_ZERO_ZALLOC(ptr, size) do { \
                                            if (size != 0) { \
                                                ptr = uai_malloc(size); \
                                                if(ptr != NULL) \
                                                    memset(ptr, 0, size); \
                                            } else {  \
                                                ptr = NULL; \
                                            } \
                                        }while(0);

#define UAI_NON_ZERO_FREE(ptr) do { \
                                    if (ptr != NULL) { \
                                        uai_free(ptr); \
                                        ptr = NULL;    \
                                    }   \
                                }while(0);

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_COMMON_H */
