/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include "uai_cnn.h"
#include "stdio.h"

static uai_cnn_model_t *g_model = NULL;
static uai_conv_func          conv_func_local[UAI_CONV_END - 1] = {uai_conv, uai_conv_depthwise_sp};
static uai_fconn_func       fconn_func_local[UAI_FCONN_END - 1] = {uai_fconn, uai_fconn_opt};
static uai_act_func             act_func_local[UAI_ACT_END - 1] = {uai_relu, uai_relu6, uai_sigmod, uai_tanh};
static uai_softmax_func softmax_func_local[UAI_SOFTMAX_END - 1] = {uai_softmax};
static uai_pool_func          pool_func_local[UAI_POOL_END - 1] = {uai_pool_max, uai_pool_ave};

const static char    layer_type_str[UAI_LAYER_END - 1][16] = {"fully connected", "convolution", "pool"};
const static char      conv_type_str[UAI_CONV_END - 1][26] = {"conv", "depthwise separable conv"};
const static char    fconn_type_str[UAI_FCONN_END - 1][20] = {"fully connected", "fully connected opt"};
const static char         act_type_str[UAI_ACT_END - 1][8] = {"relu", "relu6", "sigmod", "tanh"};
const static char softmax_type_str[UAI_SOFTMAX_END - 1][8] = {"softmax"};
const static char       pool_type_str[UAI_POOL_END - 1][8] = {"max", "average"};

