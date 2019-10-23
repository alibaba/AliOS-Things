/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <cli/cli_api.h>
#include <task_group.h>

struct ucli_command {
    klist_t                      node;       /**< ucli cmd node */
    const struct cli_command_st *cmd;        /**< cli cmd pointer */
    kbuf_queue_t                *push_queue; /**< where the command should be pushed into */
    int                          owner_pid;  /**< owner process ID */
};

static klist_t ucmd_list_head;
static kmutex_t ucli_mutex;

static const struct ucli_command* ucli_lookup_cmd(char *name, int len)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    klist_t             *next;

    head = &ucmd_list_head;
    iter = head->next;
    /* check whether the cmd has been registered */
    while (iter != head) {
        next = iter->next;
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        if (len) {
            if (!strncmp(ucmd->cmd->name, name, len)) {
                return ucmd;
            }
        } else {
            if (!strcmp(ucmd->cmd->name, name)) {
                return ucmd;
            }
        }
        iter = next;
    }

    return NULL;
}

int ucli_proc_cmd(int argc, const char *argv[])
{
    const struct ucli_command *ucmd;

    task_group_t  *group;
    void          *user_ptr;
    char         **argv_ptr;
    char          *ptr;
    size_t         size;
    size_t         str_len;
    int            arg_cnt;
    ucli_msg_t     ucli_msg;

    ucmd = ucli_lookup_cmd(argv[0], 0);
    if (ucmd != NULL) {
        if (argc > 0) {
            size = 0;
            for (arg_cnt = 0; arg_cnt < argc; arg_cnt++) {
                 size += strlen(argv[arg_cnt]) + 1;
            }
            size += arg_cnt * sizeof(void*);
            user_ptr = res_malloc(ucmd->owner_pid, size);
            if (user_ptr) {
                memset(user_ptr, 0, size);
                argv_ptr = (char**)user_ptr;
                ptr = (char*)user_ptr + argc * sizeof(void*);
                for (arg_cnt = 0; arg_cnt < argc; arg_cnt++) {
                    str_len = strlen(argv[arg_cnt]);
                    memcpy(ptr, argv[arg_cnt], str_len);
                    argv_ptr[arg_cnt] = ptr;
                    ptr += str_len + 1;
                }
                ucli_msg.argc = argc;
                ucli_msg.argv = argv_ptr;
            }
        } else {
            ucli_msg.argc = 0;
            ucli_msg.argv = NULL;
        }
        ucli_msg.func = (void*)ucmd->cmd->function;
        krhino_buf_queue_send(ucmd->push_queue, (void*)&ucli_msg, sizeof(ucli_msg_t));
        return 0;
    }

   return 1;
}

int ucli_register_cmd(const struct cli_command_st *cmd)
{
    klist_t             *head;
    struct ucli_command *ucmd;
    klist_t             *iter, *next;
    ktask_t             *cur_task;
    task_group_t        *group;
    int                  ret;

    cur_task = krhino_cur_task_get();
    group = cur_task->task_group;
    if (NULL == group) {
        return CLI_ERR_DENIED;
    }

    krhino_mutex_lock(&ucli_mutex, RHINO_WAIT_FOREVER);
    head = &ucmd_list_head;
    iter = head->next;
    while (iter != head) {
        next = iter->next;
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        if (!strcmp(ucmd->cmd->name, cmd->name)) {
            cli_printf("Warning: user cmd %s is already registered\r\n", cmd->name);
            ret = CLI_OK;
            goto out;
        }
        iter = next;
    }

    ucmd = (struct ucli_command*)malloc(sizeof(struct ucli_command));
    if (NULL == ucmd) {
        ret = CLI_ERR_NOMEM;
        goto out;
    }

    ucmd->cmd = cmd;
    ucmd->push_queue = group->cli_q;
    ucmd->owner_pid = group->pid;
    klist_add(head, &ucmd->node);
    ret = CLI_OK;

out:
    krhino_mutex_unlock(&ucli_mutex);

    return ret;
}

int ucli_unregister_cmd(const struct cli_command_st *cmd)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    klist_t             *next;
    int                  ret;

    krhino_mutex_lock(&ucli_mutex, RHINO_WAIT_FOREVER);
    head = &ucmd_list_head;
    iter = head->next;
    while (iter != head) {
        next = iter->next;
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        if (!strcmp(ucmd->cmd->name, cmd->name)) {
            cli_printf("%s: unregister ucmd %s\r\n",  __func__, ucmd->cmd->name);
            klist_rm(&ucmd->node);
            cli_free(ucmd);
            ret = CLI_OK;
            goto out;
        }
        iter = next;
    }

    ret = CLI_ERR_CMDNOTEXIST;

out:
    krhino_mutex_unlock(&ucli_mutex);

    return ret;
}

void ucli_show_cmds(void)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;

    cli_printf("====User space Commands====\r\n");
    head = &ucmd_list_head;
    iter = head->next;
    while (iter != head) {
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        iter = iter->next;
        if (ucmd->cmd->name) {
            cli_printf("%-10s: %s\r\n", ucmd->cmd->name,
                            ucmd->cmd->help ? ucmd->cmd->help: "");
        }
    }
}

void ucli_early_init(void)
{
    klist_init(&ucmd_list_head);
    krhino_mutex_create(&ucli_mutex, "ucli_mutex");
}

static void process_info_cmd(char *buf, int len, int argc, char **argv)
{
    klist_t      *head;
    klist_t      *iter;
    task_group_t *group;

    head  = task_group_get_list_head();

    if (!is_klist_empty(head)) {
        cli_printf("============ process info ============\r\n");
        cli_printf("Name                pid       tasks\r\n");
        for (iter  = head->next; iter != head; iter = iter->next) {
            group = group_info_entry(iter, task_group_t, node);
            cli_printf("%-20s%-10d%-10d\r\n",group->tg_name, group->pid, group->task_cnt);
        }
    }
}

static void kill_process_cmd(char *buf, int len, int argc, char **argv)
{
    int pid;

    if (argc != 2) {
        cli_printf("Usage: kill pid\r\n");
        return;
    }

    pid = atoi(argv[1]);

    if (pid > 0) {
        krhino_uprocess_kill(pid);
    }
}

static const struct cli_command_st ucli_builtin_cmds[] = {
    { "ps", "process info", process_info_cmd },
    { "kill", "kill process", kill_process_cmd },
};

int ucli_register_default_cmds(void)
{
    return cli_register_commands(ucli_builtin_cmds, sizeof(ucli_builtin_cmds)/sizeof(struct cli_command_st));
}

int ucli_init(task_group_t *group)
{
    kbuf_queue_t *cli_buf_q;
    int ret;

    ret = krhino_fix_buf_queue_dyn_create(&cli_buf_q,
                                          "cli_buf_queue",
                                          sizeof(ucli_msg_t),
                                          2);
    if (ret != RHINO_SUCCESS) {
        return -1;
    }

    group->cli_q = cli_buf_q;

    return 0;
}

void ucli_exit(task_group_t *group)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    int                  pid;

    pid   = group->pid;
    head = &ucmd_list_head;
    iter = head->next;
    while (iter != head) {
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        iter = iter->next;
        if (ucmd->owner_pid == pid) {
            klist_rm(&ucmd->node);
            free(ucmd);
        }
    }

   krhino_buf_queue_dyn_del(group->cli_q);
   group->cli_q = NULL;
}

