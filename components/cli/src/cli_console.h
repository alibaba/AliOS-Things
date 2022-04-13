
#ifndef CLI_CONSOLE_H
#define CLI_CONSOLE_H

#include <stdint.h>
#include <stdlib.h>
#include "aos/kernel.h"
#include "aos/list.h"

#define CLI_CONSOLE_TAG_LEN 64

#define CLI_CONSOLE_MAX_NAME_LEN 32

typedef void (* command_callback)(void *private_data);

typedef struct _wraper_task {
    dlist_t task_list_node;
    void   *task;
} wraper_task;

typedef struct _device_console {
    char *name;
    int   fd;
    int (*read)(void *buf, size_t len, void *private_data);
    int (*write)(const void *buf, size_t len, void *private_data);
    int (*init)(void *private_data);
    int (*deinit)(void *private_data);
} device_console;

typedef struct _command_line_console {
    dlist_t i_list;
    device_console *dev_console;
    char name[CLI_CONSOLE_MAX_NAME_LEN];
    int init_flag;
    int exit_flag;
    int alive;
    void *private_data;
    void *task;
    char cli_tag[CLI_CONSOLE_TAG_LEN];
    unsigned char cli_tag_len;
    aos_sem_t sem;
    aos_sem_t console_exit_sem;
    unsigned int att_pid;
    command_callback finsh_callback;
    command_callback start_callback;
    dlist_t task_list;
    char cli_console_inbuf[CLI_INBUF_SIZE];
} cli_console;

int get_alive_console_num(void);

int32_t create_default_console_task(uint32_t stack_size, uint32_t priority);

cli_console *get_current_console(void);

cli_console *get_default_console(void);

cli_console *set_default_console(void *console);

cli_console *get_global_console(void);

cli_console *set_global_console(void *console);

cli_console *get_clitask_console(void);

void cli_console_set_exit_flag(cli_console *console);

void cli_console_set_tag_len(uint8_t cli_tag_len, int *error);

uint8_t cli_console_get_tag_len(int *error);

void cli_console_set_tag(char cli_tag, uint32_t idx, int *error);

char cli_console_get_tag(uint32_t idx, int *error);

char *cli_console_get_all_tag(int *error);

int cli_console_read(cli_console *console, void *buf, size_t nbytes);

int cli_console_write(cli_console *console, const void *buf, size_t nbytes);

int cli_console_deinit(cli_console *console);

int cli_console_init(cli_console *console);

cli_console *cli_console_create(device_console *dev_console, const char *name, void *private_data);

int cli_console_destory(cli_console *console);

int cli_console_task_create(cli_console *console, uint32_t stack_size, uint32_t priority);

int cli_console_task_destory(cli_console *console);

void cli_console_current_task_destory(void);

char *cli_task_get_console_name(void);

char *cli_console_get_name(cli_console *console);

int cli_console_task_check_exit_flag(void);

void check_console_task_exit(void);

int cli_task_set_console(void *task_handle, void *console);

char *cli_task_get_console(void *task_handle);

void cli_console_remove_task_list_node(cli_console *console, void *task);

void cli_console_add_task_list_node(void *current_console, void *task);

int cli_console_check_task_list_node(cli_console *console, void *task);

int cli_task_clear_console(void *task_handle);

void multiple_console_init(void);

int uart_console_write(const void *buf, size_t len, void *privata_data);

int uart_console_read(void *buf, size_t len, void *privata_data);

void cli_console_show(void);

#endif  /*CLI_CONSOLE_H*/
