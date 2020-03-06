/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UAI_PAD_H
#define UAI_PAD_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_pad(uai_tensor_s *input, const int32_t* padding_front, const int32_t* padding_back, uai_tensor_s *output);
#else

#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_PAD_H */
