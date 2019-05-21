/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) Beken Corp 2011-2020
 *
 ****************************************************************************************
 */
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "app.h"
#include "ate_app.h"

void entry_set_world_flag(void)
{
    *(volatile uint32_t *)0x00400000 = 1;
}

void entry_main(void)
{  
    ate_app_init();
	
    #if CFG_SUPPORT_BOOTLOADER
    entry_set_world_flag();
    #endif
    
    /* step 1: driver layer initialization*/
    driver_init();

    /* step 2: function layer initialization*/
    func_init();  

    /* step 3: startup application layer*/
    if(get_ate_mode_state())
    {
	    ate_start();
    }
    else
    {
	    app_start();
    }
         
    /* step 4: start freertos scheduler*/
#if (!CFG_SUPPORT_RTT)
    vTaskStartScheduler();
#endif
}
// eof

