/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

static void uai_sigmod_software(int8_t *data, uint16_t size, uint16_t out_width)
{
    /* TODO: This version does not support */
}

void uai_sigmod(int8_t *data, uint16_t size, uint16_t out_width)
{
#if defined(UAI_USE_CMSIS_NN)
    if(out_width <= 3) {
        arm_nn_activations_direct_q7(data, size, out_width, ARM_SIGMOID);
    } else {
        uai_sigmod_software(data, size, out_width);
    }
#elif defined(UAI_USE_HARDWARE_NN)
    uai_sigmod_hal(data, size, out_width);
#else
    uai_sigmod_software(data, size, out_width);
#endif
}