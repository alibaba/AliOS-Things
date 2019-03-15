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
#include "cmd_sntp.h"
#include "net/sntp/sntp.h"

static OS_Thread_t g_sntp_thread;
#define SNTP_THREAD_STACK_SIZE		(1 * 1024)
#define SNTP_THREAD_EXIT OS_ThreadDelete

void sntp_run(void *arg)
{
	CMD_LOG(1, "<net> <sntp> <request>\n");
        if (sntp_request(NULL) != 0) {
		CMD_LOG(1, "<net> <sntp> <response : fail>\n");
		goto exit;
	}

	sntp_time *time = (sntp_time *)sntp_obtain_time();
	CMD_LOG(1, "<net> <sntp> <response : success>\n");
	CMD_LOG(1,"sntp(%u  %u  %u ",time->week, time->mon, time->day);
        CMD_LOG(1,"%u:%u:%u %u)\n", time->hour, time->min, time->sec, time->year);
exit:
        SNTP_THREAD_EXIT(&g_sntp_thread);

}

int sntp_start()
{
        if (OS_ThreadIsValid(&g_sntp_thread)) {
                CMD_ERR("sntp task is running\n");
                return -1;
        }

        if (OS_ThreadCreate(&g_sntp_thread,
                                "",
                                sntp_run,
                                NULL,
                                OS_THREAD_PRIO_APP,
                                SNTP_THREAD_STACK_SIZE) != OS_OK) {
                CMD_ERR("sntp task create failed\n");
                return -1;
        }

        return 0;
}

enum cmd_status cmd_sntp_exec(char *cmd)
{
	sntp_start();
        return CMD_STATUS_OK;
}
