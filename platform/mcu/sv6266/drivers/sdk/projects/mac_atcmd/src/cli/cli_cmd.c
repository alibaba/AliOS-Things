//#define __SFILE__ "cli_cmd.c"
#include "soc_types.h"
#include "aos/cli.h"
//#include <regs.h>



/*---------------------------------- CMDs -----------------------------------*/






// r addr [size]
static void Cmd_Read(s32 argc, s8 *argv[])
{
    static u32 read_size = 1;
    static u32 read_addr = 0xCD010000;

    if (argc < 2)
    {
        //printf("Usage: r addr [size]\n");
        hex_dump((const u32 *)read_addr, read_size);
        return;
    }

    read_addr = (u32)ssv6xxx_atoi_base(argv[1], 16);
    if (argc > 2)
        read_size = (u32)ssv6xxx_atoi(argv[2]);
    else
        read_size = 1;

    read_addr &= 0xFFFFFFE0;
    read_size = (read_size+7)&0xFFFFFFF8;
    hex_dump((const u32 *)read_addr, read_size);
} // end of - Cmd_Read -

// w addr value
static void Cmd_Write(s32 argc, s8 *argv[])
{
    volatile u32 *write_addr;
    u32           value;

    if (argc != 3)
    {
        printf("Usage: w addr value\n");
        return;
    }

    write_addr = (volatile u32 *)ssv6xxx_atoi_base(argv[1], 16);
    value = (u32)ssv6xxx_atoi_base(argv[2], 16);
    *write_addr = value;
    printf("\n    %08X => %08X : %08X\n", value, (u32)write_addr, *write_addr);
} // end of - Cmd_Write

typedef struct _stParam
{
	char *argv[10];
	int	argc;
} stParam;

extern stParam param;


static void Cmd_iperf(s32 argc, s8 *argv[])
{
	char tmp0[20], tmp1[20], tmp2[20], tmp3[20];
	param.argc = argc - 1;
	param.argv[0] = argv[1];
	param.argv[1] = argv[2];
	param.argv[2] = argv[3];
	param.argv[3] = argv[4];
#ifdef IPERF3_EN
	iperf3(param.argc, param.argv);
#else
	iperf(param.argc, param.argv);
#endif
}

static void Cmd_echo(s32 argc, s8 *argv[])
{
	int loop_i;
	for (loop_i = 0;loop_i < argc; loop_i++) {
		printf("argv[%d] = %s\n", loop_i, argv[loop_i]);
	}
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
		{ "echo", Cmd_echo, "echo"},
    { "r",              Cmd_Read,           "Read memory or register."},
    { "w",              Cmd_Write,          "Write memory or register."},
	{ "iperf",			Cmd_iperf,			"iperf"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};


