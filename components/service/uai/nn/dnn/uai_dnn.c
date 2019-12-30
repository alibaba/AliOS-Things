/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include "uai_dnn.h"

static uai_dnn_model_t *g_model = NULL;
static uai_fconn_func       fconn_func_local[UAI_FCONN_END - 1] = {uai_fconn, uai_fconn_opt};
static uai_act_func             act_func_local[UAI_ACT_END - 1] = {uai_relu, uai_relu6, uai_sigmod, uai_tanh};
static uai_softmax_func softmax_func_local[UAI_SOFTMAX_END - 1] = {uai_softmax};

static char    fconn_type_str[UAI_FCONN_END - 1][20] = {"fully connected", "fully connected opt"};
static char         act_type_str[UAI_ACT_END - 1][8] = {"relu", "relu6", "sigmod", "tanh"};
static char softmax_type_str[UAI_SOFTMAX_END - 1][8] = {"softmax"};

static void uai_dnn_info_show()
{
    int32_t idx = 0;

    UAI_LOGI("DNN info:");
    UAI_LOGI("layer  num: %d", g_model->layer_num);
    UAI_LOGI("result num: %d", g_model->result_num);
    UAI_LOGI("model data preload: %s", (g_model->preload)?"yes":"no");
    UAI_LOGI("model data source : %s\r\n", g_model->model_data_src);

    for(idx = 0; idx < g_model->layer_num; idx++) {
        uai_dnn_layer_t *layer_info = &g_model->layer_info[idx];
        UAI_LOGI(" layer %d info:", idx);

        /* input */
        UAI_LOGI("   input info:");
        UAI_PRINT_LAYER_INFO(input, batch);
        UAI_PRINT_LAYER_INFO(input, height);
        UAI_PRINT_LAYER_INFO(input, width);
        UAI_PRINT_LAYER_INFO(input, depth);
        UAI_PRINT_LAYER_INFO(input, size);
        UAI_PRINT_LAYER_INFO(input, channels);

        /* weight */
        UAI_LOGI("  weight info:");
        UAI_PRINT_LAYER_INFO(weight, height);
        UAI_PRINT_LAYER_INFO(weight, width);
        UAI_PRINT_LAYER_INFO(weight, depth);
        UAI_PRINT_LAYER_INFO(weight, size);
        UAI_PRINT_LAYER_INFO(weight, in_channels);
        UAI_PRINT_LAYER_INFO(weight, out_channels);

        /* bias */
        UAI_LOGI("    bias info:");
        UAI_PRINT_LAYER_INFO(bias, height);
        UAI_PRINT_LAYER_INFO(bias, width);
        UAI_PRINT_LAYER_INFO(bias, depth);
        UAI_PRINT_LAYER_INFO(bias, size);
        UAI_PRINT_LAYER_INFO(bias, shift);

        /* output */
        UAI_LOGI("  output info:");
        UAI_PRINT_LAYER_INFO(output, height);
        UAI_PRINT_LAYER_INFO(output, width);
        UAI_PRINT_LAYER_INFO(output, depth);
        UAI_PRINT_LAYER_INFO(output, size);
        UAI_PRINT_LAYER_INFO(output, channels);
        UAI_PRINT_LAYER_INFO(output, shift);

        if(layer_info->fconn_func != NULL) {
            UAI_LOGI("fully connected type: %s", fconn_type_str[layer_info->fconn_type - 1]);
        }
        if(layer_info->act_func != NULL) {
            UAI_LOGI("activation type: %s", act_type_str[layer_info->act_type - 1]);
            if(layer_info->act_type == UAI_ACT_SIGMOD) {
                UAI_LOGI("sigmod width: %d", layer_info->act_width);
            }
        }
        if(layer_info->softmax_func != NULL) {
            UAI_LOGI("softmax type: %s", softmax_type_str[layer_info->softmax_type - 1]);
        }
        UAI_LOGI("\r\n");
    }
}

static int uai_dnn_preload_model_data(uai_dnn_layer_t *layer_info, int32_t offset, char *model_data_src, uai_src_type_e type)
{
    if(type != UAI_SRC_TYPE_MEM) {
        layer_info->weight.buffer = uai_malloc(layer_info->weight.size);
        layer_info->bias.buffer   = uai_malloc(layer_info->bias.size);
        uai_load_model_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, model_data_src);
    } else {
        uai_load_model_data((int8_t *)&layer_info->weight.buffer, (int8_t *)&layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, model_data_src);
    }

    return layer_info->weight.size + layer_info->bias.size;
}

