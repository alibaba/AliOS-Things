/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <be_osal.h>
#include "cli_ext.h"

#ifdef BE_OS_AOS
#include <aos/aos.h>
#include <netmgr.h>
#endif

// assembel information finished then write \n to indicates finished serialport
// data
#define BE_CLI_REPLY "#be-cli-reply#"
#define BE_CLI_REPLY_SUCCESS "#be-cli-reply#success\n"

#define BE_DEBUGER_DMESG_CMD_REPLY "#be-debuger-dmesg-reply#\n"
#define BE_DEBUGER_REBOOT_CMD_REPLY "#be-debuger-reboot-reply#\n"
#define BE_DEBUGER_RENAME_CMD_REPLY "#be-debuger-rename-reply#\n"
#define BE_DEBUGER_IFCONFIG_CMD_REPLY "#be-debuger-ifconfig-reply#"

extern void sub_call_rename(void *arg);
extern void sub_call_wifi(void *arg);

#define CLI_REPLY printf

static void handle_devdmesg_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    // aos_cli_stop();
    // following code is serial port reply msg
    CLI_REPLY(BE_DEBUGER_DMESG_CMD_REPLY);
}

static void handle_devreboot_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    CLI_REPLY(BE_DEBUGER_REBOOT_CMD_REPLY);
    be_osal_reboot();
}

static void handle_devrename_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    CLI_REPLY(BE_DEBUGER_RENAME_CMD_REPLY);
    be_osal_schedule_call(sub_call_rename, argv[1]);
}

static void handle_devwifi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *ssid = argv[1];
    char *password = argv[2];

    be_osal_wifi_config_t *config = malloc(sizeof(be_osal_wifi_config_t));
    if (NULL == config) {
        return;
    }
    memset(config, 0, sizeof(be_osal_wifi_config_t));

    if ( strlen(ssid) > MAX_SSID_SIZE ||
         strlen(password) > MAX_PWD_SIZE ) {
        return;
    } else {
        strncpy(config->ssid, ssid, strlen(ssid));
        strncpy(config->pwd, password, strlen(password));
    }

    be_osal_schedule_call(sub_call_wifi, config);
}

static void get_ifconfig(void *arg)
{
    char tmp[256] = {0};
    strcpy(tmp, BE_CLI_REPLY);
    char mac[6] = {0};
    char formatMac[32] = {0};
    hal_wifi_get_mac_addr(NULL, mac);

#ifndef STM32L496xx
    sprintf(formatMac, "MAC address:%02x-%02x-%02x-%02x-%02x-%02x\r\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
    sprintf(formatMac, "MAC address:%02x-%02x-%02x-%02x-%02x-%02x\r\n", (mac[0] & 0x000000ff),
            (mac[1] & 0x000000ff), (mac[2] & 0x000000ff), (mac[3] & 0x000000ff), (mac[4] & 0x000000ff), (mac[5] & 0x000000ff));
#endif
    strcat(tmp, formatMac);

    char ip[16] = {0};
    char formatIP[48] = {0};

    be_osal_wifi_get_ip(ip);
    sprintf(formatIP, "%sIP address:%s\r\n", BE_CLI_REPLY, ip);
    strcat(tmp, formatIP);

    netmgr_ap_config_t config;
    char formatSSID[32] = {0};

    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    if (config.ssid[0] != '0') {
        sprintf(formatSSID, "%sWifi SSID:%s\r\n", BE_CLI_REPLY, config.ssid);
        strcat(tmp, formatSSID);
    } else {
        strcat(tmp, "#be-cli-reply#Wifi SSID:\r\n");
    }
    strcat(tmp, BE_CLI_REPLY_SUCCESS);
    // send reply
    CLI_REPLY(tmp);
}

static void handle_devifconfig_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    be_osal_schedule_call(get_ifconfig, NULL);
}

static struct cli_command devdmesgcmd = {
    .name = "dmesg",
    .help = "dmesg",
    .function = handle_devdmesg_cmd
};

static struct cli_command devrebootcmd = {
    .name = "reboot",
    .help = "reboot the device",
    .function = handle_devreboot_cmd
};

static struct cli_command devrenamecmd = {
    .name = "rename",
    .help = "rename newname",
    .function = handle_devrename_cmd
};

static struct cli_command devwificmd = {
    .name = "wifi",
    .help = "wifi ssid pswd",
    .function = handle_devwifi_cmd
};

static struct cli_command devifconfigcmd = {
    .name = "ifconfig",
    .help = "ifconfig",
    .function = handle_devifconfig_cmd
};

void cli_cmd_register_dev()
{
#ifdef BE_OS_AOS
    aos_cli_register_command(&devdmesgcmd);
    aos_cli_register_command(&devrebootcmd);
    aos_cli_register_command(&devrenamecmd);
    aos_cli_register_command(&devwificmd);
    aos_cli_register_command(&devifconfigcmd);
#endif
}