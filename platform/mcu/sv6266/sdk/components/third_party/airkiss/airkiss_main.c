#include "airkiss.h"
#include "airkiss_main.h"
#include "smart_config.h"
#include "ssv_lib.h"
#include "wifi_api.h"

int cur_ch = 0;
static airkiss_result_t airkiss_wifi_result;

airkiss_context_t air_context;
const airkiss_config_t air_conf = {
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
    (airkiss_printf_fn)&printf
};

void airkiss_ch_change_cb(void)
{
    airkiss_change_channel(&air_context);   
}

void airkisswificbfunc(WIFI_RSP *msg)
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
void airkiss_scan_cbfunc(void *data)
{
    int ret = 0;
    printf("scan finish\n");

    ret = set_wifi_config(airkiss_wifi_result.ssid, airkiss_wifi_result.ssid_length, airkiss_wifi_result.pwd, airkiss_wifi_result.pwd_length, NULL, 0);

    if(ret == 0)
        ret = wifi_connect(airkisswificbfunc);

}
int airkiss_connect_ap(airkiss_result_t *pResult)
{
    int ret = 0;
    memset(&airkiss_wifi_result, 0, sizeof(airkiss_wifi_result));
    memcpy(&airkiss_wifi_result, pResult, sizeof(airkiss_wifi_result));
    DUT_wifi_start(DUT_STA);
    scan_AP(airkiss_scan_cbfunc);
    return ret;
}

void airkiss_result(void)
{
    airkiss_result_t air_result;
    airkiss_get_result(&air_context, &air_result);
    printf("# airkiss get result ################\n");
    printf("# SSID: %s, len: %d\n", air_result.ssid, air_result.ssid_length);
    printf("# PAWD: %s, len: %d\n", air_result.pwd,  air_result.pwd_length);
    printf("#####################################\n");

    airkiss_connect_ap(&air_result);
}

void airkiss_sniffer_cb(packetinfo *packet)
{

    airkiss_status_t ret = AIRKISS_STATUS_CONTINUE;
    
    ret = airkiss_recv(&air_context, packet->data, (unsigned short)packet->len);
    if(ret == AIRKISS_STATUS_CHANNEL_LOCKED) {
        printf("airkiss channel locked\n");
        printf("lock ch: %d\n", gwifistatus.run_channel);
        cur_ch = gwifistatus.run_channel;
        auto_ch_switch_stop();
    } else if(ret == AIRKISS_STATUS_COMPLETE) {
        printf("airkiss status complete\n");
        airkiss_stop();
        airkiss_result();
    }

}

void airkiss_start(u8 *key)
{
    u8 *version = NULL;
    int ret = 0;
    version = (u8 *)airkiss_version();
    printf("%s: version %s\n", __func__, version);
    ret = airkiss_init(&air_context, &air_conf);
    if (ret) {
        printf("airkiss_init failed: %d\n", ret);
        return;
    }
#if AIRKISS_ENABLE_CRYPT
    ret = airkiss_set_key(&air_context, (const unsigned char*)key, 16);
    if (ret) {
        printf("airkiss_set_key failed: %d\n", ret);
        return;    
    }
#endif
	attach_sniffer_cb(RECV_DATA_BCN, &airkiss_sniffer_cb, 512);
    attach_channel_change_cb(&airkiss_ch_change_cb);
	start_sniffer_mode();
	auto_ch_switch_start(100);

}

void airkiss_stop(void)
{
    printf("%s\n", __func__);
	auto_ch_switch_stop();
    deattach_channel_change_cb();
	deattach_sniffer_cb();
	stop_sniffer_mode();
}
