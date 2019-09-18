/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_NN_LOCAL_H
#define UAI_NN_LOCAL_H

#include "uai_function.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UAI_PRINT_LAYER_INFO(param, name)   do {                                                               \
                                                if(layer_info->param.name != 0) {                              \
                                                    UAI_LOGI("       %6s: %u", #name, layer_info->param.name); \
                                                }                                                              \
                                            }while(0);

#define UAI_GET_VALUE_BEGIN_ONE(value)   ((value != 0) ? (value) : 1)

typedef int (*uai_conv_func)(uai_input_s *input, uai_weight_s *weight, uint32_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);

typedef int (*uai_fconn_func)(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output);

typedef void (*uai_act_func)(int8_t *data, uint16_t size, uint16_t int_width);

typedef void (*uai_softmax_func)(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

typedef void (*uai_pool_func)(uai_input_s *input, uai_weight_s *filter, uint32_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_NN_LOCAL_H */
