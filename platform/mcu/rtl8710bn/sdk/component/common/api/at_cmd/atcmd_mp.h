#ifndef __ATCMD_MP_H__
#define __ATCMD_MP_H__

#define CONFIG_ATCMD_MP_EXT0	0 //support MP ext0 AT command

typedef struct _at_command_mp_ext_item_{
	char	*mp_ext_cmd;
	int		(*mp_ext_fun)(void **argv, int argc);
	char	*mp_ext_usage;
}at_mp_ext_item_t;

#endif
