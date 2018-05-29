/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/aos.h>
#include <netmgr.h>
#include <hal/wifi.h>

#define TAG "netmgrapp"

#ifdef TEST_WIFI_HAL_ONLY
#include <string.h>
static void start_wifi(char *ssid, char *pw)
{
    hal_wifi_init_type_t type;

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    strncpy(type.wifi_key, pw, sizeof(type.wifi_key) - 1);
    hal_wifi_start(NULL, &type);
}

static void start_scan()
{
    hal_wifi_start_scan(NULL);
}

static void start_scan_adv()
{
    hal_wifi_start_scan_adv(NULL);
}

static void monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    printf("In %s\r\n", __func__);
}

static void mngt_monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    printf("In %s\r\n", __func__);
}

static void start_monitor(uint8_t type)
{
    if (type == 0) {
        hal_wifi_stop_wifi_monitor(NULL);
        hal_wifi_register_monitor_cb(NULL, monitor_cb);
        hal_wifi_start_wifi_monitor(NULL);
    } else {
        hal_wifi_stop_wifi_monitor(NULL);
        hal_wlan_register_mgnt_monitor_cb(NULL, mngt_monitor_cb);
    }
}

static void send_80211()
{

}

static void get_mac(uint8_t *mac)
{
    hal_wifi_get_mac_addr(NULL, mac);
}

static int get_ip_stat(hal_wifi_ip_stat_t *ip_stat)
{
    return hal_wifi_get_ip_stat(NULL, ip_stat, STATION);
}

static void handle_test_wifi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *ctype = argc > 1 ? argv[1] : "";
    char *ssid, *passwd;

    aos_cli_printf("Hello, testing WiFi HAL ...\r\n");

    if (strcmp(ctype, "start") == 0) {
        if (argc != 4) {
            LOGE(TAG, "%s %d: invalid argument.", __func__, __LINE__);
            return;
        }

        ssid = argv[2];
        passwd = argv[3];
        start_wifi(ssid, passwd);
    } else if (strcmp(ctype, "scan") == 0) {
        start_scan();
    } else if (strcmp(ctype, "scan_adv") == 0) {
        start_scan_adv();
    } else if (strcmp(ctype, "get_mac") == 0) {
        uint8_t mac[6];
        get_mac(mac);
        printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else if (strcmp(ctype, "ip_info") == 0) {
        hal_wifi_ip_stat_t pnet;
        if (get_ip_stat(&pnet) == 0) {
            printf("IP : %s, gw : %s, mask : %s", pnet.ip, pnet.gate, pnet.mask);
        }
    } else if (strcmp(ctype, "monitor") == 0) {
        uint8_t monitor_type = 0; // default data monitor
        if (argc == 3 && strcmp(argv[2], "mngt") == 0) monitor_type = 1;
        start_monitor(monitor_type);
    } else if (strcmp(ctype, "80211send") == 0) {
        send_80211();
    } else {
        aos_cli_printf(TAG, "Not supported yet - cmd type: %s.\r\n", ctype);
    }
}
#endif // TEST_WIFI_HAL_ONLY

static void handle_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_cli_printf("Hello world\r\n");
}

static struct cli_command ncmds[] = {
    {
        .name = "test",
        .help = "test",
        .function = handle_test_cmd
    },
#ifdef TEST_WIFI_HAL_ONLY
    {
        .name = "test_wifi_hal",
        .help = "test_wifi_hal [start|scan|scan_adv|monitor [mngt]|80211send|get_mac|ip_info|all]",
        .function = handle_test_wifi_cmd
    }
#endif
};

int application_start(int argc, char *argv[])
{
    aos_cli_register_commands(&ncmds[0], sizeof(ncmds) / sizeof(ncmds[0]));
#ifndef TEST_WIFI_HAL_ONLY
    netmgr_init();
    netmgr_start(false);
#endif
    aos_loop_run();
    return 0;
}
