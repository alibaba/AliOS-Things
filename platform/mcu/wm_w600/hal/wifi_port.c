/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/hal/base.h"
#include "network/hal/wifi.h"

#include "wm_wifi.h"
#include "wm_ram_config.h"
#include "wm_params.h"
#include "wm_config.h"
#include "wm_netif.h"
#include "inc/tls_wireless.h"
#include "wm_efuse.h"


struct tls_link_status_t {
    u8 ip[4];
    u8 netmask[4];
    u8 gw[4];
    u8 dns1[4];
    u8 dns2[4];
    u8 status; 
};


static void tls_auto_connect_softap(void)
{
    struct tls_param_ssid ssid;
    struct tls_softap_info_t *apinfo;
    struct tls_ip_info_t *ipinfo;
    struct tls_param_ip ip_param;
    struct tls_param_key key;

    apinfo = tls_mem_alloc(sizeof(struct tls_softap_info_t));
    if (apinfo == NULL)
    {
        return ;
    }
    ipinfo = tls_mem_alloc(sizeof(struct tls_ip_info_t));
    if (ipinfo == NULL)
    {
        tls_mem_free(apinfo);
        return ;
    }

    tls_param_get(TLS_PARAM_ID_SOFTAP_SSID, (void*) &ssid, TRUE);
    memcpy(apinfo->ssid, ssid.ssid, ssid.ssid_len);
    apinfo->ssid[ssid.ssid_len] = '\0';

	strcpy(apinfo->ssid, "alios_ap");

    tls_param_get(TLS_PARAM_ID_SOFTAP_ENCRY, (void*) &apinfo->encrypt, TRUE);
    tls_param_get(TLS_PARAM_ID_SOFTAP_CHANNEL, (void*) &apinfo->channel, TRUE);

    tls_param_get(TLS_PARAM_ID_SOFTAP_KEY, (void*) &key, TRUE);
    apinfo->keyinfo.key_len = key.key_length;
    apinfo->keyinfo.format = key.key_format;
    apinfo->keyinfo.index = key.key_index;
    memcpy(apinfo->keyinfo.key, key.psk, key.key_length);

    tls_param_get(TLS_PARAM_ID_SOFTAP_IP, &ip_param, TRUE);
    memcpy(ipinfo->ip_addr, ip_param.ip, 4);
    memcpy(ipinfo->netmask, ip_param.netmask, 4);
    tls_param_get(TLS_PARAM_ID_DNSNAME, ipinfo->dnsname, FALSE);

    tls_wifi_softap_create(apinfo, ipinfo);
    tls_mem_free(apinfo);
    tls_mem_free(ipinfo);
    return ;
}

static int tls_get_link_status(struct tls_link_status_t *lks)
{
    struct tls_ethif *ni;

    ni = tls_netif_get_ethif();
    if (ni->status)
        lks->status = 1;
    else
        lks->status = 0;
#if TLS_CONFIG_LWIP_VER2_0_3
    MEMCPY(lks->ip, (char *)ip_2_ip4(&ni->ip_addr), 4);
    MEMCPY(lks->netmask, (char *)ip_2_ip4(&ni->netmask), 4);
    MEMCPY(lks->gw, (char *)ip_2_ip4(&ni->gw), 4);
    MEMCPY(lks->dns1, (char *)ip_2_ip4(&ni->dns1), 4);
    MEMCPY(lks->dns2, (char *)ip_2_ip4(&ni->dns2), 4);
#else
    MEMCPY(lks->ip, (char *)&ni->ip_addr.addr, 4);
    MEMCPY(lks->netmask, (char *)&ni->netmask.addr, 4);
    MEMCPY(lks->gw, (char *)&ni->gw.addr, 4);
    MEMCPY(lks->dns1, (char *)&ni->dns1.addr, 4);
    MEMCPY(lks->dns2, (char *)&ni->dns2.addr, 4);
#endif
    return 0;
}

