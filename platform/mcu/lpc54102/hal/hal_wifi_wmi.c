/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * POSIX getopt for Windows
 * Code given out at the 1985 UNIFORUM conference in Dallas.
 *
 * From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
 * Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
 * Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
 * Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
 * From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
 * Newsgroups: mod.std.unix
 * Subject: public domain AT&T getopt source
 * Message-ID: <3352@ut-sally.UUCP>
 * Date: 3 Nov 85 19:34:15 GMT
 * Date-Received: 4 Nov 85 12:25:09 GMT
 * Organization: IEEE/P1003 Portable Operating System Environment Committee
 * Lines: 91
 * Approved: jsq@ut-sally.UUC
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */

#include <string.h>
#include <stdio.h>
#include <k_api.h>
#include "hal/wifi.h"
#include "qcom_api.h"
#include "fsl_debug_console.h"

#define WIFI_CONNECT_TIMEOUT_MS 20000 //20s
#define WIFI_DHCP_RETRY_TIME 20
static int8_t _traceQcomApi = 1;

static volatile int32_t devId = 0;
static volatile int8_t _isConnected = 0;
static ksem_t wifi_event_sem;

   
#define UINT32_IPADDR_TO_CSV_BYTES(a) (((a) >> 24) & 0xFF), (((a) >> 16) & 0xFF), (((a) >> 8) & 0xFF), ((a)&0xFF)
static void parseIPv4(char *in, uint8_t *addr)
{
    int8_t i, j;
    for (i = 3; i >= 0; i--) {
        for (j = 0; j < 3; j++) {
            if(('0' <= in[j]) && (in[j] <= '9')) {
                addr[i] *= 10;
                addr[i] += (in[j] - 0x30);
            } else {
                break;
            }
        }
        in += (j + 1);
    }
}

static void printError(uint32_t value, const char *funcName)
{
    PRINTF("ERROR: %s() returned %d\r\n", funcName, value);
}

static int isQcomError(A_STATUS status, const char *funcName)
{
    if (status != A_OK)
    {
        printError(status, funcName);
    }
    else if (_traceQcomApi)
    {
        PRINTF("%s() OK\r\n", funcName);
    }
    return (status != A_OK);
}

static void hal_wifi_onConnect(uint8_t event, uint8_t devId, char *bssid, uint8_t bssConn)
{
    switch (event)
    {
        case 1:
            if (devId == 0)
            {
                PRINTF("%s connected\r\n", bssConn ? "AP" : "CLIENT");
            }
            else
            {
                PRINTF("Connected\r\n");
            }
            _isConnected = 1;
            // NOTE that station is not fully connected until PEER_FIRST_NODE_JOIN_EVENT
            krhino_sem_give(&wifi_event_sem);
            break;
        case 0:
            _isConnected = 0;
            if (devId == 0)
            {
                PRINTF("%s disconnect\r\n", bssConn ? "AP" : "CLIENT");
            }
            krhino_sem_give(&wifi_event_sem);
            break;			
        case INVALID_PROFILE:
            // this event is used to indicate RSNA failure
            _isConnected = 0;
            PRINTF("4 way handshake failure for device=%d n", devId);
            break;
        case PEER_FIRST_NODE_JOIN_EVENT:
            // this event is used to RSNA success
            PRINTF("4 way handshake success for device=%d\r\n", devId);
            break;
        default:
            PRINTF("code %d\r\n");
    }
}

static void hal_dhcps_cb(void)
{
    PRINTF("DHCP Setup Event\r\n");
    krhino_sem_give(&wifi_event_sem);
}


static int wifi_init(hal_wifi_module_t *m)
{
    A_STATUS status = A_OK;
    PRINTF("WIFI_Init\r\n");

    status = WIFISHIELD_Init();
    PRINTF("WIFISHIELD_Init return %d\r\n", status);
    if (status != A_OK) {
        return status;
    }

    status = wlan_driver_start();
    PRINTF("wlan_driver_start return %d\r\n", status);
    if (status != A_OK) {
        return status;
    }

    krhino_sem_create(&wifi_event_sem, "wifi_event_sem", 0);

    return status;
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    mac[0] = 0x84; mac[1] = 0x5D; mac[2] = 0xD7; mac[3] = 0x68; mac[4] = 0x3B; mac[5] = 0x91;    
    PRINTF("wifi_get_mac_addr\r\n");
}


