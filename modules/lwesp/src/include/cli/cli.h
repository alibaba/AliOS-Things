/**
 * \file            cli.h
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
#ifndef CLI_HDR_CLI_H
#define CLI_HDR_CLI_H

#include <stdbool.h>
#include <stdint.h>
#include "cli/cli_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        CLI Command line interface
 * \brief           Command line interface
 * \{
 *
 * Functions to initialize everything needed for command line interface (CLI).
 */

/**
 * \brief           Printf handle for CLI
 * \param[in]       format: string format
 */
typedef void cli_printf(const char* format, ...);

/**
 * \brief           CLI entriy function
 * \param[in]       cliprintf: Printf handle callback
 * \param[in]       argc: Number of arguments
 * \param[in]       argv: Pointer to pointer to arguments
 */
typedef void cli_function(cli_printf cliprintf, int argc, char** argv);

/**
 * \brief           CLI command structure
 */
typedef struct {
    const char* name;                           /*!< Command name */
    const char* help;                           /*!< Command help */
    cli_function* func;                         /*!< Command function */
} cli_command_t;

/**
 * \brief           List of commands
 */
typedef struct {
    const cli_command_t* commands;              /*!< Pointer to commands */
    size_t num_of_commands;                     /*!< Total number of commands */
} cli_commands_t;

const cli_command_t*    cli_lookup_command(char* command);
void                    cli_tab_auto_complete(cli_printf cliprintf, char* cmd_buffer, uint32_t* cmd_pos, bool print_options);
bool                    cli_register_commands(const cli_command_t* commands, size_t num_of_commands);
void                    cli_init(void);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_CLI_H */

