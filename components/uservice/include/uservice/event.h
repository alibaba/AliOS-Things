/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_USERVICE_EVENT_H
#define YOC_USERVICE_EVENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*event_callback_t)(uint32_t event_id, const void *data, void *context);

#ifdef __cplusplus
}
#endif

#endif