int32_t uai_dnn_init(uai_dnn_config_t *model_config, char *model_data_src)
{
    int32_t idx     = 0;
    uint32_t offset = 0;
    uai_src_type_e type;

    if((model_config == NULL) || (model_data_src == NULL) || (model_config->layer_info == NULL)) {
        UAI_LOGE("param null");
        return UAI_FAIL;
    }

    if(g_model != NULL) {
        UAI_LOGE("repeat init");
        return UAI_FAIL;
    }

    g_model = uai_malloc(sizeof(uai_dnn_model_t));
    if(g_model == NULL) {
        UAI_LOGE("uai_malloc fail");
        return UAI_FAIL;
    }

    g_model->preload    = model_config->preload;
    g_model->layer_num  = model_config->layer_num;
    g_model->result_num = model_config->result_num;

    g_model->layer_info = (uai_dnn_layer_t *)uai_malloc(sizeof(uai_dnn_layer_t) * g_model->layer_num);
    if(g_model->layer_info == NULL) {
        UAI_LOGE("uai_malloc fail");
        uai_free(g_model);
        g_model = NULL;
        return UAI_FAIL;
    }
    memset(g_model->layer_info, 0, sizeof(uai_dnn_layer_t) * g_model->layer_num);

    type = uai_src_type_parse(model_data_src);
    if (type == UAI_SRC_TYPE_MEM) {
        g_model->preload = true;
    }

    for(idx = 0; idx < g_model->layer_num; idx ++) {
        uai_dnn_layer_t *layer_info = &g_model->layer_info[idx];

        /* input */
        layer_info->input.height  = model_config->layer_info[idx].input.height;
        layer_info->input.width   = model_config->layer_info[idx].input.width;
        layer_info->input.size    = layer_info->input.height * layer_info->input.width;
        layer_info->input.dtype   = model_config->layer_info[idx].input.dtype;

        /* output */
        layer_info->output.dtype   = model_config->layer_info[idx].output.dtype;
        layer_info->output.height  = model_config->layer_info[idx].output.height;
        layer_info->output.width   = model_config->layer_info[idx].output.width;
        layer_info->output.size    = layer_info->output.height * layer_info->output.width;
        layer_info->output.shift   = model_config->layer_info[idx].out_shift;

        /* weight */
        layer_info->weight.width   = model_config->layer_info[idx].input.height;
        layer_info->weight.height  = model_config->layer_info[idx].output.height;
        layer_info->weight.size    = layer_info->weight.height * layer_info->weight.width;
        layer_info->weight.dtype   = model_config->layer_info[idx].input.dtype;

        /* bias */
        layer_info->bias.dtype   = model_config->layer_info[idx].output.dtype;
        layer_info->bias.shift   = model_config->layer_info[idx].bias_shift;
        layer_info->bias.size    = layer_info->output.size;

        /* fully connected */
        layer_info->fconn_type = model_config->layer_info[idx].fconn_type;
        if((layer_info->fconn_type > UAI_FCONN_NULL) &&
            (layer_info->fconn_type < UAI_FCONN_END)) {
            layer_info->fconn_func = fconn_func_local[layer_info->fconn_type - 1];
        }

        /* activation */
        layer_info->act_type = model_config->layer_info[idx].act_type;
        if((layer_info->act_type > UAI_ACT_NULL) &&
            (layer_info->act_type < UAI_ACT_END)) {
            layer_info->act_func = act_func_local[layer_info->act_type - 1];
        }

        /* softmax */
        layer_info->softmax_type = model_config->layer_info[idx].softmax_type;
        if((layer_info->softmax_type > UAI_SOFTMAX_NULL) &&
            (layer_info->softmax_type < UAI_SOFTMAX_END)) {
            layer_info->softmax_func = softmax_func_local[layer_info->softmax_type - 1];
        }

        /* pre load model data*/
        if(g_model->preload == true) {
            offset += uai_dnn_preload_model_data(layer_info, offset, model_data_src, type);
        }
    }

    g_model->model_data_src = (char *)uai_malloc(strlen(model_data_src) + 1);
    memset(g_model->model_data_src, 0, strlen(model_data_src) + 1);
    memcpy(g_model->model_data_src, model_data_src, strlen(model_data_src));

    uai_dnn_info_show();
    UAI_LOGI("dnn init success\n");

    return UAI_SUCCESS;
}