int tls_get_softap_link_status(struct tls_link_status_t *lks)
{
    struct tls_ethif * ni = tls_netif_get_softap_ethif();

    if (ni->status)
        lks->status = 1;
    else
        lks->status = 0;
#if TLS_CONFIG_LWIP_VER2_0_3
    MEMCPY(lks->ip, (char *)ip_2_ip4(&ni->ip_addr), 4);
    MEMCPY(lks->netmask, (char *)ip_2_ip4(&ni->netmask), 4);
    MEMCPY(lks->gw, (char *)ip_2_ip4(&ni->gw), 4);
    MEMCPY(lks->dns1, (char *)ip_2_ip4(&ni->dns1), 4);
    MEMCPY(lks->dns2, (char *)ip_2_ip4(&ni->dns2), 4);
#else
    MEMCPY(lks->ip, (char *)&ni->ip_addr.addr, 4);
    MEMCPY(lks->netmask, (char *)&ni->netmask.addr, 4);
    MEMCPY(lks->gw, (char *)&ni->gw.addr, 4);
    MEMCPY(lks->dns1, (char *)&ni->dns1.addr, 4);
    MEMCPY(lks->dns2, (char *)&ni->dns2.addr, 4);
#endif
    return 0;
}


void wifi_net_status_changed(u8 status)
{
	struct tls_ethif *ethif;
	hal_wifi_module_t *m;
	struct tls_param_ip ip_param;
	u8 *buf;
	u8 mac[6];
	hal_wifi_ip_stat_t ip_stat;

	m = hal_wifi_get_default_module();
	
	switch (status)
	{
		case NETIF_WIFI_JOIN_SUCCESS:
			printf("WiFi JOIN SUCCESS\r\n");
            if (m->ev_cb && m->ev_cb->stat_chg) 
			{
                m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
            }
		break;
		case NETIF_WIFI_JOIN_FAILED:
			printf("WiFi JOIN FAILED:%d\r\n", tls_wifi_get_errno());
			if(m->ev_cb->connect_fail)
			{
				m->ev_cb->connect_fail(m, tls_wifi_get_errno(), NULL);
			}
		break;
		case NETIF_WIFI_DISCONNECTED:
			printf("WiFi DISCONNECT\r\n");
            if (m->ev_cb && m->ev_cb->stat_chg) 
			{
                m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
            }
		break;
		case NETIF_IP_NET_UP:
		{			
			ethif = tls_netif_get_ethif();
#if TLS_CONFIG_LWIP_VER2_0_3
			printf("NET UP OK,Local IP:%d.%d.%d.%d\r\n", ip4_addr1(ip_2_ip4(&ethif->ip_addr)),ip4_addr2(ip_2_ip4(&ethif->ip_addr)),

					ip4_addr3(ip_2_ip4(&ethif->ip_addr)),ip4_addr4(ip_2_ip4(&ethif->ip_addr)));
#else
      		printf("NET UP OK,Local IP:%d.%d.%d.%d\n",ip4_addr1(&ethif->ip_addr.addr),ip4_addr2(&ethif->ip_addr.addr),
    	    ip4_addr3(&ethif->ip_addr.addr),ip4_addr4(&ethif->ip_addr.addr));
#endif
            if (m->ev_cb && m->ev_cb->ip_got) 
			{
				tls_param_get(TLS_PARAM_ID_IP, &ip_param, FALSE);
				if(ip_param.dhcp_enable)
					ip_stat.dhcp = DHCP_CLIENT;
				else
					ip_stat.dhcp = DHCP_DISABLE;
				buf = inet_ntoa(ethif->ip_addr);
				memcpy(ip_stat.ip, buf, 16);
				buf = inet_ntoa(ethif->gw);
				memcpy(ip_stat.gate, buf, 16);
				buf = inet_ntoa(ethif->netmask);
				memcpy(ip_stat.mask, buf, 16);	
				buf = inet_ntoa(ethif->dns1);
				memcpy(ip_stat.dns, buf, 16); 
				tls_get_mac_addr(mac);
				sprintf(ip_stat.mac, "%02X%02X%02X%02X%02X%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
				memcpy(ip_stat.broadcastip, "255.255.255.255", 16);
                m->ev_cb->ip_got(m, &ip_stat, NULL);
            }			
		}
		break;
        case NETIF_WIFI_SOFTAP_SUCCESS:
			printf("softap create success.\n");
            if (m->ev_cb && m->ev_cb->stat_chg) 
			{
                m->ev_cb->stat_chg(m, NOTIFY_AP_UP, NULL);
            }
            break;
        case NETIF_WIFI_SOFTAP_FAILED:
            printf("softap create failed.\n");		
            break;
        case NETIF_WIFI_SOFTAP_CLOSED:
            printf("softap closed.\n");
            if (m->ev_cb && m->ev_cb->stat_chg) 
			{
                m->ev_cb->stat_chg(m, NOTIFY_AP_DOWN, NULL);
            }
            break;
        case NETIF_IP_NET2_UP:
//			printf("softap set up ip.\n");
            break;	
		default:
			printf("Not Expected Value:%d\r\n", status);
		break;
	}
}


extern u8 tx_gain_group[TX_GAIN_LEN];

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;
	u8 mac_addr[6] = {0x00,0x25,0x08,0x09,0x01,0x0F};

	printf("wifi_init start\n");
    if (inited) 
	{
        printf("wifi already started.\r\n");
        return 0;
    }

	tls_get_tx_gain(&tx_gain_group[0]);

	if (tls_wifi_mem_cfg(WIFI_MEM_START_ADDR, 7, 7)) /*wifi tx&rx mem customized interface*/
	{
		printf("wifi mem initial failured\n");
		return -1;
	}

	tls_get_mac_addr(&mac_addr[0]);
	if(tls_wl_init(NULL, &mac_addr[0], NULL) == NULL)
	{
		printf("wifi driver initial failured\n");
		return -1;
	}
	if (wpa_supplicant_init(mac_addr)) 
	{
		printf("supplicant initial failured\n");
		return -1;
	}

    inited = 1;
    printf("wifi init success!!\n");

    return 0;
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, u8 *mac)
{
    tls_get_mac_addr(mac);
}


