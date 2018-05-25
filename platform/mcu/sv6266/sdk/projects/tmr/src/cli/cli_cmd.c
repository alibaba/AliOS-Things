#include <stdio.h>

#include "soc_types.h"
#include "soc_defs.h"
#include "cli.h"

#include "tmr/drv_tmr.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? (__builtin_strrchr(__FILE__, '/') + 1) : (__FILE__))
/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static void tmr_irq_handler (uint32_t irq_num)
{
    printf ("%s:%d: In function: '%s': tmr%ld interrupt occurred\n", __FILENAME__, __LINE__ - 1, __func__, irq_num - IRQ_UTIMER_0);
    drv_tmr_clear_interrupt_status (irq_num - IRQ_UTIMER_0);
    printf ("\n");
}

static void interrupt (int32_t argc, char *argv[])
{
    TM_ID_E     tmr_id;
    TM_MODE_E   tmr_mode;
    uint32_t    tmr_init_count;

    int32_t     retval;
    int32_t     i;

    if (argc == 4) {
        tmr_id = strtol (argv[1], NULL, 10);
        tmr_mode = strtol (argv[2], NULL, 10);
        tmr_init_count = strtol (argv[3], NULL, 10);

    } else {
        printf ("\nUsage: int [id] [mode] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
        printf ("                       0: Countdown to 0 then stop\n");
        printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    if (((tmr_id < 1) || (tmr_id > 7) || (tmr_id == 4)) ||
        ((tmr_mode < 0) || (tmr_mode > 1)) ||
        ((tmr_init_count < 0) || (tmr_init_count > 65535))) {
        printf ("\nUsage: start [id] [mode] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
        printf ("                       0: Countdown to 0 then stop\n");
        printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    printf ("tmr%d interrupt program (Build: %s %s)\n", tmr_id, __DATE__, __TIME__);
    printf ("tmr%d wait for interrupt event...\n", tmr_id);

    retval = drv_tmr_init (tmr_id);
    retval = drv_tmr_sw_rst (tmr_id);
    retval = drv_tmr_register_irq_handler (tmr_id, tmr_irq_handler);
    retval = drv_tmr_enable (tmr_id, tmr_mode,  tmr_init_count);
}

static void busy (int32_t argc, char *argv[])
{
    TM_ID_E     tmr_id;
    uint32_t    tmr_init_count;

    int32_t     retval;
    int32_t     i;

    if (argc == 3) {
        tmr_id = strtol (argv[1], NULL, 10);
        tmr_init_count = strtol (argv[2], NULL, 10);

    } else {
        printf ("\nUsage: busy [id] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    if (((tmr_id < 1) || (tmr_id > 7) || (tmr_id == 4)) ||
        ((tmr_init_count < 0) || (tmr_init_count > 65535))) {
        printf ("\nUsage: busy [id] [mode] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    printf ("CPU busy waiting program (Build: %s %s)\n", __DATE__, __TIME__);

    retval = drv_tmr_init (tmr_id);
    retval = drv_tmr_sw_rst (tmr_id);
    retval = drv_tmr_register_irq_handler (tmr_id, NULL);

    printf ("CPU wait for the tmr%d countdown to zero...\n", tmr_id);
    while (!drv_uart_is_transmitter_idle ());

    retval = drv_tmr_enable (tmr_id, TM_MODE_ONE_SHOT, tmr_init_count);

    while (!drv_tmr_get_interrupt_status (tmr_id)) {}
    printf ("time over!\n");
    drv_tmr_clear_interrupt_status (tmr_id);
}

static void get (int32_t argc, char *argv[])
{
    TM_ID_E     tmr_id;

    if (argc == 2) {
        tmr_id = strtol (argv[1], NULL, 10);

    } else {
        printf ("\nUsage: get [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");

        return;
    }

    if (((tmr_id < 1) || (tmr_id > 7) || (tmr_id == 4))) {
        printf ("\nUsage: get [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");

        return;
    }

    printf ("tmr%d get program (Build: %s %s)\n", tmr_id, __DATE__, __TIME__);
    printf ("current count: %05ld\n", drv_tmr_get_current_count (tmr_id));
}

static void stop (int32_t argc, char *argv[])
{
    TM_ID_E     tmr_id;

    int32_t     retval;

    if (argc == 2) {
        tmr_id = strtol (argv[1], NULL, 10);

    } else {
        printf ("\nUsage: stop [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");

        return;
    }

    if (((tmr_id < 1) || (tmr_id > 7) || (tmr_id == 4))) {
        printf ("\nUsage: stop [id]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");

        return;
    }

    printf ("tmr%d stop program (Build: %s %s)\n", tmr_id, __DATE__, __TIME__);
    retval = drv_tmr_disable (tmr_id);
}

static void start (int32_t argc, char *argv[])
{
    TM_ID_E     tmr_id;
    TM_MODE_E   tmr_mode;
    uint32_t    tmr_init_count;

    int32_t     retval;
    int32_t     i;

    if (argc == 4) {
        tmr_id = strtol (argv[1], NULL, 10);
        tmr_mode = strtol (argv[2], NULL, 10);
        tmr_init_count = strtol (argv[3], NULL, 10);

    } else {
        printf ("\nUsage: start [id] [mode] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
        printf ("                       0: Countdown to 0 then stop\n");
        printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    if (((tmr_id < 1) || (tmr_id > 7) || (tmr_id == 4)) ||
        ((tmr_mode < 0) || (tmr_mode > 1)) ||
        ((tmr_init_count < 0) || (tmr_init_count > 65535))) {
        printf ("\nUsage: start [id] [mode] [init_count]\n");
        printf ("Options:\n");
        printf ("id                     Select a microsecond or millisecond tmr ID.\n");
        printf ("                       1: ID for microsecond tmr 1\n");
        printf ("                       2: ID for microsecond tmr 2\n");
        printf ("                       3: ID for microsecond tmr 3\n");
        printf ("                       5: ID for millisecond tmr 5\n");
        printf ("                       6: ID for millisecond tmr 6\n");
        printf ("                       7: ID for millisecond tmr 7\n");
        printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
        printf ("                       0: Countdown to 0 then stop\n");
        printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
        printf ("init_count             The tmr initial count.\n");
        printf ("                       If init_count is 0, then the tmr is disabled\n");
        printf ("                       Contains at least the [0, 65535] range\n");

        return;
    }

    printf ("tmr%d start program (Build: %s %s)\n", tmr_id, __DATE__, __TIME__);

    retval = drv_tmr_init (tmr_id);
    retval = drv_tmr_sw_rst (tmr_id);
    retval = drv_tmr_register_irq_handler (tmr_id, NULL);
    retval = drv_tmr_enable (tmr_id, tmr_mode,  tmr_init_count);
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: start [id] [mode] [init_count]\n");
    printf ("Options:\n");
    printf ("id                     Select a microsecond or millisecond tmr ID.\n");
    printf ("                       1: ID for microsecond tmr 1\n");
    printf ("                       2: ID for microsecond tmr 2\n");
    printf ("                       3: ID for microsecond tmr 3\n");
    printf ("                       5: ID for millisecond tmr 5\n");
    printf ("                       6: ID for millisecond tmr 6\n");
    printf ("                       7: ID for millisecond tmr 7\n");
    printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
    printf ("                       0: Countdown to 0 then stop\n");
    printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
    printf ("init_count             The tmr initial count.\n");
    printf ("                       If init_count is 0, then the tmr is disabled\n");
    printf ("                       Contains at least the [0, 65535] range\n");
    printf ("\n");

    printf ("Usage: stop [id]\n");
    printf ("Options:\n");
    printf ("id                     Select a microsecond or millisecond tmr ID.\n");
    printf ("                       1: ID for microsecond tmr 1\n");
    printf ("                       2: ID for microsecond tmr 2\n");
    printf ("                       3: ID for microsecond tmr 3\n");
    printf ("                       5: ID for millisecond tmr 5\n");
    printf ("                       6: ID for millisecond tmr 6\n");
    printf ("                       7: ID for millisecond tmr 7\n");
    printf ("\n");

    printf ("Usage: get [id]\n");
    printf ("Options:\n");
    printf ("id                     Select a microsecond or millisecond tmr ID.\n");
    printf ("                       1: ID for microsecond tmr 1\n");
    printf ("                       2: ID for microsecond tmr 2\n");
    printf ("                       3: ID for microsecond tmr 3\n");
    printf ("                       5: ID for millisecond tmr 5\n");
    printf ("                       6: ID for millisecond tmr 6\n");
    printf ("                       7: ID for millisecond tmr 7\n");
    printf ("\n");

    printf ("Usage: busy [id] [init_count]\n");
    printf ("Options:\n");
    printf ("id                     Select a microsecond or millisecond tmr ID.\n");
    printf ("                       1: ID for microsecond tmr 1\n");
    printf ("                       2: ID for microsecond tmr 2\n");
    printf ("                       3: ID for microsecond tmr 3\n");
    printf ("                       5: ID for millisecond tmr 5\n");
    printf ("                       6: ID for millisecond tmr 6\n");
    printf ("                       7: ID for millisecond tmr 7\n");
    printf ("init_count             The tmr initial count.\n");
    printf ("                       If init_count is 0, then the tmr is disabled\n");
    printf ("                       Contains at least the [0, 65535] range\n");
    printf ("\n");

    printf ("Usage: int [id] [mode] [init_count]\n");
    printf ("Options:\n");
    printf ("id                     Select a microsecond or millisecond tmr ID.\n");
    printf ("                       1: ID for microsecond tmr 1\n");
    printf ("                       2: ID for microsecond tmr 2\n");
    printf ("                       3: ID for microsecond tmr 3\n");
    printf ("                       5: ID for millisecond tmr 5\n");
    printf ("                       6: ID for millisecond tmr 6\n");
    printf ("                       7: ID for millisecond tmr 7\n");
    printf ("mode                   Countdown to 0 then stop or auto reload the tmr value.\n");
    printf ("                       0: Countdown to 0 then stop\n");
    printf ("                       1: Countdown to 0 then auto reload the tmr value and continue counting\n");
    printf ("init_count             The tmr initial count.\n");
    printf ("                       If init_count is 0, then the tmr is disabled\n");
    printf ("                       Contains at least the [0, 65535] range\n");
    printf ("\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
    {"help",            help,               "tmr help"},
    {"start",           start,              "tmr start countdown"},
    {"stop",            stop,               "tmr stop countdown"},
    {"get",             get,                "tmr get current count"},
    {"busy",            busy,               "cpu busy waiting"},
    {"int",             interrupt,          "tmr interrupt"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
