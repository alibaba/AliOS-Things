/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <netmgr.h>
#include <stdlib.h>
#include <string.h>
#include "libwebsockets.h"
#include "oled.h"
#include <inttypes.h>
#ifdef CSP_LINUXHOST
#include "signal.h"
#else

#endif


extern void pal_sample(void * p) ;

int client_init=0;

aos_task_t pal_task;
void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type == EV_WIFI && event->code == CODE_WIFI_ON_GOT_IP)
    {
		if (client_init) {
			return;
		}
		client_init = 1;
       // aos_task_new("pal init task", pal_sample, NULL,1024*4);
        aos_task_new_ext(&pal_task, "pal_test", pal_sample, NULL,
                         1024*2,AOS_DEFAULT_APP_PRI-1);
    }
}
extern void dumpsys_cli_init(void);

int application_start(int argc, char *argv[])
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#elif defined(MCU_XR871)
    draw_text(0,0,1,"net connecting...");
#endif
    aos_set_log_level(AOS_LL_DEBUG);
    long long test=-1;
    LOG("application_start!! %lu %" PRIu64 " %lld %llu %l64u %l64d %f",test,test,test,test,test,test,0.2444);
    dumpsys_cli_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_init();
	netmgr_start(0);

    aos_loop_run();

    return 0;
}

