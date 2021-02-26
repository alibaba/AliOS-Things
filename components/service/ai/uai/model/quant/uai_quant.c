#include "stdint.h"
#include "uai_quant.h"

void uai_quant_normalize_uint8(uint8_t *data, int size, uint32_t scale, uint16_t shift)
{
    int i = 0;
    uint8_t min = 255;
    uint8_t max = 0;
    uint8_t off = 0;
    uint32_t scale_ori = scale >> shift;

    for(i = 0; i < size; i++) {
        if(data[i] < min) {
            min = data[i];
        }
        if(data[i] > max) {
            max = data[i];
        }
    }
    off = max - min;

    for(i = 0; i < size; i++) {
        uint32_t temp = (uint32_t)data[i];
		uint64_t new_temp = 0;
		new_temp = (((uint64_t)scale * ((temp - (uint32_t)min)) / (uint32_t)off));
		temp = new_temp >> shift;

        if(temp > (uint32_t)scale_ori) {
            temp = (uint32_t)scale_ori;
        }
        data[i] = (uint8_t)temp;
    }
}