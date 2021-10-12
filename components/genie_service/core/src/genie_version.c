/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "genie_service.h"

#ifndef GENIE_SDK_VERSION
#define GENIE_SDK_VERSION "1.3.2"
#endif

uint32_t genie_version_appver_get(void)
{
    return PROJECT_SW_VERSION;
}

int genie_version_appver_string_get(char *version, int len)
{
    return snprintf(version, len, "%d.%d.%d", (PROJECT_SW_VERSION >> 16 & 0xFF), (PROJECT_SW_VERSION >> 8 & 0xFF), PROJECT_SW_VERSION & 0xFF);
}

const char *genie_version_sysver_get(void)
{
    return GENIE_SDK_VERSION;
}
