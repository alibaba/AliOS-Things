/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "k_proc.h"
#include "k_api.h"
#include "aos/cli.h"

/**
 * Reserved physical memory region,
 * defined in link script
 */
extern char *_phy_region_start;
extern char *_phy_region_len;

int uspace_init(void)
{
    int ret;

    printf("proc init, phy region start 0x%x, len 0x%x\r\n", (int)&_phy_region_start, (int)&_phy_region_len);
    ret = k_proc_init((void*)&_phy_region_start, (size_t)&_phy_region_len);
    if (ret) {
        printf("proc init failed\r\n, ret %d\r\n", ret);
        return -1;
    }

    return 0;
}

