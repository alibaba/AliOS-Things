#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "k_api.h"
#include "cli_console.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#include "cli_api.h"
#include "cli_adapt.h"

#if AOS_COMP_DEBUG
#include "aos/debug.h"
#endif

#define CONSOLE_USER_INFO_ID (RHINO_CLI_CONSOLE_USER_INFO_POS)

extern cli_console cli_uart_console;

static dlist_t gCliConsolelist = AOS_DLIST_INIT(gCliConsolelist);

static cli_console *default_console = &cli_uart_console;
static cli_console *global_console = NULL;

static aos_sem_t console_sem;
static int mul_console_init_flag = 0;

static inline void cli_console_shared_sem_take(aos_sem_t *sem);
static inline void cli_console_shared_sem_post(aos_sem_t *sem);

static int register_cli_console_to_list(cli_console *console)
{
    if (console == NULL) {
        return -1;
    }

    dlist_init(&console->i_list);

    cli_console_shared_sem_take(&console_sem);
    dlist_add(&console->i_list, &gCliConsolelist);
    cli_console_shared_sem_post(&console_sem);
    return 0;
}

static int remove_cli_console_from_list(cli_console *console)
{
    dlist_t     *pos;
    dlist_t     *q;
    cli_console *tmp;

    if (console == NULL) {
        return -1;
    }

    cli_console_shared_sem_take(&console_sem);
    dlist_for_each_safe(pos, q, &gCliConsolelist) {
        tmp = aos_list_entry(pos, cli_console, i_list);
        if (tmp == console) {
            dlist_del(pos);
            break;
        }
    }
    cli_console_shared_sem_post(&console_sem);
    return 0;
}

static int cli_console_check_is_in_list(cli_console *console)
{
    dlist_t     *pos;
    dlist_t     *q;
    cli_console *tmp;

    int ret = 0;

    if (console == NULL) {
        return ret;
    }

    cli_console_shared_sem_take(&console_sem);
    dlist_for_each_safe(pos, q, &gCliConsolelist) {
        tmp = aos_list_entry(pos, cli_console, i_list);
        if (console == tmp) {
            ret = 1;
        }
    }
    cli_console_shared_sem_post(&console_sem);
    return ret;
}

int get_alive_console_num(void)
{
    dlist_t     *pos;
    dlist_t     *q;
    cli_console *tmp;

    int alive_console_n = 0;

    cli_console_shared_sem_take(&console_sem);
    dlist_for_each_safe(pos, q, &gCliConsolelist) {
        tmp = aos_list_entry(pos, cli_console, i_list);
        if (tmp->alive == 1) {
            alive_console_n++;
        }
    }
    cli_console_shared_sem_post(&console_sem);
    return alive_console_n;
}

cli_console *get_default_console(void)
{
    if (default_console->exit_flag == 1 || default_console->init_flag == 0) {
        return NULL;
    }
    return default_console;
}

cli_console *set_default_console(void *console)
{
    cli_console *last_console = default_console;
    default_console = console;
    return last_console;
}

cli_console *get_global_console(void)
{
    return global_console;
}

cli_console *set_global_console(void *console)
{
    cli_console *last_console = global_console;
    global_console = console;
    return last_console;
}

cli_console *get_current_console(void)
{
    cli_console *console = NULL;

    console = get_global_console();
    if (console) {
        return console;
    }

    console = get_clitask_console();
    return console != NULL && console->exit_flag != 1 && console->init_flag == 1 ? console : get_default_console();
}

/*
 * get real console from cli task
 * @return cli task console
 * */
cli_console *get_clitask_console(void)
{
    return (cli_console *)(cli_task_get_console(krhino_cur_task_get()));
}

void cli_console_set_tag_len(uint8_t cli_tag_len, int *error)
{
    int ret = -1;
    cli_console *console = get_clitask_console();
    if (console) {
        console->cli_tag_len = cli_tag_len;
        ret = 0;
    }

    if (error) {
        *error = ret;
    }
}

uint8_t cli_console_get_tag_len(int *error)
{
    int ret = -1;
    uint8_t cli_tag_len = 0;
    cli_console *console = get_clitask_console();
    if (console) {
        cli_tag_len = console->cli_tag_len;
        ret = 0;
    }

    if (error) {
        *error = ret;
    }

    return cli_tag_len;
}

