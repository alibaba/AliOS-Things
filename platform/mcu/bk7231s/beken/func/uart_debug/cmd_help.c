#include "include.h"
#include "cmd_help.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "uart_pub.h"
#include "command_line.h"

int _do_help(cmd_tbl_t *cmd_start, int cmd_items, cmd_tbl_t *cmdtp, int flag,
             int argc, char *const argv[])
{
    int i;
    int rcode = 0;

    if (argc == 1)  	/* show list of commands */
    {
        cmd_tbl_t *cmd_array[COMMAND_MAX_COUNT];
        int i, j, swaps;

        /* Make array of commands from .uboot_cmd section */
        cmdtp = cmd_start;
        for (i = 0; i < cmd_items; i++)
        {
            cmd_array[i] = cmdtp++;
        }

        /* Sort command list (trivial bubble sort) */
        for (i = cmd_items - 1; i > 0; --i)
        {
            swaps = 0;
            for (j = 0; j < i; ++j)
            {
                if (os_strcmp(cmd_array[j]->name,
                              cmd_array[j + 1]->name) > 0)
                {
                    cmd_tbl_t *tmp;
                    tmp = cmd_array[j];
                    cmd_array[j] = cmd_array[j + 1];
                    cmd_array[j + 1] = tmp;
                    ++swaps;
                }
            }
            if (!swaps)
                break;
        }

        /* print short help (usage) */
        for (i = 0; i < cmd_items; i++)
        {
            const char *usage = cmd_array[i]->usage;

            /* allow user abort */

            if (usage == NULL)
                continue;

            os_printf("%-*s- %s\n", CONFIG_SYS_HELP_CMD_WIDTH,
                      cmd_array[i]->name, usage);
        }
        return 0;
    }

    /*
     * command help (long version)
     */
    for (i = 1; i < argc; ++i)
    {
        cmdtp = cmd_find_tbl(argv[i], cmd_start, cmd_items);
        if (cmdtp != NULL)
        {
            rcode |= cmd_usage(cmdtp);
        }
        else
        {
            os_printf("Unknown command '%s' - try 'help' without arguments for list of all known commands\n\n",
                      argv[i]);
            rcode = 1;
        }
    }
    return rcode;
}


/*
 * Use puts() instead of os_printf() to avoid os_printf buffer overflow
 * for long help messages
 */
int do_help(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#if CFG_UART_DEBUG
    cmd_tbl_t *start = entry_get_start();
    const int len = entry_get_count();

    return _do_help(start, len, cmdtp, flag, argc, argv);
#else
    return 0;
#endif
}

// eof

