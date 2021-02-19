/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "uvoice_wetalk.h"


void test_wetalk_handle(int argc, char **argv)
{
	if (argc < 2)
		return;

	if (!strcmp(argv[1], "conn")) {
		if (argc < 5)
			return;
		wetalk_server_connect(argv[2], argv[3], atoi(argv[4]));
	} else if (!strcmp(argv[1], "send")) {
		if (argc < 3)
			return;
		wetalk_send_msg_text(argv[2]);
	} else if (!strcmp(argv[1], "account")) {
		if (argc < 3)
			return;
		wetalk_account_set(argv[2]);
	} else if (!strcmp(argv[1], "to")) {
		if (argc < 3)
			return;
		wetalk_connect_friend(argv[2]);
	} else if (!strcmp(argv[1], "disconn")) {
		wetalk_server_disconnect();
	} else if (!strcmp(argv[1], "show")) {
		wetalk_message_show();
	} else if (!strcmp(argv[1], "del")) {
		if (argc < 3)
			return;
		wetalk_message_del(atoi(argv[2]));
	} else if (!strcmp(argv[1], "load")) {
		if (argc < 3)
			return;
		wetalk_message_load(atoi(argv[2]));
	}
}

