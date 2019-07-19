// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "diskio.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_log.h"
#include "diskio_rawflash.h"
#include "aos/hal/flash.h"


static const char* TAG = "diskio_rawflash";

/*cooperate to DEV_FLASH1 and DEV_FLASH2*/
const hal_partition_t ff_raw_handles[FF_VOLUMES] = {0,0,0,HAL_PARTITION_CUSTOM_1,HAL_PARTITION_CUSTOM_2};


DSTATUS ff_raw_initialize (BYTE pdrv)
{
    return 0;
}

DSTATUS ff_raw_status (BYTE pdrv)
{
    return 0;
}


DRESULT ff_raw_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    ESP_LOGV(TAG, "ff_raw_read - pdrv=%i, sector=%i, count=%in", (unsigned int)pdrv, (unsigned int)sector, (unsigned int)count);
	hal_partition_t pno = ff_raw_handles[pdrv];
	uint32_t off = sector * SPI_FLASH_SEC_SIZE;
	
	esp_err_t err = hal_flash_read(pno, &off, buff, count * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "hal_flash_read failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

/*
DRESULT ff_raw_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	printf("22ff_raw_write in pdrv:%d\r\n",pdrv);
	
    return RES_ERROR;
}*/



DRESULT ff_raw_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    ESP_LOGV(TAG, "ff_raw_write - pdrv=%i, sector=%i, count=%in", (unsigned int)pdrv, (unsigned int)sector, (unsigned int)count);
	hal_partition_t pno = ff_raw_handles[pdrv];
	uint32_t off = sector * SPI_FLASH_SEC_SIZE;
	
	esp_err_t err =  hal_flash_erase(pno, off,count * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "hal_flash_erase failed (0x%x)", err);
        return RES_ERROR;
    }
		
	err = hal_flash_write(pno, &off, buff, count * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "hal_flash_write failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}



DRESULT ff_raw_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    hal_partition_t pno = ff_raw_handles[pdrv];
	hal_logic_partition_t hal_flash;

    hal_flash_info_get(pno, &hal_flash);

    ESP_LOGV(TAG, "ff_raw_ioctl: cmd=%in", cmd);
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *) buff) = (hal_flash.partition_length) / SPI_FLASH_SEC_SIZE;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *) buff) = SPI_FLASH_SEC_SIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
    }
	
    return RES_ERROR;
}

/*
esp_err_t ff_diskio_register_raw_partition(BYTE pdrv, const esp_partition_t* part_handle)
{
    if (pdrv >= FF_VOLUMES) {
        return ESP_ERR_INVALID_ARG;
    }
    static const ff_diskio_impl_t raw_impl = {
        .init = &ff_raw_initialize,
        .status = &ff_raw_status,
        .read = &ff_raw_read,
        .write = &ff_raw_write,
        .ioctl = &ff_raw_ioctl
    };
    ff_diskio_register(pdrv, &raw_impl);
    ff_raw_handles[pdrv] = part_handle;
    return ESP_OK;

}


BYTE ff_diskio_get_pdrv_raw(const esp_partition_t* part_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (part_handle == ff_raw_handles[i]) {
            return i;
        }
    }
    return 0xff;
}*/
