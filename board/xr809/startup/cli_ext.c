#if 0
#include "command.h"
#include <aos/aos.h>
#endif
extern void cli_cmd_add_upgrade(void);
extern void cli_cmd_add_mem(void);

#if 0
static struct cli_command ncmd = {
    .name = "xr_cmd",
    .help = "xr cmd",
//    .function = handle_upgrade_cmd,
	.function = main_cmd_exec,
};

void cli_cmd_add_xr_cmd(void)
{
    aos_cli_register_command(&ncmd);
}
#endif

void aos_cli_ext_init(void)
{
    cli_cmd_add_upgrade();
    cli_cmd_add_mem();
}
