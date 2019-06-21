#include "hal_boot_flash.h"
#include "hal_boot_process.h"
#include "aos/hal/flash.h"
#include "2ndboot.h"

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

int flash_erase(unsigned int offset, unsigned int len)
{
    offset &= ~(SECTOR_SIZE - 1);
    hal_boot_flash_erase(offset, len);
    return 0;
}

int flash_read_data(unsigned int offset, unsigned char *buffer, unsigned int len)
{
    hal_boot_flash_read(offset, buffer, len);
    return 0;
}

int flash_write_data(unsigned int offset, unsigned char *buffer, unsigned int len)
{
    hal_boot_flash_write(offset, buffer, len);
    return 0;
}

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

