/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <cli.h>

extern void os_interrupt_counter_show(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_regs(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_cd(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_pwd(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_ls(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_rm(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_cat(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_cp(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_mkdir(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_rmdir(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_touch(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_echo(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_date(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_ifconfig(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_netstat(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_ping(char *pwbuf, int blen, int argc, char **argv);
extern void cmd_free(char *pwbuf, int blen, int argc, char **argv);
struct cli_command  xm_cli_cmd[] = { 
	{"proc_interrupt",         "interrupt counter", os_interrupt_counter_show},
	{"cd",         "cd dir", cmd_cd},
	{"pwd",         "show current dir", cmd_pwd},
	{"ls",         "ls dir", cmd_ls},
	{"regs",         "regs read/write register", cmd_regs},
	{"date",         "show time", cmd_date},
	{"rm",         "rm file", cmd_rm},
	{"cat",         "cat file", cmd_cat},
	{"cp",         "copy file", cmd_cp},
	{"mkdir",         "create dir", cmd_mkdir},
	{"rmdir",         "remove dir", cmd_rmdir},
	{"touch",         "create new file", cmd_touch},
	{"echo",         "echo string to file", cmd_echo},
	{"ifconfig","list the information of network interfaces", cmd_ifconfig},
	{"netstat","list the information of TCP / IP", cmd_netstat},
	{"ping","ping network host", cmd_ping},
	{"free","free memory", cmd_free},
};

void xm_cli_init(void)
{
	aos_cli_register_commands(&xm_cli_cmd[0],sizeof(xm_cli_cmd) / sizeof(struct cli_command));
}

