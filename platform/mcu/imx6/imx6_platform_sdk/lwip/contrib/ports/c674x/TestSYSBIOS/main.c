/*
 *  ======== main.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_syscfg_OMAPL137.h>
#include "pinmux_EVM.h"
#include "cpu_c674.h"
#include "i2c.h"
#include "test.h"

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

/*
 *  ======== main ========
 */
Void main()
{ 
	Task_Handle task;
    Error_Block eb;

    System_printf("enter main()\n");

    /* Low Level Initializations */
	CPU_Clocks(24000000, 25, 2); // 24->600->300 MHz
	I2C_Init();

   	/* Create test thread */
    Error_init(&eb);
    task = Task_create((Task_FuncPtr) test, NULL, &eb);
    if (task == NULL)
    {
        System_printf("Task_create() failed!\n");
        BIOS_exit(0);
    }

    BIOS_start();     /* enable interrupts and start SYS/BIOS */
}
