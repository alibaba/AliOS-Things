/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_SERVICE_H__
#define __DAS_SERVICE_H__

#include <das/platform.h>
#include <das/sha1.h>
#include <das.h>

#define das_sum_context_t das_sha1_context_t
#define das_mac_context_t das_sha1_context_t

#define das_sum_start  das_sha1_start
#define das_mac_start  das_sha1_start

#define das_sum_update das_sha1_update
#define das_mac_update das_sha1_update

#define das_sum_finish das_sha1_finish
#define das_mac_finish das_sha1_finish

typedef enum _das_service_status {
    DAS_SRV_STATUS_START = 0,
    DAS_SRV_STATUS_NEXT,
    DAS_SRV_STATUS_PUBLISH,
    DAS_SRV_STATUS_FINISH,
} das_service_status_t;

typedef struct _das_service_state {
    das_service_status_t status;
} das_service_state_t;

typedef struct _das_service {
    const char *name;
    int (*init)(void);
    das_result_t (*info)(char *buffer, size_t size, das_service_state_t *state);
    das_result_t (*attest)(char *path, size_t size, das_sum_context_t *sum_context, das_service_state_t *state);
    das_result_t (*measure)(das_sum_context_t *sum_context, das_mac_context_t *mac_context, das_service_state_t *state);
} das_service_t;

extern das_service_t * das_service_table[];

#endif /* __DAS_SRV_H__ */
