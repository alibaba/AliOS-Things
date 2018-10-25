#include "sys/backtrace.h"
#include "sys/flash.h"
#if defined(XIP_MODE)
#include "sys/xip.h"
#endif
#include "fsal.h"
#include "osal.h"
#include "wifinetstack.h"
#include "idmanage/pbuf.h"
#include "security/drv_security.h"
#include "phy/drv_phy.h"
#include "soc_defs.h"
#include "ieee80211_mgmt.h"
#include "ieee80211_mac.h"
#include "sta_func.h"
#include "wifi_api.h"
#include "netstack_def.h"
#include "netstack.h"
#include "uart/drv_uart.h"
#include "rf/rf_api.h"

void Cli_Task( void *args );

/**********************************************************/
SSV_FS fs_handle = NULL;

void temperature_compensation_task(void *pdata)
{
    printf("%s\n", __func__);
    OS_MsDelay(1*1000);
    //load_rf_table_from_flash();
    //write_reg_rf_table();
    while(1)
    {
        OS_MsDelay(3*1000);
        do_temerature_compensation();
    }
    OS_TaskDelete(NULL);
}

void ssvradio_init_task(void *pdata)
{
    printf("%s\n", __func__);
    WIFI_INIT();
    netstack_init(NULL);
    DUT_wifi_start(DUT_STA);
    
    OS_TaskDelete(NULL);
}

extern void drv_uart_init(void);
void APP_Init(void)
{
#ifdef XIP_MODE
	xip_init();
	xip_enter();
#endif
	drv_uart_init();
    drv_uart_set_fifo(UART_INT_RXFIFO_TRGLVL_1, 0x0);
	drv_uart_set_format(115200, UART_WORD_LEN_8, UART_STOP_BIT_1, UART_PARITY_DISABLE);

	OS_Init();
	OS_StatInit();
	OS_MemInit();
	OS_PsramInit();

        load_rf_table_from_flash();
        write_reg_rf_table();

	fs_handle = FS_init();
	if(fs_handle)
	{
		FS_remove_prevota(fs_handle);
	}

    OS_TaskCreate(Cli_Task, "cli", 512, NULL, OS_TASK_PRIO3, NULL);

    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 256, NULL, OS_TASK_PRIO3, NULL);

    //this task run in low priority
    OS_TaskCreate(temperature_compensation_task, "rf temperature compensation", 256, NULL, OS_TASK_PRIO1, NULL);

    printf("[%s][%d] string\n", __func__, __LINE__);

    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
    printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
    print_callstack();
    while(1);
}

