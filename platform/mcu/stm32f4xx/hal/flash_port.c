
#include <string.h>
#include "stm32l4xx.h"
#include "stm32l4xx_hal_flash.h"
#include "aos/hal/flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

extern const hal_logic_partition_t hal_partitions[];
extern int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes);

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );
    start_addr = p_partition_info->partition_start_addr + *poff;
    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );

    if(poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length)
        return -1;
    start_addr = p_partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( pno, p_partition_info );
    if(size + off_set > p_partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);

    FLASH_unlock_erase(start_addr, size);
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