void cli_console_set_tag(char cli_tag, uint32_t idx, int *error)
{
    int ret = -1;
    cli_console *console = get_clitask_console();
    if (console && idx < CLI_CONSOLE_TAG_LEN) {
        console->cli_tag[idx] = cli_tag;
        console->cli_tag_len++;
        ret = 0;
    }

    if (error) {
        *error = ret;
    }
}

char cli_console_get_tag(uint32_t idx, int *error)
{
    int ret = -1;
    char cli_tag_idx = 0;
    cli_console *console = get_clitask_console();
    if (console && idx < CLI_CONSOLE_TAG_LEN) {
        cli_tag_idx = console->cli_tag[idx];
        ret = 0;
    }

    if (error) {
        *error = ret;
    }

    return cli_tag_idx;
}

char *cli_console_get_all_tag(int *error)
{
    char *cli_tag = NULL;
    int ret = -1;
    cli_console *console = get_clitask_console();
    if (console) {
        cli_tag = console->cli_tag;
        ret = 0;
    }

    if (error) {
        *error = ret;
    }

    return cli_tag;
}

static inline void cli_console_shared_sem_take(aos_sem_t *sem)
{
    aos_sem_wait(sem, AOS_WAIT_FOREVER);
}

static inline void cli_console_shared_sem_post(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

char *cli_task_get_console(void *task_handle)
{
    ktask_t *task = (ktask_t *)task_handle;
    void *cli_console = NULL;
    if (task) {
        if (krhino_task_info_get(task, CONSOLE_USER_INFO_ID, &cli_console) == RHINO_SUCCESS) {
            return cli_console;
        }
    }
    return NULL;
}

int _cli_task_clear_console(void *task_handle)
{
    ktask_t *task = (ktask_t *)task_handle;
    if (task) {
        if (krhino_task_info_set(task, CONSOLE_USER_INFO_ID, NULL) != RHINO_SUCCESS) {
            printf("%s fail\r\n", __func__);
            return -1;
        }
    }
    return 0;
}

int _cli_task_set_console(void *task_handle, void *console)
{
    ktask_t *task = (ktask_t *)task_handle;

    if (task) {
        if (krhino_task_info_set(task, CONSOLE_USER_INFO_ID, console) != RHINO_SUCCESS) {
            printf("%s fail\r\n", __func__);
            return -1;
        }
    }
    return 0;
}

void cli_console_add_task_list_node(void *current_console, void *task)
{
    cli_console *console = (cli_console *)(current_console);

    if (console == NULL || task == NULL) {
        return;
    }

    wraper_task *wraper_task_node = malloc(sizeof(wraper_task));
    if (wraper_task_node == NULL) {
        return;
    }
    memset(wraper_task_node, 0, sizeof(wraper_task));

    aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);

    wraper_task_node->task = task;
    dlist_init(&wraper_task_node->task_list_node);
    dlist_add(&wraper_task_node->task_list_node, &console->task_list);

    aos_sem_signal(&console->sem);
}

int cli_task_clear_console(void *task_handle)
{
    return _cli_task_clear_console(task_handle);
}


int cli_task_set_console(void *task_handle, void *console)
{
    int ret;

    if (mul_console_init_flag == 1) {
        cli_console_shared_sem_take(&console_sem);
        ret = _cli_task_set_console(task_handle, console);
        cli_console_shared_sem_post(&console_sem);
    } else {
        ret = _cli_task_set_console(task_handle, console);
    }

    return ret;
}

void cli_console_remove_task_list_node(cli_console *console, void *task)
{
    dlist_t     *pos;
    dlist_t     *q;
    wraper_task *tmp;

    if (console == NULL) {
        return;
    }

    aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);
    dlist_for_each_safe(pos, q, &console->task_list) {
        tmp = aos_list_entry(pos, wraper_task, task_list_node);
        if (tmp && tmp->task == task) {
            dlist_del(pos);
            free(tmp);
            break;
        }
    }
    aos_sem_signal(&console->sem);
}

/* find task in console task_list
 * return 0 : not exist
 *        1 : exist
 */
int cli_console_check_task_list_node(cli_console *console, void *task)
{
    dlist_t     *pos;
    dlist_t     *q;
    wraper_task *tmp;

    int ret = 0;

    if ((console == NULL) || (task == NULL)) {
        return 0;
    }

    aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);
    dlist_for_each_safe(pos, q, &console->task_list) {
        tmp = aos_list_entry(pos, wraper_task, task_list_node);
        if (tmp && tmp->task == task) {
            ret = 1;
            break;
        }
    }
    aos_sem_signal(&console->sem);

    return ret;
}