static void uai_cnn_info_show()
{
    int32_t idx = 0;

    UAI_LOGI("CNN info:");
    UAI_LOGI("layer  num: %d", g_model->layer_num);
    UAI_LOGI("result num: %d", g_model->result_num);
    UAI_LOGI("model data preload: %s", (g_model->preload) ? "yes" : "no");
    UAI_LOGI("model data source : %s\r\n", g_model->model_data_src);

    for(idx = 0; idx < g_model->layer_num; idx++) {
        uai_cnn_layer_t *layer_info = &g_model->layer_info[idx];
        UAI_LOGI("layer %d info:", idx);

        UAI_LOGI(" layer type: %s", layer_type_str[layer_info->layer_type - 1]);

        /* input */
        UAI_LOGI("   input info:");
        UAI_PRINT_LAYER_INFO(input, batch);
        UAI_PRINT_LAYER_INFO(input, height);
        UAI_PRINT_LAYER_INFO(input, width);
        UAI_PRINT_LAYER_INFO(input, depth);
        UAI_PRINT_LAYER_INFO(input, channels);
        UAI_PRINT_LAYER_INFO(input, size);

        /* weight */
        UAI_LOGI("  weight info:");
        UAI_PRINT_LAYER_INFO(weight, height);
        UAI_PRINT_LAYER_INFO(weight, width);
        UAI_PRINT_LAYER_INFO(weight, depth);
        UAI_PRINT_LAYER_INFO(weight, in_channels);
        UAI_PRINT_LAYER_INFO(weight, out_channels);
        UAI_PRINT_LAYER_INFO(weight, size);

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
        UAI_PRINT_LAYER_INFO(output, channels);
        UAI_PRINT_LAYER_INFO(output, size);
        UAI_PRINT_LAYER_INFO(output, shift);

        UAI_LOGI("pad type: %s", (layer_info->pad_type == UAI_PAD_SAME) ? "same" : "valid");

        if(layer_info->strides[0] != 0) {
            UAI_LOGI("strides : %d %d", layer_info->strides[0], layer_info->strides[1]);
        }
        switch(layer_info->layer_type) {
            case UAI_LAYER_FCONN:
                if(layer_info->fconn_func != NULL) {
                    UAI_LOGI("fully connected type: %s", fconn_type_str[layer_info->fconn_type - 1]);
                }
                break;

            case UAI_LAYER_CONV:
                if(layer_info->conv_func != NULL) {
                    UAI_LOGI("convolution type: %s", conv_type_str[layer_info->conv_type - 1]);
                }
                break;

            case UAI_LAYER_POOL:
                if(layer_info->pool_func != NULL) {
                    UAI_LOGI("pool type: %s", pool_type_str[layer_info->pool_type - 1]);
                }
                break;
            default:
                break;
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

static int uai_cnn_preload_model_data(uai_cnn_layer_t *layer_info, int32_t offset, char *model_data_src, uai_src_type_e type)
{
    if(type != UAI_SRC_TYPE_MEM) {
        UAI_NON_ZERO_ZALLOC(layer_info->weight.buffer, layer_info->weight.size);
        UAI_NON_ZERO_ZALLOC(layer_info->bias.buffer, layer_info->bias.size);
        uai_load_model_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, model_data_src);
    } else {
        uai_load_model_data((int8_t *)&layer_info->weight.buffer, (int8_t *)&layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, model_data_src);
    }

    return layer_info->weight.size + layer_info->bias.size;
}

int32_t uai_cnn_init(uai_cnn_config_t *model_config, char *model_data_src)
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

    UAI_NON_ZERO_ZALLOC(g_model, sizeof(uai_cnn_model_t));
    if(g_model == NULL) {
        UAI_LOGE("uai_malloc fail");
        return UAI_FAIL;
    }

    g_model->preload    = model_config->preload;
    g_model->layer_num  = model_config->layer_num;
    g_model->result_num = model_config->result_num;

    UAI_NON_ZERO_ZALLOC(g_model->layer_info, sizeof(uai_cnn_layer_t) * g_model->layer_num);
    if(g_model->layer_info == NULL) {
        UAI_LOGE("uai_malloc fail");
        UAI_NON_ZERO_FREE(g_model);
        return UAI_FAIL;
    }

    type = uai_src_type_parse(model_data_src);
    if (type == UAI_SRC_TYPE_MEM) {
        g_model->preload = true;
    }

    for(idx = 0; idx < g_model->layer_num; idx ++) {
        uint16_t batch  = 0;
        uint16_t height = 0;
        uint16_t width  = 0;
        uint16_t depth  = 0;
        uint16_t channels = 0;

        uai_cnn_layer_t             *layer_info = &g_model->layer_info[idx];
        uai_cnn_layer_config_t *conf_layer_info = &model_config->layer_info[idx];

        layer_info->layer_type = conf_layer_info->layer_type;

        /* input */
        layer_info->input.buffer   = NULL;
        layer_info->input.batch    = conf_layer_info->input.batch;
        layer_info->input.height   = conf_layer_info->input.height;
        layer_info->input.width    = conf_layer_info->input.width;
        layer_info->input.depth    = conf_layer_info->input.depth;
        layer_info->input.channels = conf_layer_info->input.channels;
        layer_info->input.dtype    = conf_layer_info->input.dtype;

        batch  = UAI_GET_VALUE_BEGIN_ONE(layer_info->input.batch);
        height = UAI_GET_VALUE_BEGIN_ONE(layer_info->input.height);
        width  = UAI_GET_VALUE_BEGIN_ONE(layer_info->input.width);
        depth  = UAI_GET_VALUE_BEGIN_ONE(layer_info->input.depth);
        channels = UAI_GET_VALUE_BEGIN_ONE(layer_info->input.channels);

        layer_info->input.size = batch * height * width * depth * channels;

        /* weight */
        layer_info->weight.buffer  = NULL;
        layer_info->weight.height   = conf_layer_info->weight.height;
        layer_info->weight.width    = conf_layer_info->weight.width;
        layer_info->weight.depth    = conf_layer_info->weight.depth;
        layer_info->weight.in_channels  = conf_layer_info->input.channels;
        layer_info->weight.out_channels = conf_layer_info->output.channels;
        layer_info->weight.dtype    = conf_layer_info->weight.dtype;

        height = UAI_GET_VALUE_BEGIN_ONE(layer_info->weight.height);
        width  = UAI_GET_VALUE_BEGIN_ONE(layer_info->weight.width);
        depth  = UAI_GET_VALUE_BEGIN_ONE(layer_info->weight.depth);
        channels = UAI_GET_VALUE_BEGIN_ONE(layer_info->weight.in_channels);
        channels *= UAI_GET_VALUE_BEGIN_ONE(layer_info->weight.out_channels);

        if ((layer_info->layer_type == UAI_LAYER_FCONN) || (layer_info->layer_type == UAI_LAYER_CONV)) {
            layer_info->weight.size  = height * width * depth * channels;
        } else {
            layer_info->weight.size = 0;
        }

        /* output */
        layer_info->output.buffer   = NULL;
        layer_info->output.height   = conf_layer_info->output.height;
        layer_info->output.width    = conf_layer_info->output.width;
        layer_info->output.depth    = conf_layer_info->output.depth;
        layer_info->output.channels = conf_layer_info->output.channels;
        layer_info->output.dtype    = conf_layer_info->output.dtype;
        layer_info->output.shift    = conf_layer_info->out_shift;

        batch  = UAI_GET_VALUE_BEGIN_ONE(conf_layer_info->output.batch);
        height = UAI_GET_VALUE_BEGIN_ONE(conf_layer_info->output.height);
        width  = UAI_GET_VALUE_BEGIN_ONE(conf_layer_info->output.width);
        depth  = UAI_GET_VALUE_BEGIN_ONE(conf_layer_info->output.depth);
        channels = UAI_GET_VALUE_BEGIN_ONE(conf_layer_info->output.channels);
        layer_info->output.size = batch * height * width * depth * channels;

        /* bias */
        layer_info->bias.shift   = conf_layer_info->bias_shift;
        switch(layer_info->layer_type) {
            case UAI_LAYER_FCONN:
                layer_info->bias.size = layer_info->weight.height;
                break;
            case UAI_LAYER_CONV:
                layer_info->bias.size = layer_info->output.channels;
                break;
            default:
                layer_info->bias.size = 0;
                break;
        }

        layer_info->pad_type = conf_layer_info->pad_type;
        layer_info->strides[0]  = conf_layer_info->strides[0];
        layer_info->strides[1]  = conf_layer_info->strides[1];

        switch(layer_info->layer_type) {
            case UAI_LAYER_FCONN:  /* fully connected */
                layer_info->fconn_type = conf_layer_info->fconn_type;
                if((layer_info->fconn_type > UAI_FCONN_NULL) &&
                    (layer_info->fconn_type < UAI_FCONN_END)) {
                    layer_info->fconn_func = fconn_func_local[layer_info->fconn_type - 1];
                }
                break;
            case UAI_LAYER_CONV:  /* conv */
                layer_info->conv_type = conf_layer_info->conv_type;
                if((layer_info->conv_type > UAI_CONV_NULL) &&
                    (layer_info->conv_type < UAI_CONV_END)) {
                    layer_info->conv_func = conv_func_local[layer_info->conv_type - 1];
                }
                break;
            case UAI_LAYER_POOL:  /* pooling */
                layer_info->pool_type = conf_layer_info->pool_type;
                if((layer_info->pool_type > UAI_POOL_NULL) &&
                    (layer_info->pool_type < UAI_POOL_END)) {
                    layer_info->pool_func = pool_func_local[layer_info->pool_type - 1];
                }
                break;
            default:
                break;
        }

        /* activation */
        layer_info->act_type = conf_layer_info->act_type;
        if((layer_info->act_type > UAI_ACT_NULL) &&
            (layer_info->act_type < UAI_ACT_END)) {
            layer_info->act_func = act_func_local[layer_info->act_type - 1];
        }

        /* softmax */
        layer_info->softmax_type = conf_layer_info->softmax_type;
        if((layer_info->softmax_type > UAI_SOFTMAX_NULL) &&
            (layer_info->softmax_type < UAI_SOFTMAX_END)) {
            layer_info->softmax_func = softmax_func_local[layer_info->softmax_type - 1];
        }

        /* pre load model data*/
        if(g_model->preload == true) {
            offset += uai_cnn_preload_model_data(layer_info, offset, model_data_src, type);
        }
    }

    g_model->model_data_src = (char *)uai_malloc(strlen(model_data_src) + 1);
    memset(g_model->model_data_src, 0, strlen(model_data_src) + 1);
    memcpy(g_model->model_data_src, model_data_src, strlen(model_data_src));

    uai_cnn_info_show();
    UAI_LOGI("cnn init success\n");

    return UAI_SUCCESS;
}

int32_t uai_cnn_deinit(void)
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
                UAI_NON_ZERO_FREE(g_model->layer_info[idx].weight.buffer);
                UAI_NON_ZERO_FREE(g_model->layer_info[idx].bias.buffer);
            }
        }
    }

    UAI_NON_ZERO_FREE(g_model->model_data_src);
    UAI_NON_ZERO_FREE(g_model->layer_info);
    UAI_NON_ZERO_FREE(g_model);

    return 0;
}

