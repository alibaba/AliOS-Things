#include "rec_flash.h"
#include "hal_boot_flash.h"
#include "hal_boot_process.h"

#define FLASH_SECTOR_SIZE    0x1000

/* Logic partition on flash devices */
extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

void rec_flash_init(void)
{
    ;
}

/* offset means physical address */
void rec_flash_erase(unsigned long offset)
{
    offset &= ~(FLASH_SECTOR_SIZE - 1);
    hal_boot_flash_erase(offset, FLASH_SECTOR_SIZE);
}

/* offset means physical address */
void rec_flash_read_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    hal_boot_flash_read((unsigned int)offset, buffer, (unsigned int)len);
}

/* offset means physical address */
void rec_flash_write_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    hal_boot_flash_write((unsigned int)offset, buffer, (unsigned int)len);
}

hal_logic_partition_t *rec_flash_get_info(hal_partition_t pno)
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

