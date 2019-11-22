/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "k_api.h"
#include "wifi_api.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "dhcpd.h"

#include "network/hal/wifi.h"
//#include "ethernetif.h"
//#include "portmacro.h"
//#include "dhcpd.h"
#include "wifi_lwip_helper.h"
//#include "network_default_config.h"
#include "wifi_private_api.h"
#include "ethernetif.h"
#include "os_sem.h"
static SemaphoreHandle_t wifi_connected;

static os_sem_handle_t g_ip_ready;

static void ip_ready_callback(struct netif *netif);

static int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
static int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

/**
  * @brief  dhcp got ip will callback this function.
  * @param[in] struct netif *netif: which network interface got ip.
  * @retval None
  */
static void ip_ready_callback(struct netif *netif)
{ 

	//LOG_I(common, "**********ip_ready_callback**************");
	printf("**********ip_ready_callback**************");
#if 1
	 if (!ip4_addr_isany_val(netif->ip_addr)) 
	 {
		inform_ip_got(STATION);
		//xSemaphoreGive(ip_ready);
		//os_sem_give(g_ip_ready);
#else
    if (!ip4_addr_isany_val(netif->ip_addr)) {
        char ip_addr[17] = {0};
        if (NULL != inet_ntoa(netif->ip_addr)) {
            strcpy(ip_addr, inet_ntoa(netif->ip_addr));
            LOG_I(common, "************************");
            LOG_I(common, "DHCP got IP:%s", ip_addr);
            LOG_I(common, "************************");

            xSemaphoreGive(ip_ready);
        } else {
            LOG_E(common, "DHCP got Failed");
        }
#endif
		//wifi_connection_inform_ip_ready();
#ifdef MTK_WIFI_REPEATER_ENABLE
#ifndef MTK_WIFI_DHCPD_DHCP_COEXIST_ENABLE
        uint8_t op_mode = 0;
        struct netif *ap_if = netif_find_by_type(NETIF_TYPE_AP);
        if (wifi_config_get_opmode(&op_mode) < 0) {
            return;
        }

        if (WIFI_MODE_REPEATER == op_mode) {
            netif_set_addr(ap_if, &netif->ip_addr, &netif->netmask, &netif->gw);
        }
#endif
#endif
    }
}

/**
  * @brief  wifi connected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
static int32_t wifi_station_port_secure_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;
	
    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_link_up(sta_if);
    LOG_I(common, "wifi connected");
	wifi_status_change(NOTIFY_STATION_UP);
    return 0;
}

/**
  * @brief  wifi disconnected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
static int32_t wifi_station_disconnected_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    uint8_t opmode  = 0;
	uint8_t ipmode = 0;
		hal_wifi_module_t *m;

    if (wifi_config_get_opmode(&opmode) < 0) {
        return 0;
    }

    if ((WIFI_MODE_AP_ONLY != opmode) && WIFI_EVENT_IOT_DISCONNECTED == event) {
        uint8_t link_status = 1;
        //should check link status, it will emit this event when sp disconnect with host under repeater mode.
        if (wifi_connection_get_link_status(&link_status) < 0) {
            return 0;
        }

        if (link_status == 0) {
            struct netif *sta_if;
            sta_if = netif_find_by_type(NETIF_TYPE_STA);
            netif_set_link_down(sta_if);
			wifi_config_get_ip_mode(&ipmode);
            if (ipmode == STA_IP_MODE_DHCP) {
                netif_set_addr(sta_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY);
            }

			wifi_status_change(NOTIFY_STATION_DOWN);
            LOG_I(common, "wifi disconnected");
        }
    }
    return 1;
}

/**
  * @brief  network init function. initial wifi and lwip config
  * @param None
  * @retval None
  */
#if 0
void lwip_network_init(uint8_t opmode)
{
    lwip_tcpip_config_t tcpip_config = {0, {0}, {0}, {0}, {0}, {0}, {0}};

    if (0 != tcpip_config_init(&tcpip_config)) {
        LOG_E(common, "tcpip config init fail");
        return;
    }
     wifi_connected = xSemaphoreCreateBinary();
    if (dhcp_config_init() == STA_IP_MODE_DHCP) {
         ip_ready = xSemaphoreCreateBinary();
    }
    lwip_tcpip_init(&tcpip_config, opmode);
}
#endif

#if 0
/**
  * @brief  Start lwip service in a certain operation mode.
  * @param[in] uint8_t opmode: the target operation mode.
  * @retval None
  */
void lwip_net_start(uint8_t opmode)
{
    struct netif *sta_if;
    struct netif *ap_if;

    switch (opmode) {
        case WIFI_MODE_STA_ONLY:
        case WIFI_MODE_REPEATER:
            wifi_connection_register_event_handler(WIFI_EVENT_IOT_CONNECTED, wifi_station_port_secure_event_handler);
            wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);
            if (dhcp_config_init() == STA_IP_MODE_DHCP) {
                sta_if = netif_find_by_type(NETIF_TYPE_STA);
                netif_set_default(sta_if);
                netif_set_status_callback(sta_if, ip_ready_callback);printf("----------init dhcp satrt---------\r\n");
                dhcp_start(sta_if);
            }
#ifdef MTK_WIFI_REPEATER_ENABLE
#ifdef MTK_WIFI_DHCPD_DHCP_COEXIST_ENABLE
            if (opmode == WIFI_MODE_REPEATER) {

                dhcpd_settings_t dhcpd_settings = {{0}, {0}, {0}, {0}, {0}, {0}, {0}};
                lwip_tcpip_config_t tcpip_config = {0, {0}, {0}, {0}, {0}, {0}, {0}};

                if (0 != tcpip_config_init(&tcpip_config)) {
                    LOG_E(common, "tcpip config init fail");
                    return;
                }

                dhcpd_settings_init(&tcpip_config, &dhcpd_settings);
                ap_if = netif_find_by_type(NETIF_TYPE_AP);
                netif_set_link_up(ap_if);
                dhcpd_start(&dhcpd_settings);
            }
#endif
#endif
            break;
        case WIFI_MODE_AP_ONLY: {
            dhcpd_settings_t dhcpd_settings = {{0}, {0}, {0}, {0}, {0}, {0}, {0}};
            lwip_tcpip_config_t tcpip_config = {0, {0}, {0}, {0}, {0}, {0}, {0}};

            if (0 != tcpip_config_init(&tcpip_config)) {
                LOG_E(common, "tcpip config init fail");
                return;
            }

            dhcpd_settings_init(&tcpip_config, &dhcpd_settings);
            ap_if = netif_find_by_type(NETIF_TYPE_AP);
            netif_set_default(ap_if);
            netif_set_link_up(ap_if);
            dhcpd_start(&dhcpd_settings);
            break;
        }
    }
}
#endif
/**
  * @brief  Stop lwip service in a certain operation mode.
  * @param[in] uint8_t opmode: the current operation mode.
  * @retval None
  */
