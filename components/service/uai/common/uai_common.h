/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_COMMON_H
#define UAI_COMMON_H

#include "stdlib.h"
#include "ulog/ulog.h"
#include "uai_typedef.h"
#include "aos/kernel.h"
#include "uai_function_typedef.h"

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
static inline void *uai_malloc_dbg(int size, char *file, int line)
{
    void *addr;
    addr = aos_malloc(size);

    UAI_LOGI("alloc size %u, addr %p ## file %s, line %d\n", size, addr, file, line);

    return addr;
}

static inline void *uai_zalloc_dbg(int size, char *file, int line)
{
    void *addr;
    addr = aos_zalloc(size);

    UAI_LOGI("alloc size %u, addr %p ## file %s, line %d\n", size, addr, file, line);

    return addr;
}

static inline void uai_free_dbg(void *addr, char *file, int line)
{
    UAI_LOGI("free addr %p## file %s, line %d\n", addr, file, line);

    aos_free(addr);
}

#define uai_malloc(size) uai_malloc_dbg(size, __FILE__, __LINE__)
#define uai_zalloc(size) uai_zalloc_dbg(size, __FILE__, __LINE__)
#define uai_free(addr)   uai_free_dbg(addr, __FILE__, __LINE__)

#else

#define uai_malloc(size) aos_malloc(size)
#define uai_zalloc(size) aos_zalloc(size)
#define uai_free(addr)   aos_free(addr)

#endif /* end of UAI_MEM_DBG */

#define UAI_NON_ZERO_ZALLOC(ptr, size)     \
    do {                                   \
        if (size != 0) {                   \
            ptr = uai_malloc(size);        \
            if(ptr != NULL)                \
                memset(ptr, 0, size);      \
        } else {                           \
            ptr = NULL;                    \
        }                                  \
    } while(0);

#define UAI_NON_ZERO_FREE(ptr)             \
    do {                                   \
        if (ptr != NULL) {                 \
            uai_free(ptr);                 \
            ptr = NULL;                    \
        }                                  \
    }while(0);

#define UAI_VALID_PTR_CHECK_INT(ptr, errno) \
    do {                                    \
        if (ptr == NULL) {                  \
            UAI_LOGE("ptr is NULL");        \
            return errno;                   \
        }                                   \
    }while(0);

#define UAI_VALID_PTR_CHECK_VOID(ptr)       \
    do {                                    \
        if (ptr == NULL) {                  \
            UAI_LOGE("ptr is NULL");        \
            return;                         \
        }                                   \
    }while(0);

#define UAI_VALID_COND_CHECK_RET(cond, errno)   \
    do {                                        \
        if (!(cond)) {                          \
            UAI_LOGE("cond is false");          \
            return errno;                       \
        }                                       \
    }while(0);

#define UAI_VALID_PTR_CHECK_NULL(ptr)           \
    do {                                        \
        if (ptr == NULL) {                      \
            UAI_LOGE("ptr is NULL");            \
            return NULL;                        \
        }                                       \
    }while(0);

#define UAI_RET_WITH_MEM_FREE(mem_ptr, ret)     \
    do {                                        \
        uai_free(mem_ptr);                      \
        return ret;                             \
    }while(0);

#ifdef UAI_ODLA_SUPPORT
#define UAI_IS_SAME_DIMS(input1, input2) (memcmp(&((input1)->dims), &((input2)->dims), sizeof(uai_dims)))

#define UAI_IS_SAME_LAST_DIM(input1, input2) \
    (input1->dims.dims[input1->dims.size-1] == input2->dims.dims[input2->dims.size])

#define UAI_IS_1_DIM(input1) (input1->dims.size == 1)

static inline uint32_t uai_dims_size(uai_dims *dims)
{
    int32_t i    = 0;
    int32_t size = 1;

    for (i = 0; i < dims->size; i++) {
        size *= dims->dims[i];
    }

    return size;
}

#endif


#ifdef __cplusplus
}
#endif

#endif /* end of UAI_COMMON_H */
