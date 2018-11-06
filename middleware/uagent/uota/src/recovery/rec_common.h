/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _REC_COMMON_H_
#define _REC_COMMON_H_

#include "rec_define.h"

void rec_read_status(PatchStatus *status);
void rec_write_ota_status(ota_reboot_info_t *ota_info);
void rec_startup_success();

#endif
