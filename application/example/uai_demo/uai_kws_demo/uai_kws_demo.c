/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>

#include "ulog/ulog.h"
#include "uai/uai.h"
#include "mfcc.h"
#include "kws.h"

static int8_t output_result[OUT_DIM] = {0};
static char output_class[OUT_DIM][8] = {"Silence", "Unknown", "yes", "no", "up", "down", "left", "right", "on", "off", "stop", "go"};
static int8_t *mfcc_buffer = NULL;

extern int16_t  wav_data[];
extern uint32_t wav_data_len;
extern int8_t   kws_model_data[];
extern uint32_t kws_model_data_len;

static uai_dnn_layer_config_t layer_info[4] = {{{0, IN_DIM, 1, 0, 0, 1}, {0, IP1_OUT_DIM, 1, 0, 0, 1}, 1, 7, 0, UAI_FCONN_BASIC, UAI_ACT_RELU, UAI_POOL_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL},
                                                {{0, IP1_OUT_DIM, 1, 0, 0, 1}, {0, IP2_OUT_DIM, 1, 0, 0, 1}, 2, 8, 0, UAI_FCONN_BASIC, UAI_ACT_RELU, UAI_POOL_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL},
                                                {{0, IP2_OUT_DIM, 1, 0, 0, 1}, {0, IP3_OUT_DIM, 1, 0, 0, 1}, 2, 9, 0, UAI_FCONN_BASIC, UAI_ACT_RELU, UAI_POOL_NULL, UAI_LOSS_NULL, UAI_SOFTMAX_NULL},
                                                {{0, IP3_OUT_DIM, 1, 0, 0, 1}, {0, OUT_DIM, 1, 0, 0, 1}, 0, 6, 0,UAI_FCONN_BASIC, UAI_ACT_NULL, UAI_POOL_NULL, UAI_LOSS_NULL, UAI_SOFTMAX}};

static void kws_dnn_init(void)
{
    char data_src[MAX_SRC_LEN];
    uai_dnn_config_t model_config;

    memset(&model_config, 0, sizeof(uai_dnn_config_t));
    model_config.preload    = true;
    model_config.layer_num  = 4;
    model_config.result_num = OUT_DIM;

    model_config.layer_info = layer_info;

    memset(data_src, 0, MAX_SRC_LEN);
    snprintf(data_src, sizeof(data_src), "mem:%u,size:%u", kws_model_data, kws_model_data_len);

    uai_dnn_init(&model_config, data_src);
}

static void kws_demo(void)
{
    int max_ind = 0;

    mfcc_buffer = malloc(MFCC_BUFFER_SIZE);

    kws_mfcc_init(NUM_MFCC_COEFFS, FRAME_LEN, FRAME_SHIFT, MFCC_DEC_BITS);
    kws_dnn_init();

    printf("Start running the DNN model to predict ...\r\n");

    /* run kws prediction */
    kws_get_mfcc_features(wav_data, NUM_FRAMES, mfcc_buffer);
    uai_dnn_run(mfcc_buffer, output_result);
    max_ind = kws_get_top_class(output_result, OUT_DIM);

    printf("End of running DNN model\r\n");

    printf("Detected word [%s], score %d%%\r\n", output_class[max_ind], ((int)output_result[max_ind]*100/128));

    uai_dnn_deinit();
    free(mfcc_buffer);
}

int application_start(int argc, char *argv[])
{
    printf("uai kws demo entry !\r\n");
    aos_set_log_level(AOS_LL_INFO);

    kws_demo();

    while(1) {
        aos_msleep(1000);
    };
}