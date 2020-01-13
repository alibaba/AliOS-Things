/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "k_api.h"

#include "cli_api.h"

#include "cli_conf.h"
#include "cli_adapt.h"

#if (RHINO_CONFIG_UCLI)
#include "ucli.h"
#endif

#if defined (__CC_ARM) && defined(__MICROLIB)
#define TOSTR(s) #s
#else
#define TOSTR
#endif

#define OTA_THREAD_PRIORITY 32

extern void hal_reboot(void);

static void help_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void version_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void reboot_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void uptime_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void ota_cmd(char *buf, int32_t len, int32_t argc, char **argv);

#if (CLI_MINIMUM_MODE <= 0)

static void echo_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void exit_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void devname_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void pmem_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void mmem_cmd(char *buf, int32_t len, int32_t argc, char **argv);

#endif

#if (RHINO_CONFIG_USER_SPACE > 0)
static void process_info_cmd(char *buf, int len, int argc, char **argv);
static void kill_process_cmd(char *buf, int len, int argc, char **argv);
#endif

extern void log_cli_init(void);

#ifdef AOS_COMP_DEBUG
#if (RHINO_CONFIG_SYS_STATS > 0)
extern void cpuusage_cmd(char *buf, int32_t len, int32_t argc, char **argv);
#endif
#endif

#if (CLI_SUPPORT_BOARD_CMD > 0)
extern int board_cli_init(void);
#endif

static const struct cli_command_st built_ins[] = {
    /*cli self*/
    { "help", "print this", help_cmd },

    /*rhino*/
    { "sysver", "system version", version_cmd },
    { "reboot", "reboot system", reboot_cmd },

    /*aos_rhino*/
    { "time", "system time", uptime_cmd },
    { "ota", "system ota", ota_cmd },
#ifdef AOS_COMP_DEBUG
#if (RHINO_CONFIG_SYS_STATS > 0)
    { "cpuusage", "show cpu usage", cpuusage_cmd },
#endif
#endif

#if (CLI_MINIMUM_MODE <= 0)
    { "p", "print memory", pmem_cmd },
    { "m", "modify memory", mmem_cmd },
    { "echo", "echo for command", echo_cmd },
    { "exit", "close CLI", exit_cmd },
    { "devname", "print device name", devname_cmd },
#endif
};

static void help_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    int32_t i, n, commands_num;
    struct cli_command_st *cmd = NULL;

    uint32_t build_in_count = sizeof(built_ins) / sizeof(built_ins[0]);

    commands_num = cli_get_commands_num();

    cli_printf("====Build-in Commands====\r\n");
    cli_printf("====Support %d cmds once, seperate by ; ====\r\n",
                   CLI_MAX_ONCECMD_NUM);

    for (i = 0, n = 0; i < CLI_MAX_COMMANDS && n < commands_num; i++) {
        cmd = cli_get_command(i);
        if (cmd->name) {
            cli_printf("%-10s: %s\r\n", cmd->name, cmd->help ? cmd->help : "");
            n++;
            if (n == build_in_count) {
                cli_printf("\r\n");
                cli_printf("====User Commands====\r\n");
            }
        }
    }

#if (RHINO_CONFIG_UCLI)
    ucli_show_cmds();
#endif
}

static void version_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
#ifdef OSAL_RHINO
    cli_printf("kernel version :%d\r\n", (int32_t)krhino_version_get());
#else
    cli_printf("kernel version :posix\r\n");
#endif
}

static void reboot_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    cli_printf("reboot\r\n");
    hal_reboot();
}

static void uptime_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    cli_printf("UP time %ld ms\r\n", (long)krhino_sys_time_get());
}

void tftp_ota_thread(void *arg)
{
    cli_task_exit();
}

static void ota_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    cli_task_create("LOCAL OTA", tftp_ota_thread, 0, 4096, OTA_THREAD_PRIORITY);
}

#if (CLI_MINIMUM_MODE <= 0)

static void echo_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    int32_t echo_disabled = cli_get_echo_status();

    if (argc == 1) {
        cli_printf("Usage: echo on/off. Echo is currently %s\r\n",
                       echo_disabled ? "Disabled" : "Enabled");
        return;
    }

    if (!strcmp(argv[1], "on")) {
        cli_printf("Enable echo\r\n");
        cli_set_echo_status(0);
    } else if (!strcmp(argv[1], "off")) {
        cli_printf("Disable echo\r\n");
        cli_set_echo_status(1);
    }
}

