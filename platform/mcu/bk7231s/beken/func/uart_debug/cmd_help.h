#ifndef _CMD_HELP_H_
#define _CMD_HELP_H_

#include "command_table.h"

extern int do_help(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);

#define CMD_HELP_MAXARG                          4
#define CONFIG_SYS_HELP_CMD_WIDTH                8

#define ENTRY_CMD_HELP               \
	ENTRY_CMD(help,                          \
				CMD_HELP_MAXARG,     \
				1,                             \
				do_help,             \
				"print command description/usage\r\n",\
				"\r\n"\
				"	- print brief description of all commands\r\n"\
				"help command ...\r\n"\
				"	- print detailed usage of 'command'"\
				"\r\n")

#endif // _CMD_HELP_H_
// eof

