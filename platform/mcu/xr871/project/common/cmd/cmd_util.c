/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include <stdarg.h>


/* cmd format: <command-name> <arg>... */
enum cmd_status cmd_exec(char *cmd, struct cmd_data *cdata, int count)
{
	int i;
	char *args;

	args = cmd_strchr(cmd, ' ');
	if (args) {
		*args++ = '\0'; /* has arguments */
	}

	for (i = 0; i < count; ++i, ++cdata) {
		if (cmd_strcmp(cmd, cdata->name) == 0) {
			return cdata->exec(args ? args : "");
		}
	}

	CMD_ERR("unknown cmd '%s'\n", cmd);
	return CMD_STATUS_UNKNOWN_CMD;
}

/* cmd2 format: <command-name>[ <arg>...] */
enum cmd_status cmd2_exec(char *cmd, struct cmd2_data *cdata, int count)
{
	int i;

	for (i = 0; i < count; ++i, ++cdata) {
		if (cmd_strncmp(cmd, cdata->name, cdata->name_len) == 0) {
			return cdata->exec(cmd + cdata->name_len);
		}
	}

	CMD_ERR("unknown cmd '%s'\n", cmd);
	return CMD_STATUS_UNKNOWN_CMD;
}

/* parse all argument vectors from a command string, return argument count */
int cmd_parse_argv(char *cmd, char *argv[], int size)
{
	int last, argc;
	char *start, *end;

	argc = 0;
	start = cmd;

	while (argc < size && *start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		argv[argc++] = start;
		if (last)
			break;
		start = end + 1;
	}

	if (argc > 0 && argc < size) {
		argv[argc] = NULL; /* ANSI-C requirement */
	}

	return argc;
}

static const char *cmd_status_success_desc[CMD_STATUS_SUCCESS_MAX + 1 -
                                           CMD_STATUS_SUCCESS_MIN] = {
	"OK",
};

static const char *cmd_status_error_desc[CMD_STATUS_ERROR_MAX + 1 -
                                         CMD_STATUS_ERROR_MIN] = {
	"Unknown command",
	"Invalid argument",
	"Fail",
};

static const char *cmd_empty_desc = "";

const char *cmd_get_status_desc(enum cmd_status status)
{
	if (status >= CMD_STATUS_SUCCESS_MIN &&
		status <= CMD_STATUS_SUCCESS_MAX) {
		return cmd_status_success_desc[status - CMD_STATUS_SUCCESS_MIN];
	} else if (status >= CMD_STATUS_ERROR_MIN &&
			   status <= CMD_STATUS_ERROR_MAX) {
		return cmd_status_error_desc[status - CMD_STATUS_ERROR_MIN];
	}

	return cmd_empty_desc;
}

#if 0
static const char *cmd_event_desc[CMD_EVENT_MAX + 1 - CMD_EVENT_MIN] = {
	"Test finish",
	"timer notify"
};

const char *cmd_get_event_desc(enum cmd_event event)
{
	if (event >= CMD_EVENT_MIN && event <= CMD_EVENT_MAX) {
		return cmd_event_desc[event - CMD_EVENT_MIN];
	}

	return cmd_empty_desc;
}
#endif

int cmd_write(enum cmd_code_type type, int code, const char *fmt, ...)
{
	static char str_buf[400];

	va_list ap;
	int len, left;
	char *ptr;

	ptr = str_buf;
	left = sizeof(str_buf) - 1; /* reserve 1 byte for '\n' */
	len = cmd_snprintf(ptr, left, "<%s> %d ",
	                   type == CMD_CODE_TYEP_STATUS ? "ACK" : "EVT", code);

	ptr += len;
	left -= len;
	va_start(ap, fmt);
	len = vsnprintf(ptr, left, fmt, ap);
	va_end(ap);

	ptr += len;
	left -= len;
	left += 1;
	len = cmd_snprintf(ptr, left, "\n");

	left -= len;
	len = sizeof(str_buf) - left;

	return console_write((uint8_t *)str_buf, len);
}

int32_t cmd_raw_mode_read(uint8_t *buf, int32_t size, uint32_t msec)
{
	UART_ID uart_id = console_get_uart_id();

	if (uart_id < UART_NUM) {
		return HAL_UART_Receive_Poll(uart_id, buf, size, msec);
	} else {
		return -1;
	}
}

int32_t cmd_raw_mode_write(uint8_t *buf, int32_t size)
{
	UART_ID uart_id = console_get_uart_id();

	if (uart_id < UART_NUM) {
		return HAL_UART_Transmit_Poll(uart_id, buf, size);
	} else {
		return -1;
	}
}
