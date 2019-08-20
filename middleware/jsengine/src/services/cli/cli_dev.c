/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_common.h"

#include "board_info.h"
#include "cli_ext.h"

/* assembel information finished then write \n to indicates finished serialport
   data */
#define BE_CLI_REPLY "#be-cli-reply#"
#define BE_CLI_REPLY_SUCCESS "#be-cli-reply#success\n"
#define BE_CLI_REPLY_FAILED "#be-cli-reply#failed\n"

#define BE_DEBUGER_DMESG_CMD_REPLY "#be-debuger-dmesg-reply#\n"
#define BE_DEBUGER_REBOOT_CMD_REPLY "#be-debuger-reboot-reply#\n"
#define BE_DEBUGER_RENAME_CMD_REPLY "#be-debuger-rename-reply#\n"
#define BE_DEBUGER_IFCONFIG_CMD_REPLY "#be-debuger-ifconfig-reply#"

extern void sub_call_rename(void *arg);
extern void sub_call_wifi(void *arg);

static void handle_devdmesg_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    /* aos_cli_stop(); */
    /* following code is serial port reply msg */
    jse_cli_printf("%s", BE_DEBUGER_DMESG_CMD_REPLY);
}

static void handle_devreboot_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_cli_printf("%s", BE_DEBUGER_REBOOT_CMD_REPLY);
    jse_system_reboot();
}

static void handle_devrename_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_cli_printf("%s", BE_DEBUGER_RENAME_CMD_REPLY);
    jse_task_schedule_call(sub_call_rename, argv[1]);
}

#ifdef JSE_HW_ADDON_WIFI
static void handle_devwifi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *ssid     = argv[1];
    char *password = argv[2];

    if (strlen(ssid) > JSE_MAX_SSID_SIZE ||
        strlen(password) > JSE_MAX_PWD_SIZE) {
#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/device/wifi_reply", 201, "Bad Request");
#endif
        return;
    }
    uint8_t myssid[32]   = {0};
    uint8_t mypasswd[64] = {0};
    uint32_t err;

    snprintf(myssid, sizeof(myssid), "%s", ssid);
    snprintf(mypasswd, sizeof(mypasswd), "%s", password);

    err = jse_system_wifi_connect(myssid, mypasswd);

    jse_debug("jse_system_wifi_connect %s %s return %d", myssid, mypasswd, err);
#ifdef JSE_IDE_DEBUG
    if (err == 0) {
        bone_websocket_send_frame("/device/wifi_reply", 200, "success");
    } else {
        bone_websocket_send_frame("/device/wifi_reply", 201, "failed");
    }
#endif
}

static void get_ifconfig(void *arg)
{
    char tmp[256] = {0};
    strcpy(tmp, BE_CLI_REPLY);
    char mac[6]        = {0};
    char formatMac[32] = {0};

    jse_system_wifi_mac_get(mac);

#ifndef STM32L496xx
    sprintf(formatMac, "MAC address:%02x-%02x-%02x-%02x-%02x-%02x\r\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
    sprintf(formatMac, "MAC address:%02x-%02x-%02x-%02x-%02x-%02x\r\n",
            (mac[0] & 0x000000ff), (mac[1] & 0x000000ff), (mac[2] & 0x000000ff),
            (mac[3] & 0x000000ff), (mac[4] & 0x000000ff),
            (mac[5] & 0x000000ff));
#endif
    strcat(tmp, formatMac);

    char ip[16]       = {0};
    char formatIP[48] = {0};
    jse_system_get_ip(ip);
    sprintf(formatIP, "%sIP address:%s\r\n", BE_CLI_REPLY, ip);
    strcat(tmp, formatIP);
#ifdef BE_OS_AOS
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
#endif

    strcat(tmp, BE_CLI_REPLY_SUCCESS);
    /* send reply */
    jse_cli_printf("%s", tmp);
}

static void handle_devifconfig_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_task_schedule_call(get_ifconfig, NULL);
}

#endif

static void handle_setdevinfo_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *devKey    = NULL;
    char *devName   = NULL;
    char *devSecret = NULL;

    if (argc < 4) {
        jse_cli_printf("%s", BE_CLI_REPLY_FAILED);
        return;
    }
    devKey    = argv[1];
    devName   = argv[2];
    devSecret = argv[3];

    board_setDeviceInfo(devKey, devName, devSecret);
    jse_cli_printf("%s", BE_CLI_REPLY_SUCCESS);
}

