/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include "uai_dnn.h"

static uai_dnn_model_t *g_model = NULL;
static uai_fconn_func       fconn_func_local[UAI_FCONN_END - 1] = {uai_fc, uai_fc_opt};
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
    UAI_LOGI("trained data preload: %s", (g_model->preload)?"yes":"no");
    UAI_LOGI("trained data source : %s\r\n", g_model->trained_data_src);

    for(idx = 0; idx < g_model->layer_num; idx++) {
        uai_dnn_layer_t *layer_info = &g_model->layer_info[idx];
        UAI_LOGI("layer %d info:", idx);
        UAI_LOGI("in  dim: %u", layer_info->input.rows);
        UAI_LOGI("out dim: %u", layer_info->weight.rows);
        /* input */
        UAI_LOGI("input size: %u", layer_info->input.size);

        /* weight */
        UAI_LOGI("weight rows: %u", layer_info->weight.rows);
        UAI_LOGI("weight size: %u", layer_info->weight.size);
        /* bias */
        UAI_LOGI("bias size: %u", layer_info->bias.size);
        UAI_LOGI("bias shift: %d", layer_info->bias.shift);
        /* output */
        UAI_LOGI("out  shift: %d", layer_info->output.shift);

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

static int uai_dnn_preload_trained_data(uai_dnn_layer_t *layer_info, int32_t offset, char *trained_data_src, uai_src_type_e type)
{
    if(type != UAI_SRC_TYPE_MEM) {
        layer_info->weight.buffer = uai_malloc(layer_info->weight.size);
        layer_info->bias.buffer   = uai_malloc(layer_info->bias.size);
        uai_load_trained_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, trained_data_src);
    } else {
        uai_load_trained_data((int8_t *)&layer_info->weight.buffer, (int8_t *)&layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, offset, trained_data_src);
    }

    return layer_info->weight.size + layer_info->bias.size;
}

