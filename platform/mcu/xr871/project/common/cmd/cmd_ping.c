/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "cmd_ping.h"
#include "lwip/netdb.h"
#include "net/ping/ping.h"

struct ping_data pdata;
static OS_Thread_t g_ping_thread;
#define PING_THREAD_STACK_SIZE		(1 * 1024)
#define PING_THREAD_EXIT OS_ThreadDelete

void ping_run(void *arg)
{
        struct ping_data *data = (struct ping_data *)arg;
        ping(data);
        PING_THREAD_EXIT(&g_ping_thread);

}

int ping_start(struct ping_data *data)
{
        if (OS_ThreadIsValid(&g_ping_thread)) {
                CMD_ERR("PING task is running\n");
                return -1;
        }

        if (OS_ThreadCreate(&g_ping_thread,
                                "",
                                ping_run,
                                (void *)data,
                                OS_THREAD_PRIO_APP,
                                PING_THREAD_STACK_SIZE) != OS_OK) {
                CMD_ERR("PING task create failed\n");
                return -1;
        }

        return 0;
}

int ping_get_host_by_name(char *name, unsigned int *address)
{
	struct hostent *host_entry;

	host_entry = gethostbyname(name);
        if(host_entry) {
                *(address) = *((u_long*)host_entry->h_addr_list[0]);
		return 0; // OK
	} else {
		return 1; // Error
	}
}

enum cmd_status cmd_ping_exec(char *cmd)
{
        int argc;
        char *argv[3];
        //struct ping_data pdata;
        memset((void*) &pdata, 0, sizeof(pdata));

        argc = cmd_parse_argv(cmd, argv, 3);
        if (argc < 1 || argc > 2) {
                CMD_ERR("invalid ping cmd, argc %d\n", argc);
                return CMD_STATUS_INVALID_ARG;
        }
        unsigned int address = 0;

        if (ping_get_host_by_name(argv[0], &address) != 0) {
                CMD_ERR("invalid ping host.\n");
                return CMD_STATUS_INVALID_ARG;
        }

        pdata.sin_addr.addr = address;
        if (argc > 1)
                pdata.count = atoi(argv[1]);
        else
                pdata.count = 3;

        if (ping_start(&pdata) == 0)
                return CMD_STATUS_OK;
        else
                return CMD_STATUS_FAIL;
}
