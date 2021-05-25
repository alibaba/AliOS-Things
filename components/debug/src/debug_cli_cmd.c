/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI

#include <stdlib.h>
#include <unistd.h>

#include "k_api.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#include "debug_api.h"


#if (RHINO_CONFIG_UCLI)
    #include "ucli.h"
#endif

#if defined (__CC_ARM) && defined(__MICROLIB)
    #define TOSTR(s) #s
#else
    #define TOSTR
#endif


static void version_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void uptime_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void debug_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void msleep_cmd(char *buf, int32_t len, int32_t argc, char **argv);

static void devname_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void pmem_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void mmem_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void func_cmd(char *buf, int32_t len, int32_t argc, char **argv);
static void debug_panic_runto_cli(char *buf, int32_t len, int32_t argc, char **argv);

static const struct cli_command built_ins[] = {
    { "debug", "show debug info", debug_cmd },
    /*aos_rhino*/
    { "sysver", "system version", version_cmd },
    { "time", "system time", uptime_cmd },
    { "msleep", "sleep miliseconds", msleep_cmd },
    { "p", "print memory", pmem_cmd },
    { "m", "modify memory", mmem_cmd },
    { "f", "run a function", func_cmd },
    { "devname", "print device name", devname_cmd },
    { "err2cli", "set exec runto cli", debug_panic_runto_cli },
};

static void debug_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    debug_overview(aos_cli_printf);
}

static void version_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
#ifdef OSAL_RHINO
    aos_cli_printf("kernel version :%d\r\n", (int32_t)krhino_version_get());
#else
    aos_cli_printf("kernel version :posix\r\n");
#endif
}

static void uptime_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    aos_cli_printf("UP time %ld ms\r\n", (long)krhino_sys_time_get());
}

static void msleep_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    int seconds;

    if (argc != 2) {
        aos_cli_printf("Usage: msleep seconds\r\n");
        return;
    }

    seconds = atoi(argv[1]);
    if (seconds > 0) {
        krhino_task_sleep(krhino_ms_to_ticks(seconds));
    } else {
        aos_cli_printf("invalid param %s\r\n", argv[1]);
    }
}

static void devname_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
#ifdef SYSINFO_DEVICE_NAME
    aos_cli_printf("device name: %s\r\n", TOSTR(SYSINFO_DEVICE_NAME));
#else
    aos_cli_printf("device name: not defined on board\r\n");
#endif
}

uint32_t __attribute__((weak)) _system_ram_start = 0;
uint32_t __attribute__((weak)) _system_ram_len   = 0;

static void pmem_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    int32_t i;
    int32_t nunits = 16;
    int32_t width  = 4;

    //char *addr   = NULL;
    uint32_t addr = 0;

    switch (argc) {
        case 4:
            width = strtoul(argv[3], NULL, 0);
        case 3:
            nunits = strtoul(argv[2], NULL, 0);
            nunits = nunits > 0x400 ? 0x400 : nunits;
        case 2:
            if (0 == strcmp(argv[1], "all")) {
                aos_cli_printf("ram start: 0x%x  len: 0x%x\n", _system_ram_start, _system_ram_len);
                if ((_system_ram_start != 0) && (_system_ram_len != 0)) {
                    addr   = _system_ram_start;
                    nunits = _system_ram_len / 4;
                    width  = 4;
                }
            } else {
                addr = strtoul(argv[1], NULL, 0);
            }
            break;
        default:
            aos_cli_printf("p <addr> <nunits> <width>\r\n"
                       "addr  : address to display\r\n"
                       "nunits: number of units to display (default is 16)\r\n"
                       "width : width of unit, 1/2/4 (default is 4)\r\n");
            return;
    }

    switch (width) {
        case 1:
            for (i = 0; i < nunits; i++) {
                if (i % 16 == 0) {
                    aos_cli_printf("0x%08x:", addr);
                }
                aos_cli_printf(" %02x", *(unsigned char *)addr);
                addr += 1;
                if (i % 16 == 15) {
                    aos_cli_printf("\r\n");
                }
            }
            break;
        case 2:
            for (i = 0; i < nunits; i++) {
                if (i % 8 == 0) {
                    aos_cli_printf("0x%08x:", addr);
                }
                aos_cli_printf(" %04x", *(unsigned short *)addr);
                addr += 2;
                if (i % 8 == 7) {
                    aos_cli_printf("\r\n");
                }
            }
            break;
        default:
            for (i = 0; i < nunits; i++) {
                if (i % 4 == 0) {
                    aos_cli_printf("0x%08x:", addr);
                }
                aos_cli_printf(" %08x", *(unsigned int *)addr);
                addr += 4;
                if (i % 4 == 3) {
                    aos_cli_printf("\r\n");
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
            aos_cli_printf("m <addr> <value> <width>\r\n"
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
    aos_cli_printf("value on 0x%x change from 0x%x to 0x%x.\r\n", (uint32_t)addr,
               old_value, new_value);
}

static void func_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    uint32_t idx, ret;
    uint32_t para[8] = {0};
    typedef uint32_t (*func_ptr_t)(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                   uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8);
    func_ptr_t func_ptr;

    if (argc == 1) {
        aos_cli_printf("para error\r\n");
        return;
    }

    argc = argc > 10 ? 10 : argc;

    func_ptr = (func_ptr_t)strtoul(argv[1], NULL, 0);
    for (idx = 2 ; idx < argc ; idx++) {
        para[idx - 2] = strtoul(argv[idx], NULL, 0);
    }

    aos_cli_printf("function %p runing...\r\n", func_ptr);
#ifdef ARCH_CORTEX_M
    func_ptr = (func_ptr_t)((unsigned int)(func_ptr) + 1);
#endif
    ret = func_ptr(para[0], para[1], para[2], para[3], para[4], para[5], para[6], para[7]);
    aos_cli_printf("function %p return 0x%x.\r\n", func_ptr, ret);
}


static void debug_panic_runto_cli(char *buf, int32_t len, int32_t argc, char **argv)
{
    if (argc == 2) {
        int32_t flag = strtoul(argv[1], NULL, 0);
        if (flag == 1) {
            g_crash_not_reboot = OS_PANIC_NOT_REBOOT;
        } else {
            g_crash_not_reboot = 0;
        }
        aos_cli_printf("set panic_runto_cli flag:0x%x\r\n", g_crash_not_reboot);
    }
}

static void debug_default_cmds_register(void)
{
    int32_t ret;

    ret = aos_cli_register_commands(built_ins, sizeof(built_ins) / sizeof(struct cli_command));
    if (ret) {
        printf("%s %d failed, ret = %d\r\n", __func__, __LINE__, ret);
    }
}

void debug_cli_cmd_init(void)
{
    debug_default_cmds_register();
    debug_dumpsys_cmds_register();
#if (RHINO_CONFIG_SYS_STATS > 0)
    debug_cpuusage_cmds_register();
#endif
}
#endif /* #if AOS_COMP_CLI */
