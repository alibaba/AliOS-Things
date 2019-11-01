#ifndef _MESH_SHELL_H_
#define _MESH_SHELL_H_

/** @brief Callback called when command is entered.
 *
 *  @param argc Number of parameters passed.
 *  @param argv Array of option strings. First option is always command name.
 *
 * @return 0 in case of success or negative value in case of error.
 */
typedef int (*shell_cmd_function_t)(int argc, char *argv[]);

struct mesh_shell_cmd {
        const char *cmd_name;
        shell_cmd_function_t cb;
        const char *help;
        const char *desc;
};

struct mesh_shell_cmd *bt_mesh_get_shell_cmd_list();

#endif
