#ifndef __HAL_BOOT_FLASH_H__
#define __HAL_BOOT_FLASH_H__

int hal_boot_flash_write(unsigned int dst_addr, const void *data, unsigned int size);
int hal_boot_flash_read(unsigned int address, void *pdata, unsigned int size);
int hal_boot_flash_erase(unsigned int address, unsigned int size);

#endif
