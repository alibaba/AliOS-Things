/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <k_api.h>
#include <k_proc.h>
#include <aos/cli.h>
#include <task_group.h>
#include <cli_console.h>
#include <cli_adapt.h>
#include <cli_api.h>
#include <res.h>
#include <utask.h>
#include <ucli.h>
#include <fcntl.h>
#if (RHINO_CONFIG_USIGNAL > 0)
#include <aos/signal.h>
#endif

extern int optind;

extern void dump_pgtbl(int asid, unsigned long start, unsigned long end, int verbose);

static const struct ucli_command *ucli_lookup_cmd(char *name, int len, uint32_t pid)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    klist_t             *next;
    task_group_t        *group;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return NULL;
    }

    head = &group->cli_cmd_list;
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

int ucli_proc_cmd(int argc, char *argv[], uint32_t pid)
{
    const struct ucli_command *ucmd;

    void          *user_ptr;
    char         **argv_ptr;
    char          *ptr;
    size_t         size;
    size_t         str_len;
    int            arg_cnt;
    ucli_msg_t     ucli_msg;
    aos_hdl_t      queue;
    ucmd = ucli_lookup_cmd(argv[0], 0, pid);
    if (ucmd != NULL) {
        if (argc > 0) {
            size = 0;
            for (arg_cnt = 0; arg_cnt < argc; arg_cnt++) {
                size += strlen(argv[arg_cnt]) + 1;
            }
            size += arg_cnt * sizeof(void *);
            user_ptr = res_malloc(ucmd->owner_pid, size);
            if (user_ptr) {
                memset(user_ptr, 0, size);
                argv_ptr = (char **)user_ptr;
                ptr = (char *)user_ptr + argc * sizeof(void *);
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
        ucli_msg.func             = (void *)ucmd->cmd->function;
        ucli_msg.cmd_console      = (char *)get_clitask_console();
        queue.hdl                 = ucmd->push_queue;
        aos_queue_send(&queue, (void*)&ucli_msg, sizeof(ucli_msg_t));
        return 0;
    }

    return 1;
}

int ucli_register_cmd(const struct cli_command *cmd)
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

    krhino_mutex_lock(&group->cli_mutex, RHINO_WAIT_FOREVER);
    head = &group->cli_cmd_list;
    iter = head->next;
    while (iter != head) {
        next = iter->next;
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        if (!strcmp(ucmd->cmd->name, cmd->name)) {
            printf("Warning: user cmd %s is already registered\r\n", cmd->name);
            ret = CLI_OK;
            goto out;
        }
        iter = next;
    }

    ucmd = (struct ucli_command *)malloc(sizeof(struct ucli_command));
    if (NULL == ucmd) {
        ret = CLI_ERR_NOMEM;
        goto out;
    }

    ucmd->cmd = cmd;
    ucmd->push_queue = group->cli_q;
    ucmd->owner_pid = group->pid;
    klist_add(head, &ucmd->node);
    group->cli_cmd_cnt++;

    ret = CLI_OK;

out:
    krhino_mutex_unlock(&group->cli_mutex);

    return ret;
}

int ucli_unregister_cmd(const struct cli_command *cmd)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    klist_t             *next;
    ktask_t             *cur_task;
    task_group_t        *group;
    int                  ret;

    cur_task = krhino_cur_task_get();
    group = cur_task->task_group;
    if (NULL == group) {
        return CLI_ERR_DENIED;
    }

    krhino_mutex_lock(&group->cli_mutex, RHINO_WAIT_FOREVER);
    head = &group->cli_cmd_list;
    iter = head->next;
    while (iter != head) {
        next = iter->next;
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        if (!strcmp(ucmd->cmd->name, cmd->name)) {
            printf("%s: unregister ucmd %s\r\n",  __func__, ucmd->cmd->name);
            klist_rm(&ucmd->node);
            free(ucmd);
            if (group->cli_cmd_cnt > 0) {
                group->cli_cmd_cnt--;
            }
            ret = CLI_OK;
            goto out;
        }
        iter = next;
    }

    ret = CLI_ERR_CMDNOTEXIST;

out:
    krhino_mutex_unlock(&group->cli_mutex);

    return ret;
}

/* show all app cli cmds, called by kernel cli "help"*/
void ucli_show_cmds(void)
{
    struct ucli_command *ucmd;
    task_group_t        *group;
    klist_t             *head, *iter, *group_head, *group_iter;

    group_head = task_group_get_list_head();

    if (!is_klist_empty(group_head)) {
        aos_cli_printf("\r\n==== User space Commands ====\r\n");
        for (group_iter = group_head->next; group_iter != group_head; group_iter = group_iter->next) {
            group = group_info_entry(group_iter, task_group_t, node);
            aos_cli_printf("------------------------------\r\n");
            aos_cli_printf("[%s - pid:%d - cmd_num:%d]\r\n", group->tg_name, group->pid, group->cli_cmd_cnt);

            head = &group->cli_cmd_list;
            iter = head->next;
            while (iter != head) {
                ucmd = krhino_list_entry(iter, struct ucli_command, node);
                iter = iter->next;
                if (ucmd->cmd->name) {
                    aos_cli_printf("%-20s: %s\r\n", ucmd->cmd->name, ucmd->cmd->help ? ucmd->cmd->help : "");
                }
            }
            aos_cli_printf("\r\n");
        }
    }
}