static A_STATUS wifi_start_station(hal_wifi_init_type_t *init_para)
{
    A_STATUS status;
    QCOM_SSID q_ssid;
    QCOM_PASSPHRASE q_passphrase;
    WLAN_CRYPT_TYPE crypto_mode = WLAN_CRYPT_AES_CRYPT;
    WLAN_AUTH_MODE authen_mode = WLAN_AUTH_WPA2_PSK;

    // Set station mode to station (0) or AP (1)
    status = qcom_op_set_mode(devId, QCOM_WLAN_DEV_MODE_STATION);
    if(isQcomError(status, "qcom_op_set_mode"))
        return status;

    memcpy(q_ssid.ssid, init_para->wifi_ssid, 33);
    // NOTE: qcom API requires to first set a valid SSID (before auth, cipher and passphrase)
    status = qcom_set_ssid(devId, &q_ssid);
    if(isQcomError(status, "qcom_set_ssid"))
        return status;

    status = qcom_sec_set_encrypt_mode(devId, crypto_mode);
    if(isQcomError(status, "qcom_sec_set_encrypt_mode"))
        return status;

    // Note that only 4 of all modes listed in QCA 80-Y9106-1 are supported!
    // The modes are: WLAN_AUTH_NONE, WLAN_AUTH_WPA_PSK, WLAN_AUTH_WPA2_PSK, WLAN_AUTH_WEP
    status = qcom_sec_set_auth_mode(devId, WLAN_AUTH_WPA2_PSK);
    if(isQcomError(status, "qcom_sec_set_auth_mode"))
        return status;

    memcpy(q_passphrase.passphrase, init_para->wifi_key, 65);
    // NOTE: The driver insists that the SSID is configured *before* the passphrase
    status = qcom_sec_set_passphrase(devId, &q_passphrase);
    if(isQcomError(status, "qcom_sec_set_passphrase"))
        return status;

    // The connect callback is actually used for four different callbacks:
    //     onConnect(1 (TRUE), uint8_t devId, uint8_t *ssid, uint8_t bssMac[6]);
    //     onDisconnect(status, uint8_t devId, uint8_t *ssid, uint8_t bssMac[6]);
    //         where status = 0x10 (INVALID_PROFILE) on ?
    //                      =    0 on normal disconnect
    //     onRSNASuccessEvent(uint8_t code, uint8_t devId, NULL, 0)
    //     onBitRateEvent_tx(wmi_rateTable[rateIndex][0], devId, NULL, 0);
    // It is not possible to discern the onBitRateEvent_tx event from the others
    status = qcom_set_connect_callback(devId, (void *)hal_wifi_onConnect);
    if(isQcomError(status, "qcom_set_connect_callback"))
        return status;

    status = qcom_commit(devId);
    isQcomError(status, "qcom_commit");

    return status;
}

static A_STATUS wifi_start_softAP(hal_wifi_init_type_t *init_para)
{
    A_STATUS status = A_OK;
    uint32_t ap_addr = 0;
    uint32_t ap_mask = 0;
    uint32_t ap_gateway = 0;

    status = qcom_op_set_mode(devId, QCOM_WLAN_DEV_MODE_AP);
    if (status != A_OK) {
        return status;
    }
	
    /* Set SSID */
    status = qcom_set_ssid(devId, (QCOM_SSID*)"HomeWifi");
    if (status != A_OK) {
        return status;
    }

    /* Set encryption mode */
    status = qcom_sec_set_encrypt_mode(devId, WLAN_CRYPT_NONE);
    if (status != A_OK) {
        return status;
    }

    /* Set auth mode */
    status = qcom_sec_set_auth_mode(devId, WLAN_AUTH_NONE);
    if (status != A_OK) {
        return status;
    }

    /* Set password */
    status = qcom_sec_set_passphrase(devId, (QCOM_PASSPHRASE*)"");
    if (status != A_OK) {
        return status;
    }

    status = qcom_set_connect_callback(devId, (void *)hal_wifi_onConnect);
    if (status != A_OK) {
        return status;
    }

    status = qcom_commit(devId);
    if (status != A_OK) {
        return status;
    }

    parseIPv4(init_para->local_ip_addr, &ap_addr);
    parseIPv4(init_para->net_mask, &ap_mask);
    parseIPv4(init_para->gateway_ip_addr, &ap_gateway);

    PRINTF("Configure AP IP:0x%x, MASK:0x%x, GW:0x%x\r\n", ap_addr, ap_mask, ap_gateway);
    status = qcom_ipconfig(devId, QCOM_IPCONFIG_STATIC, &ap_addr, &ap_mask, &ap_gateway);
    if (status != A_OK) {
        return status;
    }

    return A_OK;
}

