/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include <get_beacon_frame/example_get_beacon_frame.h>
#include <platform/platform_stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

typedef int (*get_beacon_frame_func_ptr)(BEACON_INFO_T beacon_frame);
extern get_beacon_frame_func_ptr get_beacon_frame_callback;

int get_beacon_frame_func(BEACON_INFO_T beacon_frame)
{
	printf("\nbeacon frame_ctl   = %02x %02x\n",beacon_frame.frame_ctl[0],beacon_frame.frame_ctl[1]);
	printf("\nbeacon duration_id = %02x %02x\n",beacon_frame.duration_id[0],beacon_frame.duration_id[1]);
	printf("\nbeacon addr1       = %02x:%02x:%02x:%02x:%02x:%02x\n",\
		beacon_frame.addr1[0],beacon_frame.addr1[1],beacon_frame.addr1[2],beacon_frame.addr1[3],beacon_frame.addr1[4],beacon_frame.addr1[5]);
	printf("\nbeacon addr2       = %02x:%02x:%02x:%02x:%02x:%02x\n",\
		beacon_frame.addr2[0],beacon_frame.addr2[1],beacon_frame.addr2[2],beacon_frame.addr2[3],beacon_frame.addr2[4],beacon_frame.addr2[5]);
	printf("\nbeacon addr3       = %02x:%02x:%02x:%02x:%02x:%02x\n",\
		beacon_frame.addr3[0],beacon_frame.addr3[1],beacon_frame.addr3[2],beacon_frame.addr3[3],beacon_frame.addr3[4],beacon_frame.addr3[5]);
	printf("\nbeacon seq_ctl     = %02x %02x\n",beacon_frame.seq_ctl[0],beacon_frame.seq_ctl[1]);
	printf("\nbeacon timestamp   = %02x %02x %02x %02x %02x %02x %02x %02x\n",\
		beacon_frame.timestamp[0],beacon_frame.timestamp[1],beacon_frame.timestamp[2],beacon_frame.timestamp[3],beacon_frame.timestamp[4],beacon_frame.timestamp[5],beacon_frame.timestamp[6],beacon_frame.timestamp[7]);

	return 0;
}

void get_beacon_frame_thread(void *param)
{
    vTaskDelay(10000);//a rough time for waiting wifi connected
    //Register callback function until wifi connected
	get_beacon_frame_callback = get_beacon_frame_func;
	vTaskDelete(NULL);
	return;
}

void example_get_beacon_frame(void)
{
    if(xTaskCreate(get_beacon_frame_thread, ((const char*)"get_beacon_frame_thread"), 256, NULL, tskIDLE_PRIORITY , NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(get_beacon_frame_thread) failed", __FUNCTION__);
    return;
}