int32_t uai_dnn_deinit(void)
{
    int32_t idx = 0;

    if(g_model == NULL) {
        UAI_LOGE("not init");
        return -1;
    }

    if(g_model->preload == true) {
        uai_src_type_e type = UAI_SRC_TYPE_NULL;
        type = uai_src_type_parse(g_model->model_data_src);
        if(type != UAI_SRC_TYPE_MEM) {
            for(idx = 0; idx < g_model->layer_num; idx ++) {
                if(g_model->layer_info[idx].weight.buffer != NULL) {
                    uai_free(g_model->layer_info[idx].weight.buffer);
                }
                if(g_model->layer_info[idx].bias.buffer != NULL) {
                    uai_free(g_model->layer_info[idx].bias.buffer);
                }
            }
        }
    }
    uai_free(g_model->model_data_src);
    uai_free(g_model->layer_info);
    uai_free(g_model);

    g_model = NULL;

    return 0;
}

static int8_t *uai_dnn_layer_run(uai_dnn_layer_t *layer_info, int8_t* in_buffer, int8_t *out_buffer, uint32_t *offset)
{
    int32_t ret            = 0;
    int8_t *scratch_buffer = NULL;

    if(out_buffer == NULL) {
        scratch_buffer = uai_malloc(layer_info->output.size);
        if(scratch_buffer == NULL) {
            UAI_LOGE("uai_malloc error");
            return NULL;
        }
        memset(scratch_buffer, 0, layer_info->output.size);
        out_buffer = scratch_buffer;
    }

    if(g_model->preload == false) {
        layer_info->weight.buffer = uai_malloc(layer_info->weight.size);
        layer_info->bias.buffer   = uai_malloc(layer_info->bias.size);
        ret = uai_load_model_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, *offset, g_model->model_data_src);
        if(ret != UAI_SUCCESS) {
            UAI_LOGE("load model data error");
            uai_free(layer_info->weight.buffer);
            uai_free(layer_info->bias.buffer);
            if(scratch_buffer != NULL) {
                uai_free(scratch_buffer);
            }

            layer_info->weight.buffer = NULL;
            layer_info->bias.buffer   = NULL;

            return NULL;
        }
        *offset += layer_info->weight.size + layer_info->bias.size;
    }

    layer_info->input.buffer  = in_buffer;
    layer_info->output.buffer = out_buffer;

    /* fully connected */
    if(layer_info->fconn_func != NULL) {
        UAI_LOGD("run fully connected\r\n");
        layer_info->fconn_func(&layer_info->input, &layer_info->weight, &layer_info->bias, &layer_info->output);
    }

    /* activation */
    if(layer_info->act_func != NULL) {
        UAI_LOGD("run activation function\r\n");
        layer_info->act_func(layer_info->output.buffer, layer_info->output.size, layer_info->act_width);
    }

    /* softmax */
    if(layer_info->softmax_func != NULL) {
        UAI_LOGD("run softmax function\r\n");
        layer_info->softmax_func(layer_info->output.buffer, layer_info->output.size, layer_info->output.buffer);
    }

    if(g_model->preload == false) {
        uai_free(layer_info->weight.buffer);
        uai_free(layer_info->bias.buffer);
        layer_info->weight.buffer = NULL;
        layer_info->bias.buffer   = NULL;
    }

    return scratch_buffer;
}

int32_t uai_dnn_run(int8_t *in_data, int8_t *out_data)
{
    int16_t idx            = 0;
    int8_t *in_buffer      = NULL;
    int8_t *out_buffer     = NULL;
    uint32_t offset        = 0;

    if(g_model == NULL) {
        UAI_LOGE("not init");
        return UAI_FAIL;
    }

    if((in_data == NULL) || (out_data == NULL)) {
        UAI_LOGE("param null");
        return UAI_FAIL;
    }

    /* input layer */
    UAI_LOGD("run input layer\r\n");
    out_buffer = uai_dnn_layer_run(&g_model->layer_info[0], in_data, NULL, &offset);
    if(out_buffer == NULL) {
        UAI_LOGE("dnn inpurt layer run fail");
        return UAI_FAIL;
    }

    /* hidden layer */
    in_buffer = out_buffer;
    for(idx = 1; idx < g_model->layer_num - 1; idx ++) {
        UAI_LOGD("run hidden layer %d\r\n", idx);
        out_buffer = uai_dnn_layer_run(&g_model->layer_info[idx], in_buffer, NULL, &offset);
        if(out_buffer == NULL) {
            UAI_LOGE("dnn layer run fail");
            return UAI_FAIL;
        }

        uai_free(in_buffer);
        in_buffer = out_buffer;
    }

    /* output layer */
    UAI_LOGD("run output layer\r\n");
    uai_dnn_layer_run(&g_model->layer_info[idx], in_buffer, out_data, &offset);
    uai_free(in_buffer);

    UAI_LOGD("dnn layer run finish\n");
    return UAI_SUCCESS;
}