int32_t uai_dnn_init(uai_dnn_config_t *model_config, char *trained_data_src)
{
    int32_t idx     = 0;
    uint32_t offset = 0;
    uai_src_type_e type;

    if((model_config == NULL) || (trained_data_src == NULL) || (model_config->layer_info == NULL)) {
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

    type = uai_src_type_parse(trained_data_src);
    if (type == UAI_SRC_TYPE_MEM) {
        g_model->preload = true;
    }

    for(idx = 0; idx < g_model->layer_num; idx ++) {
        uai_dnn_layer_t *layer_info = &g_model->layer_info[idx];

        /* input */
        layer_info->input.buffer  = NULL;
        layer_info->input.columns = 1;
        layer_info->input.rows    = model_config->layer_info[idx].in_dim;
        layer_info->input.size    = model_config->layer_info[idx].in_dim;
        layer_info->input.width   = 1;

        /* weight */
        layer_info->weight.buffer  = NULL;
        layer_info->weight.columns = model_config->layer_info[idx].in_dim;
        layer_info->weight.rows    = model_config->layer_info[idx].out_dim;
        layer_info->weight.size    = layer_info->weight.columns * layer_info->weight.rows;
        layer_info->weight.width   = 1;

        /* bias */
        layer_info->bias.buffer  = NULL;
        layer_info->bias.width   = 1;
        layer_info->bias.shift   = model_config->layer_info[idx].bias_shift;
        layer_info->bias.size    = model_config->layer_info[idx].out_dim;

        /* output */
        layer_info->output.width   = 1;
        layer_info->output.shift   = model_config->layer_info[idx].out_shift;

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

        /* pooling */
        /* TODO: This version does not support */
        
        /* loss function */
        /* TODO: This version does not support */

        /* softmax */
        layer_info->softmax_type = model_config->layer_info[idx].softmax_type;
        if((layer_info->softmax_type > UAI_SOFTMAX_NULL) &&
            (layer_info->softmax_type < UAI_SOFTMAX_END)) {
            layer_info->softmax_func = softmax_func_local[layer_info->softmax_type - 1];
        }

        /* pre load trained data*/
        if(g_model->preload == true) {
            offset += uai_dnn_preload_trained_data(layer_info, offset, trained_data_src, type);
        }
    }

    g_model->trained_data_src = (char *)uai_malloc(strlen(trained_data_src) + 1);
    memset(g_model->trained_data_src, 0, strlen(trained_data_src) + 1);
    memcpy(g_model->trained_data_src, trained_data_src, strlen(trained_data_src));

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
        type = uai_src_type_parse(g_model->trained_data_src);
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
    uai_free(g_model->trained_data_src);
    uai_free(g_model->layer_info);
    uai_free(g_model);

    g_model = NULL;

    return 0;
}

static int8_t *uai_dnn_layer_run(uai_dnn_layer_t *layer_info, int8_t* in_buffer, int8_t *out_buffer, uint32_t *offset)
{
    int32_t ret            = 0;
    int32_t size           = 0;
    int8_t *scratch_buffer = NULL;
    int16_t *vec_buffer    = NULL;

    if(out_buffer == NULL) {
        size = layer_info->input.size*sizeof(int16_t) + layer_info->weight.rows;
        scratch_buffer = uai_malloc(size);
        if(scratch_buffer == NULL) {
            UAI_LOGE("uai_malloc error");
            return NULL;
        }
        memset(scratch_buffer, 0, size);
        vec_buffer = (int16_t*)(scratch_buffer + layer_info->weight.rows);
        out_buffer = scratch_buffer;
    } else {
        size = layer_info->input.size*sizeof(int16_t);
        scratch_buffer = uai_malloc(size);
        if(scratch_buffer == NULL) {
            UAI_LOGE("uai_malloc error");
            return NULL;
        }

        memset(scratch_buffer, 0, size);
        vec_buffer = (int16_t*)(scratch_buffer);
    }

    if(g_model->preload == false) {
        layer_info->weight.buffer = uai_malloc(layer_info->weight.size);
        layer_info->bias.buffer = uai_malloc(layer_info->bias.size);
        ret = uai_load_trained_data(layer_info->weight.buffer, layer_info->bias.buffer, layer_info->weight.size, layer_info->bias.size, *offset, g_model->trained_data_src);
        if(ret != UAI_SUCCESS) {
            UAI_LOGE("load trained data error");
            uai_free(layer_info->weight.buffer);
            uai_free(layer_info->bias.buffer);
            uai_free(scratch_buffer);

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
        UAI_LOGD("run fully connected");
        layer_info->fconn_func(&layer_info->input, &layer_info->weight, &layer_info->bias, &layer_info->output, vec_buffer);
    }

    /* activation */
    if(layer_info->act_func != NULL) {
        UAI_LOGD("run activation function");
        layer_info->act_func(layer_info->output.buffer, layer_info->weight.rows, layer_info->act_width);
    }

    /* pooling */
    /* TODO: This version does not support */


    /* loss function */
    /* TODO: This version does not support */

    /* softmax */
    if(layer_info->softmax_func != NULL) {
        UAI_LOGD("run softmax function");
        layer_info->softmax_func(layer_info->output.buffer, g_model->result_num, layer_info->output.buffer);
    }

    if(g_model->preload == false) {
        uai_free(layer_info->weight.buffer);
        uai_free(layer_info->bias.buffer);
        layer_info->weight.buffer = NULL;
        layer_info->bias.buffer   = NULL;
    }

    return scratch_buffer;
}

int32_t uai_dnn_run(int8_t* in_data, int8_t* out_data)
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
    UAI_LOGD("run input layer");
    out_buffer = uai_dnn_layer_run(&g_model->layer_info[0], in_data, NULL, &offset);
    if(out_buffer == NULL) {
        UAI_LOGE("dnn inpurt layer run fail");
        return UAI_FAIL;
    }

    /* hidden layer */
    in_buffer = out_buffer;
    for(idx = 1; idx < g_model->layer_num - 1; idx ++) {
        UAI_LOGD("run hidden layer %d", idx);
        out_buffer = uai_dnn_layer_run(&g_model->layer_info[idx], in_buffer, NULL, &offset);
        if(out_buffer == NULL) {
            UAI_LOGE("dnn layer run fail");
            return UAI_FAIL;
        }

        uai_free(in_buffer);
        in_buffer = out_buffer;
    }

    /* output layer */
    UAI_LOGD("run output layer");
    out_buffer = uai_dnn_layer_run(&g_model->layer_info[idx], in_buffer, out_data, &offset);
    if(out_buffer == NULL) {
        UAI_LOGE("dnn output layer run fail");
        uai_free(in_buffer);
        return UAI_FAIL;
    }
    uai_free(out_buffer);
    uai_free(in_buffer);

    UAI_LOGD("dnn layer run finish\n");
    return UAI_SUCCESS;
}
