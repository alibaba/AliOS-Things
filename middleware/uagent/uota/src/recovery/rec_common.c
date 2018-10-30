/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "hal/soc/flash.h"
#include "hal/soc/wdg.h"
#include "CheckSumUtils.h"
#include "rec_define.h"

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

void rec_write_ota_status(ota_reboot_info_t *ota_info)
{
    PatchStatus status;
    memset(&status, 0, sizeof(PatchStatus));

    status.dst_adr = HAL_PARTITION_APPLICATION;
    status.src_adr = HAL_PARTITION_OTA_TEMP;
    status.siz = 0;
    status.crc = ota_info->ota_crc;
    status.patch_size   = ota_info->ota_len;
    status.splict_size  = ota_info->splict_size;
    status.diff_version = 1;

    memcpy(status.ota_version, ota_get_ota_version(), MAX_VERSION_LEN);
    memcpy(status.app_version, ota_get_app_version(), MAX_VERSION_LEN);

    status.recovery_phase     = REC_PHASE_INIT;
    status.rec_flag_info.flag = REC_RECOVERY_FLAG;
    status.rec_flag_info.num  = 0;

    rec_write_status(&status);
}

void rec_startup_success()
{
    PatchStatus status;

    hal_wdg_finalize(0);

    rec_read_status(&status);

    status.rec_flag_info.flag = REC_NORMAL_FLAG;
    status.rec_flag_info.num  = 0;

    rec_write_status(&status);

}