/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

void uai_softmax(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec)
{
#if defined(UAI_USE_CMSIS_NN)
    arm_softmax_q7(in_vec, dim_vec, out_vec);
#elif defined(UAI_USE_HARDWARE_NN)
    uai_softmax_hal(in_vec, dim_vec, out_vec);
#else
    uai_softmax_software(in_vec, dim_vec, out_vec);
#endif
}