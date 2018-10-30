/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <hal/wifi.h>

#include <aos/network.h>
#ifdef CONFIG_NET_LWIP
#include "lwip/ip_addr.h"
#include "lwip/apps/tftp.h"
#endif /* CONFIG_NET_LWIP */

extern int vfs_init(void);
extern int vfs_device_init(void);
extern int aos_kv_init(void);
extern void ota_service_init(void);
extern int aos_framework_init(void);
extern void trace_start(void);
extern void dumpsys_cli_init(void);
extern int application_start(int argc, char **argv);

#ifdef WITH_SAL
extern int sal_device_init(void);
#endif

#ifdef AOS_BINS
extern void *syscall_ktbl[];
extern char  app_info_addr;
extern char  framework_info_addr;
extern k_mm_head  *g_kmm_head;

struct framework_info_t *framework_info = (struct framework_info_t *)&framework_info_addr;
struct app_info_t *app_info = (struct app_info_t *)&app_info_addr;

static void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start), (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);
    memset((void *)(app_info->bss_start), 0, app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(app_info->heap_start),
                        app_info->heap_end - app_info->heap_start);

    krhino_mm_leak_region_init((void *)(app_info->data_ram_start), (void *)(app_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(app_info->bss_start), (void *)(app_info->bss_end));
}

static void framework_pre_init(void)
{
    memcpy((void *)(framework_info->data_ram_start), (void *)(framework_info->data_flash_begin),
           framework_info->data_ram_end - framework_info->data_ram_start);
    memset((void *)(framework_info->bss_start), 0, framework_info->bss_end - framework_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(framework_info->heap_start),
                        framework_info->heap_end - framework_info->heap_start);

    krhino_mm_leak_region_init((void *)(framework_info->data_ram_start), (void *)(framework_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(framework_info->bss_start), (void *)(framework_info->bss_end));
}
#endif


#ifdef CONFIG_AOS_CLI


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

#ifdef CONFIG_NET_LWIP
static void tftp_get_done(int error, int len)
{
    if (error == 0) {
        aos_cli_printf("tftp client get succeed\r\n", len);
    } else {
        aos_cli_printf("tftp client get failed\r\n");
    }
}

extern tftp_context_t client_ctx;
extern tftp_context_t ota_ctx;
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
    } else if (strncmp(argv[1], "ota", 3) == 0) {
        ip_addr_t dst_addr;
        uint8_t   gw_ip[4] = {10, 0 , 0, 2};
        memcpy(&dst_addr, gw_ip, 4);
        tftp_client_get(&dst_addr, argv[2], &ota_ctx, ota_get_done);
        return;
    }

tftp_print_usage:
    aos_cli_printf("Usage: tftp server start/stop\r\n");
    aos_cli_printf("       tftp get path/to/file\r\n");
}
#endif /* CONFIG_NET_LWIP */

struct cli_command  tcpip_cli_cmd[] = {
    /* net */
#ifdef CONFIG_NET_LWIP
    {"tftp",        "tftp server/client control", tftp_cmd},
#endif /* CONFIG_NET_LWIP */
    {"udp",         "[ip] [port] [string data] send udp data", udp_cmd},
};

void tcpip_cli_init(void)
{
    aos_cli_register_commands(&tcpip_cli_cmd[0],sizeof(tcpip_cli_cmd) / sizeof(struct cli_command));
}
#endif

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
#endif
    }
    return;
}

#endif

int aos_kernel_init(kinit_t *kinit)
{
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif
    
#ifdef CONFIG_AOS_CLI
    cli_service_init(kinit);
#else
    extern void log_no_cli_init(void);
    log_no_cli_init();
#endif
    
#ifdef AOS_KV
    aos_kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

#ifdef OSAL_RHINO
    trace_start();
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

#ifdef AOS_BINS
    app_pre_init();

#ifdef AOS_FRAMEWORK_COMMON
        aos_framework_init();
#endif

    if (app_info->app_entry) {
#if (RHINO_CONFIG_USER_SPACE > 0)
        app_info->app_entry(0, NULL);
#else
        app_info->app_entry((void *)kmbins_tbl, 0, NULL);
#endif
    }
#else

#if (RHINO_CONFIG_CPU_USAGE_PERIOD > 0)
    krhino_task_cpu_usage_init();
#endif

    application_start(kinit->argc, kinit->argv);
#endif

    return 0;
}


