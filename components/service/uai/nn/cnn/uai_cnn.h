/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_CNN_H
#define UAI_CNN_H

#include "uai_load.h"
#include "uai_function.h"
#include "uai_nn_local.h"
#include "uai/uai.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uai_layer_type_e layer_type;
    uai_input_s input;
    uai_weight_s weight;
    uint16_t strides[2];
    uai_bias_s bias;
    uai_output_s output;
    uai_pad_type_e pad_type;
    uint32_t act_width;

    union {
        struct {
            uai_conv_type_e conv_type;
            uai_conv_func conv_func;
        };
        struct {
            uai_fconn_type_e fconn_type;
            uai_fconn_func fconn_func;
        };
        struct {
            uai_pool_type_e pool_type;
            uai_pool_func pool_func;
        };
    };

    uai_act_type_e act_type;
    uai_act_func act_func;

    uai_softmax_type_e softmax_type;
    uai_softmax_func softmax_func;
}uai_cnn_layer_t;

typedef struct {
    bool preload;
    uint16_t layer_num;   /* 0 is input layer, layer_num-1 is output layer */
    uint16_t result_num;
    uai_cnn_layer_t *layer_info;
    char *model_data_src;
}uai_cnn_model_t;

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_CNN_H */