static void handle_updateimg_cmd(char *pwbuf, int blen, int argc, char **argv)
{
#if 0
        be_upgd_param_t *p_param;

        if (argc < 4) {
            goto fail;
        }

        p_param = jse_calloc(1, sizeof(be_upgd_param_t));
        if (p_param == NULL) {
            goto fail;
        }

        if (strcasecmp(argv[1], "kernel") == 0) {
            p_param->type = BE_UPGD_KERNEL_TYPE;
        } else if (strcasecmp(argv[1], "spiffs") == 0) {
            p_param->type = BE_UPGD_SPIFFS_TYPE;
        } else {
            goto fail;
        }

        p_param->url = jse_calloc(1, strlen(argv[2]) + 1);
        if (p_param->url == NULL) {
            goto fail;
        }

        strncpy(p_param->url, argv[2], strlen(argv[2]));
        strncpy(p_param->md5, argv[3], MD5_LEN);
        if (argc > 4) {
            p_param->total_size = atoi(argv[4]);
        }
        jse_osal_new_task("be_upgrader", be_upgrader_handler, p_param, 4096 +
        1496,
                         NULL);
        return;

    fail:
        jse_debug("error: updateimg [kernel/spiffs] [http://x.x.x.x:port/x.bin] "
            "[md5]\r\n");
        jse_cli_printf("%s", BE_CLI_REPLY_FAILED);
#endif

#ifdef JSE_IDE_DEBUG
    bone_websocket_send_frame("/device/updateimg_reply", 201, "Bad Request");
#endif
    return;
}

static void handle_getversion_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *p_resp_cmd = NULL;
    int buf_len      = 256;

    p_resp_cmd = jse_calloc(1, buf_len + 1);
    if (p_resp_cmd == NULL) {
        jse_cli_printf("%s", BE_CLI_REPLY_FAILED);
        return;
    }
#ifdef BE_OS_AOS
    snprintf(p_resp_cmd, buf_len, "%sVersion:%s\r\n", BE_CLI_REPLY,
             BONE_VERSION);
    jse_cli_printf("%s", p_resp_cmd);
    memset(p_resp_cmd, 0, buf_len);
    snprintf(p_resp_cmd, buf_len, "%sVersionInfo:%s\r\n%s", BE_CLI_REPLY,
             VERSION_DESCRIPTION, BE_CLI_REPLY_SUCCESS);
#endif

#ifdef ESP_PLATFORM
    snprintf(p_resp_cmd, buf_len, "%sVersion:%s\r\n", BE_CLI_REPLY,
             "esp32devkitc-sdk-1.0.0819.dkp");
    jse_cli_printf("%s", p_resp_cmd);
    memset(p_resp_cmd, 0, buf_len);
    snprintf(p_resp_cmd, buf_len, "%sVersionInfo:%s\r\n%s", BE_CLI_REPLY, "",
             BE_CLI_REPLY_SUCCESS);
#endif

    jse_cli_printf("%s", p_resp_cmd);
    jse_free(p_resp_cmd);
    return;
}

static struct jse_cli_command devdmesgcmd = {
    .name = "dmesg", .help = "dmesg", .function = handle_devdmesg_cmd};

static struct jse_cli_command devrebootcmd = {.name     = "reboot",
                                             .help     = "reboot the device",
                                             .function = handle_devreboot_cmd};

static struct jse_cli_command devrenamecmd = {.name     = "rename",
                                             .help     = "rename newname",
                                             .function = handle_devrename_cmd};
#ifdef JSE_HW_ADDON_WIFI
static struct jse_cli_command devwificmd = {
    .name = "wifi", .help = "wifi ssid pswd", .function = handle_devwifi_cmd};

static struct jse_cli_command devifconfigcmd = {
    .name = "ifconfig", .help = "ifconfig", .function = handle_devifconfig_cmd};
#endif

static struct jse_cli_command setdevinfocmd = {
    .name     = "burnKey",
    .help     = "burnKey devKey devName devSecret",
    .function = handle_setdevinfo_cmd};

static struct jse_cli_command devupdateimgcmd = {
    .name     = "updateimg",
    .help     = "updateimg [type] [url] [md5]",
    .function = handle_updateimg_cmd};

static struct jse_cli_command devgetversioncmd = {
    .name     = "getVersion",
    .help     = "getVersion",
    .function = handle_getversion_cmd};

void cli_cmd_register_dev()
{
    jse_cli_register_command(&devrebootcmd);
#ifdef JSE_HW_ADDON_WIFI
    jse_cli_register_command(&devwificmd);
    jse_cli_register_command(&devifconfigcmd);
#endif
    jse_cli_register_command(&setdevinfocmd);
    jse_cli_register_command(&devupdateimgcmd);
    jse_cli_register_command(&devgetversioncmd);
}

void websocket_call_cli(char *cmdname, char **argv)
{
#ifdef JSE_HW_ADDON_WIFI
    if (strstr(cmdname, "updateimg")) {
        handle_updateimg_cmd(NULL, 0, 5, argv);
    } else if (strstr(cmdname, "wifi")) {
        handle_devwifi_cmd(NULL, 0, 3, argv);
    }
#endif
}