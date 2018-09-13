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

/*
 * $ mem r8 <addr> <len>
 * $ mem r16 <addr> <len>
 * $ mem r32 <addr> <len>
 * $ mem w8 <addr> <data>
 * $ mem w16 <addr> <data>
 * $ mem w32 <addr> <data>
 */

#define MEM_LOG(fmt, arg...) printf(fmt, ##arg)

enum mem_rw_mode {
	MEM_RW_MODE_8BIT = 0,
	MEM_RW_MODE_16BIT,
	MEM_RW_MODE_32BIT,
	MEM_RW_MODE_NUM
};

struct mem_dump_param {
	const char *hint_fmt;
	const char *data_fmt;
	uint8_t byte_align;
};

#define MEM_DUMP_BYTES_PER_LINE	16

static const struct mem_dump_param g_mem_dump_param[MEM_RW_MODE_NUM] = {
//	  "12345678: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"
	{ "address    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F", " %02x", 1 },

//	  "12345678: 0100 0302 0504 0706 0908 0B0A 0D0C 0F0E"
	{ "address      0    2    4    6    8    A    C    E",         " %04x", 2 },

//	  "12345678: 03020100 07060504 0B0A0908 0F0E0D0C"
	{ "address          0        4        8        C",             " %08x", 4 }
};

static int mem_dump(uint32_t addr, uint32_t len, enum mem_rw_mode mode)
{
	uint32_t i, byte_align, item_cnt, item_per_line;
	const char *data_fmt;
	const struct mem_dump_param *param;

	if (mode >= MEM_RW_MODE_NUM) {
		MEM_LOG("invalid mem rw mode %d\n", mode);
		return -1;
	}

	param = &g_mem_dump_param[mode];
	byte_align = param->byte_align;
	if ((addr & (byte_align - 1)) != 0) {
		MEM_LOG("addr 0x%08x not align with %u\n", addr, byte_align);
		return -1;
	}

	item_cnt = len / byte_align;
	item_per_line = MEM_DUMP_BYTES_PER_LINE / byte_align;
	data_fmt = param->data_fmt;

	MEM_LOG(param->hint_fmt);
	switch (mode) {
	case MEM_RW_MODE_8BIT:
		for (i = 0; i < item_cnt; ++i) {
			if (i % item_per_line == 0) {
				MEM_LOG("\n%08x:", addr);
			}
			MEM_LOG(data_fmt, *((uint8_t *)addr));
			addr += byte_align;
		}
		break;
	case MEM_RW_MODE_16BIT:
		for (i = 0; i < item_cnt; ++i) {
			if (i % item_per_line == 0) {
				MEM_LOG("\n%08x:", addr);
			}
			MEM_LOG(data_fmt, *((uint16_t *)addr));
			addr += byte_align;
		}
		break;
	case MEM_RW_MODE_32BIT:
		for (i = 0; i < item_cnt; ++i) {
			if (i % item_per_line == 0) {
				MEM_LOG("\n%08x:", addr);
			}
			MEM_LOG(data_fmt, *((uint32_t *)addr));
			addr += byte_align;
		}
		break;
	default:
		break;
	}
	MEM_LOG("\n");

	return 0;
}

static enum mem_rw_mode cmd_mem_parse_arg(char *cmd, uint32_t *addr, uint32_t *val)
{
	int argc;
	char *argv[3];
	uint32_t bits;
	enum mem_rw_mode mode;

	argc = cmd_parse_argv(cmd, argv, cmd_nitems(argv));
	if (argc != 3) {
		return MEM_RW_MODE_NUM;
	}

	if (cmd_sscanf(argv[0], "%u", &bits) != 1) {
		return MEM_RW_MODE_NUM;
	}

	switch (bits) {
	case 8:
		mode = MEM_RW_MODE_8BIT;
		break;
	case 16:
		mode = MEM_RW_MODE_16BIT;
		break;
	case 32:
		mode = MEM_RW_MODE_32BIT;
		break;
	default:
		return MEM_RW_MODE_NUM;
	}

	if (cmd_sscanf(argv[1], "%x", addr) != 1) {
		return MEM_RW_MODE_NUM;
	}

	if (cmd_strncasecmp(argv[2], "0x", 2) == 0) {
		if (cmd_sscanf(argv[2], "%x", val) != 1) {
			return MEM_RW_MODE_NUM;
		}
	} else {
		if (cmd_sscanf(argv[2], "%u", val) != 1) {
			return MEM_RW_MODE_NUM;
		}
	}
	return mode;
}

static enum cmd_status cmd_mem_read_exec(char *cmd)
{
	uint32_t addr, len;
	enum mem_rw_mode mode;

	mode = cmd_mem_parse_arg(cmd, &addr, &len);
	if (mode >= MEM_RW_MODE_NUM) {
		return CMD_STATUS_INVALID_ARG;
	}

	mem_dump(addr, len, mode);
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_mem_write_exec(char *cmd)
{
	uint32_t addr, value;
	enum mem_rw_mode mode;

	mode = cmd_mem_parse_arg(cmd, &addr, &value);
	if (mode >= MEM_RW_MODE_NUM) {
		return CMD_STATUS_INVALID_ARG;
	}

	switch (mode) {
	case MEM_RW_MODE_8BIT:
		*((volatile uint8_t *)addr) = (uint8_t)value;
		MEM_LOG("write 0x%02x -> 0x%08x\n", (uint8_t)value, addr);
		break;
	case MEM_RW_MODE_16BIT:
		*((volatile uint16_t *)addr) = (uint16_t)value;
		MEM_LOG("write 0x%04x -> 0x%08x\n", (uint16_t)value, addr);
		break;
	case MEM_RW_MODE_32BIT:
		*((volatile uint32_t *)addr) = (uint32_t)value;
		MEM_LOG("write 0x%08x -> 0x%08x\n", (uint32_t)value, addr);
		break;
	default:
		break;
	}

	return CMD_STATUS_ACKED;
}

static struct cmd2_data g_mem_cmds[] = {
	{ "r",	1, cmd_mem_read_exec },
	{ "w",	1, cmd_mem_write_exec },
};

enum cmd_status cmd_mem_exec(char *cmd)
{
	return cmd2_exec(cmd, g_mem_cmds, cmd_nitems(g_mem_cmds));
}
