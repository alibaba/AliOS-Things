/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_COMMON_H
#define JSE_COMMON_H

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "jse_engine.h"
#include "jse_list.h"
#include "jse_task.h"
#include "jse_utils.h"
#include "jse_port.h"

#ifdef JSE_IDE_DEBUG
#include "jse_service.h"
#endif

int32_t bone_console_get_log_flag(void);

void bone_console_log_enable(void);

void bone_console_log_disable(void);

#endif /* JSE_COMMON_H */
