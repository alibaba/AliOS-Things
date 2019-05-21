#include "include.h"
#include "common.h"
#include "uart_pub.h"
#include "mem_pub.h"

#include "task.h"
#include "rtos_pub.h"
#include "error.h"

#include "app_lwip_udp.h"
#include "app_lwip_tcp.h"
#include "app_led.h"

#include "wlan_ui_pub.h"
#include "ieee802_11_demo.h"
#include "rw_pub.h"
#include "app_drone.h"

#include "param_config.h"
#include "drv_model_pub.h"
#include "flash_pub.h"

#include "inet.h"
#include "ieee802_11_defs.h"

#if CFG_SUPPORT_TIANZHIHENG_DRONE
#define APP_DRONE_DEBUG        1
#if APP_DRONE_DEBUG
#define APP_DRONE_PRT      warning_prf
#define APP_DRONE_WARN     warning_prf
#define APP_DRONE_FATAL    fatal_prf
#else
#define APP_DRONE_PRT      null_prf
#define APP_DRONE_WARN     null_prf
#define APP_DRONE_FATAL    null_prf
#endif

#define APP_DRONE_QITEM_COUNT           4
#define APP_DRONE_TIMER_INTVAL          500  // ms
#define CONFIG_ADDR_START               0xbd000

typedef enum
{
    DS_WIFI_DISCONECTED          = 0, 
    DS_WIFI_CONECTED,
    DS_APP_DISCONECTED,
    DS_APP_CONECTED, 
} DRONE_STATUS;

typedef struct temp_message 
{
	u32 dmsg;
}DRONE_MSG_T;

typedef struct app_drone_st
{
    xTaskHandle thread_hdl;
    beken_queue_t msg_que; 
    beken_timer_t timer; 
    DRONE_STATUS status;
}APP_DRONE_ST, *APP_DRONE_PTR;

enum{
	ARCH_CONFIG_GENERAL,
	ARCH_CONFIG_AP,
	ARCH_CONFIG_STA,
	ARCH_CONFIG_MISC,
};

enum{
	GENERAL_TYPE_ROLE,
	GENERAL_TYPE_DHCP,
	GENERAL_TYPE_IP,
	GENERAL_TYPE_MASK,
	GENERAL_TYPE_GW,
};

enum{	
	AP_TYPE_BSSID,
	AP_TYPE_SSID,
	AP_TYPE_CHANNEL,
	AP_TYPE_MODE,
	AP_TYPE_PASSWD,
};

typedef struct head_param{
	u32 type;
	u32 len;
}head_param_t;

APP_DRONE_PTR g_drone = NULL;

static int app_drone_get_general_paramters(general_param_t *general);
static int app_drone_get_ap_paramters(ap_param_t *ap);

static u32 app_drone_timer_poll(void)
{
    u8 sta_num = 0;
    
    //APP_DRONE_PRT("timer_handler %d\r\n", sta_num);

    if(sta_num) {
        if(g_drone->status == DS_WIFI_DISCONECTED)
            app_drone_send_msg(DMSG_WIFI_CONECTED);          
    } else {
        if(g_drone->status != DS_WIFI_DISCONECTED)
            app_drone_send_msg(DMSG_WIFI_DISCONECTED);  
    }

    return 0;
}

void app_drone_send_msg(u32 new_msg)
{
	OSStatus ret;
	DRONE_MSG_T msg;

    if(g_drone && g_drone->msg_que) 
    {
    	msg.dmsg = new_msg;
    	
    	ret = rtos_push_to_queue(&g_drone->msg_que, &msg, BEKEN_NO_WAIT);
    	if(kNoErr != ret)
    	{
    		os_printf("app_drone_send_msg failed\r\n");
    	}
    }
}

static void app_drone_timer_handler(void *data)
{
    app_drone_send_msg(DMSG_TIMER_POLL);
}

