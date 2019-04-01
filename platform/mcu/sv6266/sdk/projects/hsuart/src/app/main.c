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


/**********************************************************/
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

#if defined(CONFIG_OS_UCOS_II) || defined(CONFIG_OS_UCOS_III)
#define TaskWav_TASK_PRIORITY		(22)
#define TaskBmp_TASK_PRIORITY		(21)
#define TaskInit_TASK_PRIORITY		(20)
#elif defined(CONFIG_OS_FREERTOS)
#define TaskWav_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define TaskBmp_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define TaskInit_TASK_PRIORITY		( tskIDLE_PRIORITY + 5 )
#elif defined(CONFIG_OS_THREADX)
#define TaskWav_TASK_PRIORITY           (22)
#define TaskBmp_TASK_PRIORITY           (21)
#define TaskInit_TASK_PRIORITY          (20)
#else
#  error "No valid OS is defined!"
#endif

#define M_VERIFY_DATA       (0x50505050)
uint32_t bss_test = 0;
uint32_t common_test;
uint32_t data_test = M_VERIFY_DATA;
uint32_t common_array_test[10];
uint32_t bss_array_test[4] = {0, 0, 0, 0};
uint32_t data_array[4] = {123, 456, 789, 8888};
void TaskWav(void *pdata)
{
    int i = 0;
	while (bss_test)
        OS_MsDelay(100);
    while (common_test)
        OS_MsDelay(100);
    while(data_test != M_VERIFY_DATA)
        OS_MsDelay(100);
    for (i=0;i<10;i++) {
        while(common_array_test[i])
            OS_MsDelay(100);
    }
    for (i=0;i<4;i++) {
        while(bss_array_test[i])
            OS_MsDelay(100);
    }
    while(data_array[0] != 123)
        OS_MsDelay(100);
    while(data_array[1] != 456)
        OS_MsDelay(100);
    while(data_array[2] != 789)
        OS_MsDelay(100);
    while(data_array[3] != 8888)
        OS_MsDelay(100);

    while(1) {
        i++;

        //printf("Presenting wave %d...\n", i);
        OS_MsDelay(1000);
    }
}

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
        //get_local_mac(mac, 6);
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


void iperf_test()
{
		OS_MsDelay(1000);
		
            char tmp0[24];
            char tmp1[24];
            char tmp2[24];
            char tmp3[24];
            char tmp4[24]; 	

#if 1
		printf("...........<%s-begin>...........\n",__func__);

        param.argc = 5;
		param.argv[0] = tmp0;
		param.argv[1] = tmp1;
		param.argv[2] = tmp2;
		param.argv[3] = tmp3;
		param.argv[4] = tmp4;


		memset(tmp0,0x0,sizeof(tmp0));
		memset(tmp1,0x0,sizeof(tmp1));
		
		
 #if 1
		strcpy(param.argv[0],"-c");
		strcpy(param.argv[1], "192.168.0.11");
		strcpy(param.argv[2],"-t");
		strcpy(param.argv[3],"300");
		strcpy(param.argv[4],"-u");
#else
        param.argc = 1;
		strcpy(param.argv[0],"-s");
#endif

		iperf(param.argc, param.argv);
#else


		param.argc = 2;
		

		OS_MsDelay(1000);
		printf("<%s>%d\n",__func__,__LINE__);
		memset(tmp0,0x0,sizeof(tmp0));
		memset(tmp1,0x0,sizeof(tmp1));
		param.argv[0] = tmp0;
		param.argv[1] = tmp1;
		//strcpy(param.argv[0],"192.168.0.100,48569");
		strcpy(param.argv[0],"192.168.0.100");
		strcpy(param.argv[1],"5001");


extern		int At_TCPConnect (stParam *param);
extern		int At_TCPSend (stParam *param);


		
 		At_TCPConnect(&param);


		param.argc = 3;
		
		OS_MsDelay(1000);
		printf("<%s>%d\n",__func__,__LINE__);
		memset(tmp0,0x0,sizeof(tmp0));
		memset(tmp1,0x0,sizeof(tmp1));
		memset(tmp2,0x0,sizeof(tmp2));
		
		//strcpy(param.argv[0],"0,1234567890,10");
//		strcpy(param.argv[0],"0");
	//	strcpy(param.argv[1],"1234567890");
		//strcpy(param.argv[2],"10");
		
		//At_TCPSend(&param);

		
		printf("...........<%s-end> 104...........\n",__func__);
		
#endif

}

void tcptask(void *args)
{
	while(1){
		if(1==get_wifi_status()) {
			//iperf_task(NULL);
			//iperf_task ();
			break;
		} else {
			OS_MsDelay(500);
			//printf("get_wifi_status()=%d\n",get_wifi_status());
		}

	}

	OS_TaskDelete(NULL);
}

#define APSSID "SSV_AP8"
#define APPASSPHRASE "nononono"

void scanAPcbfunc()
{
    u8 i;

    printf("\nCount:%d\n", getAvailableIndex());
    for(i = 0; i < getAvailableIndex(); i++)
    {
        printf("%2d - name:%32s, rssi:%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n", i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel
            , ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2], ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
    printf("end\n");

    if(set_wifi_config((u8 *)APSSID, (u8)strlen(APSSID), (u8 *)APPASSPHRASE, (u8)strlen(APPASSPHRASE), NULL, 0) == 0)
    {
        wifi_connect(wifirspcbfunc);
    }
    else
    {
        printf("%s\n",APSSID);
    }
}

void TaskBmp(void *pdata)
{
    int i = 0;
	while (bss_test)
        OS_MsDelay(100);
    while (common_test)
        OS_MsDelay(100);
    while(data_test != M_VERIFY_DATA)
        OS_MsDelay(100);
    for (i=0;i<10;i++) {
        while(common_array_test[i])
            OS_MsDelay(100);
    }
    for (i=0;i<4;i++) {
        while(bss_array_test[i])
            OS_MsDelay(100);
    }
    while(data_array[0] != 123)
        OS_MsDelay(100);
    while(data_array[1] != 456)
        OS_MsDelay(100);
    while(data_array[2] != 789)
        OS_MsDelay(100);
    while(data_array[3] != 8888)
        OS_MsDelay(100);

    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
    netstack_init(NULL);
    printf("init end 2\n");

    DUT_wifi_start(DUT_STA);
	OS_MsDelay(500);
    if(get_DUT_wifi_mode() == DUT_STA || get_DUT_wifi_mode() == DUT_CONCURRENT)
        scan_AP(scanAPcbfunc);
	
    while(1) {
        i++;

        OS_MsDelay(500);
    }
}


extern void drv_uart_init(void);
extern uint32_t FLASH_SIZE, FLASH_START_AT, FLASH_TOTAL_SIZE;
SSV_FS fs_handle = NULL;
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
	OS_MemInit();

	fs_handle = FS_init();
	if(fs_handle)
	{
		FS_remove_prevota(fs_handle);
	}

#if 1	
	OS_TaskCreate(TaskWav, "TaskWav", 1024, NULL, TaskWav_TASK_PRIORITY, NULL);
#endif

#if 1
	OS_TaskCreate(TaskBmp, "TaskBmp", 2048, NULL, TaskBmp_TASK_PRIORITY, NULL);
#endif

#if 1
	OS_TaskCreate(Cli_Task, "cli", 1024, NULL, 1, NULL);
#endif

    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
	printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
	print_callstack();
	while(1);
}

