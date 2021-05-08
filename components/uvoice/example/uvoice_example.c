/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "k_api.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#include "uvoice_init.h"
#include "uvoice_test.h"

static void cmd_play_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_play_test(argc, argv);
}

static void cmd_record_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_record_test(argc, argv);
}

static void cmd_tts_handler(char *buf, int len, int argc, char **argv)
{
    extern void test_tts_handle(int argc, char **argv);
    return test_tts_handle(argc, argv);
}

#if AOS_COMP_CLI
struct cli_command uvoicedemo_commands[] = {
	{"play", "player test", cmd_play_handler},
	{"record", "record test", cmd_record_handler},
    {"tts", "tts test", cmd_tts_handler}
};

void uvoice_example(int argc, char **argv)
{
    static int uvoice_example_inited = 0;

    if (uvoice_example_inited == 1) {
        printf("uvoice example already initialized !\n");
        return;
    }
    uvoice_example_inited = 1;

	uvoice_init();

	aos_cli_register_commands(uvoicedemo_commands,
		sizeof(uvoicedemo_commands) / sizeof(struct cli_command));

    printf("uvoice example initialization succeeded !\n");
    return;
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(uvoice_example, uvoice_example, uvoice test example)
#endif