#if 0
void lwip_net_stop(uint8_t opmode)
{
    struct netif *sta_if;
    struct netif *ap_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    ap_if = netif_find_by_type(NETIF_TYPE_AP);
    switch (opmode) {
        case WIFI_MODE_AP_ONLY:
            dhcpd_stop();
            netif_set_link_down(ap_if);
            break;
        case WIFI_MODE_STA_ONLY:
        case WIFI_MODE_REPEATER:
            netif_set_status_callback(sta_if, NULL);
            dhcp_release(sta_if);
            dhcp_stop(sta_if);
            netif_set_link_down(sta_if);
            break;
    }
}
#endif
void lwip_net_stop(uint8_t opmode)
{
    struct netif *sta_if;
    struct netif *ap_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    ap_if = netif_find_by_type(NETIF_TYPE_AP);
    switch (opmode) {
        case SOFT_AP:
            dhcps_deinit();
            netif_set_link_down(ap_if);
            break;
        case STATION:
            netif_set_status_callback(sta_if, NULL);
            dhcp_release(sta_if);
            dhcp_stop(sta_if);
            netif_set_link_down(sta_if);
            break;
    }
}

#if 0
/**
  * @brief  when wifi and ip ready will return, only used in station and repeater mode.
  * @param None
  * @retval None
  */
void lwip_net_ready()
{
	while(!wifi_connected)
	{
		os_delay(1000);
	}
    xSemaphoreTake(wifi_connected, portMAX_DELAY);
    if (dhcp_config_init() == STA_IP_MODE_DHCP) {
        xSemaphoreTake(ip_ready, portMAX_DELAY);
    }

}
#endif
/**
  * @brief  Change operation mode dynamically.
  * @param[in] uint8_t target_mode: the target switched operation mode.
  * @retval None
  */
uint8_t wifi_set_opmode(uint8_t target_mode)
{
    uint8_t origin_op_mode = 0;
    if (wifi_config_get_opmode(&origin_op_mode) < 0) {
        return 1;
    }

    if (target_mode == origin_op_mode) {
        LOG_I(wifi, "same opmode %d, do nothing", target_mode);
        return 0;
    }
    lwip_net_stop(origin_op_mode);

    if (wifi_config_set_opmode(target_mode) < 0) {
        return 1;
    }
    LOG_I(wifi, "set opmode to [%d]", target_mode);

    lwip_net_start(target_mode);
    return 0;
}
//for alios
#include "wifi.h"

static int32_t wifi_station_connect_fail_handler(wifi_event_t event, uint8_t *payload, uint32_t length)
{
	printf("connecte fail reason:%s\r\n",payload);
}
static int32_t wifi_init_complete_handler(wifi_event_t event, uint8_t *payload, uint32_t length)
{
	printf("wifi init down\r\n");
}


