/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include <netdb.h>
#include <sys/socket.h>
#include <aos/aos.h>
#include "event_type_code.h"
#ifdef CSP_LINUXHOST
#include <arpa/inet.h>
#endif

#include "umesh.h"

void *aos_malloc(size_t size);
void  aos_free(void *mem);

#if defined(CSP_LINUXHOST)
#define LINUXHOST_RHINO
#endif

/*
 * under linuxhost, we do tricky things
 * DDA will run as pthread, not Rhino tasks
 * and directly call Linux socket
 * and mesh HAL work as an HW in Rhino utilizing cpu_event infra
 */
#ifndef LINUXHOST_RHINO
#include "aos/log.h"
#include "csp.h"
#define dda_task_new aos_task_new
#define dda_task_exit aos_task_exit
#else
#undef  LOGD
#undef  LOGI
#undef  LOGE
#define LOGD(mod, arg...)
#define LOGI(mod, arg...) printf(mod arg)
#define LOGE(mod, arg...) printf(mod arg)
#include <pthread.h>
static int dda_task_new(const char *name, void (*fn)(void *), void *arg, int stacksize)
{
    pthread_t tid;
    int err = pthread_create(&tid, NULL, (void *(*)(void *))fn, arg);
    return err;
}

static void dda_task_exit(int code)
{
    pthread_exit(NULL);
}

extern int aos_task_new(const char *name, void (*fn)(void *), void *arg, int stacksize);
extern void aos_task_exit(int code);
#endif

#include "eloop.h"
#include "packet.h"

#define MYTAG "DDA"

typedef struct {
    dlist_t next;
    void *buf;
    int size;
} sending_ent_t;

typedef struct {
    dlist_t next;
    int seqno;
    dda_p2p_cb cb;
    void *cb_data;
} dda_p2p_t;

typedef struct {
    dlist_t next;
    dda_mesh_cb cb;
    void *cb_data;
    int media_type;
} mesh_cb_t;

typedef struct agent_info_s {
    uint64_t agent_id;
    uint64_t mac_addr;
    struct sockaddr_in master_addr;
    uint32_t seqno;
    int sockfd;
    int port;
    uint64_t macaddr;
    int acked;
    dlist_t sending_queue;

    int p2p_seqno;
    dlist_t p2p_queue;

    dlist_t mesh_cbs;
} agent_info_t;

static agent_info_t agent_info = {
    .agent_id = 0,
    .sockfd = -1,
    .sending_queue = AOS_DLIST_INIT(agent_info.sending_queue),
    .p2p_queue = AOS_DLIST_INIT(agent_info.p2p_queue),
    .mesh_cbs = AOS_DLIST_INIT(agent_info.mesh_cbs),
};

static int create_socket(void)
{
    struct sockaddr_in my_addr;
    const int opt = 1;
    int ret = -1;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        goto out;
    }

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
#ifndef CSP_LINUXHOST
    my_addr.sin_port = htons(DFL_PORT_AGENT);
#endif
    ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (ret < 0) {
        goto out1;
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    if (ret < 0) {
        goto out1;
    }

    socklen_t len = sizeof(my_addr);
    getsockname(sockfd, (struct sockaddr *)&my_addr, &len);
    agent_info.port = ntohs(my_addr.sin_port);

    agent_info.sockfd = sockfd;
    agent_info.master_addr.sin_family = AF_INET;
    agent_info.master_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    agent_info.master_addr.sin_port = htons(DFL_PORT_MASTER);

    LOGI(MYTAG, "agent started @ %s %d", inet_ntoa(my_addr.sin_addr), agent_info.port);
    return ret;
out1:
    close(sockfd);
out:
    return ret;
}

#define QUEUE_CHECK_TIMER (100 * 1000)
static void check_sending_queue(void *d1, void *d2)
{
    agent_info_t *agent = (agent_info_t *)d2;
    bool is_empty = dlist_empty(&agent->sending_queue);

    if (is_empty) {
        return;
    }

    do {
        sending_ent_t *ent = dlist_first_entry(&agent->sending_queue, sending_ent_t, next);
        ipc_msg_t *cmsg = (ipc_msg_t *)ent->buf;
        struct sockaddr_in *paddrto = &agent->master_addr;

        sendto(agent->sockfd, ent->buf, ent->size, 0, (struct sockaddr *)paddrto, sizeof(*paddrto));

        if (cmsg->cmd & CMD_NEED_ACK_FLAG) {
            break;
        }

        dlist_del(&ent->next);
        free(ent->buf);
        free(ent);
    } while (!dlist_empty(&agent->sending_queue));

    eloop_register_timeout(0, QUEUE_CHECK_TIMER, check_sending_queue, 0, agent);
}

