/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <aos/aos.h>


static aos_task_t sensor_demo_task;

static void sensor_demo_work(void *arg)
{
#ifdef SOUND_LOCATION_ENABLE
	acoustic_sl_start();
#endif
}

int application_start(int argc, char **argv)
{
#ifdef DISPLAY_ENABLE
	display_init();
#endif

	aos_task_new_ext(&sensor_demo_task, "sensor_demo_task",
		sensor_demo_work, NULL, 2048, AOS_DEFAULT_APP_PRI);

	printf("%s: sensor demo running\n", __func__);
	aos_loop_run();

	return 0;
}

