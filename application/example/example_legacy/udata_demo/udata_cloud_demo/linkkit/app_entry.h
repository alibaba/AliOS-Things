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
#define PRODUCT_KEY    "a1Aeb21CMq5"
#define PRODUCT_SECRET "b1GadfPW0om"

#define DEVICE_NAME    "eaSkr41w8gz8sdVOFwA9"
#define DEVICE_SECRET  "34nReP7DWHw8wNLHhNXxOnu3o5U8GXif"

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

int linkkit_init(void);

int linkkit_sample_start(void);

int app_cloud_report(void *pdata, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* APP_ENTRY_H */