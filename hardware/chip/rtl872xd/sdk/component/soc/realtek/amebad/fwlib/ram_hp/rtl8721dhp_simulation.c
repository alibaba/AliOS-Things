/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

VOID app_simulation_task(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;
	
	//u8  *argv[6];
	static u32 test_done = 0;

	do{
		/* this is an example, you can change to yourself code if needed */
		if (test_done == 0) {
			simulation_stage_set(SIMULATION_KM0_CPUID, BIT_KM0_SIMULATION_START);
#if 0
			//add simulation code here
			argv[1] = "0";
			argv[2] = "1";
			argv[3] = "0";
			argv[4] = "0";
			argv[5] = "1";
			AudioTestVera(argv);
#endif
			test_done = 1;
			simulation_stage_set(SIMULATION_KM0_CPUID, BIT_KM0_SIMULATION_END);
		} else {
			vTaskDelay(1000);
		}		
	} while (1);
}


VOID app_simulation_init(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;
	
	if (pdTRUE != xTaskCreate(app_simulation_task, (const char * const)"simulation_task", 1024, 
		NULL, tskIDLE_PRIORITY + 5 , NULL))
	{
		DiagPrintf("Create simulation_task Err!!\n");
	}
}
