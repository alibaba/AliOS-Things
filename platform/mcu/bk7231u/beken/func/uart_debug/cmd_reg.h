#ifndef _CMD_REG_H_
#define _CMD_REG_H_

#include "command_table.h"

#define REG_RD_MAX_CNT                      4
#define REG_MASK                            3

#define CREG_DEBUG

#ifdef CREG_DEBUG
#define CREG_PRT       os_printf
#define CREG_WPRT      warning_prf
#else
#define CREG_PRT       os_null_printf
#define CREG_WPRT      os_null_printf
#endif

extern int do_reg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);

#define CMD_REG_MAXARG                              6

#define ENTRY_CMD_REG                       \
	ENTRY_CMD(reg,                          \
				CMD_REG_MAXARG,             \
				1,                          \
				do_reg,                     \
				"reg [-r register register...] [-w register value]\r\n",\
				"\r\n"\
				"	read register, or write register\r\n"\
				"Options:\r\n"\
				"     -r register register...            register: hex\r\n"\
				"     -w register value                  register: hex, value: hex\r\n"\
				"\r\n")

#endif // _CMD_REG_H_
// eof

