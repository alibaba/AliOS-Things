/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <netmgr.h>
#include <stdlib.h>
#include <string.h>
#include "libwebsockets.h"
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


int application_start(int argc, char *argv[])
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif
    netmgr_ap_config_t apconfig;
    memset(&apconfig, 0, sizeof(apconfig));

    strcpy(apconfig.ssid,"aos_test_01");
    strcpy(apconfig.pwd, "Alios@Embedded");
    netmgr_set_ap_config(&apconfig);

    aos_set_log_level(AOS_LL_DEBUG);
    LOG("application_start!!");

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_init();
	netmgr_start(0);

    aos_loop_run();

    return 0;
}