static int8_t *uai_cnn_layer_run(uai_cnn_layer_t *layer_info, int8_t* in_buffer, int8_t *out_buffer, uint32_t *offset)
{
    int32_t ret            = 0;
    int8_t *scratch_buffer = NULL;

    if(out_buffer == NULL) {
        UAI_NON_ZERO_ZALLOC(scratch_buffer, layer_info->output.size);
        if(scratch_buffer == NULL) {
            UAI_LOGE("uai_malloc error");
            return NULL;
        }
        out_buffer = scratch_buffer;
    }

    if(g_model->preload == false) {
        UAI_NON_ZERO_ZALLOC(layer_info->weight.buffer, layer_info->weight.size);
        UAI_NON_ZERO_ZALLOC(layer_info->bias.buffer,   layer_info->bias.size);

        ret = uai_load_model_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, *offset, g_model->model_data_src);
        if(ret != UAI_SUCCESS) {
            UAI_LOGE("load model data error");
            UAI_NON_ZERO_FREE(layer_info->weight.buffer);
            UAI_NON_ZERO_FREE(layer_info->bias.buffer);
            UAI_NON_ZERO_FREE(scratch_buffer);

            return NULL;
        }
        *offset += layer_info->weight.size + layer_info->bias.size;
    }

    layer_info->input.buffer  = in_buffer;
    layer_info->output.buffer = out_buffer;

    switch(layer_info->layer_type) {
        case UAI_LAYER_FCONN:  /* fully connected */
            if(layer_info->fconn_func != NULL) {
                UAI_LOGD("run fully connected");
                layer_info->fconn_func(&layer_info->input, &layer_info->weight, &layer_info->bias, &layer_info->output);
            }
            break;
        case UAI_LAYER_CONV:  /* conv */
            if(layer_info->conv_func != NULL) {
                UAI_LOGD("run convolution function");
                layer_info->conv_func(&layer_info->input, &layer_info->weight, &layer_info->strides[0], layer_info->pad_type, &layer_info->bias, &layer_info->output);
            }
            break;
        case UAI_LAYER_POOL:  /* pooling */
            if(layer_info->pool_func != NULL) {
                UAI_LOGD("run pool function");
                layer_info->pool_func(&layer_info->input, &layer_info->weight, &layer_info->strides[0], layer_info->pad_type, &layer_info->bias, &layer_info->output);
            }
            break;
        default:
            break;
    }

    /* activation */
    if(layer_info->act_func != NULL) {
        UAI_LOGD("run activation function");
        layer_info->act_func(layer_info->output.buffer, layer_info->output.size, layer_info->act_width);
    }

    /* softmax */
    if(layer_info->softmax_func != NULL) {
        UAI_LOGD("run softmax function");
        layer_info->softmax_func(layer_info->output.buffer, g_model->result_num, layer_info->output.buffer);
    }

    if(g_model->preload == false) {
        UAI_NON_ZERO_FREE(layer_info->weight.buffer);
        UAI_NON_ZERO_FREE(layer_info->bias.buffer);
    }

    return scratch_buffer;
}

