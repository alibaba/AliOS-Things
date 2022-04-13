/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"


int uvoice_event_init(void);
int audio_device_init(void);

int uvoice_init(void)
{
	if (uvoice_event_init()) {
		M_LOGE("init uvoice event failed !\n");
		return -1;
	}

	if (audio_device_init()) {
		M_LOGE("init audio device failed !\n");
		return -1;
	}

	M_LOGR("uvoice release v%d.%d @%s, %s\n",
		UVOICE_RELEASE_VERSION_MAIN,
		UVOICE_RELEASE_VERSION_SUB, __TIME__, __DATE__);
	return 0;
}

int uvoice_free(void)
{
	return 0;
}

