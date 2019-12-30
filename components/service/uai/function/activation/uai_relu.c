/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_activation.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#ifndef UAI_USE_HARDWARE_NN
void uai_relu6_software(int8_t *data, uint16_t size)
{
    uint16_t  i = 0;

    for (i = 0; i < size; i++) {
        if (data[i] < 0) {
            data[i] = 0;
        }else if (data[i] > 6) {
            data[i] = 6;
        }
    }
}
#endif

void uai_relu(int8_t *data, uint16_t size, uint16_t out_width)
{
    (void)out_width;
#if defined(UAI_USE_CMSIS_NN)
    arm_relu_q7(data, size);
#elif defined(UAI_USE_HARDWARE_NN)
    uai_relu_hal(data, size);
#else
    uai_relu_software(data, size);
#endif
}

void uai_relu6(int8_t *data, uint16_t size, uint16_t out_width)
{
    (void)out_width;
#if defined(UAI_USE_CMSIS_NN)
    uai_relu6_software(data, size);
#elif defined(UAI_USE_HARDWARE_NN)
    uai_relu6_hal(data, size);
#else
    uai_relu6_software(data, size);
#endif
}