static void check_ack_cmd(agent_info_t *agent, uint16_t cmd)
{
    if (!(cmd & CMD_ACK_FLAG)) {
        return;
    }

    if (dlist_empty(&agent->sending_queue)) {
        return;
    }

    sending_ent_t *ent = dlist_first_entry(&agent->sending_queue, sending_ent_t, next);
    ipc_msg_t *cmsg = (ipc_msg_t *)ent->buf;

    cmd &= ~CMD_ACK_FLAG;
    cmd |= CMD_NEED_ACK_FLAG;
    if (cmd == cmsg->cmd) {
        dlist_del(&ent->next);
        free(ent->buf);
        free(ent);

        check_sending_queue(NULL, agent);
    }
}

static void push_to_queue(agent_info_t *agent, void *buf, int size)
{
    sending_ent_t *ent = malloc(sizeof(*ent));
    bool need_timer = dlist_empty(&agent->sending_queue);

    ent->buf = buf;
    ent->size = size;
    dlist_add_tail(&ent->next, &agent->sending_queue);

    if (need_timer) {
        eloop_register_timeout(0, QUEUE_CHECK_TIMER, check_sending_queue, 0, agent);
    }
}

static int send_master(agent_info_t *agent, void *buf, int size)
{
    struct sockaddr_in *paddrto = &agent->master_addr;

    if (dlist_empty(&agent->sending_queue)) {
        sendto(agent->sockfd, buf, size, 0, (struct sockaddr *)paddrto, sizeof(*paddrto));
        return size;
    }

    return 0;
}

static int send_data_ext(agent_info_t *agent, int type, int cmd, uint64_t cmd_priv,
                         int dst_id, const void *buf, int len)
{
    int csize = sizeof(ipc_msg_t) + len;
    ipc_msg_t *cmsg;

    cmsg = alloc_msg(type, cmd, csize, dst_id, agent->agent_id);
    if (cmsg == NULL) {
        return -1;
    }
    cmsg->cmd_priv.opaque = cmd_priv;
    cmsg->sequence = ++agent->seqno;

    memcpy(cmsg + 1, buf, len);

    int ret = send_master(agent, cmsg, csize);
    if (!ret || (cmd & CMD_NEED_ACK_FLAG)) {
        push_to_queue(agent, cmsg, csize);
        return csize;
    }

    free(cmsg);

    return csize;
}

static int send_data(agent_info_t *agent, int type, int cmd, int dst_id, void *buf, int len)
{
    return send_data_ext(agent, type, cmd, 0, dst_id, buf, len);
}

static int connect_master(agent_info_t *agent)
{
    if (!agent->agent_id) {
        umesh_hal_module_t *m = hal_umesh_get_default_module();
        if (!m) {
            return 0;
        }

        const mac_address_t *mac;
        mac = hal_umesh_get_mac_address(m);
        if (mac) {
            memcpy(&agent->mac_addr, mac->addr, 6);
        }
    }

    req_cmd_mgmt_device_up_t req = {
        .agent_id = agent->agent_id,
        .mac_addr = agent->mac_addr,
    };
    return send_data(agent, TYPE_MGMT, CMD_NEED_ACK(CMD_MGMT_DEVICE_UP), MASTER_ID, &req, sizeof req);
}

#ifdef CSP_LINUXHOST
#include <cpu_event.h>

static void aos_loop(void *arg)
{
    umesh_init(MODE_RX_ON);
    umesh_start(NULL);
    aos_loop_run();
    aos_task_exit(0);
}
#else
#define cpu_event_malloc malloc
#define cpu_event_free free
static void cpu_call_handler(void (*f)(const void *), const void *arg)
{
    f(arg);
}
#endif

static void run_aos_loop(const void *arg)
{
    aos_post_event(EV_DDA, CODE_DDA_ON_CONNECTED, 0u);

#ifdef CSP_LINUXHOST
    aos_task_new("aos", aos_loop, NULL, 8192);
#endif
}

