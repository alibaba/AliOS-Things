/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <tlsf.h>
#include <u_api.h>

#define USERSPACE_HEAP_MAXSZ    (1024 * 1024)

static tlsf_t tlsf_ptr = NULL;
static kmutex_t *tlsf_mutex = NULL;

void *umm_init(void *mem, size_t size)
{
    kstat_t stat;

    if (mem == NULL || size == 0 || size > USERSPACE_HEAP_MAXSZ) {
        return NULL;
    }

    stat = krhino_mutex_dyn_create(&tlsf_mutex, "tlsf_mutex");
    if (stat != RHINO_SUCCESS) {
        return NULL;
    }

    tlsf_ptr = tlsf_create_with_pool(mem, size);

    return tlsf_ptr;
}

void *umm_malloc(size_t size)
{
    void *ptr;

    if (tlsf_ptr == NULL || size > USERSPACE_HEAP_MAXSZ) {
        return NULL;
    }

    krhino_mutex_lock(tlsf_mutex, RHINO_WAIT_FOREVER);
    ptr = tlsf_malloc(tlsf_ptr, size);
    krhino_mutex_unlock(tlsf_mutex);

    return ptr;
}

void *umm_realloc(void *ptr, size_t size)
{
    void *p;

    if (tlsf_ptr == NULL || size > USERSPACE_HEAP_MAXSZ) {
        return NULL;
    }

    krhino_mutex_lock(tlsf_mutex, RHINO_WAIT_FOREVER);
    p = tlsf_realloc(tlsf_ptr, ptr, size);
    krhino_mutex_unlock(tlsf_mutex);

    return p;
}

void umm_free(void *ptr)
{
    if (tlsf_ptr == NULL) {
        return;
    }

    krhino_mutex_lock(tlsf_mutex, RHINO_WAIT_FOREVER);
    tlsf_free(tlsf_ptr, ptr);
    krhino_mutex_unlock(tlsf_mutex);
}

