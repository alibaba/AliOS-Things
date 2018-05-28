 /**
 * @file
 * hello icomm example.
 *
 * @example hello_icomm.c
 */

#include "sys/dbgcon.h"
#include "sys/backtrace.h"
#include "osal.h"

void APP_Init(void)
{
    dbgcon_init();
    OS_Init();
    OS_MemInit();

    printf("Hello iComm\n");
    
    OS_StartScheduler();
}
