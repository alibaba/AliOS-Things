/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "uai_odla.h"

odla_computation odla_CreateComputation(odla_helper_func helper_func) {
    odla_computation p = (odla_computation)malloc(sizeof(struct _odla_Computation));
    p->helper_func = helper_func;
    p->model_quant_scale = uai_load_model_scale();

    return p;
}

odla_session odla_CreateSession(odla_computation comp) {
    odla_session s    = (odla_session)malloc(sizeof(struct _odla_Session));
    s->comp           = comp;
    s->comp->layer_id = 0;
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    s->comp->mem_list = uai_odla_init_memlist();
#endif
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
    memcpy(comp->output->buffer, val, comp->output->size);
}

/* only support INT8 now */
void odla_BindArgument(odla_session session, const char* name,
                       odla_element_type type, odla_dims dims,
                       const void* ptr) {
    session->comp->input = uai_malloc(sizeof(uai_tensor_s));
    session->comp->input = uai_odla_tensor_transofrm(session->comp->input, (const odla_value)ptr, dims, type, CHANNELS_LAST);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(session->comp->mem_list, session->comp->input);
#endif
}

void odla_BindOutput(odla_session session, const char* name,
                     odla_element_type type, odla_dims dims, void* ptr) {
    session->comp->output = uai_malloc(sizeof(uai_tensor_s));
    session->comp->output = uai_odla_tensor_transofrm(session->comp->output, ptr, dims, type, CHANNELS_LAST);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(session->comp->mem_list, session->comp->output);
#endif
}

odla_status odla_Execute(odla_session session) {
    session->comp->helper_func(session->comp);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_free_memlist(session->comp->mem_list);
#endif
    return 0;
}

odla_value odla_Add(odla_computation comp, const odla_value lhs,
                    const odla_value rhs, odla_dims dims_lhs,
                    odla_dims dims_rhs)
{
    int ret = 0;

    UAI_TENSOR_PREFIX uai_tensor_s *input1 = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *input2 = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *output = NULL;

    odla_value add_value = NULL;

    if(!input1) input1 = uai_malloc(sizeof(uai_tensor_s));
    if(!input2) input2 = uai_malloc(sizeof(uai_tensor_s));
    if(!output) output = uai_malloc(sizeof(uai_tensor_s));

    input1 = uai_odla_tensor_transofrm(input1, lhs, dims_lhs, INT8, CHANNELS_LAST);
    input2 = uai_odla_tensor_transofrm(input2, rhs, dims_rhs, INT8, CHANNELS_LAST);

    UAI_VALID_PTR_CHECK_NULL(input1);
    UAI_VALID_PTR_CHECK_NULL(input2);
    UAI_VALID_PTR_CHECK_NULL(output);

    ret = uai_add(input1, input2, output);

    add_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

#ifndef UAI_TENSOR_STATIC_MEM
    uai_free(input1);
    uai_free(input2);
    uai_free(output);
#endif

    return add_value;
}

odla_value odla_Pad(odla_computation comp, odla_element_type type,
                    const odla_value input, odla_dims input_dims,
                    const int* padding_front, const int* padding_back,
                    odla_dims output_dims)
{
    #if 0
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
    #endif
}

odla_value odla_Relu(odla_computation comp, odla_element_type type,
                     odla_dims dims, odla_value input) {
    uai_relu(input, uai_get_total_elements(dims), 0);
    return input;
}

