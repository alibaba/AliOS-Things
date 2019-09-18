/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>

#include "ulog/ulog.h"
#include "uai/uai.h"
#include "cifar10_weights.h"

#define MAX_SRC_LEN 128

extern int8_t   cifar10_model_data[];
extern uint32_t cifar10_model_data_len;

extern void imgdata_preprocess(int8_t *img_buffer);

static char output_class[IP1_OUT][11] = {"airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck"};
static uai_cnn_layer_config_t layer_info[7] =   {   {
                                                        UAI_LAYER_CONV,
                                                    /*  batch height       width        depth channels    dtype  */
                                                        {1, CONV1_IM_DIM,  CONV1_IM_DIM,  0, CONV1_IM_CH, 1},   /* input */
                                                        {1, CONV1_KER_DIM, CONV1_KER_DIM, 0, 0, 1},             /* weight */
                                                        {1, CONV1_OUT_DIM, CONV1_OUT_DIM, 0, CONV1_OUT_CH, 1},  /* output */
                                                        {CONV1_STRIDE, CONV1_STRIDE},                           /* strides[2] */
                                                        CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT, 0, UAI_PAD_SAME,    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_CONV_BASIC}, UAI_ACT_RELU, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 1, convolution */
                                                    {
                                                        UAI_LAYER_POOL,
                                                    /*  batch height       width        depth channels    dtype  */
                                                        {1, CONV1_OUT_DIM, CONV1_OUT_DIM, 0, CONV1_OUT_CH, 1},     /* input */
                                                        {1, POOL1_KER_DIM, POOL1_KER_DIM, 0, 0, 1},                /* weight */
                                                        {1, POOL1_OUT_DIM, POOL1_OUT_DIM, 0, CONV1_OUT_CH, 1},     /* output */
                                                        {POOL1_STRIDE, POOL1_STRIDE},                              /* strides[2] */
                                                        0, 0, 0, UAI_PAD_SAME,                                     /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_POOL_MAX}, UAI_ACT_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 1, pool */
                                                    {
                                                        UAI_LAYER_CONV,
                                                        {1, CONV2_IM_DIM,  CONV2_IM_DIM,  0, CONV2_IM_CH, 1},   /* input */
                                                        {0, CONV2_KER_DIM, CONV2_KER_DIM, 0, 0, 1},             /* weight */
                                                        {1, CONV2_OUT_DIM, CONV2_OUT_DIM, 0, CONV2_OUT_CH, 1},  /* output */
                                                        {CONV2_STRIDE, CONV2_STRIDE},                           /* strides[2] */
                                                        CONV2_BIAS_LSHIFT, CONV2_OUT_RSHIFT, 0, UAI_PAD_SAME,    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_CONV_BASIC}, UAI_ACT_RELU, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 2, convolution */
                                                    {
                                                        UAI_LAYER_POOL,
                                                    /*  batch height       width        depth channels    dtype  */
                                                        {1, CONV2_OUT_DIM, CONV2_OUT_DIM, 0, CONV2_OUT_CH, 1},    /* input */
                                                        {1, POOL2_KER_DIM, POOL2_KER_DIM, 0, 0, 1},               /* weight */
                                                        {1, POOL2_OUT_DIM, POOL2_OUT_DIM, 0, CONV2_OUT_CH, 1},    /* output */
                                                        {POOL2_STRIDE, POOL2_STRIDE},                             /* strides[2] */
                                                        0, 0, 0, UAI_PAD_SAME,                                    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_POOL_MAX}, UAI_ACT_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 2, pool */
                                                    {
                                                        UAI_LAYER_CONV,
                                                        {1, CONV3_IM_DIM,  CONV3_IM_DIM,  0, CONV3_IM_CH, 1},   /* input */
                                                        {0, CONV3_KER_DIM, CONV3_KER_DIM, 0, 0, 1},             /* weight */
                                                        {0, CONV3_OUT_DIM, CONV3_OUT_DIM, 0, CONV3_OUT_CH, 1},  /* output */
                                                        {CONV3_STRIDE, CONV3_STRIDE},                           /* strides[2] */
                                                        CONV3_BIAS_LSHIFT, CONV3_OUT_RSHIFT, 0, UAI_PAD_SAME,    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_CONV_BASIC}, UAI_ACT_RELU, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 3, convolution */
                                                    {
                                                        UAI_LAYER_POOL,
                                                    /*  batch height       width        depth channels    dtype  */
                                                        {1, CONV3_OUT_DIM,  CONV3_OUT_DIM,  0, CONV3_OUT_CH, 1},   /* input */
                                                        {0, POOL3_KER_DIM, POOL3_KER_DIM, 0, 0, 1},             /* weight */
                                                        {0, POOL3_OUT_DIM, POOL3_OUT_DIM, 0, CONV3_OUT_CH, 1},  /* output */
                                                        {POOL3_STRIDE, POOL3_STRIDE},                           /* strides[2] */
                                                        0, 0, 0, UAI_PAD_SAME,    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_POOL_MAX}, UAI_ACT_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL
                                                    }, /* layer 3, pool */
                                                    {
                                                        UAI_LAYER_FCONN,
                                                        {1, IP1_DIM, 1, 0, 0, 1},              /* input */
                                                        {0, IP1_OUT, IP1_DIM, 0, 0, 1},        /* weight */
                                                        {0, IP1_OUT, 1, 0, 0, 1},               /* output */
                                                        {0, 0},                                  /* strides[2] */
                                                        IP1_BIAS_LSHIFT, IP1_OUT_RSHIFT, 0, 0,    /* bias_shift, out_shift, act_width, pad_type */
                                                        {UAI_FCONN_OPT}, UAI_ACT_NULL, UAI_LOSS_NULL, UAI_SOFTMAX
                                                    }  /* layer 4, fully connected */
                                                };

