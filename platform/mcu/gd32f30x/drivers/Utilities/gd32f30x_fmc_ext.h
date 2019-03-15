#ifndef _GD32F30X_FMC_EXT_H_
#define _GD32F30X_FMC_EXT_H_

#include "gd32f30x.h"

void xxx_fmc_region_read(uint32_t start_addr, uint8_t *data, uint32_t size);
ErrStatus xxx_fmc_region_write(uint32_t start_addr, uint8_t *data, uint32_t size);
ErrStatus xxx_fmc_region_erase(uint32_t start_addr, uint32_t size);

#endif
