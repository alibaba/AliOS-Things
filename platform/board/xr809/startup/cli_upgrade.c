
#include <cli.h>
#include "driver/chip/hal_chip.h"
#include "driver/chip/system_chip.h"
#include "net/wlan/wlan.h"
#include "common/cmd/cmd_upgrade.h"
#include "common/cmd/cmd_defs.h"
static void handle_upgrade_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    enum cmd_status status = CMD_STATUS_FAIL;
#if 0
    net_sys_stop();
    SystemDeInit(SYSTEM_DEINIT_FLAG_RESET_CLK);
    HAL_PRCM_SetCPUABootFlag(PRCM_CPUA_BOOT_FROM_SYS_UPDATE);
    *((volatile unsigned long*)0xE000ED08) = 0;
    *((volatile unsigned long*)0xE000ED0C) = 0x05fa0001;
#endif
	status = cmd_upgrade_exec(NULL);
	if(status != CMD_STATUS_ACKED) {
		printf("%s,err status %d\n", __func__, status);
	}
	return status;
}

static struct cli_command ncmd = {
    .name = "upgrade",
    .help = "upgrade",
//    .function = handle_upgrade_cmd,
     .function = cmd_upgrade_exec,
};

void cli_cmd_add_upgrade(void)
{
    aos_cli_register_command(&ncmd);
}
