#include "rtl8195a.h"
#include "build_info.h"
#ifdef PLATFORM_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
#include "osdep_service.h"
#include "lwip_netconf.h"
#include "ethernet_api.h"
#include "lwip_intf.h"
#include "ethernet_mii.h"
#include "platform_opts.h"
#include "ethernet_ex_api.h"

static _sema mii_rx_sema;
static _mutex mii_tx_mutex;
extern volatile u32 ethernet_unplug;

extern struct netif  xnetif[NET_IF_NUM];

static u8 TX_BUFFER[1536];
static u8 RX_BUFFER[1536];

static u8 *pTmpTxDesc = NULL;
static u8 *pTmpRxDesc = NULL;
static u8 *pTmpTxPktBuf = NULL;
static u8 *pTmpRxPktBuf = NULL;	

int ethernet_init_done = 0;
int dhcp_ethernet_mii = 1;
int ethernet_if_default = 0;
int link_is_up = 0;


link_up_down_callback p_link_change_callback = 0;

extern int lwip_init_done;

static _sema mii_linkup_sema;

void mii_rx_thread(void* param){
	u32 len = 0;
	u8* pbuf = RX_BUFFER;
	while(1){
		if (rtw_down_sema(&mii_rx_sema) == _FAIL){
			DBG_8195A("%s, Take Semaphore Fail\n", __FUNCTION__);
			goto exit;
		}
		// continues read the rx ring until its empty
		while(1){
			len = ethernet_receive();
			if(len){
				//DBG_8195A("mii_recv len = %d\n\r", len);
				ethernet_read(pbuf, len);
// calculate the time duration
				ethernetif_mii_recv(&xnetif[ETHERNET_IDX], len);		
				//__rtl_memDump_v1_00(pbuf, len, "ethernet_receive Data:");
				//rtw_memset(pbuf, 0, len);
			}else if(len == 0){
				break;
			}
		}
	}
exit:
	rtw_free_sema(&mii_rx_sema);
	vTaskDelete(NULL);
}

void mii_intr_thread(void* param)
{
	u32 dhcp_status = 0;
	
	while(1)
	{
		if (rtw_down_sema(&mii_linkup_sema) == _FAIL){
			DBG_8195A("%s, Take Semaphore Fail\n", __FUNCTION__);
			break;
		}

		//Used to process there is no cable plugged in when power-on
		if(1 == ethernet_unplug){
			if(p_link_change_callback)
				p_link_change_callback(link_is_up);
		}

		if(1 == ethernet_init_done){		
			if(link_is_up){
				DBG_8195A("...Link up\n");
				if(dhcp_ethernet_mii == 1)
		    		dhcp_status = LwIP_DHCP(ETHERNET_IDX, DHCP_START);
				if(DHCP_ADDRESS_ASSIGNED == dhcp_status){
					if(1 == ethernet_if_default)
						netif_set_default(&xnetif[ETHERNET_IDX]);  //Set default gw to ether netif
					else
						netif_set_default(&xnetif[WLAN_IDX]);
				}
			}else{
				DBG_8195A("...Link down\n");
				netif_set_default(&xnetif[WLAN_IDX]);	//Set default gw to wlan netif
#if CONFIG_LWIP_LAYER
				LwIP_ReleaseIP(ETHERNET_IDX);
#endif
			}
		
			if(p_link_change_callback)
				p_link_change_callback(link_is_up);
		}
	}
	
	rtw_free_sema(&mii_linkup_sema);
	vTaskDelete(NULL);
}


void mii_intr_handler(u32 Event, u32 Data)
{
	switch(Event)
	{
		case ETH_TXDONE:
			//DBG_8195A("TX Data = %d\n", Data);
			break;
		case ETH_RXDONE:
			//DBG_8195A("\r\nRX Data = %d\n", Data);
			// wake up rx thread to receive data
			rtw_up_sema_from_isr(&mii_rx_sema);
			break;
		case ETH_LINKUP:
			DBG_8195A("Link Up\n");
			link_is_up = 1;
			rtw_up_sema_from_isr(&mii_linkup_sema);
			break;
		case ETH_LINKDOWN:
			DBG_8195A("Link Down\n");
			link_is_up = 0;
			rtw_up_sema_from_isr(&mii_linkup_sema);
			break;
		default:
			DBG_8195A("Unknown event !!\n");
			break;
	}
}

