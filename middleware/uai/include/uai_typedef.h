/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_TYPEDEF_H
#define UAI_TYPEDEF_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UAI_FAIL   -1
#define UAI_SUCCESS 0

typedef enum {
    UAI_FCONN_NULL,
    UAI_FCONN_BASIC,
    UAI_FCONN_OPT,
    UAI_FCONN_END,
}uai_fconn_type_e;

typedef enum {
    UAI_CONV_NULL,
    UAI_CONV_BASIC,
    UAI_CONV_RGB,
    UAI_CONV_FAST,
    UAI_CONV_END,
}uai_conv_type_e;

typedef enum {
    UAI_ACT_NULL,
    UAI_ACT_RELU,
    UAI_ACT_RELU6,
    UAI_ACT_SIGMOD,
    UAI_ACT_TANH,
    UAI_ACT_END
}uai_act_type_e;

typedef enum {
    UAI_POOL_NULL,
    UAI_POOL_MAX,
    UAI_POOL_AVG,
    UAI_POOL_END,
}uai_pool_type_e;

typedef enum {
    UAI_SOFTMAX_NULL,
    UAI_SOFTMAX,
    UAI_SOFTMAX_END,
}uai_softmax_type_e;

typedef enum {
    UAI_LOSS_NULL,
    UAI_LOSS,         /* TODO: This version does not support */
    UAI_LOSS_END
}uai_loss_type_e;

#ifdef UAI_DNN
typedef struct {
    uint16_t in_dim;
    uint16_t out_dim;
    uint16_t bias_shift;
    uint16_t out_shift;
    uint32_t act_width;
    uai_fconn_type_e fconn_type;
    uai_act_type_e act_type;
    uai_pool_type_e pool_type;
    uai_loss_type_e loss_type;
    uai_softmax_type_e softmax_type;
}uai_dnn_layer_config_t;

typedef struct {
    bool preload;
    uint16_t layer_num;   /* 0 is input layer, layer_num-1 is output layer */
    uint16_t result_num;
    uai_dnn_layer_config_t *layer_info;
}uai_dnn_config_t;

#endif

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_TYPEDEF_H */
