#include "sys/backtrace.h"
#include "sys/xip.h"
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
#include "netstack.h"
#include "netstack_def.h"
#include "uart/drv_uart.h"
void Cli_Task( void *args );

void ssvradio_init_task(void *pdata)
{
    WIFI_INIT();
#ifdef TCPIPSTACK_EN
    netstack_init(NULL);
#endif
    OS_TaskDelete(NULL);
}

void temperature_compensation_task(void *pdata)
{
    printf("temperature compensation task\n");
    OS_MsDelay(1*1000);
    while(1)
    {
        OS_MsDelay(3*1000);
        do_temerature_compensation();
    }
    OS_TaskDelete(NULL);
}

/**********************************************************/
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

    load_rf_table_from_flash();
    write_reg_rf_table();

	FS_init();

    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
    OS_TaskCreate(temperature_compensation_task, "rf temperature compensation", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    OS_TaskCreate(Cli_Task, "cli", 1024, NULL, 1, NULL);

    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
	printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
	print_callstack();
	while(1);
}

