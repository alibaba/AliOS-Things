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
#include "rf/rf_api.h"

void Cli_Task( void *args );


/**********************************************************/
SSV_FS fs_handle = NULL;

extern IEEE80211STATUS gwifistatus;
void wifirspcbfunc(WIFI_RSP *msg)
{
    u8 dhcpen;
    u8 mac[6];
    u8 ipv4[4];

    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;
    s8 ret;

    if(msg->wifistatus == 1)
    {
        printf("wifi connected\n");
        get_if_config(&dhcpen, &ipaddr, &submask, &gateway, &dnsserver);
        memcpy(mac, gwifistatus.local_mac, ETH_ALEN);
        printf("mac             - %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("ip addr         - %d.%d.%d.%d\n", ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3]);
        printf("netmask         - %d.%d.%d.%d\n", submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3]);
        printf("default gateway - %d.%d.%d.%d\n", gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);
        printf("DNS server      - %d.%d.%d.%d\n", dnsserver.u8[0], dnsserver.u8[1], dnsserver.u8[2], dnsserver.u8[3]);
    }
}


typedef struct _stParam
{
	char *argv[10];
	int	argc;
} stParam;

stParam param;

void tcptask(void *args)
{
	while(1){
		if(1==get_wifi_status()) {
			break;
		} else {
			OS_MsDelay(500);
		}

	}

	OS_TaskDelete(NULL);
}

void wifi_auto_connect_task(void *pdata)
{  
    if( get_auto_connect_flag() == 1 )
    {
        printf("run wifi_auto_connect_task\n");
        OS_MsDelay(3*1000);
        printf("run wifi_auto_connect_task\n");
        DUT_wifi_start(DUT_STA);
        OS_MsDelay(1*1000);
        do_wifi_auto_connect();
    }
    
    OS_TaskDelete(NULL);
}

void ssvradio_init_task(void *pdata)
{
    WIFI_INIT();
#ifdef TCPIPSTACK_EN
    netstack_init(NULL);
#endif
    OS_TaskDelete(NULL);
}

extern void drv_uart_init(void);
extern struct st_rf_table ssv_rf_table;
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
    if(ssv_rf_table.boot_flag == 0xFF)
    {
        build_default_rf_table(&ssv_rf_table);
        load_rf_table_to_mac(&ssv_rf_table);
        save_rf_table_to_flash();
        dump_rf_table();
    }
    else
    {
        load_rf_table_to_mac(&ssv_rf_table);
        dump_rf_table();
    }

    fs_handle = FS_init();
    if(fs_handle)
    {
        FS_remove_prevota(fs_handle);
    }

#if 1
    OS_TaskCreate(Cli_Task, "cli", 1024, NULL, 1, NULL);
#endif

    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 512, NULL, tskIDLE_PRIORITY + 2, NULL);

    init_global_conf();
    OS_TaskCreate(wifi_auto_connect_task, "wifi_auto_connect", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);

    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
	printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
	print_callstack();
	while(1);
}

