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

#include <string.h>
#include "sys/xr_debug.h"
#include "cmd_debug.h"
#include "cmd_util.h"
#include "efpg/efpg.h"
#include "console/console.h"

/*
 * efpg
 *
 * efpg get hosc
 * efpg get boot
 * efpg get dcxo
 * efpg get pout
 * efpg get mac
 * efpg get chipid
 */

static enum cmd_status cmd_efpg_start(void)
{
	const char *efpg_key = "efpgtest";

	cmd_write_respond(CMD_STATUS_OK, "OK");

	UART_ID uart_id = console_get_uart_id();
	if (uart_id == UART_NUM) {
		CMD_ERR("get uart id failed\n");
		return CMD_STATUS_ACKED;
	}

	if (efpg_start((uint8_t *)efpg_key, strlen(efpg_key), uart_id, console_disable, console_enable) < 0) {
		CMD_ERR("efpg program failed\n");
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_ACKED;
}

static void cmd_efpg_log(uint8_t *buf, uint8_t size)
{
	int i;

	CMD_SYSLOG("\n");
	for (i = 0; i < size; i++)
		CMD_SYSLOG("[%02d] 0x%02x\n", i, buf[i]);
	CMD_SYSLOG("\n");
}

static enum cmd_status cmd_efpg_get_exec(char *cmd)
{
	uint8_t buf[32] = {0};
	efpg_field_t field;

	if (cmd_strcmp(cmd, "hosc") == 0) {
		field = EFPG_FIELD_HOSC;
	} else if (cmd_strcmp(cmd, "boot") == 0) {
		field = EFPG_FIELD_BOOT;
	} else if (cmd_strcmp(cmd, "dcxo") == 0) {
		field = EFPG_FIELD_DCXO;
	} else if (cmd_strcmp(cmd, "pout") == 0) {
		field = EFPG_FIELD_POUT;
	} else if (cmd_strcmp(cmd, "mac") == 0) {
		field = EFPG_FIELD_MAC;
	} else if (cmd_strcmp(cmd, "chipid") == 0) {
		field = EFPG_FIELD_CHIPID;
	} else {
		CMD_ERR("invalid arg '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (efpg_read(field, buf) != 0) {
		CMD_ERR("efpg read failed, field %d\n", field);
		return CMD_STATUS_FAIL;
	}

	if (field == EFPG_FIELD_HOSC) {
		cmd_efpg_log(buf, 1);
	} else if (field == EFPG_FIELD_BOOT) {
		cmd_efpg_log(buf, 32);
	} else if (field == EFPG_FIELD_DCXO) {
		cmd_efpg_log(buf, 1);
	} else if (field == EFPG_FIELD_POUT) {
		cmd_efpg_log(buf, 3);
	} else if (field == EFPG_FIELD_MAC) {
		cmd_efpg_log(buf, 6);
	} else if (field == EFPG_FIELD_CHIPID) {
		cmd_efpg_log(buf, 16);
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_efpg_read_ua_exec(char *cmd)
{
	uint8_t* buf;
	int32_t cnt;
	uint32_t start_addr, len_bit, len_byte;

	cnt = cmd_sscanf(cmd, "a=%u l=%u", &start_addr, &len_bit);
	if (cnt != 2) {
		CMD_ERR("invalid arg '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	len_byte = len_bit / 8;
	if (len_bit % 8)
		len_byte ++;
	buf = cmd_malloc(len_byte);
	if (efpg_read_ua(start_addr, len_bit, buf)) {
		CMD_ERR("efpg read user area failed\n");
		return CMD_STATUS_FAIL;
	}
	CMD_DBG("data:\n");
	print_hex_dump_bytes(buf, len_byte);
	cmd_free(buf);

	return CMD_STATUS_OK;
}


static struct cmd_data g_efpg_cmds[] = {
    { "get", cmd_efpg_get_exec},
    { "read_user_area", cmd_efpg_read_ua_exec},
};

enum cmd_status cmd_efpg_exec(char *cmd)
{
	if (cmd[0] == '\0')
		return cmd_efpg_start();

	return cmd_exec(cmd, g_efpg_cmds, cmd_nitems(g_efpg_cmds));
}

