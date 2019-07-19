#include <string.h>

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}


static int _convert_partition_to_nor(nor_dev_t *dev, hal_logic_partition_t *partition_info)
{
    int ret = hal_nor_init(dev);

    if (ret != 0) {
        return -1;
    }

    dev->base_addr = partition_info->partition_start_addr;
    dev->config.chip_size = partition_info->partition_length;

    return 0; 
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nor_dev_t dev;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );

    ret = _convert_partition_to_nor(&dev, p_partition_info);
    if (ret != 0) {
        return -1;
    }

    ret = hal_nor_write(&dev, poff, (uint8_t *)buf, buf_size);

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nor_dev_t dev;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );

    if(poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length)
        return -1;

    ret = _convert_partition_to_nor(&dev, p_partition_info);
    if (ret != 0) {
        return -1;
    }

    ret = hal_nor_read(&dev, poff, buf, buf_size);
    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    nor_dev_t dev;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );
    if(size + off_set > p_partition_info->partition_length)
        return -1;

    ret = _convert_partition_to_nor(&dev, p_partition_info);
    if (ret != 0) {
        return -1;
    }

    start_addr = ROUND_DOWN(off_set, dev.config.block_size);
    end_addr = ROUND_DOWN((off_set + size - 1), dev.config.block_size);

    for (addr = start_addr; addr <= end_addr; addr += dev.config.block_size) {
        ret = hal_nor_erase_block(&dev, &addr, 1);
        if (ret != 0)
            return ret;
    }

    return 0;

}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


