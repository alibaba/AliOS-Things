/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "rec_pub.h"
#include "rec_define.h"
#include "rec_common.h"
#include "aos/hal/flash.h"
#include "aos/hal/wdg.h"

void rec_read_status(PatchStatus *status)
{
    unsigned int offset = 0;
    hal_flash_read( HAL_PARTITION_PARAMETER_1, &offset, status, sizeof(PatchStatus));
}

void rec_write_status(PatchStatus *status)
{
    unsigned int offset = 0;
    CRC16_Context contex;
    uint16_t crc;

    CRC16_Init(&contex);
    CRC16_Update(&contex, status, sizeof(PatchStatus) - sizeof(uint16_t));
    CRC16_Final(&contex, &crc);
    status->patch_crc = crc;

    offset = 0x00;
    hal_flash_erase( HAL_PARTITION_PARAMETER_1, offset, sizeof(PatchStatus) );
    hal_flash_write( HAL_PARTITION_PARAMETER_1, &offset, (const void *)status, sizeof(PatchStatus));

    offset = 0x00;
    hal_flash_erase( HAL_PARTITION_RECOVERY_BACK_PARA, offset, sizeof(PatchStatus) );
    hal_flash_write( HAL_PARTITION_RECOVERY_BACK_PARA, &offset, (const void *)status, sizeof(PatchStatus));
}

void rec_startup_success()
{
    PatchStatus status;
    hal_wdg_finalize(0);
    rec_read_status(&status);
    status.upg_flag = REC_NORMAL_FLAG;
    status.boot_count  = 0;

    rec_write_status(&status);

}
