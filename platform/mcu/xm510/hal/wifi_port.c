/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <hal/base.h>
#include <hal/wifi.h>
#include <stdio.h>
#include <sdio/mmcsd_card.h>
#include <board.h>
#include <stdlib.h>
#include "lwip/sockets.h"
#include "lwip/af_packet.h"
#include "lwip/prot/ethernet.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "pthread.h"
#include <fcntl.h>
#include <sys/prctl.h>

extern int sdio_wifi_init(unsigned int power_io);
extern int32_t sdio_wifi_probe(struct rt_mmcsd_card *card);
extern int sdio_wifi_driver_register(int32_t (*func_probe)(struct rt_mmcsd_card *));
extern void wlan_set_mode(char *p);
extern int getWifiIpstate(hal_wifi_ip_stat_t* pnetstat);
extern void wifi_config(char* ssid,char* psk);
extern void getWifiIpInfo(hal_wifi_ip_stat_t* pnetstat);
extern void wifi_scan(void);
extern char getScanApNum(void);
extern void copyScan2hal(void* arg);
extern void wlan_set_channel(int num);
extern void wifi_sem_init(void);
extern void wifi_wait_probesem(void);
extern void wifi_disconnect(void);
//extern void monitor_input( monitor_data_cb_t cb);
monitor_data_cb_t monitor_fn = NULL;


static int wifi_init(hal_wifi_module_t *m)
{
	writel(1, 0x2000002c);
	writel(0x1018000C, 0x20000020);
	writel(0x00000C18, 0x20000024);
	writel(0, 0x2000002c);
	wifi_sem_init();
	sdio_wifi_init(0x65);
	sdio_wifi_driver_register(sdio_wifi_probe);
	wifi_wait_probesem();
	printf("init over\n");
    return 0;
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
	if(mac == NULL){
		return;
	}
	else{
		get_wifi_mac(mac);
	}
	return;
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
	hal_wifi_ip_stat_t netstat;
	memset(&netstat,0,sizeof(netstat));
	wlan_set_mode("sta");
	wifi_config(init_para->wifi_ssid,init_para->wifi_key);
	if ( !getWifiIpstate(&netstat) )
		m->ev_cb->ip_got(m,&netstat,NULL);
	else 
		return -1;
    return 0;
}
static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
	if(out_net_para == NULL)
		return -1;
	getWifiIpInfo(out_net_para);
	return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{  
	hal_wifi_scan_result_t result;
	wifi_scan();
	result.ap_num = getScanApNum();
	if(result.ap_num == 0)
		return ;
	result.ap_list = malloc( result.ap_num * sizeof (ap_list_t ));
	if(result.ap_list == NULL)
		return ;
	memset(result.ap_list,0,sizeof(result.ap_num * sizeof (ap_list_t )));
	copyScan2hal(result.ap_list);
	if(m->ev_cb->scan_compeleted != NULL)
		m->ev_cb->scan_compeleted(m,&result,NULL);
	free(result.ap_list);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	hal_wifi_scan_result_adv_t result;
	wifi_scan();
	result.ap_num = (char)getScanApNum();
	if(result.ap_num == 0)
		return ;
	result.ap_list = malloc( result.ap_num * sizeof (ap_list_adv_t ));
	if(result.ap_list == NULL)
		return ;
	memset(result.ap_list,0,sizeof(result.ap_num * sizeof (ap_list_t )));
	copyScan2hal_adv(result.ap_list);
	if(m->ev_cb->scan_adv_compeleted != NULL)
		m->ev_cb->scan_adv_compeleted(m,&result,NULL);
	free(result.ap_list);
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
	wifi_disconnect();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
	wlan_set_channel(ch);
    return 0;
}

static int taskCreated = 0;
static aos_task_t taskHandle;
void recvpkt(void* arg);
static void start_monitor(hal_wifi_module_t *m)
{
	wlan_set_mode("mon");
	if(monitor_fn != NULL &&  taskCreated == 0){
		taskCreated  = 1;
		//aos_task_new("recvpkt", recvpkt, NULL, 8*1024);
        aos_task_new_ext(&taskHandle, "recvpkt", recvpkt, NULL, 8*1024, 6);
	}else{ 
		taskCreated  = 0;
	}
}
#if 1
static void dump(unsigned char *buf,int packetLen)
{
	int i;
	printf("packetLen %d\n",packetLen);
#if 1
	for(i = 0;i < packetLen  ; i++){
		printf("%02x ",*((unsigned char*)buf+i));
	}
	printf("\n");
#endif
}
#endif



static void stop_monitor(hal_wifi_module_t *m)
{
	taskCreated = 0;
}

void monitor_register( monitor_data_cb_t fn){
	monitor_fn = fn;
}
void recvpkt(void* arg)
{
	int packetLen;
	static char recvbuf[2048];
	struct ifreq ifr;
	struct sockaddr_ll	sll;
	int sockfd;
	struct  timeval tv;
	struct sockaddr saddr;	
	int saddr_size = sizeof saddr;
	hal_wifi_link_info_t info;
	info.rssi = 0;

	sockfd = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
	if(sockfd < 0){
		printf("get fd failed\n");
		return ;
	}
	bzero(&ifr, sizeof(ifr));
	strcpy(ifr.ifr_name, "en1");
	if (ioctlsocket(sockfd, SIOCGIFINDEX, &ifr) < 0)
	{
		perror("get index:");
		close(sockfd);
		return ;
	}
	memset( &sll, 0, sizeof( sll ) );
	sll.sll_family	 = AF_PACKET;
	sll.sll_ifindex  = ifr.ifr_ifindex;
	sll.sll_protocol	= htons(ETH_P_ALL);
	if( bind(  sockfd , (struct sockaddr *) &sll, sizeof( sll ) ) < 0 )
	{	
		perror( "wifi_hal bind(ETH_P_ALL) failed" );
		return ;
	}
	tv.tv_sec = 0;
	tv.tv_usec = 300*1000;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	while(taskCreated)
	{
		packetLen = recvfrom(sockfd , recvbuf ,sizeof(recvbuf), 0 , &saddr , (socklen_t*)&saddr_size);
		if(packetLen <0 ){
			if (errno == EAGAIN ||errno == EWOULDBLOCK){
				printf("error == EAGAIN or error == EWOULDBLOCK\n");
				continue;
			}else {
				perror("Recvfrom error , failed to get packets\n");
				return ;
			}
		}
		else{
	//		dump((unsigned char*)recvbuf,packetLen);
		}
		if( monitor_fn != NULL){
			monitor_fn((uint8_t*)(recvbuf+14),packetLen-14,&info);	
		}else{ 
			break;
		}
	}
	wlan_set_mode("sta");
	close(sockfd);
	aos_task_exit(0);
}
static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	monitor_register(fn);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

hal_wifi_module_t aos_wifi_xm510 = {
    .base.name           = "aos_wifi_xm510",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};

