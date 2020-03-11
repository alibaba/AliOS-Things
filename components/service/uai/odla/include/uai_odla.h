/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef _UAI_ODLA_H_
#define _UAI_ODLA_H_

#include "open_dla_c.h"

int32_t uai_get_total_elements(const odla_dims dims);
uai_tensor_s *uai_odla_tensor_transofrm(const odla_value value, odla_dims dims, odla_element_type type, odla_layout layout);

#endif

