
//===- Sinian Compiler Generated File -------------------------------------===//
#ifndef HAAS_UAI_INTERFACE_H
#define HAAS_UAI_INTERFACE_H
#include "stdio.h"
#include "uai_odla.h"
#include "uai_load.h"
#include "uai_quant.h"
#include "uai_odla_demo_data.h"

#define DEC_IMG_WIDTH  32
#define DEC_IMG_HEIGHT 48
#define DEC_IMG_CHANNEL 1


static odla_computation Comp;
static odla_session Sess;

static void alios_b0_helper(void *arg) {
    odla_computation comp = (odla_computation)arg;

    odla_value input = odla_AddArgument(comp, "input", INT8,
                                (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, 1}});
    odla_value conv_1_kernel_ = odla_AddConstant(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, DEC_IMG_CHANNEL, 2 * DEC_IMG_CHANNEL}}, conv_1_kernel);
    odla_value conv_1_bias_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {2}}, conv_1_bias);
    odla_value conv_2_kernel_ = odla_AddConstant(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, 2 * DEC_IMG_CHANNEL, 8 * DEC_IMG_CHANNEL }}, conv_2_kernel);
    odla_value conv_2_bias_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {8}}, conv_2_bias);
    odla_value conv_3_kernel_ = odla_AddConstant(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, 8, 16}}, conv_3_kernel);
    odla_value conv_3_bias_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {16}}, conv_3_bias);
    odla_value fc_1_kernel_ = odla_AddConstant(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1536, 48}}, fc_1_kernel);
    odla_value fc_1_bias_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {48}}, fc_1_bias);
    odla_value fc_2_kernel_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {48, 21}}, fc_2_kernel);
    odla_value fc_2_bias_ =
        odla_AddConstant(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {21}}, fc_2_bias);

    /* layer 1 */
    odla_value conv_1_convolution = odla_Convolution(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, 1}},
        CHANNELS_LAST, input, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, 1, 2}},
        SIO, conv_1_kernel_, (const unsigned[]){1, 1},
        (const unsigned[]){1, 1}, (const unsigned[]){2, 2}, (const unsigned[]){2, 2},
        conv_1_bias_, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {2}}, SIO,
        (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, 2}});

    odla_value activation_1_Relu = odla_Relu(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, 2}}, conv_1_convolution);

    odla_value max_pooling2d_1_MaxPool = odla_MaxPooling(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, 2}},
        CHANNELS_LAST, activation_1_Relu, (const unsigned[]){2, 2},
        (const unsigned[]){2, 2}, (const unsigned[]){0, 0},
        (const unsigned[]){0, 0}, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 24, 16, 2}});

    /* layer 2 */
    odla_value conv_2_convolution = odla_Convolution(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 24, 16, 2}},
        CHANNELS_LAST, max_pooling2d_1_MaxPool,
        (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, 2, 8}}, SIO, conv_2_kernel_,
        (const unsigned[]){1, 1}, (const unsigned[]){1, 1}, (const unsigned[]){2, 2}, (const unsigned[]){2, 2},
        conv_2_bias_, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {8}}, SIO,
        (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 24, 16, 8}});

    odla_value activation_2_Relu = odla_Relu(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 24, 16, 8}}, conv_2_convolution);

    odla_value max_pooling2d_2_MaxPool = odla_MaxPooling(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 24, 16, 8}},
        CHANNELS_LAST, activation_2_Relu, (const unsigned[]){2, 2},
        (const unsigned[]){2, 2}, (const unsigned[]){0, 0},
        (const unsigned[]){0, 0}, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 12, 8, 8}});

    /* layer 3 */
    odla_value conv_3_convolution = odla_Convolution(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 12, 8, 8}},
        CHANNELS_LAST, max_pooling2d_2_MaxPool,
        (odla_dims)&(const _odla_dims) {.size = 4, .dims = {5, 5, 8, 16}}, SIO, conv_3_kernel_,
        (const unsigned[]){1, 1}, (const unsigned[]){1, 1}, (const unsigned[]){2, 2}, (const unsigned[]){2, 2},
        conv_3_bias_, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {16}}, SIO,
        (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 12, 8, 16}});

    odla_value activation_3_Relu = odla_Relu(
        comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 12, 8, 16}}, conv_3_convolution);

    odla_value flatten_1_Reshape =
        odla_Reshape(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims = {1, 12, 8, 16}},
                    activation_3_Relu, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 1536}});

    /* layer 4 */
    odla_value fc_1_MatMul =
        odla_Gemm(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 1536}}, flatten_1_Reshape,
                    false, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1536, 48}}, fc_1_kernel_, false,
                    fc_1_bias_, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {48}}, SIO,
                    (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 48}});

    odla_value activation_4_Relu =
        odla_Relu(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 48}}, fc_1_MatMul);

    /* layer 5 */
    odla_value fc_2_MatMul =
        odla_Gemm(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 48}}, activation_4_Relu,
                    false, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {48, 21}}, fc_2_kernel_, false,
                    fc_2_bias_, (odla_dims)&(const _odla_dims) {.size = 1, .dims = {21}}, SIO,
                    (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 21}});
    odla_value output_Softmax =
        odla_Softmax(comp, INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims = {1, 21}}, fc_2_MatMul);

    odla_SetOutput(comp, "output_Softmax", output_Softmax);
    return;
}

void alios_b0(const int8_t* input, int8_t* out_output_Softmax) {
    if (Comp == NULL) {
        Comp = odla_CreateComputation(alios_b0_helper);
    }
    if (Sess == NULL) {
        Sess = odla_CreateSession(Comp);
    }

    odla_BindArgument(Sess, "input", INT8, (odla_dims)&(const _odla_dims) {.size = 4, .dims={1, DEC_IMG_HEIGHT, DEC_IMG_WIDTH, DEC_IMG_CHANNEL}}, input);
    odla_BindOutput(Sess, "output_Softmax", INT8, (odla_dims)&(const _odla_dims) {.size = 2, .dims={1, 21}}, out_output_Softmax);
    odla_Execute(Sess);
}
#endif
