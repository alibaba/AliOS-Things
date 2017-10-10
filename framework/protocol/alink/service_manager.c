/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/aos.h"
#include "service_manager.h"
#include "os.h"
#define sm_init_service(svc) _##svc##_init()

#define MODULE_NAME "service_mgr"

dlist_t g_service_list;

SERVICE_DELCARE(accs);
static char init;

int sm_init()
{
    if (init) {
        return SERVICE_RESULT_OK;
    }

    dlist_init(&g_service_list);

    sm_init_service(accs);//_accs_init
    sm_add_service(&accs);


    init = 1;

    return SERVICE_RESULT_OK;
}

int sm_exit()
{
    sm_del_service(&accs);
    dlist_init(&g_service_list);

    init = 0;
    return SERVICE_RESULT_OK;
}

int sm_add_service(service_t *svc)
{
    dlist_add(&svc->list_head, &g_service_list);
    return SERVICE_RESULT_OK;
}

int sm_del_service(service_t *svc)
{
    svc->stop();
    dlist_del(&svc->list_head);
    return SERVICE_RESULT_OK;
}

int sm_get_service_state(service_t *svc)
{
    return svc->state;
}

int sm_query_service()
{
    service_t *pos;
    if (dlist_empty(&g_service_list)) {
        LOGI(MODULE_NAME, "No service!");
    } else {
        dlist_for_each_entry_reverse(pos, &g_service_list, list_head, service_t) {
            LOGI(MODULE_NAME, "%s is found!", ((service_t *)pos)->name);
        }
    }
    return SERVICE_RESULT_OK;
}

service_t *sm_get_service(char *svc_name)
{
    service_t *pos;

    if (!init) {
        sm_init();
    }
    dlist_for_each_entry_reverse(pos, &g_service_list, list_head, service_t) {
        service_t *svc = (service_t *)pos;
        if (!strcmp(svc->name, svc_name)) {
            return svc;
        }
    }
    return 0;
}

int sm_attach_service(char *svc_name, service_cb listen)
{
    service_t *svc = sm_get_service(svc_name);
    if (svc) {
        return svc->add_listener(listen);
    }
    return SERVICE_RESULT_ERR;
}

int sm_detach_service(char *svc_name, service_cb listen)
{
    service_t *svc = sm_get_service(svc_name);
    if (svc) {
        return svc->del_listener(listen);
    }
    return SERVICE_RESULT_ERR;
}

const char *sm_code2string(int state)
{
    if (state == SERVICE_RESULT_OK) {
        return "SERVICE_RESULT_OK";
    } else if (state == SERVICE_RESULT_ERR) {
        return "SERVICE_RESULT_ERR";
    } else if (state == SERVICE_EVENT) {
        return "SERVICE_EVENT";
    } else if (state == SERVICE_DATA) {
        return "SERVICE_DATA";
    } else if (state == SERVICE_ATTACH) {
        return "SERVICE_ATTACH";
    } else if (state == SERVICE_DETACH) {
        return "SERVICE_DETACH";
    } else if (state == SERVICE_CONN_INIT) {
        return "SERVICE_CONN_INIT";
    } else if (state == SERVICE_CONN_READY) {
        return "SERVICE_CONN_READY";
    } else if (state == SERVICE_STATE_PREPARE) {
        return "SERVICE_STATE_PREPARE";
    } else if (state == SERVICE_CONN_CLOSE) {
        return "SERVICE_CONN_CLOSE";
    } else if (state == SERVICE_STATE_INIT) {
        return "SERVICE_STATE_INIT";
    } else if (state == SERVICE_STATE_READY) {
        return "SERVICE_STATE_READY";
    } else if (state == SERVICE_STATE_BUSY) {
        return "SERVICE_STATE_BUSY";
    } else if (state == SERVICE_STATE_STOP) {
        return "SERVICE_STATE_STOP";
    }
    return "SERVICE_UNKNOWN";
}