typedef struct {
    frame_t frm;
    frame_info_t fino;
    void *buf;
    int len;
    int media_type;
    uint64_t cmd_src;
    uint64_t cmd_private;
} compound_msg_t;

static void pass_to_urmesh(const void *arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;
    mesh_cb_t *mesh_cb;

    dlist_for_each_entry(&agent_info.mesh_cbs, mesh_cb, mesh_cb_t, next) {
        if (mesh_cb->media_type != cmsg->media_type) {
            continue;
        }
        mesh_cb->cb(&cmsg->frm, &cmsg->fino, mesh_cb->cb_data);
    }

    cpu_event_free(cmsg);
}

static void handle_data(agent_info_t *agent, ipc_msg_t *msg, struct sockaddr *paddr)
{
    if (dlist_empty(&agent->mesh_cbs)) {
        return;
    }

    uint8_t chn = msg->cmd_priv.mesh.channel;
    uint8_t rssi = msg->cmd_priv.mesh.rssi;
    uint8_t media = msg->cmd_priv.mesh.media_type;
    int8_t key_index = msg->cmd_priv.mesh.key_index;

    compound_msg_t *cmsg = cpu_event_malloc(sizeof(*cmsg) + msg->payload_len);

    cmsg->fino.rssi = rssi;
    cmsg->fino.channel = chn;
    cmsg->fino.key_index = key_index;
    cmsg->fino.peer.value = msg->src;
    cmsg->fino.peer.len = 8;

    cmsg->frm.data = (void *)(cmsg + 1);
    cmsg->frm.len = msg->payload_len;
    memcpy(cmsg->frm.data, msg + 1, cmsg->frm.len);

    cmsg->media_type = media;
    cpu_call_handler(pass_to_urmesh, cmsg);
}

static void handle_mesh(agent_info_t *agent, ipc_msg_t *msg, struct sockaddr *paddr)
{
    if (msg->cmd == CMD_MESH_DATA) {
        handle_data(agent, msg, paddr);
    }
}

#ifdef CONFIG_AOS_MESH
#define CLI_RES_SIZE 0x10000
void dda_cli_log(char *buf);
static void handle_cli_response(char *buf, int len, void *priv)
{
    if (!buf && !len) {
        dda_cli_log(priv);
        cpu_event_free(priv);
        return;
    }

    if (!len) {
        return;
    }

    strncat(priv, buf, CLI_RES_SIZE - strlen(priv));
}

static void pass_to_urcli(const void *arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;
    char *buf = cpu_event_malloc(CLI_RES_SIZE);
    memcpy(buf, "\n", 2);

    umesh_cli_cmd(cmsg->buf, cmsg->len, handle_cli_response, buf);

    cpu_event_free(cmsg);
}

static void handle_input(char *buf, int len)
{
    compound_msg_t *cmsg = cpu_event_malloc(sizeof(*cmsg) + len);
    cmsg->buf = cmsg + 1;
    cmsg->len = len;
    memcpy(cmsg->buf, buf, len);
    cpu_call_handler(pass_to_urcli, cmsg);
}

struct cmd_res {
    uint64_t cmd_private;
    uint64_t cmd_src;
    char     buf[0];
};

static void handle_cmd_response(char *buf, int len, void *priv)
{
    struct cmd_res *res = (struct cmd_res *)priv;
    if (!buf && !len) {
        send_data_ext(&agent_info, TYPE_P2P, CMD_P2P_RES,
                      res->cmd_private, res->cmd_src,
                      res->buf, strlen(res->buf) + 1);
        cpu_event_free(priv);
        return;
    }

    if (!len) {
        return;
    }

    strncat(res->buf, buf, CLI_RES_SIZE - strlen(res->buf));
}

static void got_p2p_request(const void *arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;
    struct cmd_res *res = (struct cmd_res *)cpu_event_malloc(sizeof(*res) + CLI_RES_SIZE);
    res->buf[0] = 0;

    res->cmd_src = cmsg->cmd_src;
    res->cmd_private = cmsg->cmd_private;
    umesh_cli_cmd(cmsg->buf, cmsg->len, handle_cmd_response, res);

    cpu_event_free(cmsg);
}

