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
#include "sys/ota.h"

/*
 * ota file <url>
 * ota http <url>
 */

enum cmd_status cmd_ota_file_exec(char *cmd)
{
	if (cmd[0] == '\0') {
		CMD_ERR("OTA empty file url\n");
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	if (ota_get_image(OTA_PROTOCOL_FILE, cmd) != OTA_STATUS_OK) {
		CMD_ERR("OTA file get image failed\n");
		return CMD_STATUS_ACKED;
	}

	if (ota_verify_image(OTA_VERIFY_NONE, NULL)  != OTA_STATUS_OK) {
		CMD_ERR("OTA file verify image failed\n");
		return CMD_STATUS_ACKED;
	}

	ota_reboot();

	return CMD_STATUS_ACKED;
}

enum cmd_status cmd_ota_http_exec(char *cmd)
{
	if (cmd[0] == '\0') {
		CMD_ERR("OTA empty http url\n");
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	if (ota_get_image(OTA_PROTOCOL_HTTP, cmd) != OTA_STATUS_OK) {
		CMD_ERR("OTA http get image failed\n");
		return CMD_STATUS_ACKED;
	}

	if (ota_verify_image(OTA_VERIFY_NONE, NULL)  != OTA_STATUS_OK) {
		CMD_ERR("OTA http verify image failed\n");
		return CMD_STATUS_ACKED;
	}

	ota_reboot();

	return CMD_STATUS_ACKED;
}

static struct cmd_data g_ota_cmds[] = {
    { "file",	cmd_ota_file_exec},
    { "http",	cmd_ota_http_exec},
};

enum cmd_status cmd_ota_exec(char *cmd)
{
	return cmd_exec(cmd, g_ota_cmds, cmd_nitems(g_ota_cmds));
}