#if 1
extern struct netif sta_if, ap_if;
void dhcp_sever_init(hal_wifi_init_type_t wifi_para)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct netif * pnetif = &ap_if;
	printf("dhcp sever ip==>%s\r\n",wifi_para.local_ip_addr);
	printf("dhcp sever net_mask==>%s\r\n",wifi_para.net_mask);
	printf("dhcp sever gateway_ip_addr==>%s\r\n",wifi_para.gateway_ip_addr);
	ip4addr_aton(wifi_para.local_ip_addr, &ipaddr);
    ip4addr_aton(wifi_para.net_mask, &netmask);
    ip4addr_aton(wifi_para.gateway_ip_addr, &gw);
	
    netif_set_addr(pnetif, &ipaddr, &netmask, &gw);
	dhcps_init(pnetif);
}

#endif
int wifi_set_default_config(void)
{
  lwip_tcpip_config_t ip_config={0};

  char deflaut_ip[]="11.11.11.200";
  char deflaut_netmask[]="255.255.255.0";
  char deflaut_gw[]="11.11.11.254";
 hw_wifi_init();
  wifi_config_t wifi_config = {0};
  wifi_config.opmode = WIFI_PORT_STA;
  strcpy((char *)wifi_config.sta_config.ssid, "lsd");
  wifi_config.sta_config.ssid_length = strlen(wifi_config.sta_config.ssid);
  
  wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE, wifi_init_complete_handler);
  wifi_init(&wifi_config, NULL);
  
  ip4addr_aton((const char *)deflaut_ip, &(ip_config.sta_addr));
  ip4addr_aton((const char *)deflaut_netmask, &(ip_config.sta_mask));
  ip4addr_aton((const char *)deflaut_gw, &(ip_config.sta_gateway));
  lwip_tcpip_init(&ip_config,WIFI_PORT_STA);
  network_start(WIFI_PORT_STA,(sta_ip_mode_t)ip_config.ip_mode);
  //os_delay(1000);
 // wifi_config_set_channel(WIFI_PORT_STA, 6);
}
void Lwip_init()
{
	  lwip_tcpip_config_t ip_config={0};
	  char deflaut_ip[]="11.11.11.200";
	  char deflaut_netmask[]="255.255.255.0";
	  char deflaut_gw[]="11.11.11.254";

    ip_config.ip_mode = WIFI_MODE_STA_ONLY;

   ip4addr_aton((const char *)deflaut_ip, &(ip_config.sta_addr));
   ip4addr_aton((const char *)deflaut_netmask, &(ip_config.sta_mask));
   ip4addr_aton((const char *)deflaut_gw, &(ip_config.sta_gateway));

   lwip_tcpip_init(&ip_config,ip_config.ip_mode);
}

void lwip_start(hal_wifi_init_type_t wifi_para)
{
	struct netif *sta_netif;
	 unsigned char opmode;
	  ip4_addr_t ip;
	  ip4_addr_t netmask;
	  ip4_addr_t gw;
	// lwip_net_stop(wifi_para.wifi_mode);
	 switch(wifi_para.wifi_mode)
	 {
	 	case STATION:
	 	{
			if(DHCP_CLIENT == wifi_para.dhcp_mode)
			{	//printf("wifi in dhcp mode\r\n");
				dhcps_deinit();
			    wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, wifi_station_port_secure_event_handler);			  
			    wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);
			    sta_netif = netif_find_by_type(NETIF_TYPE_STA);
			    netif_set_default(sta_netif);
			    netif_set_status_callback(sta_netif, ip_ready_callback);
			    dhcp_start(sta_netif);
			}
			else 
			{
		       ip4addr_aton((const char *)wifi_para.local_ip_addr, &ip);
		       ip4addr_aton((const char *)wifi_para.net_mask, &netmask);
		       ip4addr_aton((const char *)wifi_para.gateway_ip_addr, &gw);
			   netif_set_addr(&sta_netif, &ip, &netmask, &gw);
			}
	 	}break;
		case SOFT_AP:
		{
		    struct netif * pnetif = &ap_if;
			printf("dhcp sever ip==>%s\r\n",wifi_para.local_ip_addr);
			printf("dhcp sever net_mask==>%s\r\n",wifi_para.net_mask);
			printf("dhcp sever gateway_ip_addr==>%s\r\n",wifi_para.gateway_ip_addr);
			ip4addr_aton(wifi_para.local_ip_addr, &ip);
		    ip4addr_aton(wifi_para.net_mask, &netmask);
		    ip4addr_aton(wifi_para.gateway_ip_addr, &gw);
			
		    netif_set_addr(pnetif, &ip, &netmask, &gw);
			dhcps_init(pnetif);
			wifi_status_change(NOTIFY_AP_UP);
		}break;
		default:printf("not support this wifi mode\r\n");break;
	 }
}