static void wifi_set_mac_addr(hal_wifi_module_t *m, u8 *mac)
{
    tls_set_mac_addr(mac);
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
	u8 wmode;
	u8 dhcp_enable = 0;
	struct tls_param_ip ip_param;

    switch (init_para->wifi_mode) 
	{
        case STATION:
            wmode = IEEE80211_MODE_INFRA;
            break;

        case SOFT_AP:
            wmode = IEEE80211_MODE_AP;
            break;

        default:
            return -1;
    }
    tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, (bool)TRUE);

	switch(init_para->dhcp_mode)
	{
		case DHCP_DISABLE:
			dhcp_enable = 0;
			break;
		case DHCP_CLIENT:
			dhcp_enable = 1;
			break;
		case DHCP_SERVER:
			dhcp_enable = 1;
			break;
	}
	if(WM_WIFI_JOINED == tls_wifi_softap_get_state())
	{
		tls_wifi_softap_destroy();
	}
	if(WM_WIFI_JOINED == tls_wifi_get_state())
	{
		tls_wifi_disconnect();
	}
	
	if((init_para->wifi_mode == STATION))
	{
		tls_param_get(TLS_PARAM_ID_IP, &ip_param, TRUE);
		memcpy(ip_param.ip, inet_addr(init_para->local_ip_addr), 4);
		memcpy(ip_param.gateway, inet_addr(init_para->gateway_ip_addr), 4);
		memcpy(ip_param.netmask, inet_addr(init_para->net_mask), 4);
		memcpy(ip_param.dns1, inet_addr(init_para->dns_server_ip_addr), 4);
		ip_param.dhcp_enable = dhcp_enable;
		tls_param_set(TLS_PARAM_ID_IP, &ip_param, TRUE);
		tls_wifi_connect(init_para->wifi_ssid, strlen(init_para->wifi_ssid), init_para->wifi_key, strlen(init_para->wifi_key));
	}
    else if(init_para->wifi_mode == SOFT_AP)
    {
		tls_param_get(TLS_PARAM_ID_SOFTAP_IP, &ip_param, TRUE);
		ip_param.dhcp_enable = dhcp_enable;
    	tls_param_set(TLS_PARAM_ID_SOFTAP_IP, &ip_param, TRUE);
		tls_auto_connect_softap();
	}
	
    return 0;
}

void test_wifi_start(void)
{
	hal_wifi_module_t *m;
	hal_wifi_init_type_t init_para;

	init_para.wifi_mode = STATION;
	init_para.dhcp_mode = DHCP_CLIENT;
	strcpy(init_para.wifi_key,"1234567890");
	strcpy(init_para.wifi_ssid,"TEST_111");
	
	m = hal_wifi_get_default_module();
	wifi_start(m, &init_para);
}

void test_wifi_start_ap(void)
{
	hal_wifi_module_t *m;
	hal_wifi_init_type_t init_para;

	init_para.wifi_mode = SOFT_AP;
	init_para.dhcp_mode = DHCP_SERVER;
	strcpy(init_para.wifi_key,"1234567890");
	strcpy(init_para.wifi_ssid,"TEST_111");
	
	m = hal_wifi_get_default_module();
	wifi_start(m, &init_para);
}


