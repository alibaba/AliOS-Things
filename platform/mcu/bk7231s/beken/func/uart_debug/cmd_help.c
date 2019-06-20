#include "include.h"
#include "cmd_help.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "uart_pub.h"
#include "command_line.h"

int _do_help(cmd_tbl_t *cmd_start, int cmd_items, cmd_tbl_t *cmdtp, int flag,
             int argc, char *const argv[])
{
    int rcode = 0;
    int ret;

    cmd_tbl_t *cmd_array[COMMAND_MAX_COUNT];
    int i, j, swaps;
    cmd_tbl_t *temp;
    const char *usage = NULL;

    if (argc == 1)
    {
        cmdtp = cmd_start;
        for (i = 0; i < cmd_items; i++)
        {
            cmd_array[i] = cmdtp++;
        }

        for (i = cmd_items - 1; i > 0; i--)
        {
            swaps = 0;
            for (j = 0; j < i; j++)
            {
                ret = os_strcmp(cmd_array[j]->name, cmd_array[j + 1]->name);
                if (ret > 0)
                {
                    temp = cmd_array[j];
                    cmd_array[j] = cmd_array[j + 1];
                    cmd_array[j + 1] = temp;
                    swaps++;
                }
            }
            if (!swaps)
                break;
        }

        for (i = 0; i < cmd_items; i++)
        {
            usage = cmd_array[i]->usage;

            if (usage == NULL)
                continue;

            os_printf("msg %-*s- %s\n", CONFIG_SYS_HELP_CMD_WIDTH,
                      cmd_array[i]->name, usage);
        }
        return 0;
    }

    for (i = 1; i < argc; ++i)
    {
        cmdtp = cmd_find_tbl(argv[i], cmd_start, cmd_items);
        if (cmdtp != NULL)
        {
            rcode |= cmd_usage(cmdtp);
        }
        else
        {
            os_printf("Unknown command '%s' - try 'help' to list all of known commands\n\n",
                      argv[i]);
            rcode = 1;
        }
    }

    return rcode;
}

int do_help(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#if CFG_UART_DEBUG
    cmd_tbl_t *start = NULL;
    const int len = 0;

    start = entry_get_start();
    len = entry_get_count();

    return _do_help(start, len, cmdtp, flag, argc, argv);
#else
    return 0;
#endif
}

// eof

