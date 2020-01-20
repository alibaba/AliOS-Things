/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/kv.h>



static void debug_help_show(void)
{
    printf("You can use debug cmd to show api :\r\n");
	printf("debug help  --- show this\r\n");
	printf("debug 1     --- show memory info\r\n");
	printf("debug 2     --- show task info\r\n");
	printf("debug 3     --- show bufqueue info\r\n");
	printf("debug 4     --- show queue info\r\n");
	printf("debug 5     --- show sem info\r\n");
	printf("debug 6     --- show backtrace now\r\n");
	printf("debug 7     --- show backtrace task\r\n");
	printf("debug 8     --- show reboot reason(depend on kv)\r\n");
	printf("debug all   --- show all above\r\n");
}


static void debug_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    unsigned int reason;

	if (argc == 2) {
		if (strcmp(argv[0], "debug")) {
			printf("debug app cmd err\r\n");
			return;
		}

		if (strcmp(argv[1], "help") == 0) {
			debug_help_show();
		} else if (strcmp(argv[1], "1") == 0) {
			aos_debug_mm_overview(NULL);
		} else if (strcmp(argv[1], "2") == 0) {
			aos_debug_task_overview(NULL);
		} else if (strcmp(argv[1], "3") == 0) {
			aos_debug_buf_queue_overview(NULL);
		} else if (strcmp(argv[1], "4") == 0) {
			aos_debug_queue_overview(NULL);
		} else if (strcmp(argv[1], "5") == 0) {
			aos_debug_sem_overview(NULL);
		} else if (strcmp(argv[1], "6") == 0) {
			aos_debug_backtrace_now();
		} else if (strcmp(argv[1], "7") == 0) {
			aos_debug_backtrace_task("aos-init");
		} else if (strcmp(argv[1], "8") == 0) {
			reason = aos_debug_reboot_reason_get();
            switch (reason) {
                case 1:
                    printf("watchdog trig\r\n");
                    break;
                case 2:
                    printf("system coredump\r\n");
                    break;
                case 3:
                    printf("system repower\r\n");
                    break;
                case 4:
                    printf("syste, fatal err\r\n");
                    break;
                default:
                    printf("system repower\r\n");
                    break;
            }
		} else if (strcmp(argv[1], "all") == 0) {
			aos_debug_overview();
		} else {
			printf("debug app cmd not exist\r\n");
		}
	} else {
		printf("debug app para err\r\n");
	}
}

static int debug_cmd_init(void)
{
    int ret;

    static struct cli_command debug_ins[] = {
        { "debug","show debug api", debug_cmd },
    };

    ret = aos_cli_register_commands(debug_ins, sizeof(debug_ins)/sizeof(struct cli_command));
    if (ret) {
        return ret;
    }

    return 0;
}

int application_start(int argc, char *argv[])
{
    printf("******** debug app entry here! **********\r\n");

    debug_help_show();

	if(debug_cmd_init()) {
		printf("debug cmd init fail\r\n");
		return -1;
	}

    while(1) {
        aos_msleep(1000);
    };
}