static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    struct tls_param_ssid ssid;
    struct tls_softap_info_t *apinfo;
    struct tls_ip_info_t *ipinfo;
    struct tls_param_ip ip_param;
    struct tls_param_key key;

    apinfo = tls_mem_alloc(sizeof(struct tls_softap_info_t));
    if (apinfo == NULL)
    {
        return ;
    }
    ipinfo = tls_mem_alloc(sizeof(struct tls_ip_info_t));
    if (ipinfo == NULL)
    {
        tls_mem_free(apinfo);
        return ;
    }
	
	strcpy(apinfo->ssid, init_para_adv->ap_info.ssid);
	apinfo->encrypt = init_para_adv->ap_info.security;
	apinfo->channel = init_para_adv->ap_info.channel;

    apinfo->keyinfo.key_len = init_para_adv->key_len;
    apinfo->keyinfo.format = 1;
    apinfo->keyinfo.index = 0;
    memcpy(apinfo->keyinfo.key, init_para_adv->key, init_para_adv->key_len);
	memcpy(ipinfo->ip_addr, inet_addr(init_para_adv->local_ip_addr), 4);
	memcpy(ipinfo->netmask, inet_addr(init_para_adv->net_mask), 4);

	memcpy(ssid.ssid, apinfo->ssid, strlen(apinfo->ssid));
	ssid.ssid_len = strlen(apinfo->ssid);
	tls_param_set(TLS_PARAM_ID_SOFTAP_SSID, (void*) &ssid, TRUE);
    tls_param_set(TLS_PARAM_ID_SOFTAP_ENCRY, (void*) &apinfo->encrypt, TRUE);
    tls_param_set(TLS_PARAM_ID_SOFTAP_CHANNEL, (void*) &apinfo->channel, TRUE);	

	key.key_format = 1;
	key.key_index = 0;
	key.key_length = apinfo->keyinfo.key_len;
	memcpy(key.psk, apinfo->keyinfo.key, apinfo->keyinfo.key_len);
	tls_param_set(TLS_PARAM_ID_SOFTAP_KEY, (void*) &key, TRUE);

	tls_param_get(TLS_PARAM_ID_SOFTAP_IP, &ip_param, TRUE);	
    memcpy(ip_param.ip, ipinfo->ip_addr, 4);
    memcpy(ip_param.netmask, ipinfo->netmask, 4);
    tls_param_set(TLS_PARAM_ID_SOFTAP_IP, &ip_param, TRUE);

    tls_wifi_softap_create(apinfo, ipinfo);
    tls_mem_free(apinfo);
    tls_mem_free(ipinfo);	

    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
	struct tls_link_status_t lk;
	struct tls_param_ip ip_param;
	u8 *buf;
	u8 mac[6];

	if(wifi_type == SOFT_AP)
	{
		//printf("get_ip_stat soft_ap\n");
		if(WM_WIFI_JOINED != tls_wifi_softap_get_state())
		{
			printf("ap not create\n");
			return -1;		
		}
		tls_get_softap_link_status(&lk);
		out_net_para->dhcp = DHCP_SERVER;
		buf = inet_ntoa(lk.ip);
		memcpy(out_net_para->ip, buf, 16);
		buf = inet_ntoa(lk.gw);
		memcpy(out_net_para->gate, buf, 16);
		buf = inet_ntoa(lk.netmask);
		memcpy(out_net_para->mask, buf, 16);	
		buf = inet_ntoa(lk.dns1);
		memcpy(out_net_para->dns, buf, 16);			
		buf = hostapd_get_mac();
		sprintf(out_net_para->mac, "%02X%02X%02X%02X%02X%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);	
		memcpy(out_net_para->broadcastip, "255.255.255.255", 16);

	}
	else if(wifi_type == STATION)
	{
		if(WM_WIFI_JOINED != tls_wifi_get_state())
		{
			printf("sta not connect\n");
			return -1;
		}
		tls_get_link_status(&lk);
    	tls_param_get(TLS_PARAM_ID_IP, &ip_param, FALSE);
		if(ip_param.dhcp_enable)
			out_net_para->dhcp = DHCP_CLIENT;
		else
			out_net_para->dhcp = DHCP_DISABLE;
		buf = inet_ntoa(lk.ip);
		memcpy(out_net_para->ip, buf, 16);
		buf = inet_ntoa(lk.gw);
		memcpy(out_net_para->gate, buf, 16);
		buf = inet_ntoa(lk.netmask);
		memcpy(out_net_para->mask, buf, 16);	
		buf = inet_ntoa(lk.dns1);
		memcpy(out_net_para->dns, buf, 16);	
		tls_get_mac_addr(mac);
		sprintf(out_net_para->mac, "%02X%02X%02X%02X%02X%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
		memcpy(out_net_para->broadcastip, "255.255.255.255", 16);
	}
    
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
	struct tls_curr_bss_t bss;
	
	if(WM_WIFI_JOINED != tls_wifi_get_state())
	{
		out_stat->is_connected = 0;
		out_stat->wifi_strength = 0;
		memset(out_stat->ssid, 0, 32);
		memset(out_stat->bssid, 0, 6);
		out_stat->channel = 0;
		return 0;
	}
	out_stat->is_connected = 1;
	tls_wifi_get_current_bss(&bss);
	out_stat->wifi_strength = (char)bss.rssi;
	memcpy(out_stat->ssid, bss.ssid, bss.ssid_len);	//ע��ϵͳ�е�SSIDֻ��32�ֽڣ���û�г���
	out_stat->ssid[bss.ssid_len] = '\0';
	memcpy(out_stat->bssid, bss.bssid, 6);
	out_stat->channel = bss.channel;
	
    return 0;
}

static void scan_cb(void)
{
    char *buf;
    u32 buflen;
	int i;
	int err;
	hal_wifi_module_t *m;
    hal_wifi_scan_result_t *result = NULL;
	struct iw_scan_bss *wsr;
    struct iw_bss_info *bss_info;
	ap_list_t *res;

    buflen = 2000;
    buf = tls_mem_alloc(buflen);
    if (!buf)
    {
		goto end;
	}
        
    err = tls_wifi_get_scan_rslt((u8 *)buf, buflen);
    if (err) 
	{
        goto end;
    }

	wsr = (struct iw_scan_bss *)buf;
	bss_info = (struct iw_bss_info *)(buf + 8);

	result = tls_mem_alloc(sizeof(hal_wifi_scan_result_t));
	if(!result)
	{
		goto end;
	}

	result->ap_num = (char)wsr->count;
	result->ap_list = tls_mem_alloc(result->ap_num * sizeof(ap_list_t));
	if(!result->ap_list)
	{
		printf("result->ap_list mlc err\n");
		goto end;
	}
	for(i=0; i<result->ap_num; i++)
	{
		res = result->ap_list + i;
		memset(res->ssid, 0, 32);
		if (bss_info->ssid_len < 32){
			memcpy(res->ssid, bss_info->ssid, bss_info->ssid_len);
		}else{
			memcpy(res->ssid, bss_info->ssid, 31);
		}

		res->ap_power = (char)bss_info->rssi;
		bss_info ++;
	}

	m = hal_wifi_get_default_module();

	if(m->ev_cb && m->ev_cb->scan_compeleted)
	{
		 m->ev_cb->scan_compeleted(m, result, NULL);	
	}
	
end:
	if(buf)
	{
		tls_mem_free(buf);
	}
	if(result)
	{
		if(result->ap_list)
		{
			tls_mem_free(result->ap_list);
		}
		tls_mem_free(result);
	}
}

static void start_scan(hal_wifi_module_t *m)
{	
	tls_wifi_scan_result_cb_register(scan_cb);
	tls_wifi_scan();
}


void test_start_scan(void)
{	
	int ret;
	
	tls_wifi_scan_result_cb_register(scan_cb);
	ret = tls_wifi_scan();
	printf("test_start_scan:%d\n", ret);
}


static void scan_cb_adv(void)
{
    char *buf;
    u32 buflen;
	int i;
	int err;
	hal_wifi_module_t *m;
    hal_wifi_scan_result_adv_t *result_adv = NULL;
	struct iw_scan_bss *wsr;
    struct iw_bss_info *bss_info;
	ap_list_adv_t *res_adv;


    buflen = 2000;
    buf = tls_mem_alloc(buflen);
    if (!buf)
    {
		goto end;
	}
        
    err = tls_wifi_get_scan_rslt((u8 *)buf, buflen);
    if (err) 
	{
        goto end;
    }

	wsr = (struct iw_scan_bss *)buf;
	bss_info = (struct iw_bss_info *)(buf + 8);

	result_adv = tls_mem_alloc(sizeof(hal_wifi_scan_result_adv_t));
	if(!result_adv)
	{
		goto end;
	}

	result_adv->ap_num = wsr->count;
	result_adv->ap_list = tls_mem_alloc(result_adv->ap_num * sizeof(ap_list_adv_t));
	if(!result_adv->ap_list)
	{
		goto end;
	}
	for(i=0; i<result_adv->ap_num; i++)
	{
		res_adv = result_adv->ap_list + i;	
		memset(res_adv->ssid, 0, 32);
		if (bss_info->ssid_len < 32){
			memcpy(res_adv->ssid, bss_info->ssid, bss_info->ssid_len);
		}else{
			memcpy(res_adv->ssid, bss_info->ssid, 31);
		}

		res_adv->ap_power = (char)bss_info->rssi;
		memcpy(res_adv->bssid, bss_info->bssid, sizeof(res_adv->bssid));
		res_adv->channel = bss_info->channel;
		res_adv->security = bss_info->privacy;
		bss_info ++;
	}

	m = hal_wifi_get_default_module();

	if(m->ev_cb && m->ev_cb->scan_adv_compeleted)
	{
		 m->ev_cb->scan_adv_compeleted(m, result_adv, NULL);	
	}
	
end:
	if(buf)
	{
		tls_mem_free(buf);
	}
	if(result_adv)
	{
		if(result_adv->ap_list)
		{
			tls_mem_free(result_adv->ap_list);
		}
		tls_mem_free(result_adv);
	}
}

static void start_scan_adv(hal_wifi_module_t *m)
{
	tls_wifi_scan_result_cb_register(scan_cb_adv);
	tls_wifi_scan();
}



static int power_off(hal_wifi_module_t *m)
{
	tls_wl_if_ps(0);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
	tls_wl_if_ps(1);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
	u8 wmode;
	
	tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, (bool)TRUE);
	if(wmode & IEEE80211_MODE_INFRA)
	{
		tls_wifi_disconnect();
	}
	if(wmode & IEEE80211_MODE_AP)
	{
		tls_wifi_softap_destroy();
	}
}

