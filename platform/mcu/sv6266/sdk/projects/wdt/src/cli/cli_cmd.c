#include <stdio.h>

#include "soc_types.h"
#include "cli.h"

#include "wdt/drv_wdt.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? (__builtin_strrchr(__FILE__, '/') + 1) : (__FILE__))

/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static void wdt_isr (void)
{
    printf ("%s:%d: In function: '%s': wdt interrupt occurred\n", __FILENAME__, __LINE__ - 1, __func__);
    printf ("wait for wdt reset...\n\n");
}

static void interrupt (int32_t argc, char *argv[])
{
    WDT_ID_E    wdt_id;
    uint32_t    wdt_time_count;
    uint32_t    wdt_int_time_count_offset;

    if (argc == 4) {
        wdt_id = strtol (argv[1], NULL, 10);
        wdt_time_count = strtol (argv[2], NULL, 10);
        wdt_int_time_count_offset = strtol (argv[3], NULL, 10);

    } else {
        printf ("\nUsage: int [id] [time_count] [int_time_count_offset]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");
        printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
        printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
        printf ("                       Contains at least the [0, 65535] range.\n");
        printf ("int_time_count_offset  The watchdog interrupt time offset (in unit of ms).\n");
        printf ("                       Contains at least the [0, 255] range.\n");

        return;
    }

    if (((wdt_id < 0) || (wdt_id > 1)) ||
        ((wdt_time_count < 0) || (wdt_time_count > 65535)) ||
        ((wdt_int_time_count_offset < 0) || (wdt_int_time_count_offset > 255))) {
        printf ("\nUsage: int [id] [time_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");
        printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
        printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
        printf ("                       Contains at least the [0, 65535] range.\n");

        return;
    }

    printf ("wdt%d interrupt program (Build: %s %s)\n", wdt_id, __DATE__, __TIME__);
    printf ("wdt%d wait for interrupt event...\n", wdt_id);

    drv_wdt_init ();
    drv_wdt_sw_rst ();
    drv_wdt_register_isr (wdt_id, wdt_int_time_count_offset, wdt_isr);
    drv_wdt_enable (wdt_id, wdt_time_count);
}

static void kick (int32_t argc, char *argv[])
{
    WDT_ID_E    wdt_id;

    if (argc == 2) {
        wdt_id = strtol (argv[1], NULL, 10);

    } else {
        printf ("\nUsage: kick [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");

        return;
    }

    if (((wdt_id < 0) || (wdt_id > 1))) {
        printf ("\nUsage: kick [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");

        return;
    }

    printf ("wdt%d kick program (Build: %s %s)\n", wdt_id, __DATE__, __TIME__);

    drv_wdt_kick (wdt_id);
}

static void stop (int32_t argc, char *argv[])
{
    WDT_ID_E    wdt_id;

    if (argc == 2) {
        wdt_id = strtol (argv[1], NULL, 10);

    } else {
        printf ("\nUsage: stop [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");

        return;
    }

    if (((wdt_id < 0) || (wdt_id > 1))) {
        printf ("\nUsage: stop [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");

        return;
    }

    printf ("wdt%d stop program (Build: %s %s)\n", wdt_id, __DATE__, __TIME__);

    drv_wdt_disable (wdt_id);
}

static void start (int32_t argc, char *argv[])
{
    WDT_ID_E    wdt_id;
    uint32_t    wdt_time_count;

    if (argc == 3) {
        wdt_id = strtol (argv[1], NULL, 10);
        wdt_time_count = strtol (argv[2], NULL, 10);

    } else {
        printf ("\nUsage: start [id] [time_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");
        printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
        printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
        printf ("                       Contains at least the [0, 65535] range.\n");

        return;
    }

    if (((wdt_id < 0) || (wdt_id > 1)) ||
        ((wdt_time_count < 0) || (wdt_time_count > 65535))) {
        printf ("\nUsage: start [id] [time_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
        printf ("                       0: MCU_WDT\n");
        printf ("                       1: SYS_WDT\n");
        printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
        printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
        printf ("                       Contains at least the [0, 65535] range.\n");

        return;
    }

    printf ("wdt%d start program (Build: %s %s)\n", wdt_id, __DATE__, __TIME__);

    drv_wdt_init ();
    drv_wdt_sw_rst ();
    drv_wdt_register_isr (wdt_id, 0, NULL);
    drv_wdt_enable (wdt_id, wdt_time_count);
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: start [id] [time_count]\n");
    printf ("Options:\n");
    printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
    printf ("                       0: MCU_WDT\n");
    printf ("                       1: SYS_WDT\n");
    printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
    printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
    printf ("                       Contains at least the [0, 65535] range.\n");
    printf ("\n");

    printf ("Usage: stop [id]\n");
    printf ("Options:\n");
    printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
    printf ("                       0: MCU_WDT\n");
    printf ("                       1: SYS_WDT\n");
    printf ("\n");

    printf ("Usage: kick [id]\n");
    printf ("Options:\n");
    printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
    printf ("                       0: MCU_WDT\n");
    printf ("                       1: SYS_WDT\n");
    printf ("\n");

    printf ("Usage: int [id] [time_count] [int_time_count_offset]\n");
    printf ("Options:\n");
    printf ("id                     Select a MCU or SYS watchdog timer ID.\n");
    printf ("                       0: MCU_WDT\n");
    printf ("                       1: SYS_WDT\n");
    printf ("time_count             The amount of time in ms units to wait before the watchdog timer is fired.\n");
    printf ("                       If wdt_time_cnt is 0, then the watchdog timer is disabled.\n");
    printf ("                       Contains at least the [0, 65535] range.\n");
    printf ("int_time_count_offset  The watchdog interrupt time offset (in unit of ms).\n");
    printf ("                       Contains at least the [0, 255] range.\n");
    printf ("\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
    {"help",            help,               "wdt help"},
    {"start",           start,              "wdt start countdown"},
    {"stop",            stop,               "wdt stop countdown"},
    {"kick",            kick,               "wdt kick"},
    {"int",             interrupt,          "wdt interrupt"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
