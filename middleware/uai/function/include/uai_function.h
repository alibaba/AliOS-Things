/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_FUNCTION_H
#define UAI_FUNCTION_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int8_t  *buffer;
    uint16_t columns;
    uint16_t rows;
    uint16_t size;
    uint16_t width;
}uai_input_s;

typedef struct {
    int8_t  *buffer;
    uint16_t columns;
    uint16_t rows;
    uint16_t size;
    uint16_t width;
    uint16_t shift;
}uai_weight_s;

typedef struct {
    int8_t  *buffer;
    uint16_t columns;
    uint16_t rows;
    uint16_t size;
    uint16_t width;
    uint16_t shift;
}uai_bias_s;

typedef struct {
    int8_t  *buffer;
    uint16_t columns;
    uint16_t rows;
    uint16_t size;
    uint16_t width;
    uint16_t shift;
}uai_output_s;

int uai_fc(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output, int16_t *vec_buffer);

int uai_fc_opt(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output, int16_t *vec_buffer);

void uai_relu(int8_t *data, uint16_t size, uint16_t out_width);
void uai_relu6(int8_t *data, uint16_t size, uint16_t out_width);
void uai_sigmod(int8_t *data, uint16_t size, uint16_t out_width);
void uai_tanh(int8_t *data, uint16_t size, uint16_t out_width);

void uai_softmax(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FUNCTION_H */
