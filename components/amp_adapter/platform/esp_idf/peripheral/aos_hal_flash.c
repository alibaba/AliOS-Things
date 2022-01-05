/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "aos_hal_flash.h"
#include "esp_partition.h"

static hal_logic_partition_t default_partitions[HAL_PARTITION_MAX];
int32_t hal_partition_to_esp_partition_type(hal_partition_t in_partition, esp_partition_type_t *type, esp_partition_subtype_t *subtype)
{
   int ret = 0;

    switch (in_partition) {
    case HAL_PARTITION_PARAMETER_1:
        *type = ESP_PARTITION_TYPE_DATA;
        *subtype = 0x91;
        break;
    case HAL_PARTITION_PARAMETER_2:
        *type = ESP_PARTITION_TYPE_DATA;
        *subtype = 0x83;
        break;
    case HAL_PARTITION_PARAMETER_3:
        *type = ESP_PARTITION_TYPE_DATA;
        *subtype = 0x92;
        break;
    case HAL_PARTITION_APPLICATION:
        *type = ESP_PARTITION_TYPE_APP;
        *subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
        break;
    case HAL_PARTITION_OTA_TEMP:
        *type = ESP_PARTITION_TYPE_APP;
        *subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
        break;
    case HAL_PARTITION_LITTLEFS:
        *type = ESP_PARTITION_TYPE_DATA;
        *subtype = ESP_PARTITION_SUBTYPE_DATA_FAT;
        break;

    default:
        ret = -1;
        break;
   }
   return ret;
}

int32_t aos_hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t **partition)
{
    int ret;
    const esp_partition_t *esp_partition;
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;
    hal_logic_partition_t *tPartition;

    tPartition = &default_partitions[in_partition];
    ret = hal_partition_to_esp_partition_type(in_partition, &type, &subtype);
    if (ret < 0) {
        return ret;
    }
    esp_partition = esp_partition_find_first(type, subtype, NULL);
    tPartition->partition_start_addr = esp_partition->address;
    tPartition->partition_length = esp_partition->size;
    *partition  = tPartition;
    return 0;
}

int32_t aos_hal_flash_read(hal_partition_t id, uint32_t *offset, void *buffer, uint32_t buffer_len)
{
    int ret;
    const esp_partition_t *esp_partition;
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;

    ret = hal_partition_to_esp_partition_type(id, &type, &subtype);
    if (ret < 0) {
        return ret;
    }
    esp_partition = esp_partition_find_first(type, subtype, NULL);
    return esp_partition_read(esp_partition, *offset, buffer, buffer_len);
}

int32_t aos_hal_flash_write(hal_partition_t id, uint32_t *offset, const void *buffer, uint32_t buffer_len)
{
    int ret;
    const esp_partition_t *esp_partition;
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;

    ret = hal_partition_to_esp_partition_type(id, &type, &subtype);
    if (ret < 0) {
        return ret;
    }
    esp_partition = esp_partition_find_first(type, subtype, NULL);
    return esp_partition_write(esp_partition, *offset, buffer, buffer_len);
}

int32_t aos_hal_flash_erase(hal_partition_t id, uint32_t offset, uint32_t size)
{
    int ret;
    const esp_partition_t  *esp_partition;
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;
    /*
        esp32 needs 4K align.
    */
    if (size % 0x1000) {
        size = ((size + 0x1000 - 1) / 0x1000) * 0x1000;
    }
    ret = hal_partition_to_esp_partition_type(id, &type, &subtype);
    if (ret < 0) {
        return ret;
    }
    esp_partition = esp_partition_find_first(type, subtype, NULL);
    return esp_partition_erase_range(esp_partition, offset, size);
}
