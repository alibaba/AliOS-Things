/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include <stdint.h>
#include <network/network.h>
#include <aos/yloop.h>
#include <netmgr/netmgr.h>
#include <umesh2/core/umesh.h>
#include <umesh2/umesh_api.h>

/*for get mac addr*/
#include "hal/wifi.h"

#define TEST_CNT  20
#define SRV_TYPE "_mesh2"

void *net_handle = NULL;
typedef struct peer_list {
    peer_id_t id;
    uint8_t joined;
    struct peer_list *next;
} peer_list_t;

typedef struct peer_list_state {
    peer_list_t *found_peer_list;
    aos_mutex_t mutex;
} peer_list_state_t;

static peer_list_state_t user_peer_state;

static int user_peer_state_init(peer_list_state_t *state)
{
    if (state == NULL) {
        return -1;
    }
    memset(state, 0, sizeof(peer_list_state_t));
    if (0 != aos_mutex_new(&state->mutex)) {
        return -1;
    }
    return 0;
}

static int user_peer_state_deinit(peer_list_state_t *state)
{
    if (state == NULL) {
        return -1;
    }
    aos_mutex_lock(&state->mutex, AOS_WAIT_FOREVER);
    peer_list_t *cur = state->found_peer_list;
    peer_list_t *next;
    while (cur != NULL) {
        next = cur->next;
        aos_free(cur);
        cur = next;
    }
    state->found_peer_list = NULL;
    aos_mutex_unlock(&state->mutex);
    aos_mutex_free(&state->mutex);
    return 0;
}

static const char *test_data = "===== test data from %s-%s =======";
static const char *test_data_mcast = "=====mcast test data from %s-%s =======";

extern int hal_wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac);

static int get_id_to_str(peer_id_t *id, char *buff)
{

    if (id == NULL || buff == NULL) {
        return -1;
    }
    snprintf(buff, 9, "%02x:%02x:%02x", id->ip6.s6_addr[13], id->ip6.s6_addr[14], id->ip6.s6_addr[15]);
    return 0;
}



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


static void service_found(service_t *service, peer_state_t state, void *context)
{
    LOG("service: %s-%s, %s", service->srv_type, service->srv_name,  state == PEER_FOUND ? "found" : "lost");
    peer_list_t *peer = NULL;
    peer_list_t *cur;

    aos_mutex_lock(&user_peer_state.mutex, AOS_WAIT_FOREVER);
    cur = user_peer_state.found_peer_list;
    if (state == PEER_FOUND) {
        txt_item_t *txt =  service->txt_items;
        while (txt != NULL) {
            LOG("get txt:%s", txt->txt);
            txt = txt->next;
        }

        while (cur != NULL) {
            if (!memcmp(&cur->id, &service->id, sizeof(peer_id_t))) {
                LOG("found peer already in peer list!");
                aos_mutex_unlock(&user_peer_state.mutex);
                return;
            }
            cur = cur->next;
        }
        peer = aos_malloc(sizeof(struct peer_list));
        if (peer == NULL) {
            LOG("malloc peer fail");
            aos_mutex_unlock(&user_peer_state.mutex);
            return;
        }
        memset(peer, 0, sizeof(struct peer_list));

        peer->next = user_peer_state.found_peer_list;
        user_peer_state.found_peer_list = peer;
        memcpy(&peer->id, &service->id, sizeof(peer_id_t));
    } else {
        peer_list_t *pre = cur;
        while (cur != NULL) {
            if (!memcmp(&cur->id, &service->id, sizeof(peer_id_t))) {
                LOG("remove peer from user peerlist");
                pre->next = cur->next;
                if (cur == user_peer_state.found_peer_list) {
                    user_peer_state.found_peer_list = user_peer_state.found_peer_list->next;
                }
                aos_free(cur);
                break;
            }
            pre = cur;
            cur = cur->next;

        }
    }
    aos_mutex_unlock(&user_peer_state.mutex);
}

static int peer_invite(session_t *session, peer_id_t *peer_id, void *context)
{
    char ip_str[64] = {0};
    get_id_to_str(peer_id, ip_str);
    LOG("recieve invitation from %s ", ip_str);
    return 0;
}

static void  session_state_changed_func(session_t *session, peer_id_t *peer_id, session_state_t state, void *context)
{
    char ip_str[64] = {0};
    get_id_to_str(peer_id, ip_str);
    LOG("session changed ! peer = %s, state = %d ", ip_str, state);
}

static void umesh_receive_func(session_t *session, peer_id_t *from, uint8_t *data, uint16_t len, void *user_data)
{
    char ip_str[64] = {0};
    get_id_to_str(from, ip_str);

    LOG("recv data from %s ,len = %d", ip_str, len);
    LOG("str data= %s", data);
}

