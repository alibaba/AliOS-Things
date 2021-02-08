## Contents

```sh
uai
├── 3rdparty
│   └── cmsis_nn
│       ├── aos.mk
│       ├── Config.in
│       └── get_cmsis_nn.sh
├── aos.mk
├── common
│   └── uai_common.h
├── Config.in
├── function
│   ├── activation
│   │   ├── uai_relu.c
│   │   ├── uai_sigmoid.c
│   │   └── uai_tanh.c
│   ├── connected
│   │   └── uai_fully_connected.c
│   ├── include
│   │   └── uai_function.h
│   └── softmax
│       └── uai_softmax.c
├── include
│   ├── uai_api.h
│   └── uai_typedef.h
├── model
│   └── load
│       ├── uai_load.c
│       └── uai_load.h
├── nn
│   ├── dnn
│   │   ├── uai_dnn.c
│   │   └── uai_dnn.h
│   └── include
│       └── uai_nn_local.h
└── README.md

```

## Introduction

uAI is an Neural Network module of AliOS Things, support common Neural Network model, such as DNN, CNN, RNN and so on. Users can use these models through simple configurations.

## Features

1. support Open source NN framework, like CMSIS NN, Tensor Flow Lite(future);
2. support load model model data from flash, memory or file system;
3. support Neural Network model configuration;
4. support hardware Neural Network accelerator engine.(future)
5. footprint: ROM-->4K RAM: 1K. (now)

## Dependencies

* CMSIS NN
* ulog

## API

User service APIs:

```c
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

#ifdef UAI_DNN
/**
 * init dnn configuration.
 *
 * @param[in]  model_config     configuration information.
 * @param[in]  model_data_src   model data source description.
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_init(uai_dnn_config_t *model_config, char *model_data_src);

/*
 * deinit dnn, free allocated memory
 *
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_deinit(void);

/*
 * run dnn model
 *
 *
 * @param  in_data    input data
 * @param  out_data   output data
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_run(int8_t *in_data, int8_t *out_data);
#endif

#ifdef UAI_CNN
/**
 * init cnn configuration.
 *
 * @param[in]  model_config     configuration information.
 * @param[in]  model_data_src   model data source description.
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_init(uai_cnn_config_t *model_config, char *model_data_src);

/*
 * deinit cnn, free allocated memory
 *
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_deinit(void);

/*
 * run dnn model
 *
 *
 * @param  in_data    input data
 * @param  out_data   output data
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_run(int8_t *in_data, int8_t *out_data);
#endif

```
for sample code please check [uai_demo](../../../app/example/uai_demo/).

## Model data source example
* Flash:

  ```pt:1,offset:4096,size:81920```

  pt is flash partition number, always defined is file board/[board_name]/board.c

* memory:

  ```mem:900000,size:81920 ```

  mem is memory address

* file system:

  ```fs:/sdcard/kws_dnn.data```

### ``` Notice: address, offset and size only support decimal number ```

## RTOS build
```sh
aos make uai_demo.uai_kws_demo@board -c config;
aos make
```
