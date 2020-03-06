/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "uai_odla.h"

odla_computation odla_CreateComputation(odla_helper_func helper_func, uai_quant_scale **quant_scale) {
    odla_computation p = (odla_computation)malloc(sizeof(struct _odla_Computation));
    p->helper_func = helper_func;
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    p->mem_list    = uai_odla_init_memlist();
#endif
#ifdef UAI_PRELOAD_SCALE_ONCE
    uai_modelP->quant_scale = quant_scale;
#endif
    return p;
}

odla_session odla_CreateSession(odla_computation comp) {
    odla_session s = (odla_session)malloc(sizeof(struct _odla_Session));
    s->comp = comp;
    s->comp->layer_id = 0;
    return s;
}

odla_value odla_AddArgument(odla_computation comp, const char* name,
                            odla_element_type type, odla_dims dims) {
    comp->layer_id = 0;
    return (odla_value)comp->input->buffer;
}

odla_value odla_AddConstant(odla_computation comp, odla_element_type type,
                            odla_dims dims, const void* ptr) {
    return (odla_value)ptr;
}

void odla_SetOutput(odla_computation comp, const char* name,
                    const odla_value val) {

}

/* only support INT8 now */
void odla_BindArgument(odla_session session, const char* name,
                       odla_element_type type, odla_dims dims,
                       const void* ptr) {
    session->comp->input = uai_odla_tensor_transofrm((const odla_value)ptr, dims, type, CHANNELS_LAST);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(session->comp->mem_list, session->comp->input);
#endif
}

void odla_BindOutput(odla_session session, const char* name,
                     odla_element_type type, odla_dims dims, void* ptr) {
    session->comp->output = uai_odla_tensor_transofrm(ptr, dims, type, CHANNELS_LAST);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(session->comp->mem_list, session->comp->output);
#endif
}

odla_status odla_Execute(odla_session session) {
    session->comp->helper_func(session->comp);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_free_memlist(session->comp->mem_list);
#endif
    session->comp->mem_list = NULL;
    return 0;
}

odla_value odla_Add(odla_computation comp, const odla_value lhs,
                    const odla_value rhs, odla_dims dims_lhs,
                    odla_dims dims_rhs)
{
    int ret = 0;
    uai_quant_scale *bias_scale = NULL;
    uai_quant_scale *act_scale  = NULL;
    uai_tensor_s *input1 = uai_odla_tensor_transofrm(lhs, dims_lhs, INT8, CHANNELS_LAST);
    uai_tensor_s *input2 = uai_odla_tensor_transofrm(rhs, dims_rhs, INT8, CHANNELS_LAST);
    uai_tensor_s *output = uai_zalloc(sizeof(uai_tensor_s));
    odla_value add_value = NULL;

    UAI_VALID_PTR_CHECK_NULL(input1);
    UAI_VALID_PTR_CHECK_NULL(input2);
    UAI_VALID_PTR_CHECK_NULL(output);

#ifdef UAI_PRELOAD_SCALE_ONCE
    bias_scale = comp->quant_scale[UAI_BIAS_SCALE][comp->layer_id];
    act_scale  = comp->quant_scale[UAI_ACT_SCALE][comp->layer_id];
#else
    uai_load_model_scale(comp->layer_id, UAI_BIAS_SCALE, &bias_scale);
    uai_load_model_scale(comp->layer_id, UAI_ACT_SCALE, &act_scale);
#endif
    ret = uai_bias_add(input1, input2, bias_scale, act_scale, output);

    add_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    uai_free(input1);
    uai_free(input2);
    uai_free(output);

#ifndef UAI_PRELOAD_SCALE_ONCE
    uai_free(bias_scale->scale);
    uai_free(bias_scale);
    uai_free(act_scale->scale);
    uai_free(act_scale);
#endif

    return add_value;
}

odla_value odla_Pad(odla_computation comp, odla_element_type type,
                    const odla_value input, odla_dims input_dims,
                    const int* padding_front, const int* padding_back,
                    odla_dims output_dims)
{
    int ret = 0;
    uai_tensor_s *input_pad = uai_odla_tensor_transofrm(input, input_dims, INT8, type);
    uai_tensor_s *output    = uai_odla_tensor_transofrm(NULL, output_dims, INT8, type);
    odla_value pad_value    = NULL;

    output->buffer = (odla_value)uai_malloc(output->size);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    ret = uai_pad(input_pad, padding_front, padding_back, output);
    pad_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;

    uai_free(input_pad);
    uai_free(output);

    return pad_value;
}

odla_value odla_Relu(odla_computation comp, odla_element_type type,
                     odla_dims dims, odla_value input) {
    uai_relu(input, uai_get_total_elements(dims), 0);
    return input;
}

odla_value odla_Softmax(odla_computation comp, odla_element_type type,
                        odla_dims dims, odla_value input) {
    uai_softmax(input, uai_get_total_elements(dims), input);
    return input;
}

odla_value odla_ReduceMean(odla_computation comp, odla_element_type type,
                           odla_dims dims, odla_value input, size_t num_of_axes,
                           const unsigned* axes, odla_dims output_dims)
{
    return NULL;
}

