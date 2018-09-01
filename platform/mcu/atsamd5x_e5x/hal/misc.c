/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <hpl_reset.h>

void hal_reboot(void)
{
    printf("reboot!\n");
	_reset_mcu();
}

