/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_SHA1_ALT_H__
#define __DAS_SHA1_ALT_H__

#include <das/platform.h>

typedef struct _das_sha1_context {
    void *context;
    uint32_t size;
} das_sha1_context_t;

#endif /* __DAS_SHA1_ALT_H__ */