static void demo_cnn_init(void)
{
    char data_src[MAX_SRC_LEN];
    uai_cnn_config_t model_config;

    memset(&model_config, 0, sizeof(uai_cnn_config_t));
    model_config.preload    = true;
    model_config.layer_num  = 7;
    model_config.result_num = IP1_OUT;

    model_config.layer_info = layer_info;

    memset(data_src, 0, MAX_SRC_LEN);
    snprintf(data_src, sizeof(data_src), "mem:%u,size:%u", cifar10_model_data, cifar10_model_data_len);

    uai_cnn_init(&model_config, data_src);
}

static void cifar10_demo()
{
    int8_t output_result[IP1_OUT] = {0};
    int8_t result = 0;
    int8_t *img_buffer = NULL;
    int32_t i = 0;
    int32_t max_ind = 0;

    img_buffer = malloc(CONV1_IM_DIM * CONV1_IM_DIM * CONV1_IM_CH);
    memset(img_buffer, 0, CONV1_IM_DIM * CONV1_IM_DIM * CONV1_IM_CH);

    imgdata_preprocess(img_buffer);

    demo_cnn_init();

    printf("Start running the CNN model to predict ...\r\n");

    uai_cnn_run(img_buffer, output_result);

    printf("End of running CNN model\r\n");

    for (i = 0; i < IP1_OUT; i++) {
        if(result < output_result[i] ) {
            result = output_result[i];
            max_ind = i;
        }
        printf("%d: %d\r\n", i, output_result[i]);
    }

    printf("Detected image is [%s], score is %d%%\r\n", output_class[max_ind], ((int32_t)result) * 100 / 128);

    free(img_buffer);
}

int application_start(int argc, char *argv[])
{
    printf("uai cifar10 demo entry !\r\n");
    aos_set_log_level(AOS_LL_INFO);

    cifar10_demo();

    while(1) {
        aos_msleep(1000);
    };
}