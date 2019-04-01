
#include "sys/dbgcon.h"
#include "sys/backtrace.h"
#include "osal.h"

extern void Cli_Task( void *args );

typedef struct _stParam
{
	char *argv[10];
	int	argc;
} stParam;

stParam param;

void APP_Init(void)
{
#if defined(FLASH_CTL_v1)
    REG32(0xc0000124) = 0x1;	//force to go back XIP!!
#endif
    
    dbgcon_init();
    OS_Init();
    OS_MemInit();
    
    OS_TaskCreate(Cli_Task, "cli", 512, NULL, 1, NULL);
    
    printf("[%s][%d] string\n", __func__, __LINE__);
    
    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
    printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
    OS_SysInfo();
    print_callstack();
    while(1);
}

