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

#include "cmd_debug.h"
#include "cmd_util.h"
#include "cmd_flash.h"
#include "driver/chip/hal_flash.h"

#define MFLASH 0

static enum cmd_status cmd_flash_start_exec(char *cmd)
{
	if (HAL_Flash_Open(MFLASH, 5000) != HAL_OK)
	{
		CMD_ERR("flash driver open failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_flash_stop_exec(char *cmd)
{
	/* deinie driver */
	if (HAL_Flash_Close(0) != HAL_OK) {
		CMD_ERR("flash driver close failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

#define FLASH_TEST_BUF_SIZE (0x100)

static enum cmd_status cmd_flash_erase_exec(char *cmd)
{
	int32_t cnt;
	char size_str[8];
	uint32_t addr;
	FlashEraseMode size_type;
	int32_t size;
	uint8_t buf[FLASH_TEST_BUF_SIZE];

	/* get param */
	cnt = cmd_sscanf(cmd, "s=%7s a=0x%x", size_str, &addr);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(size_str, "chip") == 0) {
		size_type = FLASH_ERASE_CHIP;
		size = 0;
	} else if (cmd_strcmp(size_str, "64kb") == 0) {
		size = 0x10000;
		size_type = FLASH_ERASE_64KB;
	} else if (cmd_strcmp(size_str, "32kb") == 0) {
		size_type = FLASH_ERASE_32KB;
		size = 0x8000;
	} else if (cmd_strcmp(size_str, "4kb") == 0) {
		size = 0x1000;
		size_type = FLASH_ERASE_4KB;
	} else {
		CMD_ERR("invalid size %s\n", size_str);
		return CMD_STATUS_INVALID_ARG;
	}

	/* erase */
	HAL_Flash_MemoryOf(MFLASH, size_type, addr, &addr);
	if (HAL_Flash_Erase(MFLASH, size_type, addr, 1) != HAL_OK) {
		CMD_ERR("flash erase failed\n");
		return CMD_STATUS_FAIL;
	}

	while (size > 0) {
		int32_t tmp_size = (size < FLASH_TEST_BUF_SIZE) ? size : FLASH_TEST_BUF_SIZE;

//		CMD_DBG("tmp_size: %d\n", tmp_size);

		if (HAL_Flash_Read(MFLASH, addr, buf, tmp_size) != HAL_OK) {
			CMD_ERR("flash read failed\n");
			return CMD_STATUS_FAIL;
		}

		size -= tmp_size;
		addr += tmp_size;

		while (--tmp_size >= 0) {
			if ((uint8_t)(~(buf[tmp_size])) != 0) {
				CMD_ERR("flash write failed: read data from flash != 0xFF, ~data = 0x%x, tmp_size = %d\n", (uint8_t)(~(buf[tmp_size])), tmp_size);
				return CMD_STATUS_FAIL;
			}
		}

	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_flash_write_exec(char *cmd)
{
	uint32_t cnt;
	uint32_t addr;
	int32_t size;
	uint8_t *wbuf;
	uint8_t *rbuf;

	/* get param */
	cnt = cmd_sscanf(cmd, "a=0x%x s=%d", &addr, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	wbuf = cmd_malloc(size);
	if (wbuf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	rbuf = cmd_malloc(size);
	if (rbuf == NULL) {
		CMD_ERR("no memory\n");
		cmd_free(wbuf);
		return CMD_STATUS_FAIL;
	}


	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	cmd_raw_mode_read(wbuf, size, 30000);


	/* write */
	if (HAL_Flash_Write(MFLASH, addr, wbuf, size) != HAL_OK) {
		CMD_ERR("flash write failed\n");
	}

	if (HAL_Flash_Read(MFLASH, addr, rbuf, size) != HAL_OK) {
		CMD_ERR("flash read failed\n");
	}

	cmd_raw_mode_write(rbuf, size);
	cmd_raw_mode_disable();

	cmd_free(wbuf);
	cmd_free(rbuf);

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_flash_read_exec(char *cmd)
{
	int32_t cnt;
	uint8_t *buf;
	uint32_t addr;
	uint32_t size;

	/* get param */
	cnt = cmd_sscanf(cmd, "a=0x%x s=%u", &addr, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}


	/* read */
	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		cmd_free(buf);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	if (HAL_Flash_Read(MFLASH, addr, buf, size) != HAL_OK) {
		CMD_ERR("spi driver read failed\n");
	}

	cmd_raw_mode_enable();
	cmd_msleep(1000);
	cmd_raw_mode_write(buf, size);
	cmd_raw_mode_disable();

	cmd_free(buf);
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_flash_overwrite_exec(char *cmd)
{
	uint32_t cnt;
	uint32_t addr;
	int32_t size;
	uint8_t *wbuf;
	int ret;

	/* get param */
	cnt = cmd_sscanf(cmd, "a=0x%x s=%d", &addr, &size);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	wbuf = cmd_malloc(size);
	if (wbuf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	cmd_raw_mode_read(wbuf, size, 30000);

	/* write */
	if ((ret = HAL_Flash_Overwrite(MFLASH, addr, wbuf, size)) != HAL_OK) {
		CMD_ERR("flash write failed: %d\n", ret);
	}

	if ((ret = HAL_Flash_Check(MFLASH, addr, wbuf, size)) != 0) {
		CMD_ERR("flash write not success %d\n", ret);
	}

	cmd_raw_mode_write(wbuf, size);
	cmd_raw_mode_disable();

	cmd_free(wbuf);

	return CMD_STATUS_ACKED;
}

/*
 * brief Flash Auto Test Command
 * command	start {spiNum} {csNum} {freq} {mode}
 * 			stop
 * 			config {ioMode}// -----need to be supported by Flash chip
 * 			erase {size} {addr}
 * 			write {addr} "{str}"
 * 			read {str/hex} {addr} {size} // recommanded that size should not too large
 */
static struct cmd_data g_flash_cmds[] = {
	{ "start",	cmd_flash_start_exec	},
	{ "stop",	cmd_flash_stop_exec		},
	{ "erase",	cmd_flash_erase_exec	},
	{ "write",	cmd_flash_write_exec	},
	{ "read",	cmd_flash_read_exec		},
	{ "overwrite",	cmd_flash_overwrite_exec		},
};

enum cmd_status cmd_flash_exec(char *cmd)
{
	return cmd_exec(cmd, g_flash_cmds, cmd_nitems(g_flash_cmds));
}