static void exit_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    cli_stop();
}

static void devname_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    cli_printf("device name: %s\r\n", TOSTR(SYSINFO_DEVICE_NAME));
}

static void pmem_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    int32_t i;
    int32_t nunits = 16;
    int32_t width  = 4;

    char *pos    = NULL;
    char *addr   = NULL;

    switch (argc) {
        case 4:
            width = strtoul(argv[3], NULL, 0);
        case 3:
            nunits = strtoul(argv[2], NULL, 0);
            nunits = nunits > 0x400 ? 0x400 : nunits;
        case 2:
            addr = (char *)strtoul(argv[1], &pos, 0);
            break;
        default:
            break;
    }

    if (pos == NULL || pos == argv[1] || nunits > 0x1000 || addr == NULL) {
        cli_printf("p <addr> <nunits> <width>\r\n"
                   "addr  : address to display\r\n"
                   "nunits: number of units to display (default is 16)\r\n"
                   "width : width of unit, 1/2/4 (default is 4)\r\n");
        return;
    }

    switch (width) {
        case 1:
            for (i = 0; i < nunits; i++) {
                if (i % 16 == 0) {
                    cli_printf("0x%08x:", (uint32_t)addr);
                }
                cli_printf(" %02x", *(unsigned char *)addr);
                addr += 1;
                if (i % 16 == 15) {
                    cli_printf("\r\n");
                }
            }
            break;
        case 2:
            for (i = 0; i < nunits; i++) {
                if (i % 8 == 0) {
                    cli_printf("0x%08x:", (uint32_t)addr);
                }
                cli_printf(" %04x", *(unsigned short *)addr);
                addr += 2;
                if (i % 8 == 7) {
                    cli_printf("\r\n");
                }
            }
            break;
        default:
            for (i = 0; i < nunits; i++) {
                if (i % 4 == 0) {
                    cli_printf("0x%08x:", (uint32_t)addr);
                }
                cli_printf(" %08x", *(unsigned int *)addr);
                addr += 4;
                if (i % 4 == 3) {
                    cli_printf("\r\n");
                }
            }
            break;
    }
}

static void mmem_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    void *addr  = NULL;

    int32_t  width = 4;
    uint32_t value = 0;

    uint32_t old_value;
    uint32_t new_value;

    switch (argc) {
        case 4:
            width = strtoul(argv[3], NULL, 0);
        case 3:
            value = strtoul(argv[2], NULL, 0);
        case 2:
            addr = (void *)strtoul(argv[1], NULL, 0);
            break;
        default:
            addr = NULL;
            break;
    }

    if (addr == NULL) {
        cli_printf("m <addr> <value> <width>\r\n"
                   "addr  : address to modify\r\n"
                   "value : new value (default is 0)\r\n"
                   "width : width of unit, 1/2/4 (default is 4)\r\n");
        return;
    }

    switch (width) {
        case 1:
            old_value = (uint32_t)(*(uint8_t volatile *)addr);
            *(uint8_t volatile *)addr = (uint8_t)value;
            new_value = (uint32_t)(*(uint8_t volatile *)addr);
            break;
        case 2:
            old_value = (uint32_t)(*(unsigned short volatile *)addr);
            *(unsigned short volatile *)addr = (unsigned short)value;
            new_value = (uint32_t)(*(unsigned short volatile *)addr);
            break;
        case 4:
        default:
            old_value = *(uint32_t volatile *)addr;
            *(uint32_t volatile *)addr = (uint32_t)value;
            new_value = *(uint32_t volatile *)addr;
            break;
    }
    cli_printf("value on 0x%x change from 0x%x to 0x%x.\r\n", (uint32_t)addr,
                   old_value, new_value);
}

#endif

int32_t cli_register_default_commands(void)
{
    int32_t ret;

    ret = cli_register_commands(built_ins, sizeof(built_ins)/sizeof(struct cli_command_st));
    if (ret != CLI_OK) {
        return ret;
    }

#if (CLI_SUPPORT_BOARD_CMD > 0)
    board_cli_init();
#endif

#if (RHINO_CONFIG_UCLI)
    ret = ucli_register_default_cmds();
    if (ret != CLI_OK) {
        return ret;
    }
#endif

    return CLI_OK;
}

