/*-----------------------------------------------------------------------
 *	drafter	:	huangyucai
 *	date	:	20110731
 *	file	:	shell.h
 *	desc	:
 *  maintainer Halley
 *-----------------------------------------------------------------------*/
#ifndef	__SHELL_H__
#define	__SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define		CFG_SYS_LONGHELP
#define		SYMBOL_STRLEN_MIDDLE		(0x20)
#define		SYMBOL_STRLEN_SMALL			(0x10)
#define		CFG_MAX_ARGS				(5)

#define		__shell_start	__attribute__((__section__(".shell0.cmd")))
#define		__shell			__attribute__((__section__(".shell1.cmd")))
#define		__shell_end		__attribute__((__section__(".shell2.cmd")))

#ifdef CFG_AUTO_COMPLETE
# define _CMD_COMPLETE(x) x,
#else
# define _CMD_COMPLETE(x)
#endif
#ifdef CFG_SYS_LONGHELP
# define _CMD_HELP(x) x,
#else
# define _CMD_HELP(x)
#endif

#define SHELL_CMD_MKENT_COMPLETE(name, maxargs, cmd, usage, help, comp) \
	{#name, sizeof(#name) - 1, maxargs, cmd, usage, _CMD_HELP(help) _CMD_COMPLETE(comp)}

#define SHELL_CMD_COMPLETE(name, maxargs, cmd, usage, help, comp) \
	const cmd_tbl_t __shell_cmd_##name __shell = \
				SHELL_CMD_MKENT_COMPLETE(name, maxargs, cmd, usage, help, comp)

#define SHELL_CMD(name, maxargs, cmd, usage, help) \
	SHELL_CMD_COMPLETE(name, maxargs, cmd, usage, help, NULL)

typedef struct cmd_tbl_s
{
	char*   name;		/* Command Name			*/
	int		nlen;		/* Command Name Length */
	int		maxargs;	/* maximum number of arguments	*/
	int	(*cmd)(int argc, char** argv);/* Implementation function	*/
	char*   usage;		/* Usage message	(short)	*/
#ifdef	CFG_SYS_LONGHELP
	char*   help;		/* Help  message	(long)	*/
#endif
} cmd_tbl_t __attribute__((aligned(sizeof(long))));

typedef struct cmd_hist_tag
{
	int				handle;
	int				hist_idx;//history record command line index
	int				hist_cur;//current record command line index int				hist_num;//number record command line index
	int				hist_max;//maximum record command line index
#define	CMD_HIST_MAX	(0x10)
#define	CMD_HIST_SZ	(SYMBOL_STRLEN_MIDDLE)
	char 			hist_list[CMD_HIST_MAX][CMD_HIST_SZ];//maximum history record command line
} cmd_hist_t;

extern int command_parse_spawn(int argc, char** argv);
extern void hexdump(char* buf, int dlen);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//SHELL_H