int32_t uai_cnn_run(int8_t* in_data, int8_t* out_data)
{
    int16_t idx        = 0;
    int8_t *in_buffer  = NULL;
    int8_t *out_buffer = NULL;
    uint32_t offset    = 0;

    if(g_model == NULL) {
        UAI_LOGE("cnn not init");
        return UAI_FAIL;
    }

    if((in_data == NULL) || (out_data == NULL)) {
        UAI_LOGE("param null");
        return UAI_FAIL;
    }

    /* input layer */
    UAI_LOGD("\r\nrun input layer");
    out_buffer = uai_cnn_layer_run(&g_model->layer_info[0], in_data, NULL, &offset);
    if(out_buffer == NULL) {
        UAI_LOGE("cnn inpurt layer run fail");
        return UAI_FAIL;
    }

    /* hidden layer */
    in_buffer = out_buffer;
    for(idx = 1; idx < g_model->layer_num - 1; idx ++) {
        UAI_LOGD("\r\nrun hidden layer %d", idx);
        out_buffer = uai_cnn_layer_run(&g_model->layer_info[idx], in_buffer, NULL, &offset);
        if(out_buffer == NULL) {
            UAI_LOGE("cnn layer run fail");
            return UAI_FAIL;
        }

        UAI_NON_ZERO_FREE(in_buffer);
        in_buffer = out_buffer;
    }

    /* output layer */
    UAI_LOGD("\r\nrun output layer");
    uai_cnn_layer_run(&g_model->layer_info[idx], in_buffer, out_data, &offset);
    UAI_NON_ZERO_FREE(in_buffer);

    UAI_LOGD("cnn layer run finish\n");
    return UAI_SUCCESS;
}

