/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <lcsfc.h>
#include <dh_type.h>

#include "aos/hal/flash.h"

extern DH_S32 DH_MDK_LCSFC_Erase(DH_U32 u32StartAddr, DH_U32 u32Length);
extern DH_S32 DH_MDK_LCSFC_WriteFlash(DH_U32 u32Addr, DH_U32 u32Cnt, DH_U8 *pu8Buf);
extern DH_S32 DH_MDK_LCSFC_Init(DH_U32 u32FMaxSize, DH_U32 u32Baudrate);

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr = LCSFC_KV_ADDR + off_set;
    int32_t ret = DH_MDK_LCSFC_Erase(addr, size);
    if (ret) {
        return -1;
    }

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t addr = LCSFC_KV_ADDR + *off_set;
    DH_MDK_LCSFC_WriteFlash(addr, in_buf_len, in_buf);
    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t addr = LCSFC_KV_ADDR + *off_set;
    memcpy(out_buf, addr, out_buf_len);
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

void hal_flash_init(void)
{
    DH_MDK_LCSFC_Init(1024*1024, 8);
}
