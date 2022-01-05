/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PERIPH_CONSOLE_H_
#define _PERIPH_CONSOLE_H_

#include "sys/queue.h"
#include "audio_error.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef esp_err_t (*console_cmd_callback_t)(esp_periph_handle_t periph, int argc, char *argv[]);

#define CONSOLE_DEFAULT_TASK_PRIO       (5)
#define CONSOLE_DEFAULT_TASK_STACK      (1024*5)
#define CONSOLE_DEFAULT_BUFFER_SIZE     (256)
#define CONSOLE_DEFAULT_PROMPT_STRING   "esp32>"



/**
 * @brief      Command structure
 */
typedef struct {
    const char              *cmd;   /*!< Name of command, must be unique */
    int                     id;     /*!< Command ID will be sent together when the command is matched */
    const char              *help;  /*!< Explanation of the command */
    console_cmd_callback_t  func;   /*!< Function callback for the command */
} periph_console_cmd_t;

/**
 * @brief      Console Peripheral configuration
 */
typedef struct {
    int                         command_num;    /*!< Total number of commands */
    const periph_console_cmd_t  *commands;      /*!< Pointer to array of commands */
    int                         task_stack;     /*!< Console task stack, using default if the value is zero */
    int                         task_prio;      /*!< Console task priority (based on freeRTOS priority), using default if the value is zero */
    int                         buffer_size;    /*!< Size of console input buffer */
    const char                  *prompt_string; /*!< Console prompt string, using default CONSOLE_PROMPT_STRING if the pointer is NULL */
} periph_console_cfg_t;

/**
 * @brief      Initialize Console Peripheral
 *
 * @param      config  The configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_console_init(periph_console_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
