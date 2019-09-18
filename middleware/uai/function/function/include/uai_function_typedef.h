/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_FUNCTION_TYPEDEF_H
#define UAI_FUNCTION_TYPEDEF_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int8_t  *buffer;
    uint16_t batch;
    uint16_t height;
    uint16_t width;
    uint16_t depth;
    uint32_t size;
    uint16_t channels;
    int16_t dtype;
}uai_input_s;

typedef struct {
    int8_t  *buffer;
    uint16_t height;
    uint16_t width;
    uint16_t depth;
    uint32_t size;
    uint16_t in_channels;
    uint16_t out_channels;
    int16_t dtype;
}uai_weight_s;

typedef struct {
    int8_t  *buffer;
    uint16_t height;
    uint16_t width;
    uint16_t depth;
    uint32_t size;
    uint16_t shift;
    int16_t dtype;
}uai_bias_s;

typedef struct {
    int8_t  *buffer;
    uint16_t height;
    uint16_t width;
    uint16_t depth;
    uint32_t size;
    uint16_t channels;
    int16_t shift;
    int16_t dtype;
}uai_output_s;

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FUNCTION_TYPEDEF_H */