/* clear console for all binded task; clean task_list for console*/
static void cli_console_clear_task_list(cli_console *console)
{
    dlist_t     *pos;
    dlist_t     *q;
    wraper_task *tmp;
    if (console == NULL) {
        return;
    }

    aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);

    dlist_for_each_safe(pos, q, &console->task_list) {
        tmp = aos_list_entry(pos, wraper_task, task_list_node);
        if (tmp && tmp->task) {
            cli_task_clear_console(tmp->task);
            dlist_del(pos);
            free(tmp);
        }
    }

    aos_sem_signal(&console->sem);
}

int cli_console_read(cli_console *console, void *buf, size_t nbytes)
{
    int rbytes = -1;

    if (!cli_console_check_is_in_list(console)) {
        console = get_current_console();
    }

    if (console == NULL) {
        console = get_current_console();
    }

    device_console *dev_console;
    if (console) {
        dev_console = console->dev_console;
        if (dev_console && dev_console->read) {
            rbytes = dev_console->read(buf, nbytes, console->private_data);
        }
    }

    return rbytes;
}

int cli_console_write(cli_console *console, const void *buf, size_t nbytes)
{
    int wbytes = -1, ret;

    if (!cli_console_check_is_in_list(console)) {
        console = get_current_console();
    }

    if (console == NULL) {
        console = get_current_console();
    }

    device_console *dev_console = NULL;
    if (console) {
        ret = aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);
        dev_console = console->dev_console;
        if (dev_console && dev_console->write) {
            wbytes = dev_console->write(buf, nbytes, console->private_data);
        }
        if (ret == 0) {
            /* sem take success*/
            aos_sem_signal(&console->sem);
        }
    }
    return wbytes;
}

int cli_console_init(cli_console *console)
{
    int ret;

    device_console *dev_console = NULL;

    if (console && console->init_flag == 0) {
        dlist_init(&console->task_list);
        register_cli_console_to_list(console);

        ret = aos_sem_new(&console->sem, 1);
        if (ret) {
            printf("%s %d %d\r\n", __func__, __LINE__, ret);
            return ret;
        }

        ret = aos_sem_new(&console->console_exit_sem, 0);
        if (ret) {
            printf("%s %d %d\r\n", __func__, __LINE__, ret);
            return ret;
        }

        console->att_pid = 0;

        dev_console = console->dev_console;
        if (dev_console && dev_console->init) {
            ret = dev_console->init(console->private_data);
            if (ret) {
                printf("%s %d %d\r\n", __func__, __LINE__, ret);
                return ret;
            }
            console->exit_flag = 0;
            console->init_flag = 1;
        }
        memset(console->cli_console_inbuf, 0, CLI_INBUF_SIZE);
    }

    return 0;
}

int cli_console_deinit(cli_console *console)
{
    int ret = -1;
    device_console *dev_console = NULL;

    if (global_console != NULL && global_console == console) {
        set_global_console(NULL);
    }

    if (console && console->init_flag == 1) {
        aos_sem_wait(&console->sem, AOS_WAIT_FOREVER);
        console->exit_flag = 1;
        aos_sem_signal(&console->sem);

        cli_console_clear_task_list(console);
        remove_cli_console_from_list(console);
        aos_sem_free(&console->sem);
        aos_sem_free(&console->console_exit_sem);
        console->att_pid = 0;
        dev_console = console->dev_console;
        if (dev_console && dev_console->deinit) {
            ret = dev_console->deinit(console->private_data);
            if (ret == 0) {
                console->init_flag = 0;
            }
        }
    }
    return ret;
}

/*
 * create new cli_console
 * @param dev_console: the device console is attached to the new cli console
 * @param name: the name of new cli console
 * @param private_data: the private data of the new cli console
 */
cli_console *cli_console_create(
            device_console *dev_console,
            const char *name,
            void *private_data)
{
    cli_console *console;
    if (dev_console == NULL) {
        return NULL;
    }

    console = malloc(sizeof(cli_console));
    if (console == NULL) {
        return NULL;
    }
    memset(console, 0, sizeof(cli_console));

    console->dev_console = dev_console;
    memcpy(console->name, name, CLI_CONSOLE_MAX_NAME_LEN - 1 > strlen(name) ? strlen(name) : CLI_CONSOLE_MAX_NAME_LEN - 1);
    console->private_data = private_data;
    return console;
}

