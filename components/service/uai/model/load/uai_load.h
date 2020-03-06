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

#define UAI_MAX_LAYERS 32

typedef enum {
    UAI_SRC_TYPE_NULL,
    UAI_SRC_TYPE_FLASH,
    UAI_SRC_TYPE_MEM,
    UAI_SRC_TYPE_FILE,
    UAI_SRC_TYPE_END
}uai_src_type_e;

#ifdef UAI_ODLA_SUPPORT
typedef struct {
    uint32_t magic_num;
    int32_t total_layer;
    int16_t scale_offset[UAI_SCALE_END][UAI_MAX_LAYERS];
    int16_t scale_num[UAI_SCALE_END][UAI_MAX_LAYERS];
}uai_model_quant_scale_t;
#endif

uai_src_type_e uai_src_type_parse(char *source);

int uai_load_model_data(int8_t *weight, int8_t *bias, int32_t weight_size, int32_t bias_size, int32_t offset, char *model_data_src);
#ifdef UAI_ODLA_SUPPORT
int uai_load_model_scale(int layer_id, uai_quant_scale_type type, uai_quant_scale **quant_scale);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_DNN_H */
