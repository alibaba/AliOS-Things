/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_DNN_H
#define UAI_DNN_H

#include "uai_load.h"
#include "uai_function.h"
#include "uai_nn_local.h"
#include "uai/uai.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uai_input_s input;
    uai_weight_s weight;
    uai_bias_s bias;
    uai_output_s output;
    uint32_t act_width;
    uai_fconn_type_e fconn_type;
    uai_fconn_func fconn_func;
    uai_act_type_e act_type;
    uai_act_func act_func;
    uai_softmax_type_e softmax_type;
    uai_softmax_func softmax_func;
}uai_dnn_layer_t;

typedef struct {
    bool preload;
    uint16_t layer_num;   /* 0 is input layer, layer_num-1 is output layer */
    uint16_t result_num;
    uai_dnn_layer_t *layer_info;
    char *trained_data_src;
}uai_dnn_model_t;

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_DNN_H */
