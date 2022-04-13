

#ifndef DRV_NORFLASH_H__
#define DRV_NORFLASH_H__

void nor_init();
int nor_flash_info_get(unsigned int chipno);
int nor_flash_write(unsigned int chipno, unsigned int *poff, const void *buf, unsigned int buf_size);
int nor_flash_read(unsigned int chipno, unsigned int *poff, void *buf, unsigned int buf_size);
int nor_flash_erase(unsigned int chipno, unsigned int off_set, unsigned int size);


#endif
