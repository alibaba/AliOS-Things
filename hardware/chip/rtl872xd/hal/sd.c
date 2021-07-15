#include <errno.h>
#include <stdint.h>
#include <aos/hal/sd.h>


int32_t hal_sd_init(sd_dev_t *sd)
{
    return -1;
}

int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
{
    return -1;
}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{
    return -1;
}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    return -1;
}

int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
    return -1;
}

int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
    return -1;
}

int32_t hal_sd_finalize(sd_dev_t *sd)
{
    return -1;
}
