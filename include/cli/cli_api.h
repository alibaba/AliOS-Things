/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CLI_API_H
#define CLI_API_H

#include <stdint.h>

#define CLI_OK           0
#define CLI_ERR_NOMEM   -10000
#define CLI_ERR_DENIED  -10001
#define CLI_ERR_INVALID -10002
#define CLI_ERR_BADCMD  -10003
#define CLI_ERR_SYNTAX  -10004

#ifdef __cplusplus
extern "C" {
#endif

struct cli_command_st {
    const char *name;
    const char *help;

    void (*function)(char *outbuf, int32_t len, int32_t argc, char **argv);
};

#include <k_api.h>
#if (RHINO_CONFIG_UCLI > 0)
struct ucli_command {
    klist_t                      node;       /**< ucli cmd node */
    const struct cli_command_st *cmd;        /**< cli cmd pointer */
    kbuf_queue_t                *push_queue; /**< where the command should be pushed into */
    int                          owner_pid;  /**< owner process ID */
};

typedef struct {
    int    argc; /**< ucli cmd argu count */
    char **argv; /**< ucli cmd argu vector */
    void  *func; /**< ucli cmd function pointer */
} ucli_msg_t;

/**
 * @brief init process cli component
 *
 * @param[in] pid   the process ID
 *
 * @return On success, return 0, else return -1
 */
int cli_process_init(int pid);

/**
 * @brief delete all the cli cmds regitered by the process
 *
 * @param[in] pid   the process ID
 */
void cli_process_exit(int pid);

/**
 * @brief destory the process cli component
 *
 * @param[in] pid   the process ID
 */
void cli_process_destory(int pid);

/**
 * @brief Get CLI user command list head
 *
 *
 * @return user command list head
 *
 */
klist_t* cli_get_ucmd_list(void);
#endif

/**
 * @brief Initialize the CLI module
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_init(void);

/**
 * @brief Stop the CLI task and carry out the cleanup
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_stop(void);

/**
 * @brief Get CLI tag string for print
 *
 * @return CLI tag storing buffer
 *
 */
char *cli_tag_get(void);

/**
 * @brief This function registers a command with the command-line interface
 *
 * @param[in] cmd the structure to regiter one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_register_command(const struct cli_command_st *cmd);

/**
 * @brief This function unregisters a command from the command-line interface
 *
 * @param[in] cmd the structure to unregister one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_unregister_command(const struct cli_command_st *cmd);

/**
 * @brief This function registers a batch of CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of commands in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_register_commands(const struct cli_command_st *cmds, int32_t num);

/**
 * @brief This function unregisters a batch of CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of command in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_unregister_commands(const struct cli_command_st *cmds, int32_t num);

/**
 * @brief Print CLI message
 *
 * @param[in] buffer pointer to a char * buffer
 *
 * @return 0 on success, otherwise failed
 */
int32_t cli_printf(const char *buffer, ...);

/**
 * @brief Get the total number of CLI commands
 *
 * @return the total number
 *
 */
int32_t cli_get_commands_num(void);

/**
 * @brief Get the CLI command by index
 *
 * @param[in] index the command index
 *
 * @return the CLI command
 *
 */
struct cli_command_st *cli_get_command(int32_t index);

/**
 * @brief Get echo support status
 *
 * @return echo support status, 1: disable, 0: enable
 *
 */
int32_t cli_get_echo_status(void);

/**
 * @brief Set echo support status
 *
 * @param[in] status echo support status
 *
 * @return 0 on success, otherwise failed
 *
 */
int32_t cli_set_echo_status(int32_t status);

/**
* @brief CLI task entry
*
*/
void cli_main(void *data);

#ifdef __cplusplus
}
#endif

#endif /* CLI_API_H */