static int wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    A_STATUS status;
    uint32_t retry = 0;
    kstat_t wifi_sem_state;
    uint32_t q_addr = 0, q_mask = 0, q_gateway = 0;
    uint32_t dnsServers[3];
    uint32_t dnsnum = 0;
    hal_wifi_ip_stat_t ip_stat;

    PRINTF("wifi_start parameter: Mode - %d\r\n", init_para->wifi_mode);
    PRINTF("wifi_start parameter: SSID - %s\r\n", init_para->wifi_ssid);
    PRINTF("wifi_start parameter: PASSWORD - %s\r\n", init_para->wifi_key);

    if(init_para->wifi_mode == STATION)
    {
        status = wifi_start_station(init_para);
        if (status != A_OK) {
            return status;
        }

        wifi_sem_state = krhino_sem_take(&wifi_event_sem, krhino_ms_to_ticks(WIFI_CONNECT_TIMEOUT_MS));
        if (!((RHINO_SUCCESS == wifi_sem_state) && _isConnected)) {
            PRINTF("krhino_sem_take state = %d, isConnected = %d\r\n", wifi_sem_state, _isConnected);
            return A_TIMEOUT;
        }

        if (init_para->dhcp_mode == DHCP_DISABLE) {
            parseIPv4(init_para->local_ip_addr, &q_addr);
            parseIPv4(init_para->net_mask, &q_mask);
            parseIPv4(init_para->gateway_ip_addr, &q_gateway);
        }
        do {
            if (init_para->dhcp_mode == DHCP_CLIENT) {
                status = qcom_ipconfig(devId, QCOM_IPCONFIG_DHCP, &q_addr, &q_mask, &q_gateway);				
            } else if (init_para->dhcp_mode == DHCP_DISABLE) {			    
                status = qcom_ipconfig(devId, QCOM_IPCONFIG_STATIC, &q_addr, &q_mask, &q_gateway);
            }
            A_MDELAY(50);
            q_addr = q_mask = q_gateway = 0;
            status = qcom_ipconfig(devId, QCOM_IPCONFIG_QUERY, &q_addr, &q_mask, &q_gateway);
            if (q_addr != 0)
                break;
        } while(retry < WIFI_DHCP_RETRY_TIME);

        if (init_para->dhcp_mode == DHCP_CLIENT) 
        {
            sprintf(init_para->local_ip_addr,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_addr));
            sprintf(init_para->net_mask,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_mask));
            sprintf(init_para->gateway_ip_addr,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_gateway));
            init_para->local_ip_addr[15] = 0;
            init_para->net_mask[15] = 0;
            init_para->gateway_ip_addr[15] = 0;
        }

        PRINTF("IP: %d.%d.%d.%d\r\n", UINT32_IPADDR_TO_CSV_BYTES(q_addr));
        PRINTF("GATEWAY: %d.%d.%d.%d\r\n",UINT32_IPADDR_TO_CSV_BYTES(q_gateway));
        PRINTF("MASK: %d.%d.%d.%d\r\n", UINT32_IPADDR_TO_CSV_BYTES(q_mask));

        status = qcom_dns_server_address_get(dnsServers, &dnsnum);
        if (isQcomError(status, "qcom_dns_server_address_get"))
        {
            PRINTF("WARNING: No DNS servers returned by DHCP\r\n");
            PRINTF(" You will NOT be able to resolve domain names\r\n");
            return status;
        }

        for (int i = 0; i < dnsnum; i++)
        {
            PRINTF("DNS %d: %d.%d.%d.%d\r\n", i, UINT32_IPADDR_TO_CSV_BYTES(dnsServers[i]));
        }		
    } 
    else /* WIFI Mode SOFT_AP */
    {
        status = wifi_start_softAP(init_para);
        if (status != A_OK) {
            return status;
        }
        wifi_sem_state = krhino_sem_take(&wifi_event_sem, krhino_ms_to_ticks(WIFI_CONNECT_TIMEOUT_MS));
        if (!((RHINO_SUCCESS == wifi_sem_state) && _isConnected)) {
            return A_TIMEOUT;
        }
#define WIFI_SOFTAP_DHCP_BEGIN 0xc0a80164 //192.168.1.100
#define WIFI_SOFTAP_DHCP_END 0xc0a80196 //192.168.1.150
#define WIFI_SOFTAP_DHCP_LEASE_TIME (3600)
        status = qcom_dhcps_set_pool(devId, WIFI_SOFTAP_DHCP_BEGIN, WIFI_SOFTAP_DHCP_END, WIFI_SOFTAP_DHCP_LEASE_TIME);
        if (status != A_OK) {
            return status;
        }
        qcom_dhcps_register_cb(devId, hal_dhcps_cb);

        wifi_sem_state = krhino_sem_take(&wifi_event_sem, RHINO_WAIT_FOREVER);
        if (!(RHINO_SUCCESS == wifi_sem_state)) {
            return A_TIMEOUT;
        }
    }

    if (m->ev_cb && m->ev_cb->ip_got) {
        PRINTF("m->ev_cb->ip_got \r\n");
		wifi_get_ip_stat(m, &ip_stat, STATION);
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }
    return status;
}

