#include <hal/soc/soc.h>
#include <atmel_start.h>

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

extern const hal_logic_partition_t hal_partitions[];
extern int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes);
extern int FLASH_bank1_enabled(void);

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(pno);
    start_addr = partition_info->partition_start_addr + *poff;
    if (buf_size != flash_write(&FLASH_0, start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

	partition_info = hal_flash_get_info(pno);

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    flash_read(&FLASH_0, start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(pno);
    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), flash_get_page_size(&FLASH_0));
    flash_erase(&FLASH_0, start_addr, size / flash_get_page_size(&FLASH_0));
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


