/**
 * @file cli.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_CLI_H
#define AOS_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup aos_cli cli
 * Cli AOS API.
 * Used for APP.
 * @{
 */

/* This struct is used to define the cli cmd format */
struct cli_command {
    const char *name;   /**< cmd name */
    const char *help;   /**< cmd help info */

    void (*function)(char *outbuf, int len, int argc, char **argv); /**< cmd process function */
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
#define aos_cli_printf(fmt, ...) printf("%s" fmt, aos_cli_get_tag(), ##__VA_ARGS__)

/**
 * @brief Set cli login password
 *
 * @param[in] old_passwd
 * @param[in] new_passwd
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_chg_passwd(char *old_passwd, char *new_passwd);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_CLI_H */

