/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UAI_ADD_H
#define UAI_ADD_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_bias_add(uai_tensor_s *input, uai_tensor_s *bias, uai_quant_scale *bias_scale, uai_quant_scale *act_scale, uai_tensor_s *output);
int uai_elementwise_add(uai_tensor_s *input1, uai_tensor_s *input2, uai_tensor_s *output);
#else
int uai_add(uai_input_s *input1, uai_input_s *input2, uai_output_s *output);
int uai_elementwise_add(uai_input_s *input1, uai_input_s *input2, uai_output_s *output);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_ADD_H */