static void app_main_entry(void *arg)
{
    int ret;
#if !ENABLE_LOOP_TEST
    int cnt = 0;
#endif
    char name[SERVICE_NAME_LEN_MAX] = {0};
    char mac_str_txt[33] = {0};
    uint8_t mac[6] = {0};

    session_t *session = NULL;
    service_t *self_srv;
#if ENABLE_REPEAT_TEST
start:
#endif
    ret = user_peer_state_init(&user_peer_state);
    if (ret < 0) {
        LOG("init user peer list failed");
        return;
    }

    net_handle = umesh_network_init();
    if (net_handle == NULL) {
        LOG("umesh init failed");
        return;
    }
    while (!umesh_is_connected(net_handle)) {
        LOG("----------Waiting for networking-----------");
        aos_msleep(1000);
    }

    hal_wifi_get_mac_addr(NULL, mac);
    snprintf(name, SERVICE_NAME_LEN_MAX - 1, "dev_%02x:%02x:%02x", mac[3], mac[4], mac[5]);
    snprintf(mac_str_txt, 32, "mac=%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);



    self_srv = umesh_service_init(net_handle, name, SRV_TYPE, 8080);
    LOG("service init ret = %d", ret);
    umesh_service_add_txt(self_srv, mac_str_txt);
    umesh_service_add_txt(self_srv, "ext_info=mesh network");

    ret = umesh_start_browse_service(self_srv, service_found, NULL);
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
        uint8_t has_member = 0;
        peer_list_t *cur;

        aos_msleep(5000);
        if (!umesh_is_connected(net_handle)) {
            continue;
        }

        aos_mutex_lock(&user_peer_state.mutex, AOS_WAIT_FOREVER);
        cur = user_peer_state.found_peer_list;
        while (cur != NULL) {
            char ip_str[64] = {0};
            get_id_to_str(&cur->id, ip_str);
            if (cur->joined == 0) {
                ret = umesh_invite_peer(session, &cur->id, 5000);
                LOG("found peer:%s not in session ,invite he, ret = %d", ip_str, ret);
                if (ret == 0) {
                    cur->joined = 1;
                }
            } else {
                char send[256] = {0};
                has_member = 1;
                /*send data to the specified member*/
                snprintf(send, 255, test_data, self_srv->srv_type, self_srv->srv_name);
                ret = umesh_send(session, &cur->id, (const uint8_t *)send, strlen(send), MODE_UNRELIABLE);
                LOG("send ucast data to %s ,ret = %d", ip_str, ret);
            }
            cur = cur->next;
        }
        /*send data to all members in session*/
        if (has_member) {
            char send[256] = {0};
            snprintf(send, 255, test_data_mcast, self_srv->srv_type, self_srv->srv_name);
            ret = umesh_send(session, UMESH_SEND_BROADCAST, (const uint8_t *)send, strlen(send), MODE_UNRELIABLE);
            LOG("send mcast data to session ,ret = %d", ret);
        }
        aos_mutex_unlock(&user_peer_state.mutex);
#if ENABLE_LOOP_TEST
    } while (1);
#else
    }
    while (cnt++ < TEST_CNT);
    cnt = 0;
#endif
err:
    LOG("-----------------test end--------------------");
    umesh_stop_advertise_service(self_srv);
    umesh_stop_browse_service();

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

    user_peer_state_deinit(&user_peer_state);
#if ENABLE_REPEAT_TEST
    LOG("test again!!");
    goto start;
#endif

    return;
}

#if (ENABLE_WATCH_DOG)
#include "aos/hal/wdg.h"
#define WDG_PORT_NUM 0

/* task parameters */
#define TASK_FEEDWDG_NAME      "feedwdg"
#define TASK_FEEDWDG_STACKSIZE 2048
#define TASK_FEEDWDG_PRI       32

#define TAG "umesh2 demo"
/* task handle */
aos_task_t task_feedwdg;

static wdg_dev_t watchdog;

/* task entry */
static void task_feedwdg_entry(void *arg)
{
    unsigned int n = 0;
    /**
     * The task fed the watchdog every 1000ms. The feeding interval must be less than the
     * watchdog timeout, otherwise it will trigger by mistake.
     */
    while (1) {
        n++;
        if (n % 5 == 0) {
            LOG("feed the watchdog!\n");
        }

        hal_wdg_reload(&watchdog);

        aos_msleep(1500);
    }
}

void hal_watchdog_app_enable(void)
{
    int ret;
    /* wdg port set */
    watchdog.port = WDG_PORT_NUM;

    /*  set reload time to 1500ms */
    watchdog.config.timeout = 8000;

    /* init watchdog with the given settings */
    ret = hal_wdg_init(&watchdog);
    if (ret != 0) {
        LOGE(TAG, "watchdog init error !\n");
        return -1;
    }

    /* Create the task to feed the watchdog */
    ret = aos_task_new_ext(&task_feedwdg, TASK_FEEDWDG_NAME, task_feedwdg_entry, NULL,
                           TASK_FEEDWDG_STACKSIZE, TASK_FEEDWDG_PRI);
    if (ret != 0) {
        LOGE(TAG, "create watchdog task error\r\n");
        ret = hal_wdg_finalize(&watchdog);
        if (ret != 0) {
            LOGE(TAG, "delete watchdog error!\r\n");
        }
        return;
    }
}
#endif


int application_start(int argc, char **argv)
{

    LOG("application_start");
#if ENABLE_WATCH_DOG
    hal_watchdog_app_enable();
#endif
#if ENABLE_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, monitor_work, NULL);
#endif
    netmgr_init();
    netmgr_start(false);

    aos_set_log_level(AOS_LL_DEBUG);
    aos_task_new("meshappmain", app_main_entry, NULL, 4096);
    aos_loop_run();
    return 0;
}
