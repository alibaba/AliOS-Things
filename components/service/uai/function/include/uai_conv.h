/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_CONV_H
#define UAI_CONV_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_conv(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front, const unsigned* paddings_back, uai_quant_scale *kernel_scale, uai_tensor_s *output);
int uai_conv_depthwise_sp(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front, const unsigned* paddings_back, uai_tensor_s *output);
#else
int uai_conv(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);
int uai_conv_depthwise_sp(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_CONV_H */