odla_value odla_BatchNormalization(odla_computation comp,
                                   odla_element_type type, odla_dims input_dims,
                                   odla_layout input_layout, odla_value input,
                                   odla_value mean, odla_value var,
                                   float epsilon, odla_value scale,
                                   odla_value offset, float scalar_scale,
                                   float scalar_offset)
{
    return NULL;
}

odla_value odla_Convolution(odla_computation comp, odla_element_type type,
                            odla_dims input_dims, odla_layout input_layout,
                            odla_value input, odla_dims kernel_dims,
                            odla_layout kernel_layout, odla_value kernel,
                            const unsigned* strides, const unsigned* dilations,
                            const unsigned* paddings_front,
                            const unsigned* paddings_back,
                            odla_dims output_dims)
{
    int ret = 0;
    uai_quant_scale *kernel_scale    = NULL;
    uai_tensor_s *input_conv  = uai_odla_tensor_transofrm(input, input_dims, type, input_layout);
    uai_tensor_s *kernel_conv = uai_odla_tensor_transofrm(kernel, kernel_dims, type, kernel_layout);
    uai_tensor_s *output      = uai_odla_tensor_transofrm(NULL, output_dims, type, input_layout);
    odla_value conv_value     = NULL;

    output->buffer = (odla_value)uai_malloc(output->size * sizeof(int32_t));
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

#ifdef UAI_PRELOAD_SCALE_ONCE
    kernel_scale = comp->quant_scale[UAI_KERNEL_SCALE][comp->layer_id];
#else
    uai_load_model_scale(comp->layer_id, UAI_KERNEL_SCALE, &kernel_scale);
#endif
    ret = uai_conv(input_conv, kernel_conv, strides, paddings_front, paddings_back, kernel_scale, output);
    conv_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;

    uai_free(input_conv);
    uai_free(kernel_conv);
    uai_free(output);
#ifndef UAI_PRELOAD_SCALE_ONCE
    uai_free(kernel_scale->scale);
    uai_free(kernel_scale);
#endif

    comp->layer_id ++;

    return conv_value;
}

odla_value odla_MaxPooling(odla_computation comp, odla_element_type type,
                           odla_dims input_dims, odla_layout layout,
                           odla_value input, const unsigned* window_dims,
                           const unsigned* strides,
                           const unsigned* paddings_front,
                           const unsigned* paddings_back,
                           odla_dims output_dims)
{
    _odla_dims dims;
    uai_tensor_s *input1 = uai_odla_tensor_transofrm(input, input_dims, type, layout);
    uai_tensor_s *weight = NULL;
    uai_tensor_s *output = uai_odla_tensor_transofrm(NULL, output_dims, type, CHANNELS_LAST);
    odla_value pool_value = NULL;

    dims.size = 4;
    dims.dims[0] = 1;
    dims.dims[1] = window_dims[0];
    dims.dims[2] = window_dims[1];
    dims.dims[3] = 1;
    weight = uai_odla_tensor_transofrm(NULL, &dims, type, CHANNELS_LAST);

    output->buffer = (odla_value)uai_malloc(output->size);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    uai_pool_max(input1, weight, strides, paddings_front, paddings_back, output);

    pool_value = output->buffer;

    comp->layer_id ++;

    return pool_value;
}

odla_value odla_Gemm(odla_computation comp, odla_element_type type,
                     odla_dims lhs_dims, odla_value lhs, bool transpose_lhs,
                     odla_dims rhs_dims, odla_value rhs, bool transpose_rhs,
                     odla_dims output_dims)
{
    int ret = 0;
    uai_quant_scale *kernel_scale    = NULL;
    uai_tensor_s *input  = uai_odla_tensor_transofrm(lhs, lhs_dims, type, CHANNELS_LAST);
    uai_tensor_s *weight = uai_odla_tensor_transofrm(rhs, rhs_dims, type, CHANNELS_LAST);
    uai_tensor_s *output = uai_odla_tensor_transofrm(NULL, output_dims, type, CHANNELS_LAST);
    odla_value fconn_value = NULL;

    output->buffer = (odla_value)uai_malloc(output->size);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

#ifdef UAI_PRELOAD_SCALE_ONE_TIME
    kernel_scale = comp->quant_scale[UAI_KERNEL_SCALE][comp->layer_id];
#else
    uai_load_model_scale(comp->layer_id, UAI_KERNEL_SCALE, &kernel_scale);
#endif

    ret = uai_fconn(input, weight, kernel_scale, output);
    fconn_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;

    uai_free(input);
    uai_free(weight);
    uai_free(output);
#ifndef UAI_PRELOAD_SCALE_ONCE
    uai_free(kernel_scale->scale);
    uai_free(kernel_scale);
#endif

    comp->layer_id ++;

    return fconn_value;
}

void odla_Free(odla_computation comp, odla_value value) {
    uai_free(value);
}

void odla_DestroySession(odla_session sess)
{
    uai_free(sess);
}

void odla_DestroyComputation(odla_computation comp)
{
    uai_free(comp);
};

