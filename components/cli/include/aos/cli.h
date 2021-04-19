/**
 * @file cli.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_CLI_H
#define AOS_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup cli_aos_api cli
 * @{
 */

/* This struct is used to define the cli cmd format */
typedef void (*cmd_fun_t)(char *outbuf, int len, int argc, char **argv);

struct cli_command
{
    const char *name;
    const char *help;
    cmd_fun_t   function;
};

#define SECTION(x) __attribute__((section(x)))
#define USED __attribute__((used))

typedef int (*cli_region_func)(int argc, char **argv);

/* cli region table */
struct cli_region
{
    const char *name;     /* the name of cli cmd*/
    const char *desc;     /* description of cli cmd */
    cli_region_func func; /* the cli function */
};

#define ALIOS_CLI_CMD_REGISTER(name, cmd, desc)                        \
    const char __clisym_##cmd##_name[] SECTION(".rodata") = #cmd;      \
    const char __clisym_##cmd##_desc[] SECTION(".rodata") = #desc;     \
    static void name##_stub(char *buf, int len, int argc, char **argv) \
    {                                                                  \
        name(argc, argv);                                              \
    }                                                                  \
    USED const struct cli_region __clisym_##cmd SECTION("CliRegion") = \
        {                                                              \
            __clisym_##cmd##_name,                                     \
            __clisym_##cmd##_desc,                                     \
            (cli_region_func)&name##_stub};


/**
 * @brief Initialize the CLI module
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_init(void);

/**
 * @brief This function register a command with the command-line interface
 *
 * @param[in] cmd the structure to regiter one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_register_command(const struct cli_command *cmd);

/**
 * @brief This function unregister a command from the command-line interface
 *
 * @param[in] cmd the structure to unregister one CLI command
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_unregister_command(const struct cli_command *cmd);

/**
 * @brief This function register multi CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of commands in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_register_commands(const struct cli_command *cmds, int num);

/**
 * @brief This function unregisters multi CLI commands
 *
 * @param[in] cmds pointer to an array of commands
 * @param[in] num  number of command in the array
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_unregister_commands(const struct cli_command *cmds, int num);

/**
 * @brief use aos_cli_printf instead of printf in cli cmd
 *
 * @param[in] fmt pointer to a char * buffer
 *
 * @return 0 on success, otherwise failed
 */
int aos_cli_printf(const char *fmt, ...);

/**
 * @brief Suspend cli task
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_suspend(void);

/**
 * @brief Resume cli task
 *
 * @return 0 on success, otherwise failed
 *
 */
int aos_cli_resume(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* AOS_CLI_H */