odla_value odla_Reshape(odla_computation comp, odla_element_type type,
                        odla_dims input_dims, odla_value input, odla_dims output_dims) {
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
                            odla_value bias_input,
                            odla_dims bias_dims,
                            odla_layout bias_layout,
                            odla_dims output_dims)
{
    int ret = 0;
    UAI_TENSOR_PREFIX uai_tensor_s *input_conv  = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *kernel_conv = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *output      = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *bias        = NULL;
    odla_value conv_value     = NULL;

    UAI_ALLOC_TENSOR(input_conv);
    UAI_ALLOC_TENSOR(kernel_conv);
    UAI_ALLOC_TENSOR(output);
    UAI_ALLOC_TENSOR(bias);

    input_conv = uai_odla_tensor_transofrm(input_conv, input, input_dims, type, input_layout);
    kernel_conv =  uai_odla_tensor_transofrm(kernel_conv, kernel, kernel_dims, type, kernel_layout);
    output =  uai_odla_tensor_transofrm(output, NULL, output_dims, type, input_layout);
    bias =  uai_odla_tensor_transofrm(bias, bias_input, bias_dims, type, bias_layout);

    output->buffer = (odla_value)uai_malloc(output->size);

#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    ret = uai_conv(input_conv, kernel_conv, strides, paddings_front, paddings_back, bias,
                   KERNEL_SCALE(comp->layer_id), BIAS_SCALE(comp->layer_id), ACT_SCALE(comp->layer_id), SCALE_SHIFT(comp->layer_id),
                   output);
    conv_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;

#ifndef UAI_TENSOR_STATIC_MEM
    uai_free(input_conv);
    uai_free(kernel_conv);
    uai_free(output);
    uai_free(bias);
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
    UAI_TENSOR_PREFIX uai_tensor_s *input1 = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *weight = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *output = NULL;
    odla_value pool_value = NULL;

    UAI_ALLOC_TENSOR(input1);
    UAI_ALLOC_TENSOR(weight);
    UAI_ALLOC_TENSOR(output);

    input1 = uai_odla_tensor_transofrm(input1, input, input_dims, type, layout);
    output = uai_odla_tensor_transofrm(output, NULL, output_dims, type, CHANNELS_LAST);

    dims.size = 4;
    dims.dims[0] = 1;
    dims.dims[1] = window_dims[0];
    dims.dims[2] = window_dims[1];
    dims.dims[3] = 1;
    weight = uai_odla_tensor_transofrm(weight, NULL, &dims, type, CHANNELS_LAST);

    output->buffer = (odla_value)uai_malloc(output->size);
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    uai_pool_max(input1, weight, strides, paddings_front, paddings_back, output);

    pool_value = output->buffer;

#ifndef UAI_TENSOR_STATIC_MEM
    uai_free(input1);
    uai_free(weight);
    uai_free(output);
#endif

    return pool_value;
}

odla_value odla_Gemm(odla_computation comp, odla_element_type type,
                     odla_dims lhs_dims, odla_value lhs, bool transpose_lhs,
                     odla_dims rhs_dims, odla_value rhs, bool transpose_rhs,
                     odla_value bias_input, odla_dims bias_dims, odla_layout bias_layout,
                     odla_dims output_dims)
{
    int ret = 0;
    UAI_TENSOR_PREFIX uai_tensor_s *input  = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *weight = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *bias   = NULL;
    UAI_TENSOR_PREFIX uai_tensor_s *output = NULL;
    odla_value fconn_value = NULL;

    UAI_ALLOC_TENSOR(input);
    UAI_ALLOC_TENSOR(weight);
    UAI_ALLOC_TENSOR(bias);
    UAI_ALLOC_TENSOR(output);

    input  = uai_odla_tensor_transofrm(input, lhs, lhs_dims, type, CHANNELS_LAST);
    weight = uai_odla_tensor_transofrm(weight, rhs, rhs_dims, type, CHANNELS_LAST);
    bias   = uai_odla_tensor_transofrm(bias, bias_input, bias_dims, type, bias_layout);
    output = uai_odla_tensor_transofrm(output, NULL, output_dims, type, CHANNELS_LAST);

    output->buffer = (odla_value)uai_malloc(output->size);

#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_odla_add_memlist(comp->mem_list, output->buffer);
#endif

    ret = uai_fconn(input, weight, bias,
                    KERNEL_SCALE(comp->layer_id), BIAS_SCALE(comp->layer_id),
                    ACT_SCALE(comp->layer_id), SCALE_SHIFT(comp->layer_id),
                    output);
    fconn_value = (ret == UAI_SUCCESS) ? output->buffer : NULL;

#ifndef UAI_TENSOR_STATIC_MEM
    uai_free(input);
    uai_free(weight);
    uai_free(bias);
    uai_free(output);
#endif
    comp->layer_id ++;

    return fconn_value;
}

void odla_ResetValue(odla_computation comp, odla_value value) {
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

