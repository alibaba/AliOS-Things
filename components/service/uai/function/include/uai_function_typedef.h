/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_FUNCTION_TYPEDEF_H
#define UAI_FUNCTION_TYPEDEF_H

#include "uai_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT

#define UAI_DIM_BATCH     0
#define UAI_DIM_HEIGHT    1
#define UAI_DIM_WIDTH     2
#define UAI_DIM_CHANNEL   3

#define UAI_DIM_KEL_HEIGHT  0
#define UAI_DIM_KEL_WIDTH   1
#define UAI_DIM_KEL_IN_CH   2
#define UAI_DIM_KEL_OUT_CH  3


#define dim_batch    dims.dims[UAI_DIM_BATCH]
#define dim_height   dims.dims[UAI_DIM_HEIGHT]
#define dim_width    dims.dims[UAI_DIM_WIDTH]
#define dim_channels dims.dims[UAI_DIM_CHANNEL]

#define dim_kel_height  dims.dims[UAI_DIM_KEL_HEIGHT]
#define dim_kel_width   dims.dims[UAI_DIM_KEL_WIDTH]
#define dim_kel_in_ch   dims.dims[UAI_DIM_KEL_IN_CH]
#define dim_kel_out_ch  dims.dims[UAI_DIM_KEL_OUT_CH]

#else

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

#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FUNCTION_TYPEDEF_H */
