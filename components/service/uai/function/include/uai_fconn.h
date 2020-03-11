/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_FCONN_H
#define UAI_FCONN_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_fconn(uai_tensor_s *input, uai_tensor_s *weight, uai_tensor_s *bias, const uint32_t *kernel_scale,
              const uint32_t *bias_scale, const uint32_t act_scale, const uint32_t shift, uai_tensor_s *output);
#else
int uai_fconn(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output);
int uai_fconn_opt(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FCONN_H */