static void handle_p2p_request(ipc_msg_t *req)
{
    char *buf = (char *)(req + 1);
    int len = req->payload_len;
    compound_msg_t *cmsg = cpu_event_malloc(sizeof(*cmsg) + len);
    cmsg->buf = cmsg + 1;
    cmsg->len = len;
    cmsg->cmd_src = req->src;
    cmsg->cmd_private = req->cmd_priv.opaque;
    memcpy(cmsg->buf, buf, len);
    cpu_call_handler(got_p2p_request, cmsg);
}

static void got_p2p_response(const void *arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;
    dda_p2p_t *req;

    dlist_for_each_entry(&agent_info.p2p_queue, req, dda_p2p_t, next) {
        if (req->seqno != (int)cmsg->cmd_private) {
            continue;
        }

        dlist_del(&req->next);
        req->cb(cmsg->buf, cmsg->len, req->cb_data);
        aos_free(req);
        break;
    }

    cpu_event_free(cmsg);
}

static void handle_p2p_response(ipc_msg_t *req)
{
    char *buf = (char *)(req + 1);
    int len = req->payload_len;
    compound_msg_t *cmsg = cpu_event_malloc(sizeof(*cmsg) + len);
    cmsg->buf = cmsg + 1;
    cmsg->len = len;
    cmsg->cmd_src = req->src;
    cmsg->cmd_private = req->cmd_priv.opaque;
    memcpy(cmsg->buf, buf, len);
    cpu_call_handler(got_p2p_response, cmsg);
}

static void handle_p2p(ipc_msg_t *req)
{
    if (req->cmd == CMD_P2P_REQ) {
        handle_p2p_request(req);
    } else if (req->cmd == CMD_P2P_RES) {
        handle_p2p_response(req);
    }
}
#else
static void handle_input(char *buf, int len)
{
}
static void handle_p2p(ipc_msg_t *req)
{
}
#endif

static void report_agent_status(void *d1, void *d2)
{
    agent_info_t *agent = (agent_info_t *)d2;
    agent_status_t status;
    status.sid = umesh_get_sid();
    status.netid = umesh_get_meshnetid();
    send_data(agent, TYPE_MGMT, CMD_MGMT_STATUS, MASTER_ID, &status, sizeof status);

    eloop_register_timeout(5, 0, report_agent_status, 0, agent);
}

static void handle_event(agent_info_t *agent, ipc_msg_t *msg, struct sockaddr *paddr)
{
    uint16_t cmd = msg->cmd;

    if (cmd == CMD_MGMT_DEVICE_UP) {
        if (agent->acked) {
            LOGI(MYTAG, "duplicate ack, ignore!\n");
            return;
        }

        eloop_register_timeout(5, 0, report_agent_status, 0, agent);
        agent->acked = 1;
        req_cmd_mgmt_device_up_t req;
        memcpy(&req, msg + 1, sizeof req);
        memcpy(&agent->master_addr, paddr, sizeof(*paddr));
        agent->agent_id = req.agent_id;
        agent->mac_addr = req.mac_addr;
        LOGD(MYTAG, "ack from master %s\n", inet_ntoa(agent->master_addr.sin_addr));

        cpu_call_handler(run_aos_loop, NULL);
    } else if (cmd == CMD_MGMT_ATTACH) {
        memcpy(&agent->master_addr, paddr, sizeof(*paddr));
    } else {
        LOGE(MYTAG, "unknow cmd %x\n", msg->cmd);
    }
}

static void handle_message(agent_info_t *agent, ipc_msg_t *msg, struct sockaddr *paddr)
{
    uint16_t cmd = msg->cmd;

    msg->cmd &= ~CMD_ACK_FLAG;
    check_ack_cmd(agent, cmd);

    if (msg->type == TYPE_MGMT) {
        handle_event(agent, msg, paddr);
    } else if (msg->type == TYPE_MESH) {
        handle_mesh(agent, msg, paddr);
    } else if (msg->type == TYPE_MISC && cmd == CMD_MISC_INPUT) {
        handle_input((char *)(msg + 1), msg->payload_len);
    } else if (msg->type == TYPE_P2P) {
        handle_p2p(msg);
    } else {
        dda_handle_message(msg);
    }
}