static int wifi_start_adv(hal_wifi_module_t *m,hal_wifi_init_type_adv_t *init_para_adv)
{
    PRINTF("wifi_start_adv\r\n");
    return 0;
}

static int wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    uint32_t q_addr = 0, q_mask = 0, q_gateway = 0;
    A_STATUS status;

    q_addr = q_mask = q_gateway = 0;
    status = qcom_ipconfig(devId, QCOM_IPCONFIG_QUERY, &q_addr, &q_mask, &q_gateway);

    sprintf(out_net_para->ip,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_addr));
    sprintf(out_net_para->mask,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_mask));
    sprintf(out_net_para->gate,"%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(q_gateway));

    PRINTF("IP %s \r\n", out_net_para->ip);
    PRINTF("GATEWAY %s\r\n", out_net_para->gate);
    PRINTF("MASK %s\r\n", out_net_para->mask);

    return 0;
}

static int wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    PRINTF("wifi_get_link_stat\r\n");
    return 0;
}

static void wifi_start_scan(hal_wifi_module_t *m)
{
    A_STATUS status;
    QCOM_BSS_SCAN_INFO *scanResult;
    int16_t scanNumResults = 0;
    hal_wifi_scan_result_t scan_result;
    ap_list_t scan_list[12] = {0};
    PRINTF("wifi_start_scan\r\n");

    status = qcom_set_scan(devId, NULL);
    isQcomError(status, "qcom_set_scan");

    // NOTE: calling qcom_get_scan() before qcom_set_scan() is the same as suspending for 5s,
    // i.e. nothing happens for 5s where after 0 is returned
    // NOTE: A maximum of 12 results are returned
    status = qcom_get_scan(devId, &scanResult, &scanNumResults);
    isQcomError(status, "qcom_get_scan");

    scan_result.ap_num = scanNumResults;
    scan_result.ap_list = &scan_list;

    for (int i = 0; i < scanNumResults; i++)
    {
        QCOM_BSS_SCAN_INFO *scr = &scanResult[i];
        int ssidLen = scr->ssid_len;
        scr->ssid[ssidLen] = 0;

        PRINTF("%2d %2d %4d  %02x:%02x:%02x:%02x:%02x:%02x  %s\r\n", i, scr->channel, scr->rssi, scr->bssid[0],
               scr->bssid[1], scr->bssid[2], scr->bssid[3], scr->bssid[4], scr->bssid[5], scr->ssid);
        memcpy(scan_list[i].ssid, scr->ssid, scr->ssid_len);
		scan_list[i].ap_power = scr->preamble;
    }
	
    if (m->ev_cb && m->ev_cb->scan_compeleted) {
        PRINTF("m->ev_cb->scan_compeleted\r\n");
        m->ev_cb->scan_compeleted(m, &scan_result, NULL);
    }
}

