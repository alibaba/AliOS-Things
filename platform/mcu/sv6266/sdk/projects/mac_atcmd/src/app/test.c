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

SSV_FS fs_handle = NULL;

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

int TCPConnect ()
{

    u16 port = 12345;    
    char peeraddr[] = "192.168.0.101";
    int socket;
    struct sockaddr_in s_sockaddr;
    int listen = 0;

    memset(&s_sockaddr, 0, sizeof(s_sockaddr));
    s_sockaddr.sin_family = AF_INET;
    s_sockaddr.sin_port = htons(port);
    inet_aton(peeraddr, &s_sockaddr.sin_addr);
    s_sockaddr.sin_len = sizeof(s_sockaddr);
    
    if ((socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Failed to create socket\n");
        return -1;
    }
    
    if (connect(socket, (struct sockaddr *) &s_sockaddr, sizeof(s_sockaddr)) < 0)
    {
        printf("Failed to connect\n");
        close(socket);        
        return -1;
    }
    
    printf("AT+NEWTCPCONN OK:%d\n", socket);
    return 0;
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

/*        ret = dns_gethostbyname("www.baidu.com", &resolved, hostfound, NULL);
        if(ret == ERR_OK)
        {
            ipv4[0] = (resolved.addr >> 24) & 0xFF;
            ipv4[1] = (resolved.addr >> 16) & 0xFF;
            ipv4[2] = (resolved.addr >> 8) & 0xFF;
            ipv4[3] = resolved.addr & 0xFF;
            printf("name:www.baidu.com, addr:%d.%d.%d.%d\n", ipv4[3], ipv4[2], ipv4[1], ipv4[0]);
            wifi_disconnect(NULL);
            
            OS_TaskCreate(wifi_monitor_task, "wifi monitor task", 192, NULL, OS_TASK_PRIO2, NULL);
        }
        else
            printf("Wait callback function\n");*/

//        OS_TaskCreate(wifi_monitor_task, "wifi monitor task", 192, NULL, OS_TASK_PRIO2, &wifitask);
    }
    //TCPConnect();
}


typedef struct _stParam
{
	char *argv[10];
	int	argc;
} stParam;

stParam param;

