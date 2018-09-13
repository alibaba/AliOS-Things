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
#include "cmd_i2c.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_i2c.h"


#define DRV_TEST_I2C_NORM	0
#define DRV_TEST_I2C_MEM	1
#define DRV_TEST_I2C_SCCB	0

#define DRV_TEST_I2C_ADDR_10BIT	0

static enum cmd_status cmd_i2c_init_exec(char *cmd)
{
	int cnt;
	uint32_t id, addr_mode, clock_freq;
	I2C_InitParam i2c_param;
	HAL_Status hal_status;

	cnt = cmd_sscanf(cmd, "i=%u a=%u c=%u", &id, &addr_mode, &clock_freq);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

#if DRV_TEST_I2C_ADDR_10BIT
	if ((addr_mode != 7) && (addr_mode != 10)) {
#else /* DRV_TEST_I2C_ADDR_10BIT */
	if (addr_mode != 7) {
#endif /* DRV_TEST_I2C_ADDR_10BIT */
		CMD_ERR("invalid addr mode %u\n", addr_mode);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((clock_freq < 100000) || (clock_freq > 400000)) {
		CMD_ERR("invalid clock freq %u\n", clock_freq);
		return CMD_STATUS_INVALID_ARG;
	}

#if DRV_TEST_I2C_ADDR_10BIT
	if (addr_mode == 10)
		i2c_param.addrMode = I2C_ADDR_MODE_10BIT;
	else
#endif /* DRV_TEST_I2C_ADDR_10BIT */
		i2c_param.addrMode = I2C_ADDR_MODE_7BIT;
	i2c_param.clockFreq = clock_freq;

	hal_status = HAL_I2C_Init((I2C_ID)id, &i2c_param);
	if (hal_status == HAL_OK) {
		return CMD_STATUS_OK;
	} else {
		CMD_ERR("HAL_I2C_Init return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}
}

static enum cmd_status cmd_i2c_deinit_exec(char *cmd)
{
	int cnt;
	uint32_t id;
	HAL_Status hal_status;

	cnt = cmd_sscanf(cmd, "i=%u", &id);
	if (cnt != 1) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	hal_status = HAL_I2C_DeInit((I2C_ID)id);
	if (hal_status == HAL_OK) {
		return CMD_STATUS_OK;
	} else {
		CMD_ERR("HAL_I2C_DeInit return: hal_status = %d\n", hal_status);
		return CMD_STATUS_FAIL;
	}
}

#if DRV_TEST_I2C_NORM
static enum cmd_status cmd_i2c_transmit_norm_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, len;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u l=%u", &id, &dev_addr, &len);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

#if DRV_TEST_I2C_ADDR_10BIT
	if ((dev_addr >> 10) != 0) {
#else /* DRV_TEST_I2C_ADDR_10BIT */
	if ((dev_addr >> 7) != 0) {
#endif /* DRV_TEST_I2C_ADDR_10BIT */
		CMD_ERR("invalid dev_addr %u\n", dev_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((len >> 31) != 0) {
		CMD_ERR("invalid len %u\n", len);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(len * sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	size = cmd_raw_mode_read(buf, (int32_t)len, 10000);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but raw mode read size = %d\n", len, size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	size = HAL_I2C_Master_Transmit_IT((I2C_ID)id, (uint16_t)dev_addr, buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but I2C transmit size = %d\n", len, size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	cmd_free(buf);
	cmd_raw_mode_write((uint8_t *)"OK", 2);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_i2c_receive_norm_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, len;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u l=%u", &id, &dev_addr, &len);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

#if DRV_TEST_I2C_ADDR_10BIT
	if ((dev_addr >> 10) != 0) {
#else /* DRV_TEST_I2C_ADDR_10BIT */
	if ((dev_addr >> 7) != 0) {
#endif /* DRV_TEST_I2C_ADDR_10BIT */
		CMD_ERR("invalid dev_addr %u\n", dev_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((len >> 31) != 0) {
		CMD_ERR("invalid len %u\n", len);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(len * sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}
	cmd_memset(buf, 0, len);

	size = HAL_I2C_Master_Receive_IT((I2C_ID)id, (uint16_t)dev_addr, buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but I2C receive size = %d\n", len, size);
		cmd_free(buf);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_msleep(1000);

	cmd_raw_mode_enable();
	size = cmd_raw_mode_write(buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but raw mode write size = %d\n", len, size);
	}

	cmd_free(buf);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}
#endif /* DRV_TEST_I2C_NORM */

#if DRV_TEST_I2C_MEM
static enum cmd_status cmd_i2c_transmit_mem_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, mem_addr, len;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u m=%u l=%u", &id, &dev_addr, &mem_addr, &len);
	if (cnt != 4) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((mem_addr >> 8) != 0) {
		CMD_ERR("invalid mem_addr %u\n", mem_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if (len > 16) {
		CMD_ERR("invalid len %u\n", len);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(len * sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	size = cmd_raw_mode_read(buf, (int32_t)len, 10000);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but raw mode read size = %d\n", len, size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	size = HAL_I2C_Master_Transmit_Mem_IT((I2C_ID)id, (uint16_t)dev_addr, (uint8_t)mem_addr, buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but I2C transmit size = %d\n", len, size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	cmd_free(buf);
	cmd_raw_mode_write((uint8_t *)"OK", 2);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_i2c_receive_mem_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, mem_addr, len;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u m=%u l=%u", &id, &dev_addr, &mem_addr, &len);
	if (cnt != 4) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((mem_addr >> 8) != 0) {
		CMD_ERR("invalid mem_addr %u\n", mem_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if (len > 2048) {
		CMD_ERR("invalid len %u\n", len);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(len * sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}
	cmd_memset(buf, 0, len);

	size = HAL_I2C_Master_Receive_Mem_IT((I2C_ID)id, (uint16_t)dev_addr, (uint8_t)mem_addr, buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but I2C receive size = %d\n", len, size);
		cmd_free(buf);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_msleep(1000);

	cmd_raw_mode_enable();
	size = cmd_raw_mode_write(buf, (int32_t)len);
	if (size != (int32_t)len) {
		CMD_ERR("len = %u, but raw mode write size = %d\n", len, size);
	}

	cmd_free(buf);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}
#endif /* DRV_TEST_I2C_MEM */

#if DRV_TEST_I2C_SCCB
static enum cmd_status cmd_i2c_transmit_sccb_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, sub_addr;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u s=%u", &id, &dev_addr, &sub_addr);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((dev_addr >> 8) != 0) {
		CMD_ERR("invalid dev_addr %u\n", dev_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((sub_addr >> 8) != 0) {
		CMD_ERR("invalid sub_addr %u\n", sub_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	size = cmd_raw_mode_read(buf, 1, 10000);
	if (size != 1) {
		CMD_ERR("len = 1, but raw mode read size = %d\n", size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	size = HAL_I2C_SCCB_Master_Transmit_IT((I2C_ID)id, (uint8_t)dev_addr, (uint8_t)sub_addr, buf);
	if (size != 1) {
		CMD_ERR("len = 1, but I2C transmit size = %d\n", size);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL", 4);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}

	cmd_free(buf);
	cmd_raw_mode_write((uint8_t *)"OK", 2);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_i2c_receive_sccb_exec(char *cmd)
{
	int cnt;
	uint32_t id, dev_addr, sub_addr;
	uint8_t *buf;
	int32_t size;

	cnt = cmd_sscanf(cmd, "i=%u d=%u s=%u", &id, &dev_addr, &sub_addr);
	if (cnt != 3) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (id >= I2C_NUM) {
		CMD_ERR("invalid id %u\n", id);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((dev_addr >> 8) != 0) {
		CMD_ERR("invalid dev_addr %u\n", dev_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	if ((sub_addr >> 8) != 0) {
		CMD_ERR("invalid sub_addr %u\n", sub_addr);
		return CMD_STATUS_INVALID_ARG;
	}

	buf = (uint8_t *)cmd_malloc(sizeof(uint8_t));
	if (buf == NULL) {
		CMD_ERR("cmd_malloc return NULL.\n");
		return CMD_STATUS_FAIL;
	}
	cmd_memset(buf, 0, 1);

	size = HAL_I2C_SCCB_Master_Receive_IT((I2C_ID)id, (uint8_t)dev_addr, (uint8_t)sub_addr, buf);
	if (size != 1) {
		CMD_ERR("len = 1, but I2C receive size = %d\n", size);
		cmd_free(buf);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_msleep(1000);

	cmd_raw_mode_enable();
	size = cmd_raw_mode_write(buf, 1);
	if (size != 1) {
		CMD_ERR("len = 1, but raw mode write size = %d\n", size);
	}

	cmd_free(buf);
	cmd_raw_mode_disable();
	return CMD_STATUS_ACKED;
}
#endif /* DRV_TEST_I2C_SCCB */

static struct cmd_data g_i2c_cmds[] = {
	{ "init",			cmd_i2c_init_exec },
	{ "deinit",			cmd_i2c_deinit_exec },

#if DRV_TEST_I2C_NORM
	{ "transmit-norm",	cmd_i2c_transmit_norm_exec },
	{ "receive-norm",	cmd_i2c_receive_norm_exec },
#endif /* DRV_TEST_I2C_NORM */

#if DRV_TEST_I2C_MEM
	{ "transmit-mem",	cmd_i2c_transmit_mem_exec },
	{ "receive-mem",	cmd_i2c_receive_mem_exec },
#endif /* DRV_TEST_I2C_MEM */

#if DRV_TEST_I2C_SCCB
	{ "transmit-sccb",	cmd_i2c_transmit_sccb_exec },
	{ "receive-sccb",	cmd_i2c_receive_sccb_exec },
#endif /* DRV_TEST_I2C_SCCB */
};

enum cmd_status cmd_i2c_exec(char *cmd)
{
	return cmd_exec(cmd, g_i2c_cmds, cmd_nitems(g_i2c_cmds));
}