static void wifi_start_scan_adv(hal_wifi_module_t *m)
{
    A_STATUS status;
    QCOM_BSS_SCAN_INFO *scanResult;
    int16_t scanNumResults = 0;
    hal_wifi_scan_result_adv_t scan_adv_result;
    ap_list_adv_t scan_adv_list[12] = {0};

    status = qcom_set_scan(devId, NULL);
    isQcomError(status, "qcom_set_scan");

    // NOTE: calling qcom_get_scan() before qcom_set_scan() is the same as suspending for 5s,
    // i.e. nothing happens for 5s where after 0 is returned
    // NOTE: A maximum of 12 results are returned
    status = qcom_get_scan(devId, &scanResult, &scanNumResults);
    isQcomError(status, "qcom_get_scan");

    scan_adv_result.ap_num = scanNumResults;
    scan_adv_result.ap_list = &scan_adv_list;

    for (int i = 0; i < scanNumResults; i++)
    {
        QCOM_BSS_SCAN_INFO *scr = &scanResult[i];
        int ssidLen = scr->ssid_len;
        scr->ssid[ssidLen] = 0;

        PRINTF("%2d %2d %4d  %02x:%02x:%02x:%02x:%02x:%02x  %s\r\n", i, scr->channel, scr->rssi, scr->bssid[0],
               scr->bssid[1], scr->bssid[2], scr->bssid[3], scr->bssid[4], scr->bssid[5], scr->ssid);
        memcpy(scan_adv_list[i].ssid, scr->ssid, scr->ssid_len);
        memcpy(scan_adv_list[i].bssid, scr->bssid, 6);
		scan_adv_list[i].ap_power = scr->preamble;
		scan_adv_list[i].channel = scr->channel;
    }
	
    if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
        PRINTF("m->ev_cb->scan_adv_compeleted\r\n");
        m->ev_cb->scan_adv_compeleted(m, &scan_adv_list, NULL);
    }
}

static int wifi_power_off(hal_wifi_module_t *m)
{
    PRINTF("wifi_power_off NOT Implemented");
    return 0;
}

static int wifi_power_on(hal_wifi_module_t *m)
{
    PRINTF("wifi_power_on NOT Implemented");
    return 0;
}

static int wifi_suspend(hal_wifi_module_t *m)
{
    PRINTF("wifi_suspend NOT Implemented");
    return 0;
}

static int wifi_suspend_station(hal_wifi_module_t *m)
{
    PRINTF("wifi_suspend_station NOT Implemented");
    return 0;
}

static int wifi_suspend_soft_ap(hal_wifi_module_t *m)
{
    PRINTF("wifi_suspend_soft_ap NOT Implemented");
    return 0;
}

static int wifi_set_channel(hal_wifi_module_t *m, int ch)
{
    PRINTF("wifi_set_channel NOT Implemented");
    return 0;
}

static void wifi_start_monitor(hal_wifi_module_t *m)
{
    PRINTF("wifi_start_monitor NOT Implemented");
}

static void wifi_stop_monitor(hal_wifi_module_t *m)
{
    PRINTF("wifi_stop_monitor NOT Implemented");
}

static void wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    PRINTF("wifi_register_monitor_cb NOT Implemented");
}

static void wifi_register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    PRINTF("wifi_register_wlan_mgnt_monitor_cb NOT Implemented");
}

static int wifi_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    PRINTF("wifi_wlan_send_80211_raw_frame NOT Implemented");
    return 0;
}

hal_wifi_module_t qca_4002_wmi = {
    .base.name = "qca_4002_wmi",
    .ev_cb = NULL,
    .init = wifi_init,
    .get_mac_addr = wifi_get_mac_addr,
    .start = wifi_start,
    .start_adv = wifi_start_adv,
    .get_ip_stat = wifi_get_ip_stat,
    .get_link_stat = wifi_get_link_stat,
    .start_scan = wifi_start_scan,
    .start_scan_adv = wifi_start_scan_adv,
    .power_off = wifi_power_off,
    .power_on = wifi_power_on,
    .suspend = wifi_suspend,
    .suspend_station = wifi_suspend_station,
    .suspend_soft_ap = wifi_suspend_soft_ap,
    .set_channel = wifi_set_channel,
    .start_monitor = wifi_start_monitor,
    .stop_monitor = wifi_stop_monitor,
    .register_monitor_cb = wifi_register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = wifi_register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wifi_wlan_send_80211_raw_frame
};