static int app_drone_rw_event_func(enum rw_evt_type evt_type, void *data)
{
    struct rw_evt_payload *evt_payload = (struct rw_evt_payload *)data;

    if(evt_type == RW_EVT_AP_CONNECTED) {
        APP_DRONE_PRT("RW_EVT_AP_CONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        if(g_drone->status == DS_WIFI_DISCONECTED)
            app_drone_send_msg(DMSG_WIFI_CONECTED);          
    } else if(evt_type == RW_EVT_AP_DISCONNECTED) {
        APP_DRONE_PRT("RW_EVT_AP_DISCONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        if(g_drone->status != DS_WIFI_DISCONECTED)
            app_drone_send_msg(DMSG_WIFI_DISCONECTED);  
    }

    return 0;
}

static int app_drone_setup(void)
{
    #define APP_DRONE_DEF_SSID          "WIFI_UPV_000000"
    #define APP_DRONE_DEF_NET_IP        "192.168.4.151"
    #define APP_DRONE_DEF_NET_MASK      "255.255.255.0"
    #define APP_DRONE_DEF_NET_GW        "192.168.4.151"
    #define APP_DRONE_DEF_CHANNEL       1    
    
    general_param_t general;
    ap_param_t ap_info;
    network_InitTypeDef_st wNetConfig;
    int len;
    u8 *mac;
    
    os_memset(&general, 0, sizeof(general_param_t));
    os_memset(&ap_info, 0, sizeof(ap_param_t)); 
    os_memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));  
    
    if(app_drone_get_general_paramters(&general) == -1) {
        APP_DRONE_PRT("no flash configuration, use default\r\n");
        general.role = 1,
        general.dhcp_enable = 1,

        os_strcpy((char *)wNetConfig.local_ip_addr, APP_DRONE_DEF_NET_IP);
        os_strcpy((char *)wNetConfig.net_mask, APP_DRONE_DEF_NET_MASK);
        os_strcpy((char *)wNetConfig.dns_server_ip_addr, APP_DRONE_DEF_NET_GW);
    } else {
        APP_DRONE_PRT("read flash configuration:\r\n");
        APP_DRONE_PRT("general: %d,%d,%08x,%08x,%08x\r\n",
            general.role,
            general.dhcp_enable,
            general.ip_addr,
            general.ip_mask,
            general.ip_gw);

        general.ip_addr = htonl(general.ip_addr);
        general.ip_mask = htonl(general.ip_mask);
        general.ip_gw = htonl(general.ip_gw);

        os_strcpy((char *)wNetConfig.local_ip_addr, inet_ntoa(general.ip_addr));
        os_strcpy((char *)wNetConfig.net_mask, inet_ntoa(general.ip_mask));
        os_strcpy((char *)wNetConfig.dns_server_ip_addr, inet_ntoa(general.ip_gw));
    }

    if(app_drone_get_ap_paramters(&ap_info) == -1) {
        APP_DRONE_PRT("no flash configuration, use default\r\n");
        mac = (u8*)&ap_info.bssid.array;
        wifi_get_mac_address(mac, CONFIG_ROLE_AP);
        ap_info.chann = APP_DRONE_DEF_CHANNEL;
        ap_info.cipher_suite = 0;
        os_memcpy(ap_info.ssid.array, APP_DRONE_DEF_SSID, os_strlen(APP_DRONE_DEF_SSID));
        ap_info.key_len = 0;
        os_memset(&ap_info.key, 0, 65);   
    } else {
        // first load mac addr from flash
        wifi_get_mac_address(NULL, 0);
        mac = (u8*)&ap_info.bssid.array;
        APP_DRONE_PRT("ap: %02x:%02x:%02x:%02x:%02x:%02x,%d,%d,%s,%d,%s\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
            ap_info.chann,
            ap_info.cipher_suite,
            ap_info.ssid.array,
            ap_info.key_len,
            ap_info.key);
            
        wifi_set_mac_address(mac);
    }
    bk_wlan_ap_set_default_channel(ap_info.chann);

    len = os_strlen(ap_info.ssid.array);
    if(SSID_MAX_LEN < len)
    {
        APP_DRONE_FATAL("ssid name more than 32 Bytes\r\n");
        return -1;
    }

    os_strcpy((char *)wNetConfig.wifi_ssid, ap_info.ssid.array);
    os_strcpy((char *)wNetConfig.wifi_key, ap_info.key);
    
    wNetConfig.wifi_mode = SOFT_AP;
    wNetConfig.dhcp_mode = DHCP_SERVER;
    wNetConfig.wifi_retry_interval = 100;
    
    APP_DRONE_PRT("set ip info: %s,%s,%s\r\n",
            wNetConfig.local_ip_addr,
            wNetConfig.net_mask,
            wNetConfig.dns_server_ip_addr);
    
    APP_DRONE_PRT("ssid:%s  key:%s\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key);
	bk_wlan_start(&wNetConfig);

    return 0;    
}

static void app_drone_main( beken_thread_arg_t data )
{
    OSStatus err;
    int ret = 0;
    u32 status;

	err = rtos_init_timer(&g_drone->timer, 
							APP_DRONE_TIMER_INTVAL, 
							app_drone_timer_handler, 
							(void *)0);
    ASSERT(kNoErr == err);
    
	//err = rtos_start_timer(&g_drone->timer);
	//ASSERT(kNoErr == err);
            
    app_led_init();
    app_led_send_msg(POWER_ON);

    rw_evt_set_callback(RW_EVT_AP_CONNECTED, app_drone_rw_event_func);
    rw_evt_set_callback(RW_EVT_AP_DISCONNECTED, app_drone_rw_event_func);

    app_drone_setup();

    //app_lwip_udp_init();
    //app_lwip_tcp_init();

    g_drone->status = DS_WIFI_DISCONECTED;

    while(1) 
    {
        DRONE_MSG_T msg;
        status = g_drone->status;
        
        err = rtos_pop_from_queue(&g_drone->msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
        	switch(msg.dmsg) 
            {
                case DMSG_TIMER_POLL:
                    app_drone_timer_poll();
                    break;

                case DMSG_WIFI_DISCONECTED:
                    g_drone->status = DS_WIFI_DISCONECTED;
                    app_led_send_msg(LED_DISCONNECT);
                    app_lwip_udp_deinit();
                    //app_lwip_tcp_deinit();
                    APP_DRONE_PRT("wifi disconnected!\r\n");
                    break;                    

                case DMSG_WIFI_CONECTED:
                    g_drone->status = DS_WIFI_CONECTED;
                    app_led_send_msg(LED_CONNECT);
                    app_lwip_udp_init();
                    //app_lwip_tcp_init();
                    APP_DRONE_PRT("wifi connected!\r\n");
                    break;

                case DMSG_APP_CONECTED:
                    APP_DRONE_PRT("app connected!\r\n");
                    g_drone->status = DS_APP_CONECTED;
                    break;

                case DMSG_APP_DISCONECTED:
                    APP_DRONE_PRT("app disconnected!\r\n");
                    g_drone->status = DS_APP_DISCONECTED;
                    break;                    
                    
                case DMSG_EXIT:
                    goto app_drone_exit;
                    break;
                    
                default:
                    break;
            }
        } 
    }

app_drone_exit:
    err = rtos_deinit_timer(&g_drone->timer);
    ASSERT(kNoErr == err);

    rtos_deinit_queue(&g_drone->msg_que);

    os_free(g_drone);
    g_drone = NULL;
    
    rtos_delete_thread(NULL);
}


void user_main( beken_thread_arg_t args )
{
    int ret;

    APP_DRONE_PRT("app_drone_init\r\n");
#if 1
    if(!g_drone) 
    {
        g_drone = os_malloc(sizeof(APP_DRONE_ST));
        if(!g_drone) {
            APP_DRONE_FATAL("app_drone_init malloc failed\r\n");
            return;
        }

        ret = rtos_init_queue(&g_drone->msg_que, 
    							"app_drone_queue",
    							sizeof(DRONE_MSG_T),
    							APP_DRONE_QITEM_COUNT);
    	if (kNoErr != ret) 
    	{
    		APP_DRONE_FATAL("app_drone_init ceate queue failed\r\n");
            os_free(g_drone);
            g_drone = NULL;
            return;
    	}   
        
        ret = rtos_create_thread(&g_drone->thread_hdl,
                                      BEKEN_DEFAULT_WORKER_PRIORITY,
                                      "app_drone",
                                      (beken_thread_function_t)app_drone_main,
                                      2048,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            APP_DRONE_FATAL("Error: Failed to create spidma_intfer: %d\r\n", ret);

            rtos_deinit_queue(g_drone->msg_que);
            os_free(g_drone);
            g_drone = NULL;
            return;
        }

    }
#endif
    rtos_delete_thread(NULL);

}

static UINT32 search_by_type(UINT32 type, UINT32 start_addr)
{
	UINT32 status, addr, end_addr;
	DD_HANDLE flash_handle;
	head_param_t head;

	flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), start_addr);
	addr = start_addr + sizeof(head_param_t);
	end_addr = addr + head.len;
	while(addr < end_addr){
		ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
		if(type == head.type){
			break;
		}else{
			addr += sizeof(head_param_t);
			addr += head.len;
		}
	}

	if(addr >= end_addr){
		addr = 0;
	}
	ddev_close(flash_handle);
	
	return addr;
}