#if 0
int At_TCPConnect (stParam *param)
{

	char *pIp = 0, *pport = 0;
    uint16_t port;    
    int socket;
    struct sockaddr_in s_sockaddr;
    int listen = 0;


	printf("<%s>%d\n",__func__,__LINE__);

	if (param->argc < 2) 
			return -1;
	
	pIp = param->argv[0];
	pport = param->argv[1];

	
	printf("<%s>%d  (pIp,pport)=(%s,%s)\n",__func__,__LINE__,pIp,pport);
	
	port = atoi (pport);
	if (port == 0) 
		return -1;    
	
	memset(&s_sockaddr, 0, sizeof(s_sockaddr));
	s_sockaddr.sin_family = AF_INET;
	s_sockaddr.sin_port = htons(port);
	inet_aton(pIp, &s_sockaddr.sin_addr);
	s_sockaddr.sin_len = sizeof(s_sockaddr);
	
	if ((socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}
	
	if (connect(socket, (struct sockaddr *) &s_sockaddr, sizeof(s_sockaddr)) < 0)
	{
		printf("Failed to connect\n");
		close(socket);		  
		return -1;
	}
	

	printf("AT+NEWTCPCONN OK:%d\n", socket);
	return 0;

}
#endif

//void iperf_task(void *args)
void iperf_test()
{
		//OS_MsDelay(10000);
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
		
		
        //strcpy(param.argv[0],"-c");
        //strcpy(param.argv[1],"192.168.1.104");
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

//#define APSSID "TP-LINK_FC66C6"
//#define APPASSPHRASE "1234567890"
//#define APSSID "SSV_AP8"
//#define APPASSPHRASE "nononono"
//#define APSSID "miiicasa"
//#define APPASSPHRASE "1234567890"
#define APSSID "Jonathan"
#define APPASSPHRASE "1234567890"

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

//    scan_AP(scanAPcbfunc);
//    return;
//    set_wifi_config("AFW221-3", strlen("AFW221-3"), "87715958", strlen("87715958"), NULL, 0);
//    set_wifi_config("Jonathan", strlen("Jonathan"), "1234567890", strlen("1234567890"), mac, 6);
    if(set_wifi_config((u8 *)APSSID, (u8)strlen(APSSID), (u8 *)APPASSPHRASE, (u8)strlen(APPASSPHRASE), NULL, 0) == 0)
        wifi_connect(wifirspcbfunc);
    else
    {
        printf("%s\n",APSSID);
    }
    //OS_TaskCreate(tcptask, "iperf task", 512, NULL, OS_TASK_PRIO1, NULL);

//    set_wifi_config("SSV-AP5_2.4G", strlen("SSV-AP5_2.4G"), "nononono", strlen("nononono"), NULL, 0);
//    set_wifi_config("MyASUS", strlen("MyASUS"), "1234567890", strlen("1234567890"), NULL, 0);
//    if(set_wifi_config_2("1A2B3C", strlen("1A2B3C"), NULL, 0, NULL, 0, 5) == 0)
}

#if 0
uint32_t spi_flash_read_jedec()
{
    uint8_t Manufacturer;
    uint8_t DeviceType;
    uint8_t Capacity;
    uint32_t Size;   //Mb
    uint32_t loop_i = 0;

	printf("<%s>%d\n",__func__,__LINE__);
    drv_flash_read_jedec_id( &Manufacturer, &DeviceType, &Capacity);

	printf("<%s>%d\n",__func__,__LINE__);

    if( (Capacity < 10) || (Capacity > 32) )
    {
        Size = 0;   //invalid size
    } else {
        Size = (((1<<Capacity) << 3) >> 20);
    }
    
    //printf("(Manufacturer,DeviceType,Capacity)=(0x%x,0x%x,0x%x) Size=%d(Mb)\n",Manufacturer, DeviceType, Capacity,Size);
    printf("Manufacturer = 0x%x\n",Manufacturer);
    printf("DeviceType = 0x%x\n",DeviceType);
    printf("Capacity = 0x%x\n",Capacity);
    printf("Size = %d(Mb)\n",Size);
    
    return 0;    
}
#endif

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

    //load phy table
    load_phy_table();
    
#if 0 //system hang!! waiting for Hoz's simulation!!
	drv_flash_init();
	spi_flash_read_jedec();
#endif
    DUT_wifi_start(DUT_STA);
	OS_MsDelay(500);
//    if(get_DUT_wifi_mode() == DUT_STA || get_DUT_wifi_mode() == DUT_CONCURRENT)
//        scan_AP(scanAPcbfunc);
	
    while(1) {
        i++;

        //printf("tx:%x\n", i);
        //tx_nulldata(0);
        //tx_authentication_req_seq1();
        //tx_probe_req(NULL, 0);
        OS_MsDelay(500);
    }
}

void wifi_auto_connect_task(void *pdata)
{  
    if( get_auto_connect_flag() == 1 )
    {
        printf("run wifi_auto_connect_task\n");
        OS_MsDelay(3*1000);
        do_wifi_auto_connect();
    }
    
    OS_TaskDelete(NULL);
}

void temperature_compensation_task(void *pdata)
{
    printf("temperature compensation task\n");
    OS_MsDelay(1*1000);
    load_rf_table_from_flash();
    write_reg_rf_table();
    while(1)
    {
        OS_MsDelay(3*1000);
        do_temerature_compensation();
        
    }
    
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

#if defined(FEATURE_FFS) && (FEATURE_FFS == 1)
        init_global_conf();
	OS_TaskCreate(wifi_auto_connect_task, "wifi_auto_connect", 1024, NULL, TaskBmp_TASK_PRIORITY, NULL);
#endif

#if 1
	OS_TaskCreate(temperature_compensation_task, "rf temperature compensation", 256, NULL, TaskBmp_TASK_PRIORITY, NULL);
#endif

    printf("[%s][%d] string\n", __func__, __LINE__);

    OS_StartScheduler();
}

void vAssertCalled( const char *func, int line )
{
	printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
	print_callstack();
	while(1);
}

