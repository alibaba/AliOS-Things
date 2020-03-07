/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef _UAI_ODLA_H_
#define _UAI_ODLA_H_

#include "open_dla_c.h"

static inline void uai_get_model_scale(uai_model_quant_scale_data_t *model_quant_scale, int layer_id,
                        uai_quant_scale_type scale_type, uai_quant_scale *quant_scale)
{
    quant_scale->scale     = (int32_t *)((model_quant_scale->scale)[model_quant_scale->head.scale_offset[scale_type][layer_id]]);
    quant_scale->scale_num = model_quant_scale->head.scale_num[scale_type][layer_id];
    quant_scale->shift     = model_quant_scale->head.scale_shift[layer_id];
}

int32_t uai_get_total_elements(const odla_dims dims);
uai_tensor_s *uai_odla_tensor_transofrm(const odla_value value, odla_dims dims, odla_element_type type, odla_layout layout);

#endif

