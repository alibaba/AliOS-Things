/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_CLI_H
#define AOS_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"

typedef void (*cmd_fun_t)(char *outbuf, int len, int argc, char **argv);

struct cli_command {
    const char *name;
    const char *help;

    cmd_fun_t function;
};

#if (RHINO_CONFIG_UCLI > 0)
struct ucli_command {
    klist_t                   node;       /**< ucli cmd node */
    const struct cli_command *cmd;        /**< cli cmd pointer */
    void                     *push_queue; /**< where the command should be pushed into */
    int                       owner_pid;  /**< owner process ID */
};

typedef struct {
    int  argc;              /**< ucli cmd argu count */
    char **argv;            /**< ucli cmd argu vector */
    void *func;             /**< ucli cmd function pointer */
    char *cmd_console;      /**< ucli cmd exec console (uart/adb/websocket)*/
    void *console_exit_sem; /**< ucli console exit sem */
} ucli_msg_t;
#endif

#define SECTION(x)  __attribute__((section(x)))
#define USED        __attribute__((used))

typedef int (*cli_region_func)(int argc, char **argv);

/* cli region table */
struct cli_region {
    const char     *name;   /* the name of cli cmd*/
    const char     *desc;   /* description of cli cmd */
    cli_region_func func;   /* the cli function */
};

#define ALIOS_CLI_CMD_REGISTER(name, cmd, desc)                                      \
                const char __clisym_##cmd##_name[] SECTION(".rodata") = #cmd;        \
                const char __clisym_##cmd##_desc[] SECTION(".rodata") = #desc;       \
                static void name##_stub(char *buf, int len, int argc, char **argv) { \
                    name(argc, argv);                                                \
                }                                                                    \
                USED const struct cli_region __clisym_##cmd SECTION("CliRegion")=    \
                {                                                                    \
                    __clisym_##cmd##_name,                                           \
                    __clisym_##cmd##_desc,                                           \
                    (cli_region_func)&name##_stub                                    \
                };

/**
 * @brief Initialize the CLI module
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_init(void);

/**
 * @brief Stop the CLI task and carry out the cleanup
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_stop(void);

/**
 * @brief Get CLI tag string for print
 *
 * @return CLI tag storing buffer
 *
 */
const char *aos_cli_get_tag(void);

/**
 * @brief This function registers a command with the command-line interface
 *
 * @param[in] cmd the structure to regiter one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_register_command(const struct cli_command *cmd);

/**
 * @brief This function unregisters a command from the command-line interface
 *
 * @param[in] cmd the structure to unregister one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_unregister_command(const struct cli_command *cmd);

/**
 * @brief This function registers a batch of CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of commands in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_register_commands(const struct cli_command *cmds, int num);

/**
 * @brief This function unregisters a batch of CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of command in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_unregister_commands(const struct cli_command *cmds, int num);

/**
 * @brief Print CLI message
 *
 * @param[in] buffer pointer to a char * buffer
 *
 * @return 0 on success, otherwise failed
 */
int aos_cli_printf(const char *fmt, ...);

/**
 * @brief Prinf use cli console
 *
 * @param[in] console pointer to a console
 * @param[in] buffer pointer to a char * buffer
 * @param[in] len number of buf len
 *
 * @return len on bytes while console write
 */
int aos_cli_console_write(void *console, const void *buf, int len);

/**
 * @brief Set console to a task
 *
 * @param[in] task_handle pointer to a task
 * @param[in] console pointer to a console
 *
 * @return 0 on success, otherwise failed
 */
int aos_cli_task_set_console(void *task_handle, void *console);

#ifdef __cplusplus
}
#endif

#endif /* AOS_CLI_H */

