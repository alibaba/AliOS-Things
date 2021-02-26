
#ifndef UAI_QUANT_H
#define UAI_QUANT_H
#include "stdint.h"
#include "arm_math.h"
#include "arm_nnfunctions.h"

#ifdef __cplusplus
extern "C" {
#endif
void uai_quant_normalize_uint8(uint8_t *data, int size, uint32_t scale, uint16_t shift);

#ifdef __cplusplus
}
#endif
static inline int8_t uai_quant_int8(int32_t value, uint32_t kernel_scale, int8_t bias, uint32_t bias_scale, uint32_t act_scale, int32_t shift)
{
	int64_t value_temp;

	value_temp = ((int64_t)value) * act_scale / kernel_scale + ((int64_t)bias) * act_scale / bias_scale;

	return (int8_t)__SSAT((int32_t)(value_temp), 8);
}
#endif
