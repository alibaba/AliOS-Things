/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_function.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

int uai_fc(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output, int16_t *vec_buffer)
{
    uint16_t width = 0;

    width = ((input->width)<<4) + output->width;

#if defined(UAI_USE_CMSIS_NN)
    switch(width) {
        case 0x11:
            return arm_fully_connected_q7(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        case 0x22:
            return arm_fully_connected_q15(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        case 0x12:
            return arm_fully_connected_mat_q7_vec_q15(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        default:
            break;
    }
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal(input, weight, bias, output, vec_buffer);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_software(input, weight, bias, output, vec_buffer);
#endif
}

int uai_fc_opt(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output, int16_t *vec_buffer)
{
    uint16_t width = 0;

    width = ((input->width)<<4) + output->width;

#if defined(UAI_USE_CMSIS_NN)
    switch(width) {
        case 0x11:
            return arm_fully_connected_q7_opt(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        case 0x22:
            return arm_fully_connected_q15_opt(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        case 0x12:
            return arm_fully_connected_mat_q7_vec_q15_opt(input->buffer, weight->buffer, input->size, weight->rows, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
        default:
            break;
    }
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal_opt(input, weight, bias, output, vec_buffer);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_opt_software(input, weight, bias, output, vec_buffer);
#endif
}
