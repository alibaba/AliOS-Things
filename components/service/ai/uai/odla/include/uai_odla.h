/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef _UAI_ODLA_H_
#define _UAI_ODLA_H_

#include "open_dla_c.h"
#ifdef __cplusplus
extern "C" {
#endif
int32_t uai_get_total_elements(const odla_dims dims);
uai_tensor_s *uai_odla_tensor_transofrm(uai_tensor_s *input, const odla_value value, odla_dims dims, odla_element_type type, odla_layout layout);

odla_value odla_Reshape(odla_computation comp, odla_element_type type,
                odla_dims input_dims, odla_value input, odla_dims output_dims);
#ifdef __cplusplus
} /*  C extern */
#endif

#endif

