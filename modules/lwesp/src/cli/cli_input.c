/**
 * \file            cli_input.c
 * \brief           Command line interface helper funcions for paring input data
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
#include "cli/cli.h"
#include "cli/cli_input.h"
#include "cli/cli_opt.h"

/* Statically allocate CLI buffer to eliminate overhead of using heap*/
static char cmd_buffer[CLI_MAX_CMD_LENGTH];
static uint32_t cmd_pos;

static char cmd_history_buffer[CLI_CMD_HISTORY][CLI_MAX_CMD_LENGTH];
static uint32_t cmd_history_pos;
static uint32_t cmd_history_full;

/**
 * \brief           Clear the command buffer and reset the position
 */
static void
clear_cmd_buffer( void ) {
    memset(cmd_buffer, 0x0, sizeof(cmd_buffer));
    cmd_pos = 0;
}

/**
 * \brief           Stores the command to history
 */
static void
store_command_to_history(void) {
    uint32_t hist_count;
    if (strcmp(cmd_history_buffer[0], cmd_buffer)) {
        for (hist_count = CLI_CMD_HISTORY - 1; hist_count > 0; --hist_count) {
            memcpy(cmd_history_buffer[hist_count], cmd_history_buffer[hist_count - 1], CLI_MAX_CMD_LENGTH);
        }
        ++cmd_history_full;
        if (cmd_history_full > CLI_CMD_HISTORY) {
            cmd_history_full = CLI_CMD_HISTORY;
        }
        memcpy(cmd_history_buffer[0], cmd_buffer, CLI_MAX_CMD_LENGTH);
        cmd_history_buffer[0][CLI_MAX_CMD_LENGTH - 1] = '\0';
    }
}

/**
 * \brief           Special key sequence check
 *                      ^[3~ : Delete (TODO)
 *                      ^[A  : Up
 *                      ^[B  : Down
 *                      ^[C  : Right
 *                      ^[D  : Left
 *                      ^[1~ : Home (TODO)
 *                      ^OF  : End (TODO)
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       ch: input char from CLI
 * \return          true when special key sequence is active, else false
 */
static bool
cli_special_key_check(cli_printf cliprintf, char ch) {
    static uint32_t key_sequence;
    static char last_ch;
    bool special_key_found = false;

    if (key_sequence == 0 && ch == 27) {
        special_key_found = true;
        key_sequence = 1;
    } else if (key_sequence == 1 && (ch == '[' || ch == 'O')) {
        special_key_found = true;
        key_sequence = 2;
    } else if (key_sequence == 2 && ch >= 'A' && ch <= 'D') {
        special_key_found = true;
        key_sequence = 0;
        switch (ch) {
            case 'A':                           /* Up */
                if (cmd_history_pos < cmd_history_full) {
                    /* Clear the line */
                    memset(cmd_buffer, ' ', cmd_pos);
                    cliprintf("\r%s       \r" CLI_PROMPT, cmd_buffer);

                    strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
                    cmd_pos = strlen(cmd_buffer);
                    cliprintf("%s", cmd_buffer);

                    ++cmd_history_pos;
                } else {
                    cliprintf("\a");
                }
                break;
            case 'B':                           /* Down */
                if (cmd_history_pos > 0) {
                    /* Clear the line */
                    memset(cmd_buffer, ' ', cmd_pos);
                    cliprintf("\r%s       \r" CLI_PROMPT, cmd_buffer);

                    if (--cmd_history_pos != 0) {
                        strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
                        cmd_pos = strlen(cmd_buffer);
                        cliprintf("%s", cmd_buffer);
                    } else {
                        clear_cmd_buffer();
                    }
                } else {
                    cliprintf("\a");
                }

                break;
            case 'C':                           /* Right */
                /* TODO not finnished
                 * need to implement a courser
                 */
                if (cmd_pos < strlen(cmd_buffer)) {
                    ++cmd_pos;
                    cliprintf("\033[\1C");
                } else {
                    cliprintf("\a");
                }
                break;
            case 'D':                           /* Left */
                /* TODO not finnished
                 * need to implement a courser
                 */
                if (cmd_pos > 0) {
                    --cmd_pos;
                    cliprintf("\033[\1D");
                } else {
                    cliprintf("\a");
                }
                break;
        }
    } else if (key_sequence == 2 && (ch == 'F')) {
        /* End*/
        /* TODO: for now just return invalid key */
        cliprintf("\a");
    } else if (key_sequence == 2 && (ch == '1' || ch == '3')) {
        /* Home or Delete, we need to check one more character */
        special_key_found = true;
        key_sequence = 3;
    } else if (key_sequence == 3) {
        /* TODO Home and Delete: for now just return invalid key */
        cliprintf("\a");
        special_key_found = true;
    } else {
        /* Unknown sequence */
        key_sequence = 0;
    }

    /* Store the last character */
    last_ch = ch;

    (void)last_ch;                              /* Prevent compiler warnings */

    return special_key_found;
}

/**
 * \brief           parse and execute the given command
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       input: input string to parse
 * \return          `true` when command is found and parsed, else `false`
 */
static bool
cli_parse_and_execute_command(cli_printf cliprintf, char* input) {
    const cli_command_t* command;
    char* argv[CLI_MAX_NUM_OF_ARGS];
    uint32_t argc = 0;

    argv[argc] = strtok(input, " ");
    while (argv[argc] != NULL) {
        argv[++argc] = strtok (NULL, " ");
    }

    if ((command = cli_lookup_command(argv[0])) == NULL) {
        cliprintf("Unknown command: %s\n", argv[0]);
        return false;
    } else {
        command->func(cliprintf, argc, argv);
    }

    return true;
}

/**
 * \brief           parse new characters to the CLI
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       ch: new character to CLI
 */
void
cli_in_data(cli_printf cliprintf, char ch) {
    static char last_ch;

    if (!cli_special_key_check(cliprintf, ch)) {
        /* Parse the characters only if they are not part of the special key sequence */
        switch (ch) {
            /* Backspace */
            case '\b':
            case 127:
                if (cmd_pos != 0) {
                    /* TODO not finished
                     * in case courser is not at the end this doesn't work properly
                     */
                    cmd_buffer[--cmd_pos] = '\0';
                    cliprintf("\033[\1D");
                    cliprintf("\033[K");
                } else {
                    cliprintf("\a");
                }
                break;
            /* Tab for autocomplete */
            case '\t':
                cli_tab_auto_complete(cliprintf, cmd_buffer, &cmd_pos, (last_ch == '\t'));
                break;
            /* New line -> new command */
            case '\n':
            case '\r':
                cmd_history_pos = 0;
                if (!strlen(cmd_buffer)) {
                    clear_cmd_buffer();
                    cliprintf(CLI_NL CLI_PROMPT);
                    return;
                }

                cliprintf(CLI_NL);
                store_command_to_history();
                cli_parse_and_execute_command(cliprintf, cmd_buffer);

                clear_cmd_buffer();
                cliprintf(CLI_NL CLI_PROMPT);
                break;
            /* All other chars */
            default:
                if (cmd_pos < CLI_MAX_CMD_LENGTH) {
                    cmd_buffer[cmd_pos++] = ch;
                } else {
                    clear_cmd_buffer();
                    cliprintf(CLI_NL"\aERR: Command too long"CLI_NL CLI_PROMPT);
                    return;
                }
                cliprintf("%c", ch);
        }
    }

    /* Store last character for double tab detection */
    last_ch = ch;
}

