/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aos/init.h"
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "uagent/uagent.h"
#include "aos/kernel.h"
#include "hal/wifi.h"

#ifdef AOS_COMP_PWRMGMT
#include <pwrmgmt_api.h>
#endif

#ifdef AOS_COMP_SPIFFS
#include "spiffs/aos_spiffs.h"
#endif

#ifdef AOS_COMP_FATFS
#include "fatfs/aos_fatfs.h"
#endif

#include "network/network.h"
#ifdef WITH_LWIP_TFTP
#include "lwip/ip_addr.h"
#include "lwip/apps/tftp.h"
#endif

#ifdef AOS_UND
#include "und/und.h"
#include "debug_api.h"
#endif

#ifdef WITH_LWIP_IPERF
extern int iperf_cli_register(void);
#endif

extern int  vfs_init(void);
extern int  vfs_device_init(void);
#ifdef AOS_LOOP
#include "aos/yloop.h"
extern aos_loop_t aos_loop_init(void);
#endif
extern int32_t kv_init(void);
extern void ota_service_init(void);
extern void dumpsys_cli_init(void);
extern int  application_start(int argc, char **argv);
/* extern void aos_components_init(void); */

#ifdef WITH_SAL
extern int sal_device_init(void);
#endif

#ifdef AOS_BINS

#if !(RHINO_CONIFG_USER_SPACE > 0)
extern void *kmbins_tbl[];
#endif
extern char  app_info_addr;
extern k_mm_head  *g_kmm_head;
struct m_app_info_t *app_info = (struct m_app_info_t *) &app_info_addr;

void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start), (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0, app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(app_info->heap_start),
                         app_info->heap_end - app_info->heap_start);
}
#endif

#ifdef AOS_COMP_CLI

static uint8_t hex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return 0;
}

static void hexstr2bin(const char *macstr, uint8_t *mac, int len)
{
    int i;
    for (i=0;i < len && macstr[2 * i];i++) {
        mac[i] = hex(macstr[2 * i]) << 4;
        mac[i] |= hex(macstr[2 * i + 1]);
    }
}

#ifndef CONFIG_NO_TCPIP
static void udp_cmd(char *buf, int len, int argc, char **argv)
{
    struct sockaddr_in saddr;

    if (argc < 4) {
        return;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    saddr.sin_addr.s_addr = inet_addr(argv[1]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        aos_cli_printf("error creating socket!\n");
        return;
    }

    int ret = sendto(sockfd, argv[3], strlen(argv[3]), 0,
                     (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        aos_cli_printf("error send data %d!\n", ret);
    }

    close(sockfd);
}

#ifdef WITH_LWIP_TFTP
static void tftp_get_done(int error, int len)
{
    if (error == 0) {
        aos_cli_printf("tftp client get succeed\r\n", len);
    } else {
        aos_cli_printf("tftp client get failed\r\n");
    }
}

extern tftp_context_t client_ctx;
void ota_get_done(int error, int len);
static void tftp_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc < 3) {
        goto tftp_print_usage;
    }

    if (strncmp(argv[1], "server", 6) == 0) {
        if (strncmp(argv[2], "start", 5) == 0) {
            err_t err = tftp_server_start();
            aos_cli_printf("tftp start server %s\r\n", err == ERR_OK ? "done" : "failed");
            return;
        } else if (strncmp(argv[2], "stop", 4) == 0) {
            tftp_server_stop();
            aos_cli_printf("tftp stop server done\r\n");
            return;
        }
        goto tftp_print_usage;
    } else if (strncmp(argv[1], "get", 3) == 0) {
        ip_addr_t dst_addr;
        ipaddr_aton(argc == 4 ? argv[2] : "10.0.0.2", &dst_addr);
        tftp_client_get(&dst_addr, argv[argc - 1], &client_ctx, tftp_get_done);
        return;
    }

tftp_print_usage:
    aos_cli_printf("Usage: tftp server start/stop\r\n");
    aos_cli_printf("       tftp get path/to/file\r\n");
}
#endif /* WITH_LWIP_TFTP */

struct cli_command  tcpip_cli_cmd[] = {
    /* net */
#ifdef WITH_LWIP_TFTP
    {"tftp",        "tftp server/client control", tftp_cmd},
#endif /* WITH_LWIP_TFTP */
    {"udp",         "[ip] [port] [string data] send udp data", udp_cmd},
};

void tcpip_cli_init(void)
{
    aos_cli_register_commands(&tcpip_cli_cmd[0],sizeof(tcpip_cli_cmd) / sizeof(struct cli_command));
}

void wifi_debug_cmd(char *buf, int len, int argc, char **argv)
{
    hal_wifi_start_debug_mode(NULL);
}

void mac_cmd(char *buf, int len, int argc, char **argv)
{
    uint8_t mac[6];

    if (argc == 1)
    {
        hal_wifi_get_mac_addr(NULL, mac);
        aos_cli_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        hexstr2bin(argv[1], mac, 6);
        hal_wifi_set_mac_addr(NULL, mac);
        aos_cli_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        aos_cli_printf("invalid cmd\r\n");
    }
}


struct cli_command  wifi_cli_cmd[] = {
    { "wifi_debug", "wifi debug mode", wifi_debug_cmd },
    { "mac", "get/set mac", mac_cmd },
};

void hal_wifi_cli_init(void)
{
    aos_cli_register_commands(&wifi_cli_cmd[0],sizeof(wifi_cli_cmd) / sizeof(struct cli_command));
}
#endif /*!defined CONFIG_NO_TCPIP */

