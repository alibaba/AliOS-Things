#include "aos/hal/flash.h"
#include "xz_hal_plat.h"

int xz_flash_erase(hal_partition_t flash_partition, unsigned int offset, unsigned int len)
{
    int ret = 0;
    ret = hal_flash_erase(flash_partition, offset, len);
    return ret;
}

int xz_flash_read_data(hal_partition_t flash_partition, unsigned int offset, unsigned char *buffer, unsigned int len)
{
    int ret = 0;
    unsigned int tmp_offset = offset;
    ret = hal_flash_read(flash_partition, &tmp_offset, buffer, len);
    return ret;
}

int xz_flash_write_data(hal_partition_t flash_partition, unsigned int offset, unsigned char *buffer, unsigned int len)
{
    int ret = 0;
    unsigned int tmp_offset = offset;
    ret = hal_flash_write(flash_partition, &tmp_offset, buffer, len);
    return ret;
}