/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "k_api.h"
#include "aos/cli.h"
#include "aos/yloop.h"

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include "uvoice_init.h"
#include "uvoice_test.h"

#define APP_LOGI(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)

static void cmd_play_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_play_test(argc, argv);
}

static void cmd_record_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_record_test(argc, argv);
}

static void cmd_msleep_handler(char *buf, int len, int argc, char **argv)
{
	if (argc < 2)
		return;
	aos_msleep(atoi(argv[1]));
}

static void cmd_freemem_handler(char *buf, int len, int argc, char **argv)
{
	printf("freemem %d\n", g_kmm_head->free_size);
}

static void cmd_tts_handler(char *buf, int len, int argc, char **argv)
{
    extern void test_tts_handle(int argc, char **argv);
    return test_tts_handle(argc, argv);
}

struct cli_command uvoicedemo_commands[] = {
	{"play", "player test", cmd_play_handler},
	{"record", "record test", cmd_record_handler},
	{"msleep", "sleep millisecond", cmd_msleep_handler},
	{"freemem", "free memory size", cmd_freemem_handler},
    {"tts", "tts test", cmd_tts_handler}
};

int application_start(int argc, char **argv)
{
	uvoice_init();

	aos_cli_register_commands(uvoicedemo_commands,
		sizeof(uvoicedemo_commands) / sizeof(struct cli_command));

	netmgr_init();

    aos_loop_run();
    return 0;
}

