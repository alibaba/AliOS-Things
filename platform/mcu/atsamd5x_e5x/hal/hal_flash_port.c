/**
 * \file
 *
 * \brief Flash HAL related functionality implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#include "aos/hal/flash.h"

#include <atmel_start.h>

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length) {
        return -1;
    }

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
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;
    flash_read(&FLASH_0, start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if (off_set + size > partition_info->partition_length) {
        return -1;
    }

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

