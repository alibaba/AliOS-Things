/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

void uai_tanh(int8_t * data, uint16_t size, uint16_t int_width)
{
#if defined(UAI_USE_CMSIS_NN)
    arm_nn_activations_direct_q7(data, size, int_width, ARM_TANH);
#elif defined(UAI_USE_HARDWARE_NN)
    uai_tanh_hal(data, size, int_width);
#else
    uai_tanh_software(data, size, int_width);
#endif
}