void cli_service_init(kinit_t *kinit)
{
    if (kinit->cli_enable)
    {
        aos_cli_init();
        /*kernel basic cmds reg*/
#ifdef OSAL_RHINO
        dumpsys_cli_init();
#endif
#ifndef CONFIG_NO_TCPIP
        tcpip_cli_init();
        hal_wifi_cli_init();

#ifdef WITH_LWIP_IPERF
        iperf_cli_register();
#endif

#endif

    }
    return;
}

#endif

void aos_show_welcome(void)
{
    puts("             Welcome to AliOS Things           ");
#ifdef CONFIG_AOS_INIT_WELCOME
    puts("       ¨€¨€¨€¨€¨€¨[ ¨€¨€¨[     ¨€¨€¨[ ¨€¨€¨€¨€¨€¨€¨[ ¨€¨€¨€¨€¨€¨€¨€¨[     ");
    puts("      ¨€¨€¨X¨T¨T¨€¨€¨[¨€¨€¨U     ¨€¨€¨U¨€¨€¨X¨T¨T¨T¨€¨€¨[¨€¨€¨X¨T¨T¨T¨T¨a     ");
    puts("      ¨€¨€¨€¨€¨€¨€¨€¨U¨€¨€¨U     ¨€¨€¨U¨€¨€¨U   ¨€¨€¨U¨€¨€¨€¨€¨€¨€¨€¨[     ");
    puts("      ¨€¨€¨X¨T¨T¨€¨€¨U¨€¨€¨U     ¨€¨€¨U¨€¨€¨U   ¨€¨€¨U¨^¨T¨T¨T¨T¨€¨€¨U     ");
    puts("      ¨€¨€¨U  ¨€¨€¨U¨€¨€¨€¨€¨€¨€¨€¨[¨€¨€¨U¨^¨€¨€¨€¨€¨€¨€¨X¨a¨€¨€¨€¨€¨€¨€¨€¨U     ");
    puts("      ¨^¨T¨a  ¨^¨T¨a¨^¨T¨T¨T¨T¨T¨T¨a¨^¨T¨a ¨^¨T¨T¨T¨T¨T¨a ¨^¨T¨T¨T¨T¨T¨T¨a     ");
    puts("¨€¨€¨€¨€¨€¨€¨€¨€¨[¨€¨€¨[  ¨€¨€¨[¨€¨€¨[¨€¨€¨€¨[   ¨€¨€¨[ ¨€¨€¨€¨€¨€¨€¨[ ¨€¨€¨€¨€¨€¨€¨€¨[");
    puts("¨^¨T¨T¨€¨€¨X¨T¨T¨a¨€¨€¨U  ¨€¨€¨U¨€¨€¨U¨€¨€¨€¨€¨[  ¨€¨€¨U¨€¨€¨X¨T¨T¨T¨T¨a ¨€¨€¨X¨T¨T¨T¨T¨a");
    puts("   ¨€¨€¨U   ¨€¨€¨€¨€¨€¨€¨€¨U¨€¨€¨U¨€¨€¨X¨€¨€¨[ ¨€¨€¨U¨€¨€¨U  ¨€¨€¨€¨[¨€¨€¨€¨€¨€¨€¨€¨[");
    puts("   ¨€¨€¨U   ¨€¨€¨X¨T¨T¨€¨€¨U¨€¨€¨U¨€¨€¨U¨^¨€¨€¨[¨€¨€¨U¨€¨€¨U   ¨€¨€¨U¨^¨T¨T¨T¨T¨€¨€¨U");
    puts("   ¨€¨€¨U   ¨€¨€¨U  ¨€¨€¨U¨€¨€¨U¨€¨€¨U ¨^¨€¨€¨€¨€¨U¨^¨€¨€¨€¨€¨€¨€¨X¨a¨€¨€¨€¨€¨€¨€¨€¨U");
    puts("   ¨^¨T¨a   ¨^¨T¨a  ¨^¨T¨a¨^¨T¨a¨^¨T¨a  ¨^¨T¨T¨T¨a ¨^¨T¨T¨T¨T¨T¨a ¨^¨T¨T¨T¨T¨T¨T¨a");
#endif
}

int aos_components_init(kinit_t *kinit)
{
#ifdef AOS_COMP_VFS
    vfs_init();
#endif
#ifdef AOS_COMP_UAGENT
    uagent_init();
#endif

#ifdef AOS_COMP_CLI
    cli_service_init(kinit);
#endif

#ifdef AOS_COMP_SPIFFS
    vfs_spiffs_register();
#endif

#ifdef AOS_COMP_FATFS
    fatfs_register();
#endif

#ifdef AOS_COMP_ULOG
    ulog_init();
#endif

#ifdef AOS_COMP_KV
    kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    vfs_device_init();
    aos_loop_init();
#endif

#ifdef AOS_UOTA
    ota_service_init();
#endif

#ifdef AOS_SENSOR
    sensor_init();
#endif

#ifdef AOS_GPS
    gps_init();
#endif

/* auto_component generated by the compiler system, now gcc support */
#if defined (__GNUC__) && !defined (__CC_ARM)
    /* aos_components_init(); */
#endif

    aos_show_welcome();

#ifdef AOS_COMP_PWRMGMT
    cpu_pwrmgmt_init();
#endif

#ifdef AOS_UND
    und_update_statis(UND_STATIS_DEV_EXCEPTION_IDX, (int)debug_reboot_reason_get());
#endif

#ifdef AOS_BINS
    app_pre_init();

    if (app_info->app_entry) {
        app_info->app_entry((void *)kmbins_tbl, 0, NULL);
    }
#endif

    return 0;
}
