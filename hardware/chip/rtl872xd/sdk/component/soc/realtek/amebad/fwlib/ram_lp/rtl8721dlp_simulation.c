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
	
	//u32 argc = 4;
	//u8  *argv[5];
	static u32 test_done = 0;

	do{
		/* this is an example, you can change to yourself code if needed */
		if (test_done == 0) {
			//u32 temp = 0;
			simulation_stage_set(SIMULATION_KM0_CPUID, BIT_KM0_SIMULATION_START);
			//DCache_Disable();
			//simulation_stage_set(SIMULATION_KM0_CPUID, BIT(29));
			//DCache_Enable();
			//simulation_stage_set(SIMULATION_KM0_CPUID, BIT(30));
			//argv[0] = "aon_timer";
			//argv[1] = "4";
			//pg_sleep_Test(argc, argv);

			//argv[0] = "block";
			//argv[1] = "3";
			//argv[2] = "0";
			//argv[3] = "single";
			//hs_power_onoff_test(argc, argv);

			//while (1) {
			//	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLATFORM_PARA);	/*get KM4 sleep status*/
			//	if(temp & BIT_KM4_SLEEP_STATUS) {
			//		break;
			//	}
			//	DBG_8195A("BIT_KM4_SLEEP_STATUS:%x \n", temp);
			//}
			//ICache_Disable();
			//km4_suspend(KM4_STATUS_CG_BIT);
			//km4_resume();
			//asm volatile ("sev");

			pmu_set_sleep_type(SLEEP_CG);
			pmu_acquire_wakelock(PMU_KM4_RUN);
			pmu_release_wakelock(PMU_OS);

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

