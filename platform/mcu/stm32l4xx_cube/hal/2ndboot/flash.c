#include "hal_boot_flash.h"
#include "hal_boot_process.h"
#include "bootloader.h"

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;
    uint32_t current_bank = 0;
    hal_partition_t new_pno = pno;
    if(pno >= hal_partitions_amount)
    {
        return NULL;
    }

    current_bank = hal_get_boot_bank();
    if(current_bank == 0x02) // userbin2
    {
        if(pno == HAL_PARTITION_APPLICATION) {
            new_pno = HAL_PARTITION_OTA_TEMP;
        } else if(pno == HAL_PARTITION_OTA_TEMP){
            new_pno = HAL_PARTITION_APPLICATION;
        }
    }
    logic_partition = (hal_logic_partition_t *)&hal_partitions[ new_pno ];
    return logic_partition;
}

int32_t hal_flash_init(void)
{
    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;
    unsigned int           addr;
    unsigned int           start_addr;
    unsigned int           end_addr;

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    start_addr = ROUND_DOWN(partition->partition_start_addr + off_set, SECTOR_SIZE);
    end_addr = ROUND_DOWN((partition->partition_start_addr + off_set + size - 1), SECTOR_SIZE);

    ret = hal_boot_flash_erase(start_addr, end_addr - start_addr);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (out_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    if (0 != hal_boot_flash_read(partition->partition_start_addr + *off_set, out_buf, in_buf_len)) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (in_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    if (0 != hal_boot_flash_write(partition->partition_start_addr + *off_set, in_buf, in_buf_len)) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}