void ethernet_demo(void* param){
	u8 mac[6];
	/* Initilaize the LwIP stack */
	// can not init twice
	if(!lwip_init_done)
	  LwIP_Init();
	DBG_8195A("LWIP Init done\n");

	ethernet_irq_hook(mii_intr_handler);

    if(pTmpTxDesc)
    {
        free(pTmpTxDesc);
        pTmpTxDesc = NULL;
    }
    if(pTmpRxDesc)
    {
        free(pTmpRxDesc);
        pTmpRxDesc = NULL;
    }
    if(pTmpTxPktBuf)
    {
        free(pTmpTxPktBuf);
        pTmpTxPktBuf = NULL;
    }
    if(pTmpRxPktBuf)
    {
        free(pTmpRxPktBuf);
        pTmpRxPktBuf = NULL;
    }
    
    pTmpTxDesc = (u8 *)malloc(/*MII_TX_DESC_CNT*/MII_TX_DESC_NO * ETH_TX_DESC_SIZE);
    pTmpRxDesc = (u8 *)malloc(/*MII_RX_DESC_CNT*/MII_RX_DESC_NO * ETH_RX_DESC_SIZE);
    pTmpTxPktBuf = (u8 *)malloc(/*MII_TX_DESC_CNT*/MII_TX_DESC_NO * ETH_PKT_BUF_SIZE);
    pTmpRxPktBuf = (u8 *)malloc(/*MII_RX_DESC_CNT*/MII_RX_DESC_NO * ETH_PKT_BUF_SIZE);
    if(pTmpTxDesc == NULL || pTmpRxDesc == NULL || pTmpTxPktBuf == NULL || pTmpRxPktBuf == NULL)
    {
        printf("TX/RX descriptor malloc fail\n");
        return;
    }
    memset(pTmpTxDesc, 0, MII_TX_DESC_NO * ETH_TX_DESC_SIZE);
    memset(pTmpRxDesc, 0, MII_RX_DESC_NO * ETH_RX_DESC_SIZE);
    memset(pTmpTxPktBuf, 0, MII_TX_DESC_NO * ETH_PKT_BUF_SIZE);
    memset(pTmpRxPktBuf, 0, MII_RX_DESC_NO * ETH_PKT_BUF_SIZE);
    //size 160        128     12288   12288            
    
    ethernet_set_descnum(MII_TX_DESC_NO, MII_RX_DESC_NO);
    printf("TRX descriptor number setting done\n");
    ethernet_trx_pre_setting(pTmpTxDesc, pTmpRxDesc, pTmpTxPktBuf, pTmpRxPktBuf);
    printf("TRX pre setting done\n");

	ethernet_init();

	DBG_INFO_MSG_OFF(_DBG_MII_);  
	DBG_WARN_MSG_OFF(_DBG_MII_);
	DBG_ERR_MSG_ON(_DBG_MII_);
	
	/*get mac*/
	ethernet_address(mac);
	memcpy((void*)xnetif[ETHERNET_IDX].hwaddr,(void*)mac, 6);

	rtw_init_sema(&mii_rx_sema,0);
	rtw_mutex_init(&mii_tx_mutex);

	if(xTaskCreate(mii_rx_thread, ((const char*)"mii_rx_thread"), 1024, NULL, tskIDLE_PRIORITY+5, NULL) != pdPASS)
		DBG_8195A("\n\r%s xTaskCreate(mii_rx_thread) failed", __FUNCTION__);
	
	DBG_8195A("\nEthernet_mii Init done, interface %d", ETHERNET_IDX);
	
	if(dhcp_ethernet_mii == 1){
	  LwIP_DHCP(ETHERNET_IDX, DHCP_START);
	  netif_set_default(&xnetif[ETHERNET_IDX]);  //Set default gw to ether netif
	}
	ethernet_init_done = 1;
	
	vTaskDelete(NULL);
}

int ethernet_is_linked()
{
	 if((link_is_up == 1)&&(1 == ethernet_init_done))
		   return TRUE;
	 else
		   return FALSE;
}

int ethernet_is_unplug()
{
	if(ethernet_unplug == 1)
		   return TRUE;
	 else
		   return FALSE;
}