static int suspend_station(hal_wifi_module_t *m)
{
    tls_wifi_disconnect();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    tls_wifi_softap_destroy();
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
	static u8 lastCh5[3] = {0};
	
	if(ch < 1 || ch > 14)
	{
		return 1;
	}
	if(ch <= 4)
	{
		tls_wifi_change_chanel_width(ch-1, 3);
	}
	else if(ch <= 7)
	{
		if(lastCh5[ch-5])
		{
			lastCh5[ch-5] = 0;
			tls_wifi_change_chanel_width(ch-1, 3);
		}
		else
		{
			lastCh5[ch-5] = 1;
			tls_wifi_change_chanel_width(ch-1, 2);		
		}
	}
	else if(ch <= 13)
	{
		tls_wifi_change_chanel_width(ch-1, 2);	
	}
	else 
	{
		tls_wifi_change_chanel_width(ch-1, 0);	
	}

    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{
	printf("start_monitor\n");
	tls_wifi_disconnect();
	tls_wifi_set_listen_mode(1);
}

static void stop_monitor(hal_wifi_module_t *m)
{
	printf("stop_monitor\n");
    tls_wifi_set_listen_mode(0);
	tls_wifi_data_ext_recv_cb_register(NULL);
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	printf("register_monitor_cb\n");
	tls_wifi_data_ext_recv_cb_register(fn);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
	tls_wifi_mgmt_ext_recv_cb_register(fn);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, u8 *buf, int len)
{
	tls_wifi_send_raw_data(buf, len-4);		//LEN-FCS
}

static int get_wireless_info(hal_wifi_module_t *m, void *wireless_info)
{
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;
    struct tls_curr_bss_t bss = {0};

    printf("get wireless info\r\n");

    if (info == NULL)
        return -1;

    if (WM_WIFI_JOINED != tls_wifi_get_state()) {
        return -1;
    }

    tls_wifi_get_current_bss(&bss);
    if (bss.rssi > 0) {
        if (bss.rssi >= 128)
            bss.rssi = 127;
        bss.rssi -= 128;
    }
    info->rssi = bss.rssi;

    return 0;
}

hal_wifi_module_t aos_wifi_w600 = {
    .base.name           = "aos_wifi_w600",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .set_mac_addr        =  wifi_set_mac_addr,
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
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .get_wireless_info   = get_wireless_info,
};

void w600_wifi_register(void)
{
	hal_wifi_register_module(&aos_wifi_w600);
}
