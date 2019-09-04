/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_NN_LOCAL_H
#define UAI_NN_LOCAL_H

#include "uai_function.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*uai_fconn_func)(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output, int16_t *vec_buffer);

typedef void (*uai_act_func)(int8_t *data, uint16_t size, uint16_t int_width);

typedef void (*uai_softmax_func)(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_NN_LOCAL_H */
