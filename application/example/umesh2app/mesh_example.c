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

#define TEST_CNT  20
#define SRV_TYPE "_mesh2"

void *net_handle = NULL;

static const char *test_data = "===== test data from %s-%s =======";
static const char *test_data_mcast = "=====mcast test data from %s-%s =======";

#if ENABLE_PRINT_HEAP
#include <k_api.h>
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int    free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}

void monitor_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, monitor_work, NULL);
}
#endif

void service_found(service_t *service, peer_state_t state)
{
    LOG("found service, name = %s ,type = %s, state = %d", service->srv_name, service->srv_type, state);

    txt_item_t *txt =  service->txt_items;
    while (txt != NULL) {
        LOG("get txt:%s", txt->txt);
        txt = txt->next;
    }

}

int peer_invite(session_t *session, peer_id_t *peer_id, void *context)
{
    int i;
    LOG("recv invite form ip:");
    for (i = 0; i < 16; i ++) {
        printf("%02x ", peer_id->ip6.s6_addr[i]);
    }
    printf("\r\n");
    return 0;
}

int  session_state_changed_func(session_t *session, service_t *dest_srv, session_state_t state, void *context)
{
    LOG("session changed ! dst = %s-%s, state = %d ", dest_srv->srv_type, dest_srv->srv_name, state);
}

int umesh_receive_func(session_t *session, service_t *from, uint8_t *data, int len, void *user_data)
{
    LOG("recv data from %s-%s ,len = %d", from->srv_type, from->srv_name, len);
    LOG("str data= %s", data);
}

static void get_ap_info(const char *ssid, const char *pwd, const uint8_t *bssid, void *ctxt)
{
    LOG("++++++++++get ssid = %s, pwd = %s++++++++++++", ssid, pwd);
}


static void app_main_entry(void *arg)
{
    int ret;
    int cnt = 0;
    char name[SERVICE_NAME_LEN_MAX] = {0};
    char mac_str_txt[33] = {0};
    uint8_t mac[6] = {0};

    session_t *session = NULL;
    service_t *self_srv;

start:
    net_handle = umesh_network_init();
    if (net_handle == NULL) {
        LOG("umesh init failed");
    }
    while (!umesh_is_connected(net_handle)) {
        LOG("----------Waiting for networking-----------");
        aos_msleep(1000);
    }

    /*test zero config*/
    umesh_zero_config_set_cb(net_handle, get_ap_info, NULL);
    umesh_zero_config_request(net_handle);

    hal_wifi_get_mac_addr(NULL, mac);
    snprintf(name, SERVICE_NAME_LEN_MAX - 1, "dev_%02x:%02x:%02x", mac[3], mac[4], mac[5]);
    snprintf(mac_str_txt, 32, "mac=%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    self_srv = umesh_service_init(net_handle, name, SRV_TYPE, 8080);
    LOG("service init ret = %d", ret);
    umesh_service_add_txt(self_srv, mac_str_txt);
    umesh_service_add_txt(self_srv, "ext_info=mesh network");

    ret = umesh_start_browse_service(self_srv, service_found);
    LOG("browse_service ret = %d", ret);
    ret = umesh_start_advertise_service(self_srv);
    LOG("advertise_service ret = %d", ret);

    session = umesh_session_init(self_srv);

    if (session == NULL) {
        LOG("----session create failed !---");
        goto err;
    }

    umesh_register_state(session, session_state_changed_func, NULL);

    umesh_register_inviter(session, peer_invite, NULL);

    umesh_register_receiver(session, umesh_receive_func, session);
    aos_msleep(20000);

    do {
        service_t *serv;
        aos_msleep(5000);

        UMESH_FOUND_LIST_ITERATOR(serv) {
            int find = 0;
            service_t *serv2;
            UMESH_SESSION_LIST_ITERATOR(session, serv2) {
                if (!strcmp(serv->srv_name, serv2->srv_name) && !strcmp(serv->srv_type, serv2->srv_type)) {
                    find = 1;
                    break;
                }
            }
            if (find == 0) {
                ret = umesh_invite_peer(session, serv, 5000);
                LOG("found servcie not in session ,invite he, name = %s, ret = %d", serv->srv_name, ret);
            }
        }

        UMESH_SESSION_LIST_ITERATOR(session, serv) {
            char send[256] = {0};
            LOG("find servcie , name = %s", serv->srv_name);
            snprintf(send, 255, test_data, self_srv->srv_type, self_srv->srv_name);
            ret = umesh_send(session, serv, send, strlen(send), MODE_UNRELIABLE);
            LOG("send data to %s-%s ,ret = %d", serv->srv_type, serv->srv_name, ret);
            memset(send, 0, 256);
            snprintf(send, 255, test_data_mcast, self_srv->srv_type, self_srv->srv_name);
            ret = umesh_send(session, NULL, send, strlen(send), MODE_UNRELIABLE);
            LOG("send public data to session ,ret = %d", ret);
        }
#if ENABLE_LOOP_TEST
    } while (1);
#else
    }
    while (cnt++ < TEST_CNT);
#endif
err:
    LOG("-----------------test end--------------------");
    if (session != NULL)
    {
        ret = umesh_session_deinit(session);
        LOG("umesh_session_deinit,ret = %d", ret);
        session = NULL;
    }

    if (self_srv != NULL)
    {
        ret = umesh_service_deinit(self_srv);
        LOG("umesh_service_deinit,ret = %d", ret);
        self_srv = NULL;
    }

    if (net_handle != NULL)
    {
        ret = umesh_network_deinit(net_handle);
        LOG("umesh_network_deinit,ret = %d", ret);
        net_handle = NULL;
    }
#if ENABLE_REPEAT_TEST
    LOG("test again!!");
    goto start;
#endif

    return;
}



int application_start(int argc, char **argv)
{

    LOG("application_start");
#if ENABLE_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, monitor_work, NULL);
#endif

    aos_set_log_level(AOS_LL_DEBUG);
    aos_task_new("meshappmain", app_main_entry, NULL, 4096);
    aos_loop_run();
    return 0;
}
