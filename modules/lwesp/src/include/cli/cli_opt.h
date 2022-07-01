/**
 * \file            cli_opt.h
 * \brief           Command line interface configuration file
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
#ifndef LWESP_HDR_CLI_CONFIG_H
#define LWESP_HDR_CLI_CONFIG_H

#include "lwesp/lwesp_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         CLI
 * \defgroup        CLI_CONFIG Configuration
 * \brief           Default CLI configuration
 * \{
 *
 * Configuration for command line interface (CLI).
 */

/**
 * \brief           CLI promet, printed on every NL
 */
#ifndef CLI_PROMPT
#define CLI_PROMPT                  "> "
#endif

/**
 * \brief           CLI NL, default is NL and CR
 */
#ifndef CLI_NL
#define CLI_NL                      "\r\n"
#endif

/**
 * \brief           Max CLI command length
 */
#ifndef CLI_MAX_CMD_LENGTH
#define CLI_MAX_CMD_LENGTH          256
#endif

/**
 * \brief           Max sotred CLI commands to history
 */
#ifndef CLI_CMD_HISTORY
#define CLI_CMD_HISTORY             10
#endif


/**
 * \brief           Max CLI arguments in a single command
 */
#ifndef CLI_MAX_NUM_OF_ARGS
#define CLI_MAX_NUM_OF_ARGS         16
#endif

/**
 * \brief           Max modules for CLI
 */
#ifndef CLI_MAX_MODULES
#define CLI_MAX_MODULES             16
#endif

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_CLI_CONFIG_H */