void ethernet_mii_init()
{
	printf("\ninitializing Ethernet_mii......\n");
  
	// set the ethernet interface as default
	ethernet_if_default = 1;
	vSemaphoreCreateBinary(mii_linkup_sema);
	if( xTaskCreate((TaskFunction_t)mii_intr_thread, "DHCP_START_MII", 1024, NULL, 3, NULL) != pdPASS) {
		DBG_8195A("Cannot create demo task\n\r");
	}	

	if( xTaskCreate((TaskFunction_t)ethernet_demo, "ETHERNET DEMO", 1024, NULL, 2, NULL) != pdPASS) {
		DBG_8195A("Cannot create demo task\n\r");
	}
#if 0
	extern void ethernet_wlan_iperf_test_task(void *param);
	if(xTaskCreate((TaskFunction_t)ethernet_wlan_iperf_test_task, "wifi_entry_task", 1024, NULL, 2, NULL) != pdPASS){
		printf("\n\r%s xTaskCreate(wifi_entry_task) failed", __FUNCTION__);
	}
#endif
	
}


void rltk_mii_recv(struct eth_drv_sg *sg_list, int sg_len){
	struct eth_drv_sg *last_sg;
	u8* pbuf = RX_BUFFER;

	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		if (sg_list->buf != 0) {
			rtw_memcpy((void *)(sg_list->buf), pbuf, sg_list->len);
			pbuf+=sg_list->len;
		}			 
	}
}


s8 rltk_mii_send(struct eth_drv_sg *sg_list, int sg_len, int total_len){
	int ret =0;
	struct eth_drv_sg *last_sg;
	u8* pdata = TX_BUFFER;
	u8	retry_cnt = 0;
	u32 size = 0;
	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		rtw_memcpy(pdata, (void *)(sg_list->buf), sg_list->len);
		pdata += sg_list->len;
		size += sg_list->len;		
	}
	pdata = TX_BUFFER;
	//DBG_8195A("mii_send len= %d\n\r", size);
	rtw_mutex_get(&mii_tx_mutex);
	while(1){
		ret = ethernet_write(pdata, size);
		if(ret > 0){
			ethernet_send();
			ret = 0;
			break;
		}
		if(++retry_cnt > 3){
			DBG_8195A("TX drop\n\r");
			ret = -1;
		}
		else
			rtw_udelay_os(1);
	}
	rtw_mutex_put(&mii_tx_mutex);

	return ret; 	
}

#define ____TEST____
/* used for test */
#if 0
#include "wifi_constants.h"
struct iperf_data_t{
	uint64_t total_size;
	uint64_t bandwidth;
	int  server_fd;
	int  client_fd;
	uint32_t buf_size;
	uint32_t time;
	uint32_t report_interval;
	uint16_t port;
	uint8_t  server_ip[16];
	uint8_t  start;
	uint8_t  tos_value;
};

/*ATWT=-c,192.168.31.111,-t,10*/
int ethernet_wlan_iperf_test()
{
	char server_ip[] = "192.168.31.111";
	u16  time = 10;
	struct iperf_data_t tcp_client_data;

	printf("\n###################ethernet wlan iperf test !!!...\n");
	memset(&tcp_client_data, 0, sizeof(tcp_client_data));

	strncpy(tcp_client_data.server_ip, server_ip, (strlen(server_ip)>16)?16:strlen(server_ip));
	tcp_client_data.time = time;
	tcp_client_data.bandwidth = 268459220;
	tcp_client_data.buf_size = 1460;
	tcp_client_data.client_fd = 0;
	tcp_client_data.port = 5001;
	tcp_client_data.report_interval = -1;
	tcp_client_data.server_fd = 0;
	tcp_client_data.start = 1;
	tcp_client_data.tos_value = 0;
	tcp_client_data.total_size = 0;
	
	tcp_client_func(tcp_client_data);

	return 0;	
}

void ethernet_wlan_iperf_test_task(void *param)
{
	printf("\n#########################ethernet wlan iperf test task...\n");

	/* used for test */
	p_link_change_callback = (link_up_down_callback)ethernet_wlan_iperf_test;
	
	/*sys_reset is not necessary, upper layer can define p_link_change_callback on demand*/
	//p_link_change_callback = (link_up_down_callback)sys_reset();
	while(1)
	{
		if(((wifi_is_ready_to_transceive(RTW_STA_INTERFACE)==RTW_SUCCESS)&&ethernet_is_unplug())||(ethernet_is_linked()))
			break;
		else
			vTaskDelay(500);
	}
	ethernet_wlan_iperf_test();
	
	printf("\r\n[%s] task del", __func__);
	vTaskDelete(NULL);
}

#endif

