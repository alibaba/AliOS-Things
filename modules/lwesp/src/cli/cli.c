/**
 * \file            cli.c
 * \brief           Command line interface
 */

/*
 * Copyright (c) 2019 Miha CESNIK
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Miha CESNIK <>
 * Version:         v1.1.2-dev
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cli/cli.h"

static cli_commands_t cli_command_table[CLI_MAX_MODULES];
static size_t num_of_modules;

static void cli_list(cli_printf cliprintf, int argc, char** argv);
static void cli_help(cli_printf cliprintf, int argc, char** argv);

/**
 * \brief           List of commands
 */
static const cli_command_t
commands[] = {
    { "help",           "Displays helptext for given command",      cli_help },
    { "list",           "Lists available commands",                 cli_list },
};

/**
 * \brief           Find the CLI command that matches the input string
 * \param[in]       command: pointer to command string for which we are searching
 * \return          pointer of the command if we found a match, else NULL
 */
const cli_command_t*
cli_lookup_command(char* command) {
    uint32_t module_index, command_index;
    for (module_index = 0; module_index < num_of_modules; ++module_index) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; ++command_index) {
            if (!strcmp(command, cli_command_table[module_index].commands[command_index].name)) {
                return &cli_command_table[module_index].commands[command_index];
            }
        }
    }
    return NULL;
}

/**
 * \brief           CLI auto completion function
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       cmd_buffer: CLI command buffer
 * \param[in]       cmd_pos: pointer to current courser postion in command buffer
 * \param[in]       print_options: additional prints in case of double tab
 */
void
cli_tab_auto_complete(cli_printf cliprintf, char* cmd_buffer, uint32_t* cmd_pos, bool print_options) {
    const char* matched_command = NULL;
    uint32_t module_index, command_index;
    uint32_t num_of_matched_commands = 0;
    uint32_t common_command_len = 0;

    for (module_index = 0; module_index < num_of_modules; ++module_index) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; ++command_index) {
            const cli_command_t* command = &cli_command_table[module_index].commands[command_index];
            if (!strncmp(cmd_buffer, cli_command_table[module_index].commands[command_index].name, *cmd_pos)) {
                /* Found a new command which matches the string */
                if (num_of_matched_commands == 0) {
                    /*
                     * Save the first match for later tab completion in case
                     * print_option is true (double tab)
                     */
                    matched_command = command->name;
                    common_command_len = strlen(matched_command);
                } else {
                    /*
                     * More then one match
                     * in case of print_option we need to print all options
                     */
                    if (print_options) {
                        /*
                         * Because we want to print help options only when we
                         * have multiple matches, print also the first one.
                         */
                        if (num_of_matched_commands == 1) {
                            cliprintf(CLI_NL"%s"CLI_NL, matched_command);
                        }
                        cliprintf("%s"CLI_NL, command->name);
                    }

                    /*
                     * Find the common prefix of all the matched commands for
                     * partial completion
                     */
                    uint32_t last_common_command_len = common_command_len;
                    common_command_len = 0;
                    while (matched_command[common_command_len] == command->name[common_command_len]
                           && matched_command[common_command_len] != '\0'
                           && command->name[common_command_len] != '\0'
                           && common_command_len < last_common_command_len) {
                        ++common_command_len;
                    }
                }
                ++num_of_matched_commands;
            }
        }
    }

    /* Do the full/partial tab completion */
    if (matched_command != NULL) {
        strncpy(cmd_buffer, matched_command, common_command_len);
        *cmd_pos = strlen(cmd_buffer);
        cmd_buffer[*cmd_pos] = '\0';
        cliprintf("\r"CLI_PROMPT"%s", cmd_buffer);
    }
}

/**
 * \brief           Register new CLI commands
 * \param[in]       commands: Pointer to commands table
 * \param[in]       num_of_commands: Number of new commands
 * \return          true when new commands where succesfully added, else false
 */
bool
cli_register_commands(const cli_command_t* commands, size_t num_of_commands) {
    if (num_of_modules >= CLI_MAX_MODULES) {
        printf("Exceeded the maximum number of CLI modules\n\r");
        return false;
    }

    /*
     * Warning: Not threadsafe!
     * TODO: add mutex that is initialized in init function
     */
    cli_command_table[num_of_modules].commands = commands;
    cli_command_table[num_of_modules].num_of_commands = num_of_commands;
    ++num_of_modules;

    return true;
}

/**
 * \brief           CLI Init function for adding basic CLI commands
 */
void
cli_init(void) {
    cli_register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

/**
 * \brief           CLI command for printing help text of specific command
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       argc: Number fo arguments in argv
 * \param[in]       argv: Pointer to the commands arguments
 */
static void
cli_help(cli_printf cliprintf, int argc, char** argv) {
    const cli_command_t* command;

    if (argc < 2) {
        cliprintf("Error: No function specified (try `list`)\n");
        return;
    }

    if ((command = cli_lookup_command(argv[1])) == NULL) {
        cliprintf("Error, command `%s` not found\n", argv[1]);
        return;
    }

    if (command->help != NULL) {
        cliprintf("%s\n", command->help);
    }
}

/**
 * \brief           CLI command for listing all available commands
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       argc: Number fo arguments in argv
 * \param[in]       argv: Pointer to the commands arguments
 */
static void
cli_list(cli_printf cliprintf, int argc, char** argv) {
    uint32_t module_index, command_index;

    cliprintf("%-20s%s"CLI_NL, "Command", "Description");
    cliprintf("-----------------------------------------------------------"CLI_NL);

    for (module_index = 0; module_index < num_of_modules; ++module_index) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; ++command_index) {
            cliprintf("%-20s%s"CLI_NL, cli_command_table[module_index].commands[command_index].name, cli_command_table[module_index].commands[command_index].help);
        }
    }
}