int cli_console_destory(cli_console *console)
{
    int ret = -1;
    if (console) {
        free(console);
        ret = 0;
    }
    return ret;
}

/* get current console name
 * @return the name of current console name
 */
char *cli_task_get_console_name(void)
{
    cli_console *console;
    char *name = NULL;

    console = get_clitask_console();
    if (console) {
        name = console->name;
    }

    if (name == NULL) {
        name = default_console->name;
    }

    return name;
}

/* get target console name
 * @param console: the target console
 * @return the name of target console
 * */
char *cli_console_get_name(cli_console *console)
{
    char *name = NULL;
    if (console) {
        name = console->name;
    }

    if (name == NULL) {
        name = default_console->name;
    }

    return name;
}

int cli_console_check_invalid(cli_console *console)
{
    if (console == NULL) {
        return 0;
    }
    return 1;
}

int32_t create_default_console_task(uint32_t stack_size, uint32_t priority)
{
    int32_t ret = -1;
    extern cli_console cli_uart_console;

    if (mul_console_init_flag == 0) {
        ret = aos_sem_new(&console_sem, 1);
        if (ret) {
            printf("%s %d %d\r\n", __func__, __LINE__, ret);
            return ret;
        }
        mul_console_init_flag = 1;
    }
    return cli_console_task_create(&cli_uart_console, stack_size, priority);
}

/* create a cli console task
 * @param console: the console which is attached t the new task
 * @param stack_size: the stack size of the new cli task
 * @param priority: the priority of the new cli task
 * */
int cli_console_task_create(cli_console *console, uint32_t stack_size, uint32_t priority)
{
    int32_t ret;

    if (!cli_console_check_invalid(console)) {
        return -1;
    }
    ret = cli_console_init(console);
    if (ret) {
        printf("Error: Failed to init cli console\r\n");
        return -1;
    }
    console->alive = 1;

    ret = cli_task_create(console->name, cli_main, console, stack_size, priority);
    if (ret != CLI_OK) {
        printf("Error: Failed to create cli thread: %d\r\n", ret);
        return -1;
    }

    return 0;
}


/*
 * destory cli console task
 * @param console: cli_console need to be destoryed
 * @return 0
 * */
int cli_console_task_destory(cli_console *console)
{
    if (console == NULL) {
        return -1;
    }

#if 0
    if (console == get_default_console()) {
        printf("Can not delete default console\n");
        return -1;
    }
#endif

    cli_console_deinit(console);
    console->alive = 0;

    return 0;
}

void cli_console_current_task_destory(void)
{
    cli_console_task_destory(get_clitask_console());
}

/*
 * check cli_console exit_flag
 * @return 0/1, 0:the task should be alive; 1:the task should be deleted
 * */
int cli_console_task_check_exit_flag(void)
{
    int exit_flag = 0;
    cli_console *console;

    console = get_clitask_console();

    if (console) {
        exit_flag = console->exit_flag;
    } else {
        exit_flag = 1;
    }

    return exit_flag;
}

void cli_console_set_exit_flag(cli_console *console)
{
    if (console) {
        console->exit_flag = 1;
    }
}

void check_console_task_exit(void)
{
    krhino_task_dyn_del(NULL);
}

void cli_console_show_cmd(int argc, char **argv)
{
    dlist_t     *pos;
    dlist_t     *q;
    cli_console *tmp;

    int alive_console_n = 0;

    aos_cli_printf("-------- console show -----------\r\n");
    aos_cli_printf("id    name     att\r\n");

    dlist_for_each_safe(pos, q, &gCliConsolelist) {
        tmp = aos_list_entry(pos, cli_console, i_list);
        if (tmp == NULL)
            break;
        if (tmp->alive == 1) {
            alive_console_n++;
            aos_cli_printf("%d    %s  %d  ", alive_console_n, tmp->name, tmp->att_pid);
            if (tmp == get_clitask_console()) {
                aos_cli_printf("c\r\n");
            }
            aos_cli_printf("\r\n");
        }
    }
}

void cli_console_show(void)
{
    cli_console_show_cmd(0, NULL);
}

ALIOS_CLI_CMD_REGISTER(cli_console_show_cmd, ccs, Console Cli Console Show)
