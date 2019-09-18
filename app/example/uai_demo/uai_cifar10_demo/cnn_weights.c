#include "cifar10_weights.h"
#include "stdint.h"

const int8_t cifar10_model_data[] = {CONV1_WT, CONV1_BIAS, CONV2_WT, CONV2_BIAS, CONV3_WT, CONV3_BIAS, IP1_WT, IP1_BIAS};
const uint32_t cifar10_model_data_len = sizeof(cifar10_model_data);