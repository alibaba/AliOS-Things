/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "aos/cli.h"
#include "utask.h"
#include "uassert.h"
#include "cli_console.h"
#include "task_group.h"
#include "cli_api.h"
#include "umm.h"

typedef void (*cli_func)(char *pbuffer, int outlen, int argc, char **argv);

ktask_t *ucli_task_obj;

struct cli_status {
    uint32_t num;
    const struct cli_command *cmds[CLI_MAX_COMMANDS];
};

static struct cli_status g_cli;

void cli_cmd_add(const struct cli_command *cmd)
{
    g_cli.cmds[g_cli.num++] = cmd;
}

void cli_cmd_rm(const struct cli_command *cmd)
{
    uint32_t i, remaining_cmds;

    for (i = 0; i < g_cli.num; i++) {
        if (g_cli.cmds[i] == cmd) {
            g_cli.num--;

            remaining_cmds = g_cli.num - i;
            if (remaining_cmds > 0) {
                memmove(&g_cli.cmds[i], &g_cli.cmds[i + 1], (remaining_cmds * sizeof(struct cli_command *)));
            }

            g_cli.cmds[g_cli.num] = NULL;
        }
    }
}

void cli_cmds_add(const struct cli_command *cmd, int32_t num)
{
    uint32_t i;

    for (i = 0; i < num; i++) {
        cli_cmd_add(cmd++);
    }
}

void cli_cmds_rm(const struct cli_command *cmd, int32_t num)
{
    uint32_t i;

    for (i = 0; i < num; i++) {
        cli_cmd_rm(cmd++);
    }
}

static void cli_cmds_reset()
{
    memset(&g_cli, 0, sizeof(struct cli_status));
}

void cli_cmds_show(void)
{
    uint32_t i;
    const struct cli_command *cmd = NULL;

    for (i = 0; i < g_cli.num; i++) {
        cmd = g_cli.cmds[i];
        if (cmd->name) {
            aos_cli_printf("%-20s: %s\r\n", cmd->name, cmd->help ? cmd->help : "");
        }
    }
    aos_cli_printf("==== cmd num : %d ====\r\n", g_cli.num);
}

void ucli_cmd_exc_task(void *arg)
{
    ucli_msg_t *cmd = (ucli_msg_t *)arg;

    /* set console for cli cmd*/
    cli_console *console = (cli_console *)(cmd->cmd_console);

    aos_cli_task_set_console(krhino_cur_task_get(), console);

    /* now exec cmd */
    ((cli_func)cmd->func)(NULL, 0, cmd->argc, cmd->argv);

    if (cmd->argc > 0) {
        free(cmd->argv);
    }
    free(cmd);
}

void ucli_cmd_exec(ucli_msg_t *cmd)
{
    kstat_t stat;
    int     arg_cnt, size, str_len;
    void   *user_ptr;
    char  **argv_ptr;
    char   *ptr;
    int     argc = cmd->argc;

    ktask_t *ucli_cmd_exec_task_obj;

    ucli_msg_t *ucli_cmd = malloc(sizeof(ucli_msg_t));
    if (!ucli_cmd) {
        printf("ucli cmd malloc fail\r\n");
        return;
    }
    memset(ucli_cmd, 0, sizeof(ucli_msg_t));

    if (argc > 0) {
        size = 0;
        for (arg_cnt = 0; arg_cnt < argc; arg_cnt++) {
            size += strlen((cmd->argv)[arg_cnt]) + 1;
        }
        size += arg_cnt * sizeof(void *);
        user_ptr = malloc(size);
        if (user_ptr) {
            memset(user_ptr, 0, size);
            argv_ptr = (char **)user_ptr;
            ptr = (char *)user_ptr + argc * sizeof(void *);
            for (arg_cnt = 0; arg_cnt < argc; arg_cnt++) {
                str_len = strlen((cmd->argv)[arg_cnt]);
                memcpy(ptr, (cmd->argv)[arg_cnt], str_len);
                argv_ptr[arg_cnt] = ptr;
                ptr += str_len + 1;
            }
            ucli_cmd->argc = argc;
            ucli_cmd->argv = argv_ptr;
        }
    } else {
        ucli_cmd->argc = 0;
        ucli_cmd->argv = NULL;
    }

    ucli_cmd->func             = cmd->func;
    ucli_cmd->cmd_console      = cmd->cmd_console;
    //ucli_cmd->console_exit_sem = cmd->console_exit_sem;

    aos_cli_task_set_console(krhino_cur_task_get(), ucli_cmd->cmd_console);

    stat = krhino_utask_dyn_create(&ucli_cmd_exec_task_obj, "ucli_cmd_exc_task", (void *)ucli_cmd, 50, 0,
                                   8192, 4096, ucli_cmd_exc_task, 1);

    if (stat != RHINO_SUCCESS) {
        printf("ERROR !!! app cmd run fail !\r\n");
    }
}

void ucli_task(void *arg)
{
    kstat_t     stat;
    ucli_msg_t  ucli_msg;
    size_t      size;

    aos_hdl_t res_q;
    char name1[USER_NAME_MAX + 1] = {0};
    uint32_t pid = aos_getpid();

    snprintf(name1, USER_NAME_MAX, "%s%d", USER_CLI_BUF_QUEUE, pid);

    stat = aos_queue_open(&res_q, name1, 0, 1, 1);
    uassert(stat == 0);

    while (1) {
        stat = aos_queue_recv(&res_q, RHINO_WAIT_FOREVER, &ucli_msg, &size);
        if (0 == stat) {
            if (NULL != ucli_msg.func) {
                ucli_cmd_exec(&ucli_msg);
            }

            if (ucli_msg.argc > 0) {
                free(ucli_msg.argv);
            }
        }
    }
}

void *cli_malloc(uint32_t size)
{
    return umm_alloc(size);
}

__attribute__((weak)) struct cli_region _cli_region_begin, _cli_region_end;

void usr_cli_register_init(void)
{
    int    ret;
    int    addr;
    struct cli_region  *index;
    struct cli_command *cmd;

    for (addr = (int)&_cli_region_begin; addr < (int)&_cli_region_end;) {
        index = (struct cli_region *)addr;
        addr  += sizeof(struct cli_region);

        cmd = (struct cli_command *)cli_malloc(sizeof(struct cli_command));
        if (cmd == NULL) {
            printf("usr cli malloc fail\n");
            return;
        }

        cmd->name     = index->name;
        cmd->help     = index->desc;
        cmd->function = (cmd_fun_t)(index->func);

        ret = aos_cli_register_command(cmd);
        if (ret != CLI_OK) {
            printf("usr cli register fail\n");
            return;
        }
    }
}

int ucli_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio)
{
    kstat_t stat;

    cli_cmds_reset();

    usr_cli_register_init();

    stat = krhino_utask_dyn_create(&ucli_task_obj, "ucli_task", NULL, prio, (tick_t)50,
                                   ustack_size, kstack_size, ucli_task, 1);

    if (stat != RHINO_SUCCESS) {
        uassert(0);
    }

    return 0;
}

