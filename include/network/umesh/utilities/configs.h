/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_CONFIGS_H
#define UR_CONFIGS_H

#include "umesh_types.h"

typedef struct ur_configs_s {
    uint32_t magic_number;
    uint8_t version;
    uint8_t main_version;
} ur_configs_t;

ur_error_t ur_configs_read(ur_configs_t *config);
ur_error_t ur_configs_write(ur_configs_t *config);

#endif  /* UR_CONFIGS_H */
