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

int uai_fconn(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output);

int uai_fconn_opt(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FCONN_H */
