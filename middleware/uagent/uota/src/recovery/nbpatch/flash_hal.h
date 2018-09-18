/**
* @author E-mail: ting.guit@alibaba-inc.com
* @version time：2017年11月17日 下午7:48:57
* flash_hal.h
*/
#ifndef FLASH_HAL_H_
#define FLASH_HAL_H_

#include "rec_pub.h"

void patch_flash_init(const char *file);

int patch_flash_write(int par, const unsigned char *buffer, unsigned long address, unsigned long len);

int patch_flash_read(int par, const unsigned char *buffer, unsigned long address, unsigned long len);

int patch_flash_erase(int par, unsigned long address, size_t splict_size);

#endif /* FLASH_HAL_H_ */
