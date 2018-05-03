#include "joylink_main.h"
#include "idmanage/pbuf.h"
#include "wificonf.h"
#include "wifinetstack.h"
#include "wifi_api.h"
#include "ieee80211.h"
#include "osal/osal.h"
#include "phy/drv_phy.h"
#include "joylink_smnt.h"
#include "smart_config.h"
#include "wifi_api.h"

static joylink_smnt_result_t joylink_wifi_result;
static u8 time_interval = 150;
static OsTimer ch_change_timer;
int start = 0;

void joylinkwificbfunc(WIFI_RSP *msg)
{
    printf("=========\n");
    printf("joylinkwificbfunc wifistatus = %d\n", msg->wifistatus);
    
    uint8_t dhcpen;
    u8 mac[6];
    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;

    if(msg->wifistatus == 1)
    {
        if(msg->id == 0)
            get_if_config_2("et0", &dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver, mac, 6);
        else
            get_if_config_2("et1", &dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver, mac, 6);
        printf("Wifi Connect\n");
        printf("STA%d:\n", msg->id);
        printf("mac addr        - %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("ip addr         - %d.%d.%d.%d\n", ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3]);
        printf("netmask         - %d.%d.%d.%d\n", submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3]);
        printf("default gateway - %d.%d.%d.%d\n", gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);
        printf("DNS server      - %d.%d.%d.%d\n", dnsserver.u8[0], dnsserver.u8[1], dnsserver.u8[2], dnsserver.u8[3]);

    }
    else
    {
        printf("Wifi Disconnect\n");
    }
}

void joylink_scan_cbfunc(void *data)
{
    int ret = 0;
    printf("scan finish\n");

    ret = set_wifi_config(joylink_wifi_result.jd_ssid, joylink_wifi_result.jd_ssid_len, joylink_wifi_result.jd_password, joylink_wifi_result.jd_password_len, NULL, 0);

    if(ret == 0)
        ret = wifi_connect(joylinkwificbfunc);

}
int joylink_connect_ap(joylink_smnt_result_t *pResult)
{
    int ret = 0;
    memset(&joylink_wifi_result, 0, sizeof(joylink_wifi_result));
    memcpy(&joylink_wifi_result, pResult, sizeof(joylink_wifi_result));
    DUT_wifi_start(DUT_STA);
    scan_AP(joylink_scan_cbfunc);
    return ret;
}

void joylink_get_result_cb(joylink_smnt_result_t result)
{
    int ret = 0;
    printf("==============================\n");
    printf("flag %d\n", result.smnt_result_status);
    printf("ssid %s, len %d\n", result.jd_ssid, result.jd_ssid_len);
    printf("pswd %s, len %d\n", result.jd_password, result.jd_password_len);
    printf("==============================\n");
    joylink_stop();

    if( result.smnt_result_status == smnt_result_ok )
        joylink_connect_ap(&result);

}

void joylink_switch_ch_cb(unsigned char ch)
{
    set_channel((u8) ch);
    //drv_phy_channel_change((int)ch, TRUE);    
}

void ch_change_handler(void)
{
    joylink_smnt_cyclecall();
    OS_TimerStart(ch_change_timer);
}

int init_scan_ch_timer(void)
{
    printf("%s\n", __func__);
    if( OS_TimerCreate(&ch_change_timer, time_interval, (u8)FALSE, NULL, (OsTimerHandler)ch_change_handler) == OS_FAILED)
        return OS_FAILED;

    OS_TimerStart(ch_change_timer);
    return OS_SUCCESS;
}

void sniffer_cb(packetinfo *packet)
{
    PHEADER_802_11 hdr = (PHEADER_802_11)packet->data;
    joylink_smnt_datahandler(hdr, packet->len);
}


void joylink_init(u8 *key)
{
    printf("%s\n", __func__);

    if (start)
        return;

    attach_sniffer_cb(RECV_DATA_BCN, &sniffer_cb, 512);
    start_sniffer_mode();

    init_scan_ch_timer();
    joylink_smnt_gobal.switch_channel_callback = &joylink_switch_ch_cb;
    joylink_smnt_gobal.get_result_callback = &joylink_get_result_cb;
    memcpy(joylink_smnt_gobal.secretkey, key, 16);
    joylink_smnt_init(joylink_smnt_gobal);
    start = 1;
}

void joylink_stop(void)
{
    printf("%s\n", __func__);
    if (start) {
        if (ch_change_timer) {
            OS_TimerStop(ch_change_timer);
            ch_change_timer = NULL;
        }
        deattach_sniffer_cb();
        stop_sniffer_mode();
        joylink_smnt_release();
        start = 0;
    }
}
