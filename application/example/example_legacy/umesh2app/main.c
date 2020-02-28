/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include <stdint.h>
#include <network/network.h>
#include "string.h"
#include <stdio.h>
#include <string.h>
#include <network/umesh2/umesh_api.h>

void *net_handle = NULL;


static const char *test_data = "===== test data =======";
void service_found(service_t *service, peer_state_t state)
{     
    LOG("found service, name = %s ,type = %s, state = %d", service->srv_name, service->srv_type, state);

    txt_item_t *txt =  service->txt_items;
    while(txt!= NULL) {
            LOG("get txt:%s",txt->txt);
            txt = txt->next;
    }

}

int peer_invite(session_t *session,peer_id_t *peer_id, void *context)
{
    int i;
    char str_ip[64] = {0};
    for(i = 0; i< 16; i ++) {
        snprintf(str_ip + i*3, "%02x ",peer_id->ip6.s6_addr[i]);
    }

    LOG("recv invite form ip:%s", str_ip);
    return 0;
}

 int  session_state_changed_func(session_t *session,service_t *dest_srv, session_state_t state, void *context)
{
        LOG("session changed !");
}

 int umesh_receive_func(session_t *session, service_t *from, uint8_t *data, int len, void *user_data)
{
    LOG("recv data from %s-%s ,len = %d",from->srv_type, from->srv_name,len);
    LOG("str data= %s", data);
}

static void app_main_entry(void *arg)
{
    int ret;
   session_t *session = NULL;
    service_t *srv;
    aos_msleep(10000);

    srv = umesh_service_init(net_handle, "tt_srvice", "_mesh", 8080);
    LOG("service init ret = %d", ret);
    umesh_service_add_txt(srv, "t1=test1");
    umesh_service_add_txt(srv, "t2=iamtt");

    ret = umesh_start_browse_service(srv, service_found);
    LOG("browse_service ret = %d", ret);
    ret = umesh_start_advertise_service(srv);
    LOG("advertise_service ret = %d", ret);

    session = umesh_session_init(srv);

    if(session == NULL) {
            LOG("----session create failed !---");
                return;
    }

   umesh_register_state(session,session_state_changed_func ,NULL);

   umesh_register_inviter(session,peer_invite,NULL);

   umesh_register_receiver(session, umesh_receive_func, session);
   aos_msleep(20000);

    do {
        service_t *serv;
        aos_msleep(5000);
      
        UMESH_FOUND_LIST_ITERATOR(serv) {
                int find = 0;
                service_t * serv2;
                LOG("find servcie in found list,  %s-%s",serv->srv_type,serv->srv_name); 
                UMESH_SESSION_LIST_ITERATOR(session,serv2) {
                         LOG("find servcie in session,  %s-%s",serv2->srv_type,serv2->srv_name); 
                        if(!strcmp(serv->srv_name, serv2->srv_name)&&!strcmp(serv->srv_type, serv2->srv_type)) {
                             find = 1;
                             break;   
                        }
                }
                if(find == 0 ) {
                ret = umesh_invite_peer(session, serv, 5000);
                LOG("found servcie not in session ,invite he, name = %s, ret = %d",serv->srv_name,ret); 
                }
        }

        UMESH_SESSION_LIST_ITERATOR(session,serv) {
            LOG("find servcie , name = %s",serv->srv_name);
            ret = umesh_send(session, serv, test_data, strlen(test_data), 1);
            LOG("send data ,ret = %d", ret);
        }
    } while(1);
 
}


int application_start(int argc, char **argv)
{
    net_handle= umesh_network_init();
    if(net_handle == NULL) {
        LOG("umesh init failed");
    }
    aos_set_log_level(AOS_LL_DEBUG);
    aos_task_new("meshappmain", app_main_entry, NULL, 8000);
    aos_loop_run();
    return 0;
}