static void agent_recv_data(int fd, void *d1, void *d2)
{
    agent_info_t *agent = (agent_info_t *)d2;
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    char *buf = malloc(2048);
    int ret;

    memset(&addr, 0, sizeof(addr));
    ret = recvfrom(agent->sockfd, buf, 2048, 0, &addr, &addr_len);
    if (ret < 0) {
        perror("recv_data");
    } else {
        handle_message(agent, (ipc_msg_t *)buf, &addr);
    }

    free(buf);
}

void dda_disable(void)
{
    agent_info_t *agent = &agent_info;
    agent->agent_id = -1ULL;
}

int dda_enable(int agent_id)
{
    agent_info_t *agent = &agent_info;

    agent->agent_id = agent_id;
    agent->mac_addr = agent_id;

    return 0;
}

#define MAX_LOG_SIZE 256
void dda_log(char *buf, ...)
{
    va_list args;
    char *buffer;

    buffer = (char *)aos_malloc(MAX_LOG_SIZE);
    if (buffer == NULL) {
        return;
    }
    va_start(args, buf);
    vsnprintf(buffer, MAX_LOG_SIZE, buf, args);
    va_end(args);
    send_data(&agent_info, TYPE_MISC, CMD_MISC_LOG, 0, buffer, strlen(buffer) + 1);
    aos_free(buffer);
}

void dda_cli_log(char *buf)
{
    send_data(&agent_info, TYPE_MISC, CMD_MISC_CLI, 0, buf, strlen(buf) + 1);
}

extern void linuxhost_hal_urmesh_register(void);
int dda_service_init(void)
{
    if (agent_info.agent_id == -1ULL) {
        return 0;
    }

    linuxhost_hal_urmesh_register();

    int ret;
    ret = create_socket();
    if (ret < 0) {
        goto out;
    }
    eloop_init(0);
out:
    return ret;
}

static void dda_entry(void *arg)
{
    agent_info_t *agent = (agent_info_t *)arg;

    eloop_register_read_sock(agent->sockfd, agent_recv_data, 0, agent);
    connect_master(agent);

    eloop_run();

    dda_task_exit(0);
    LOGI(MYTAG, "dda quit");
}

int dda_service_start(void)
{
    if (agent_info.agent_id == -1ULL) {
        return 0;
    }

    dda_task_new("dda", dda_entry, &agent_info, 2048);
    return 0;
}

void dda_service_stop(void)
{
    close(agent_info.sockfd);
    agent_info.sockfd = -1;
}

void dda_service_deinit(void)
{
}

/* called from YoC context, so need to use aos_mm_{alloc,free} */
void dda_p2p_request(int dst_id, const char *cmd, dda_p2p_cb cb, void *cb_data)
{
    dda_p2p_t *req = aos_malloc(sizeof(*req));
    req->seqno = ++agent_info.p2p_seqno;
    req->cb = cb;
    req->cb_data = cb_data;
    dlist_add(&req->next, &agent_info.p2p_queue);

    send_data_ext(&agent_info, TYPE_P2P, CMD_P2P_REQ, req->seqno, dst_id, cmd, strlen(cmd) + 1);
}

void dda_p2p_remove_request(dda_p2p_cb cb, void *cb_data)
{
    dda_p2p_t *req;

    dlist_for_each_entry(&agent_info.p2p_queue, req, dda_p2p_t, next) {
        if (req->cb != cb) {
            continue;
        }
        if (req->cb_data != cb_data) {
            continue;
        }

        dlist_del(&req->next);
        aos_free(req);
        break;
    }
}

void dda_mesh_send_data(int type, int cmd, uint64_t cmd_pri,
                        int dst_id, const void *buf, int len)
{
    send_data_ext(&agent_info, type, cmd, cmd_pri, dst_id, buf, len);
}

void dda_mesh_register_receiver(int media_type, dda_mesh_cb cb, void *cb_data)
{
    /* in very early stage, aos_malloc not usable */
    mesh_cb_t *mcb = malloc(sizeof(*mcb));

    mcb->cb = cb;
    mcb->cb_data = cb_data;
    mcb->media_type = media_type;

    dlist_add_tail(&mcb->next, &agent_info.mesh_cbs);
}

uint64_t dda_mesh_get_agent_id(void)
{
    return agent_info.agent_id;
}
