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
    UAI_LAYER_NULL,
    UAI_LAYER_FCONN,
    UAI_LAYER_CONV,
    UAI_LAYER_POOL,
    UAI_LAYER_END,
}uai_layer_type_e;

typedef enum {
    UAI_FCONN_NULL,
    UAI_FCONN_BASIC,
    UAI_FCONN_OPT,
    UAI_FCONN_END,
}uai_fconn_type_e;

typedef enum {
    UAI_CONV_NULL,
    UAI_CONV_BASIC,
    UAI_CONV_DEPTHWISE_SP,   /* depth wise separable */
    UAI_CONV_TRANSPOSE,
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

#ifdef UAI_ODLA_SUPPORT

#define UAI_MAX_DIMENSION 5
typedef enum {
    UAI_NHWC = 0, /* input and output layout: batch height width channels*/
    UAI_SIO  = 1   /* kernel layout: height width input_CHANNELS output_channels*/
}uai_dim_format;

typedef struct {
    uint32_t size;
    uai_dim_format format;
    uint32_t dims[UAI_MAX_DIMENSION];
}uai_dims;

typedef struct {
    int8_t  *buffer;
    uai_dims dims;
    uint32_t size;
    int8_t shift;
    int8_t dtype;
}uai_tensor_s;

typedef enum {
    UAI_KERNEL_SCALE = 0,
    UAI_BIAS_SCALE   = 1,
    UAI_ACT_SCALE    = 2,
    UAI_SCALE_END
}uai_quant_scale_type;

typedef struct {
    int32_t *scale;
    uint32_t scale_num;
    uint32_t shift;
}uai_quant_scale;
#else
typedef struct {
    uint16_t batch;
    uint16_t height;
    uint16_t width;
    uint16_t depth;
    uint16_t channels;
    int16_t dtype;    /* data type: 1: int8_t, 2: int16_t */
}uai_shape_s;

typedef enum {
    UAI_PAD_VALID,
    UAI_PAD_SAME,
}uai_pad_type_e;

#ifdef UAI_DNN
typedef struct {
    uai_shape_s input;
    uai_shape_s output;
    uint16_t bias_shift;   /*  (fi + fw) - fb */
    uint16_t out_shift;    /*  (fi + fw) - fo */
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

#ifdef UAI_CNN
typedef struct {
    uai_layer_type_e layer_type;
    uai_shape_s input;
    uai_shape_s weight;
    uai_shape_s output;
    uint16_t    strides[2];
    uint16_t    bias_shift;   /*  (fi + fw) - fb */
    uint16_t    out_shift;    /*  (fi + fw) - fo */
    uint32_t    act_width;
    uai_pad_type_e pad_type;

    union {
        uai_conv_type_e conv_type;
        uai_fconn_type_e fconn_type;
        uai_pool_type_e pool_type;
    };

    uai_act_type_e act_type;
    uai_loss_type_e loss_type;
    uai_softmax_type_e softmax_type;
}uai_cnn_layer_config_t;

typedef struct {
    bool preload;
    uint16_t layer_num;   /* 0 is input layer, layer_num-1 is output layer */
    uint16_t result_num;
    uai_cnn_layer_config_t *layer_info;
}uai_cnn_config_t;
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_TYPEDEF_H */
