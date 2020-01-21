/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef APP_ENTRY_H
#define APP_ENTRY_H

#include "aos/kernel.h"
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

/* for demo only */
#define PRODUCT_KEY    "a11fNRwnltG"
#define PRODUCT_SECRET "b1GadfPW0om"

#define DEVICE_NAME    "ulocation_device01"
#define DEVICE_SECRET  "o0Da9rdRZOc4P1SQ7gwLzaRYhaTV82hj"

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

int linkkit_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ENTRY_H */