static int app_drone_get_general_paramters(general_param_t *general)
{
	UINT32 status, addr, addr_start;
	DD_HANDLE flash_handle;
	head_param_t head;

	if(!general){
        APP_DRONE_FATAL("get_general_paramters is null\r\n");
        return -1;
	}
	
	addr_start = search_by_type(ARCH_CONFIG_GENERAL, CONFIG_ADDR_START);
	if(!addr_start){
		APP_DRONE_FATAL("SEARCH GENERAL CLASS FAIL\r\n");
		return -1;
	}

	flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

	addr = search_by_type(GENERAL_TYPE_ROLE, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH ROLE FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&general->role, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(GENERAL_TYPE_DHCP, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH DHCP FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&general->dhcp_enable, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(GENERAL_TYPE_IP, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH IP FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&general->ip_addr, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(GENERAL_TYPE_MASK, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH MASK FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&general->ip_mask, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(GENERAL_TYPE_GW, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH GW FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&general->ip_gw, head.len, addr+sizeof(head_param_t));

	ddev_close(flash_handle);
	return 0;
}

static int app_drone_get_ap_paramters(ap_param_t *ap)
{
	UINT32 status, addr, addr_start;
	DD_HANDLE flash_handle;
	head_param_t head;

	if(!ap) {
        APP_DRONE_FATAL("get_ap_paramters is null\r\n");
        return -1;
	}

	addr_start = search_by_type(ARCH_CONFIG_AP, CONFIG_ADDR_START);
	if(!addr_start){
		APP_DRONE_FATAL("SEARCH AP CLASS FAIL\r\n");
		return -1;
	}

	flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

	addr = search_by_type(AP_TYPE_BSSID, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH BSSID FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&ap->bssid, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(AP_TYPE_SSID, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH SSID FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ap->ssid.length = head.len;
	ddev_read(flash_handle, (char *)ap->ssid.array, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(AP_TYPE_CHANNEL, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH CHANNEL FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&ap->chann, head.len, addr+sizeof(head_param_t));

	addr = search_by_type(AP_TYPE_MODE, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH MODE FAIL\r\n");
		return -1;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ddev_read(flash_handle, (char *)&ap->cipher_suite, head.len, addr+sizeof(head_param_t));

#if 1
	addr = search_by_type(AP_TYPE_PASSWD, addr_start);
	if(!addr){
		APP_DRONE_FATAL("SEARCH PASSWORD FAIL\r\n");
		return 0;
	}
	ddev_read(flash_handle, (char *)&head, sizeof(head_param_t), addr);
	ap->key_len = head.len;
	ddev_read(flash_handle, (char *)ap->key, head.len, addr+sizeof(head_param_t));
#endif
	return 0;
}



#endif  // CFG_SUPPORT_TIANZHIHENG_DRONE

