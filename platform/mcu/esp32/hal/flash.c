#include <string.h>
#include <malloc.h>
#include "aos/hal/flash.h"
#include "esp_spi_flash.h"
#include "esp_ota_ops.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

static esp_partition_t  operate_partition;
extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition = NULL;
    if((pno == HAL_PARTITION_OTA_TEMP) || (pno == HAL_PARTITION_APPLICATION)) {
        esp_partition_t find_partition;
        const esp_partition_t *esp_current_partition = esp_ota_get_boot_partition();
        if (esp_current_partition == NULL) {
            printf("Got null partition.");
            return -1;
        }
        if (esp_current_partition->type != ESP_PARTITION_TYPE_APP) {
            printf("Err part type");
            return -1;
        }
        /*choose which OTA image should we write to*/
        switch (esp_current_partition->subtype) {
            case ESP_PARTITION_SUBTYPE_APP_FACTORY:
                find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
                break;
            case  ESP_PARTITION_SUBTYPE_APP_OTA_0:
                find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
                break;
            case ESP_PARTITION_SUBTYPE_APP_OTA_1:
                find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
                break;
           default:
                find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
                break;
        }
        find_partition.type = ESP_PARTITION_TYPE_APP;
        const esp_partition_t *esp_partition = esp_partition_find_first(find_partition.type, find_partition.subtype, NULL);
        if(esp_partition != NULL) {
            logic_partition = (hal_logic_partition_t *)&hal_partitions[HAL_PARTITION_OTA_TEMP];
            if(logic_partition->partition_start_addr != esp_partition->address) {
                logic_partition = (hal_logic_partition_t *)&hal_partitions[HAL_PARTITION_APPLICATION];
                if(logic_partition->partition_start_addr != esp_partition->address) {
                    logic_partition = NULL;
                }
            }
            /*printf("ota_begin part type 0x%x sub:0x%x addr:0x%x size:0x%x label:%s encry:0x%x\n", partition->type,
                partition->subtype, partition->address, partition->size, partition->label, partition->encrypted);*/
            memset(&operate_partition, 0, sizeof(esp_partition_t));
            memcpy(&operate_partition, esp_partition, sizeof(esp_partition_t));
        }
        else {
            return -1;
        }
    }
    else {
        logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];
    }

    if (logic_partition != NULL) {
        memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));
    }

    return 0;
}

int hal_reboot_bank()
{
    int err = 0;
    err = esp_ota_set_boot_partition(&operate_partition);
    if (err != 0) {
        printf("set part failed! err=0x%x", err);
    }
    return err;
}


int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;
    ret = spi_flash_write(start_addr, buf, buf_size);

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int32_t ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    ret = spi_flash_read(start_addr, buf, buf_size);
    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;

    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SEC_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SEC_SIZE) {
        ret = spi_flash_erase_range(addr, SPI_FLASH_SEC_SIZE);
        if (ret != 0)
            return ret;
    }

    return 0;
}

uint32_t hal_flash_erase_sector_size()
{
    return SPI_FLASH_SEC_SIZE;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