static void process_info_cmd(char *buf, int len, int argc, char **argv)
{
    klist_t      *head;
    klist_t      *iter;
    task_group_t *group;

    head  = task_group_get_list_head();

    if (!is_klist_empty(head)) {
        aos_cli_printf("============ process info ============\r\n");
        aos_cli_printf("Name                pid       tasks\r\n");
        for (iter  = head->next; iter != head; iter = iter->next) {
            group = group_info_entry(iter, task_group_t, node);
            aos_cli_printf("%-20s%-10d%-10d\r\n", group->tg_name, group->pid, group->task_cnt);
        }
    }
}

static void kill_process_cmd(char *buf, int len, int argc, char **argv)
{
    uint32_t pid;

    if (argc == 2) {
        pid = atoi(argv[1]);
        if (pid > 0) {
            krhino_uprocess_kill(pid);
        }
#if (RHINO_CONFIG_USIGNAL > 0)
    } else if (argc == 3) {
        int sig = atoi(argv[1]+1);
        pid = atoi(argv[2]);
        aos_kill(pid, sig);
#endif
    } else {
        aos_cli_printf("Usage: kill [-signum] pid\r\n");
        return;
    }
}

static void attach_process_cmd(char *buf, int len, int argc, char **argv)
{
    uint32_t pid;
    task_group_t *group;

    if (argc != 2) {
        aos_cli_printf("Usage: attach pid\r\n");
        return;
    }

    pid = atoi(argv[1]);

    if (pid == 0) {
        ucli_attach_pid_set(pid);
        aos_cli_printf("Usage: attach to kernel\r\n");
        return;
    }

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        aos_cli_printf("Usage: wrong pid\r\n");
        return;
    }

    ucli_attach_pid_set(pid);
    aos_cli_printf("Usage: attach to %s success\r\n", group->tg_name);
}

static void load_app_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;

    if (argc != 2) {
        aos_cli_printf("Usage: load app_name\r\n");
        return;
    }

    ret = k_proc_load(argv[1], NULL);

    if (ret > 0) {
        aos_cli_printf("load %s success\r\n", argv[1]);
    } else {
        aos_cli_printf("load %s failed\r\n", argv[1]);
    }
}

static void dump_pgtbl_help(void)
{
    aos_cli_printf("Usage: pgtbl [-p pid] [-v]\r\n");
    aos_cli_printf("\t-p pid: print process page table whose PID is pid\r\n");
    aos_cli_printf("\t        on default, print kernel page table\r\n");
    aos_cli_printf("\t-v    : print page table verbosely\r\n");
}

static void dump_pgtbl_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    int cmd;
    int pid = 0;
    int verbose = 0;

    optind = 0;
    while ((cmd = getopt(argc, argv, "hvp:")) != -1) {
        switch (cmd) {
            case 'h':
                dump_pgtbl_help();
                return;
            case 'p':
                pid = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                dump_pgtbl_help();
                return;
        }
    }

    dump_pgtbl(pid, 0, 0, verbose);
}

static const struct cli_command ucli_builtin_cmds[] = {
    { "kps", "process info", process_info_cmd },
    { "kill", "kill process", kill_process_cmd },
    { "pgtbl", "dump pgtbl", dump_pgtbl_cmd},
    { "load", "load app in elf format", load_app_cmd},
    { "att", "attach process for app cli cmd", attach_process_cmd },
};

uint32_t ucli_attach_pid_get(void)
{
    cli_console *cur_console = get_clitask_console();
    if (cur_console) {
        return cur_console->att_pid;
    }
    return 0;
}

void ucli_attach_pid_set(uint32_t pid)
{
    cli_console *cur_console = get_clitask_console();
    if (cur_console) {
        cur_console->att_pid = pid;
    }
}

int ucli_register_default_cmds(void)
{
    return aos_cli_register_commands(ucli_builtin_cmds, sizeof(ucli_builtin_cmds) / sizeof(struct cli_command));
}

int ucli_init(task_group_t *group)
{
    int           ret;

    aos_hdl_t cli_buf_q;
    uint32_t pid = group->pid;
    char name1[USER_NAME_MAX + 1] = {0};
    snprintf(name1, USER_NAME_MAX, "%s%d", USER_CLI_BUF_QUEUE, pid);
    printf("name1 = %s \r\n", name1);


    ret = aos_queue_open(&cli_buf_q, name1, O_CREAT,
                         (sizeof(ucli_msg_t) + sizeof(intptr_t)) * 2, sizeof(ucli_msg_t));

    if (ret != 0) {
        return -1;
    }

    group->cli_q = cli_buf_q.hdl;

    klist_init(&group->cli_cmd_list);
    ret = krhino_mutex_create(&group->cli_mutex, "ucli_mutex");

    if (ret != RHINO_SUCCESS) {
        return -1;
    }

    return 0;
}

void ucli_exit(task_group_t *group)
{
    struct ucli_command *ucmd;
    klist_t             *head;
    klist_t             *iter;
    int                  pid;
    aos_hdl_t            queue;
    char name1[USER_NAME_MAX + 1] = {0};
    pid   = group->pid;
    head = &group->cli_cmd_list;
    iter = head->next;

    while (iter != head) {
        ucmd = krhino_list_entry(iter, struct ucli_command, node);
        iter = iter->next;
        if (ucmd->owner_pid == pid) {
            klist_rm(&ucmd->node);
            free(ucmd);
        }
    }


    snprintf(name1, USER_NAME_MAX, "%s%d", USER_CLI_BUF_QUEUE, pid);
    queue.hdl = group->cli_q;
    aos_queue_close(&queue);
    aos_queue_unlink(name1);
    group->cli_q = NULL;

    krhino_mutex_del(&group->cli_mutex);

    /* set attach pid to kernel*/
    ucli_attach_pid_set(0);
}

