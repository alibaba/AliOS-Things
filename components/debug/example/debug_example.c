/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include "aos/debug.h"
#include "aos/cli.h"

static void debug_help_show(void)
{
    aos_cli_printf("You can use debug cmd to show api test:\r\n");
	aos_cli_printf("debug_api help  --- show this\r\n");
	aos_cli_printf("debug_api 1     --- show memory info\r\n");
	aos_cli_printf("debug_api 2     --- show task info\r\n");
	aos_cli_printf("debug_api 3     --- show bufqueue info\r\n");
	aos_cli_printf("debug_api 4     --- show queue info\r\n");
	aos_cli_printf("debug_api 5     --- show sem info\r\n");
	aos_cli_printf("debug_api 6     --- show mutex info\r\n");
	aos_cli_printf("debug_api 7     --- show backtrace now\r\n");
	aos_cli_printf("debug_api 8     --- show backtrace task\r\n");
	aos_cli_printf("debug_api all   --- show all above\r\n");
}


static void debug_api_example(int argc, char **argv)
{
	if (argc != 2) {
		aos_cli_printf("use 'debug_api help' for test\r\n");
		return;
	}

	if (strcmp(argv[1], "help") == 0) {
		debug_help_show();
	} else if (strcmp(argv[1], "1") == 0) {
		aos_debug_mm_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "2") == 0) {
		aos_debug_task_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "3") == 0) {
		aos_debug_buf_queue_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "4") == 0) {
		aos_debug_queue_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "5") == 0) {
		aos_debug_sem_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "6") == 0) {
		aos_debug_mutex_overview(aos_cli_printf);
	} else if (strcmp(argv[1], "7") == 0) {
		aos_debug_backtrace_now(aos_cli_printf);
	} else if (strcmp(argv[1], "8") == 0) {
		aos_debug_backtrace_task("idle_task", aos_cli_printf);
	} else if (strcmp(argv[1], "all") == 0) {
		aos_debug_overview(aos_cli_printf);
	} else {
		aos_cli_printf("debug example cmd not exist\r\n");
	}
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(debug_api_example, debug_api, debug api example)
#endif /* AOS_COMP_CLI */
