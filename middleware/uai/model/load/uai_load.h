/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_LOAD_H
#define UAI_LOAD_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uai_common.h"
#include "uai/uai.h"
#include "aos/hal/flash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UAI_SRC_TYPE_NULL,
    UAI_SRC_TYPE_FLASH,
    UAI_SRC_TYPE_MEM,
    UAI_SRC_TYPE_FILE,
    UAI_SRC_TYPE_END
}uai_src_type_e;

uai_src_type_e uai_src_type_parse(char *source);

int uai_load_trained_data(int8_t *weight, int8_t *bias, int32_t weight_size, int32_t bias_size, int32_t offset, char *trained_data_src);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_DNN_H */
