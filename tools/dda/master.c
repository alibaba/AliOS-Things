/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define _GNU_SOURCE
#undef malloc
#undef free
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

#include "aos/list.h"

#include "eloop.h"
#include "packet.h"
#include "dgraph.h"
#include "config_parser.h"

#define MYTAG "DDM"
/*
 * devices id is starting from 1, as 0 means master
 * while device index is starting from 0, as norml c array
 */
#define ID2IDX(id) ((id)-1)
#define IDX2ID(id) ((id)+1)

typedef struct {
    uint64_t mac_addr;
    uint8_t channel;
    uint8_t on;

    /*
     * rssi from this agent to other agents
     * indexed by rssi_vecotr[agent_id-1]
     */
    uint8_t *rssi_vector;

    agent_status_t status;
} if_status_t;

typedef struct {
    dlist_t dev_node;

    struct sockaddr_in addr;
    uint64_t agent_id;
    int log_off;
    int pid;
    int alive;

    if_status_t ifs[IF_NUM];
} agent_t;

typedef struct master_info_s {
    struct sockaddr_in master_addr;
    struct sockaddr_in watch_addr;
    uint32_t seqno;
    int sockfd;

    dlist_t dev_nodes;
    agent_t **agents;

    int loss_rate;
    const char *self_exec;
    int log_off;
    FILE *log_file;

    /* from config file */
    int devices;
    uint8_t *rssi_metrics[IF_NUM];
    struct node_config *node_configs;
} master_info_t;
static master_info_t master_info;

#define set_metric(p, t, i, j, v) do { \
    *(p->rssi_metrics[t] + i * p->devices + j) = v; \
} while(0)

#define get_metric(p, t, i, j) ({ \
    *(p->rssi_metrics[t] + i * p->devices + j); \
})

#define for_each_agent(agent) \
    int __i; \
    for (__i=0; \
        agent=__i<master_info.devices?master_info.agents[__i]:NULL, \
        __i<master_info.devices; \
        __i++)

static const char *env_get_value(const char *key);
static void dgraph_sync_agent(agent_t *);
static void dgraph_sync_msg(ipc_msg_t *);
static void dgraph_reset(void);

static int env_get_value_int(const char *key)
{
    const char *v = env_get_value(key);
    if (v)
        return atoi(v);
    return -1;
}

static inline agent_t *get_agent_by_idx(master_info_t *master, int idx)
{
    agent_t *agent = master->agents[idx];

    if (idx >= 0 && idx < master->devices)
        return agent;

    return NULL;
}

static inline agent_t *get_agent_by_id(master_info_t *master, int did)
{
    return get_agent_by_idx(master, ID2IDX(did));
}

typedef struct cmd_entry_s {
    const char *    token;
    const char *    usage;
    int             (*handler)(master_info_t *master, char **argv);
} cmd_entry_t;

static const cmd_entry_t cmd_table[];

#define L_DBG 1
#define L_ERR 2
static int partial_mode;
static void mprintf(int level, const char *format, ...)
{
    struct timeval tv;
    va_list args;

    rl_redisplay();
    gettimeofday(&tv, NULL);
    va_start(args, format);
    if (master_info.log_file) {
        if (!partial_mode)
            fprintf(master_info.log_file, "[%06ld.%06ld][0000]", tv.tv_sec, tv.tv_usec);
        vfprintf(master_info.log_file, format, args);
    }
    if (level > master_info.log_off) {
        if (!partial_mode)
            printf("[%06ld.%06ld][0000]", tv.tv_sec, tv.tv_usec);
        vprintf(format, args);
    }
    va_end(args);
    fflush(stdout);
    rl_redisplay();
}

static int create_socket(void)
{
    struct sockaddr_in my_addr;
    int ret = -1;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        goto out;

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int));

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(DFL_PORT_MASTER);
    ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (ret < 0)
        goto out1;

    master_info.sockfd = sockfd;
    return ret;
out1:
    close(sockfd);
out:
    return ret;
}

static int send_agent(master_info_t *master, agent_t *agent, void *buf, int size)
{
    struct sockaddr *paddr = (struct sockaddr *)&agent->addr;
    return sendto(master->sockfd, buf, size, 0, paddr, sizeof(*paddr));
}

static int send_agent_data(master_info_t *master, agent_t *agent,
                           int type, int cmdid, const void *buf, int size)
{
    int csize = sizeof(ipc_msg_t) + size;
    ipc_msg_t *cmsg = alloc_msg(type, cmdid, csize, agent->agent_id, MASTER_ID);
    int ret;

    if (cmsg == NULL) {
        return -1;
    }

    memcpy(cmsg+1, buf, size);
    ret = send_agent(master, agent, cmsg, csize);
    free(cmsg);
    return ret;
}

static void send_unicast(master_info_t *master, ipc_msg_t *msg, int dst_idx)
{
    agent_t *s_agent = master->agents[ID2IDX(msg->src)];
    agent_t *d_agent = master->agents[dst_idx];
    const char *lost_reason = "unknown";
    int if_type = msg->cmd_priv.mesh.media_type;
    if_status_t *sif = s_agent->ifs + if_type;
    int rssi = get_metric(master, if_type, ID2IDX(msg->src), dst_idx);
    if_status_t *dif;

    if (dst_idx >= master->devices)
        return;
    if (!d_agent)
        return;
    if (!d_agent->alive)
        return;
    if (rssi <= 0) {
        lost_reason = "rssi";
        goto lost;
    }

    dif = d_agent->ifs + msg->cmd_priv.mesh.media_type;
    if (dif->channel != sif->channel) {
        lost_reason = "channel";
        goto lost;
    }

    int rate = rand() % 100;
    if (rate < master->loss_rate) {
        lost_reason = "random";
        goto lost;
    }

    mprintf(L_DBG, " (id=%d,rssi=%d)", IDX2ID(dst_idx), rssi);
    msg->dst = IDX2ID(dst_idx);
    msg->cmd_priv.mesh.rssi = rssi;
    msg->cmd_priv.mesh.channel = dif->channel;
    send_agent(master, d_agent, msg, sizeof(*msg) + msg->payload_len);

    dgraph_sync_msg(msg);
    return;

lost:
    mprintf(L_DBG, " !(id=%d,reason=%s)", IDX2ID(dst_idx), lost_reason);
    return;
}

static void send_broadcast(master_info_t *master, ipc_msg_t *msg)
{
    int i;
    for (i=0;i<master->devices;i++) {
        if (i == (int)ID2IDX(msg->src))
            continue;

        send_unicast(master, msg, i);
    }
}

static void handle_mesh(master_info_t *master, ipc_msg_t *msg, struct sockaddr *paddr)
{
    agent_t *agent = get_agent_by_id(master, msg->src);
    int chn = msg->cmd_priv.mesh.channel;
    if_status_t *dif;
    if (!agent) {
        mprintf(L_ERR, "invalid agent %d\n", msg->src);
        return;
    }

    dif = agent->ifs + msg->cmd_priv.mesh.media_type;
    if (msg->cmd == CMD_MESH_SET_CHN) {
        dif->channel = msg->cmd_priv.mesh.channel;
        return;
    }

    if (msg->cmd != CMD_MESH_DATA) {
        mprintf(L_ERR, "unsupport mesh cmd %d\n", msg->cmd);
        return;
    }

    dif->channel = chn;
    const char *media = if_idx2name(msg->cmd_priv.mesh.media_type);
    mprintf(L_DBG, "recv from %llx to %llx @ %5s chn %d, forwared to",
                 msg->src, msg->dst, media, dif->channel);
    partial_mode = 1;
    if (msg->dst == BCAST_ID)
        send_broadcast(master, msg);
    else
        send_unicast(master, msg, ID2IDX(msg->dst));
    mprintf(L_DBG, "\n");
    partial_mode = 0;
}

static agent_t *new_agent(master_info_t *master, int id)
{
    int if_type, idx = ID2IDX(id);
    agent_t *agent = master->agents[idx];

    if (agent)
        return agent;

    agent = (agent_t *)calloc(1, sizeof(*agent));
    master->agents[idx] = agent;
    agent->agent_id = id;

    for (if_type = IF_WIFI;if_type<IF_NUM;if_type++) {
        if (!(master->node_configs[idx].ifs & (1 << if_type)))
            continue;
        agent->ifs[if_type].on = 1;
        agent->ifs[if_type].rssi_vector = master->rssi_metrics[if_type] + idx * master->devices;
    }

    return agent;
}

static void new_device_up(master_info_t *master, ipc_msg_t *msg, struct sockaddr *paddr)
{
    req_cmd_mgmt_device_up_t req;
    uint64_t agent_id;
    agent_t *agent;
    if_status_t *dif;

    memcpy(&req, msg+1, sizeof(req));
    agent_id = req.agent_id;
    if (agent_id > (uint64_t)master->devices) {
        goto err_out;
    }

    if (agent_id != 0)
        goto found;

    if (req.mac_addr == 0)
        goto err_out;

    for_each_agent(agent) {
        if (!agent) continue;
        dif = agent->ifs + msg->cmd_priv.mesh.media_type;
        if (dif->mac_addr != req.mac_addr)
            continue;
        agent_id = __i + 1;
        req.agent_id = agent_id;
        goto found;
    }
err_out:
    mprintf(L_ERR, "error agent id %d %08llx\n", agent_id, req.mac_addr);
    return;
found:
    agent = new_agent(master, agent_id);
    agent->alive = 1;
    memcpy(&agent->addr, paddr, sizeof(*paddr));
    dif = agent->ifs + msg->cmd_priv.mesh.media_type;
    mprintf(L_DBG, "new device:%llx mac:%llx\n", agent->agent_id, dif->mac_addr);

    dgraph_sync_agent(agent);
    return;
}

static void handle_event(master_info_t *master, ipc_msg_t *msg, struct sockaddr *paddr)
{
    if (msg->cmd == CMD_MGMT_DEVICE_UP) {
        new_device_up(master, msg, paddr);
    }
    else if (msg->cmd == CMD_MGMT_STATUS) {
        if_status_t *dif;
        agent_status_t *status = (agent_status_t *)(msg + 1);
        agent_t *agent = get_agent_by_id(master, msg->src);
        if (!agent)
            return;
        dif = agent->ifs + msg->cmd_priv.mesh.media_type;
        if (memcmp(&dif->status, status, sizeof(*status))) {
            memcpy(&dif->status, status, sizeof(*status));
            dgraph_sync_agent(agent);
        }
    }
}

static void handle_p2p(master_info_t *master, ipc_msg_t *msg)
{
    /* relay */
    if (msg->dst != MASTER_ID) {
        agent_t *agent = get_agent_by_id(master, msg->dst);
        if (!agent) {
            printf("error agent id %lld\n", msg->dst);
            return;
        }

        send_agent(master, agent, msg, sizeof(*msg) + msg->payload_len);
        return;
    }
}

static void handle_cmd(master_info_t *master, ipc_msg_t *msg)
{
    int cmd = msg->cmd & ~CMD_ACK_FLAG;

    if (cmd != CMD_MISC_LOG && cmd != CMD_MISC_CLI)
        return;

    if (master->log_file)
        fprintf(master->log_file, "[%06ld.%06ld][%04lld] %s", msg->tv.tv_sec, msg->tv.tv_usec, msg->src, (char *)(msg+1));

    agent_t *s_agent = get_agent_by_id(master, msg->src);
    if (s_agent && s_agent->log_off)
        return;

    if (cmd == CMD_MISC_LOG)
        printf("[%06ld.%06ld][%04lld]", msg->tv.tv_sec, msg->tv.tv_usec, msg->src);
    printf("%s", (char *)(msg+1));
}

static int send_agent(master_info_t *master, agent_t *agent, void *buf, int size);
static void handle_message(master_info_t *master, ipc_msg_t *msg, int size, struct sockaddr *paddr)
{
    bool need_ack = msg->cmd & CMD_NEED_ACK_FLAG;
    msg->cmd &= ~CMD_NEED_ACK_FLAG;
    if (msg->type == TYPE_MGMT)
        handle_event(master, msg, paddr);
    else if (msg->type == TYPE_MESH)
        handle_mesh(master, msg, paddr);
    else if (msg->type == TYPE_P2P)
        handle_p2p(master, msg);
    else if (msg->type == TYPE_MISC)
        handle_cmd(master, msg);
    else
        mprintf(L_ERR, "unknow message %d\n", msg->type);

    if (need_ack) {
        agent_t *s_agent = master->agents[ID2IDX(msg->src)];
        msg->cmd = CMD_ACK(msg->cmd);
        send_agent(master, s_agent, msg, size);
    }
}

static void master_recv_data(int fd, void *d1, void *d2)
{
    master_info_t *master = (master_info_t *)d2;
    struct sockaddr         addr;
    socklen_t               addr_len = sizeof(addr);
    char *buf = malloc(2048);
    int ret;

    memset(&addr, 0, sizeof(addr));
    ret = recvfrom(master->sockfd, buf, 2048, 0, &addr, &addr_len);
    if (ret < 0)
        perror("recv_data");
    else if (ret == 0) {
        printf("socket closed!\n");
        eloop_unregister_read_sock(fd);
        close(fd);
    }
    else
        handle_message(master, (ipc_msg_t *)buf, ret, &addr);

    free(buf);
}

#include <sys/mman.h>
#ifndef MEM_DEBUG
static void *alloc_mem(int sz)
{
    return malloc(sz);
}

static inline void free_mem(void *m, int sz)
{
    free(m);
}

static void set_mem_rdonly(void *m, int sz)
{
    mprotect(m, sz, PROT_READ);
}

static inline void set_mem_rw(void *m, int sz)
{
    mprotect(m, sz, PROT_READ | PROT_WRITE);
}
#else
static void *alloc_mem(int sz)
{
    return mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
}

static inline void free_mem(void *m, int sz)
{
    munmap(m, sz);
}

static void set_mem_rdonly(void *m, int sz)
{
    mprotect(m, sz, PROT_READ);
}

static inline void set_mem_rw(void *m, int sz)
{
    mprotect(m, sz, PROT_READ | PROT_WRITE);
}
#endif

static void init_device_metrics(master_info_t *master, dda_config_result_t *pcres)
{
    int if_type;
    int devices = pcres->devices;
    uint64_t *macs = pcres->infs[IF_WIFI].macs;
    int sz = devices * devices;
    if (!devices)
        return;

    master->agents = alloc_mem(devices * sizeof(void *));
    bzero(master->agents, devices * sizeof(void *));

    for (if_type = IF_WIFI;if_type<IF_NUM;if_type++) {
        char *rssi_metrics = pcres->infs[if_type].metrics;
        if (!rssi_metrics)
            continue;

        uint8_t *metrics = alloc_mem(sz);
        master->rssi_metrics[if_type] = metrics;
        memcpy(metrics, rssi_metrics, sz);
        set_mem_rdonly(metrics, sz);
    }

    master->devices = devices;

    sz = devices * sizeof(struct node_config);
    master->node_configs = malloc(sz);
    memcpy(master->node_configs, pcres->nodes, sz);

    if (!macs)
        return;

    int i;
    for (i=0;i<devices;i++) {
        if (!macs[i])
            continue;
        agent_t *agent = new_agent(master, i+1);
        if_status_t *sif = agent->ifs + IF_WIFI;
        sif->mac_addr = macs[i];
    }
}


/*
 * command line interpreter
 */
static struct {
    int cur_id;
    char cmd_prefix[64];
} cli_status = {
    .cmd_prefix = "[Master] ",
};

static int filter_handler(master_info_t *master, char **argv)
{
    if (!argv[1])
        return -1;

    int off = strcmp(argv[1], "off") == 0;
    int target = argv[2] ? atoi(argv[2]) : -1;

    if (target < 0) {
        agent_t *agent;
        master->log_off = off;
        for_each_agent(agent) {
            if (!agent) continue;
            agent->log_off = off;
        }
    }
    else if (target == 0)
        master->log_off = off;
    else if (target > 0) {
        agent_t *agent = new_agent(master, target);
        agent->log_off = off;
    }

    return 0;
}

static int isolate_handler(master_info_t *master, char **argv)
{
    int if_idx = if_name2idx(env_get_value("if"));

    if (!argv[1])
        return -1;

    int i;
    int idx1 = ID2IDX(atoi(argv[1]));
    for (i=0;i<master->devices;i++) {
        set_metric(master, if_idx, idx1, i, 0);
        set_metric(master, if_idx, i, idx1, 0);
    }

    return 0;
}

static int rssi_handler(master_info_t *master, char **argv)
{
    int if_idx = if_name2idx(env_get_value("if"));

    if (if_idx < 0)
        if_idx = IF_WIFI;

    if (!argv[1] || !argv[2])
        return -1;

    int idx1 = ID2IDX(atoi(argv[1]));
    int idx2 = ID2IDX(atoi(argv[2]));
    int rssi1, rssi2;
    if (!argv[3]) {
        rssi1 = get_metric(master, if_idx, idx1, idx2);
        rssi2 = get_metric(master, if_idx, idx2, idx1);
        printf("rssi=%d,%d\n", rssi1, rssi2);
        return 0;
    }

    rssi1 = atoi(argv[3]);
    rssi2 = argv[4] ? atoi(argv[4]) : rssi1;
    set_metric(master, if_idx, idx1, idx2, rssi1);
    set_metric(master, if_idx, idx2, idx1, rssi2);
    return 0;
}

static void master_recv_tty(int fd, void *d1, void *d2)
{
    char buf[256];
    do {
        int ret = read(fd, buf, sizeof(buf) - 1);
        if (ret < 0)
            return;
        if (ret == 0) {
            eloop_unregister_read_sock(fd);
            printf("%s detached\n", (char *)d2);
            free(d2);
            close(fd);
            return;
        }

        buf[ret] = 0;
        printf("%s", buf);
    } while(1);
}

static int tty_handler(master_info_t *master, char **argv)
{
    int ttyfd = open(argv[1], O_RDWR);
    if (ttyfd < 0)
        return -1;

    int mode = fcntl(ttyfd, F_GETFL, 0);
    mode |= O_NONBLOCK;
    fcntl(ttyfd, F_SETFL, mode);

    eloop_register_read_sock(ttyfd, master_recv_tty, 0, strdup(argv[1]));

    return 0;
}

static int goto_handler(master_info_t *master, char **argv)
{
    int new_id = atoi(argv[1]);
    if (!strcmp(argv[1], "master") || !new_id) {
        cli_status.cur_id = 0;
        strcpy(cli_status.cmd_prefix, "[Master] ");
        return 0;
    }

    cli_status.cur_id = new_id;
    sprintf(cli_status.cmd_prefix, "[%04d] ", cli_status.cur_id);
    return 0;
}

static void send_attach(master_info_t *master, struct sockaddr_in *paddr)
{
    int csize = sizeof(ipc_msg_t);
    ipc_msg_t *cmsg = alloc_msg(TYPE_MGMT, CMD_MGMT_ATTACH, csize, 0, MASTER_ID);

    if (cmsg) {
        sendto(master->sockfd, cmsg, csize, 0, paddr, sizeof(*paddr));

        free(cmsg);
    }
}

static void send_attach_timer(void *d1, void *d2)
{
    send_attach(d1, d2);
    eloop_register_timeout(5, 0, send_attach_timer, d1, d2);
}

static void arg_to_sockaddr(struct sockaddr_in *paddr, char **argv)
{
    bzero(paddr, sizeof(*paddr));
    paddr->sin_family = AF_INET;
    inet_aton(argv[1], &paddr->sin_addr);
    paddr->sin_port = argv[2] ? atoi(argv[2]) : DFL_PORT_AGENT;
    paddr->sin_port = htons(paddr->sin_port);
}

static int attach_handler(master_info_t *master, char **argv)
{
    struct sockaddr_in addr;
    arg_to_sockaddr(&addr, argv);

    send_attach(master, &addr);

    return 0;
}

static int watch_handler(master_info_t *master, char **argv)
{
    struct sockaddr_in *paddr = malloc(sizeof(*paddr));
    arg_to_sockaddr(paddr, argv);

    send_attach_timer(master, paddr);
    return 0;
}

static int channel_handler(master_info_t *master, char **argv)
{
    int if_idx = if_name2idx(env_get_value("if"));

    if (!argv[1] || !argv[2])
        return -1;

    int did = atoi(argv[1]);
    int chn = atoi(argv[2]);
    agent_t *agent = master_info.agents[ID2IDX(did)];
    if (!agent)
        return -1;
    if_status_t *sif = agent->ifs + if_idx;
    sif->channel = chn;
    return 0;
}

static int start_one_device(master_info_t *master, int id, int mode, int router, int ifs)
{
    if (id > master->devices)
        return -1;

    agent_t *agent = new_agent(master, id);
    if (agent && agent->alive) {
        mprintf(L_ERR, "agent %d already exist!\n", id);
        return -1;
    }

    agent->pid = fork();
    agent->alive = 1;
    if (ifs < 0)
        ifs = master->node_configs[ID2IDX(id)].ifs;

    if (agent->pid == 0) {
        /* disable stdin/stdout */
        int fd[2];
        int ret;

        ret = pipe(fd);
        if (ret < 0) {
            return -1;
        }
        close(STDIN_FILENO);
        dup2(fd[0], STDIN_FILENO);

        char mode_str[16], router_str[16];
        char name[32];
        sprintf(name, "%d", id);
        char ifs_str[32];
        sprintf(ifs_str, "%d", ifs);
        char *args[16] = {
            (char *)master_info.self_exec,
            "--no-cli",
            "--mesh-node",
            "--mesh-ifs",
            ifs_str,
            "-s",
        };
        int idx = 5;
        if (mode >= 0) {
            sprintf(mode_str, "%d", mode);
            args[idx++] = "--mesh-mode";
            args[idx++] = mode_str;
        }
        if (router >= 0) {
            sprintf(router_str, "%d", router);
            args[idx++] = "--mesh-router";
            args[idx++] = router_str;
        }
        args[idx++] = name;

        execve(args[0], args, NULL);
    }

    return 0;
}

static int send_handler(master_info_t *master, char **argv)
{
    bool all = strcmp(argv[0], "sendall") == 0;
    int id = atoi(argv[1]);
    char buf[256];
    int i;

    buf[0] = 0;
    for(i=all?1:2;argv[i];i++) {
        strncat(buf, argv[i], sizeof(buf)-strlen(buf));
        strncat(buf, " ", sizeof(buf)-strlen(buf));

    }

    for (i=0;i<master->devices;i++) {
        agent_t *agent = master->agents[i];

        if (!agent)
            continue;
        if (!agent->alive)
            continue;
        if (!all && (int)agent->agent_id != id)
            continue;

        send_agent_data(master, agent, TYPE_MISC, CMD_MISC_INPUT, buf, strlen(buf)+1);
    }

    return 0;
}

static int start_handler(master_info_t *master, char **argv)
{
    int i, mode=-1, router=-1, ifs = -1;
    if (!argv[1])
        return -1;

    mode = env_get_value_int("mode");
    router = env_get_value_int("router");
    ifs = env_get_value_int("ifs");

    for (i=1;argv[i];i++) {
        int id = atoi(argv[i]);
        char *sep;

        sep = strchr(argv[i], '-');
        if (!sep) {
            start_one_device(master, id, mode, router, ifs);
            continue;
        }

        int id_last = atoi(sep+1);
        while (id <= id_last) {
            start_one_device(master, id++, mode, router, ifs);
        }
    }

    return 0;
}

static int stop_handler(master_info_t *master, char **argv)
{
    int i;
    if (!argv[1])
        return -1;

    for (i=1;argv[i];i++) {
        int id = atoi(argv[i]);
        agent_t *agent = master->agents[ID2IDX(id)];
        if (!agent)
            continue;
        if (!agent->pid)
            continue;

        kill(agent->pid, SIGINT);
        usleep(10 * 1000);
    }

    for (i=1;argv[i];i++) {
        int id = atoi(argv[i]);
        agent_t *agent = master->agents[ID2IDX(id)];
        if (!agent)
            continue;
        if (!agent->pid)
            continue;
        kill(agent->pid, SIGKILL);
        waitpid(agent->pid, NULL, 0);
        agent->alive = 0;
    }

    return 0;
}

static int ls_handler(master_info_t *master, char **argv)
{
    struct sockaddr_in *paddr;
    if_status_t *sif;
    agent_t *agent;

    for_each_agent(agent) {
        if (!agent) continue;
        paddr = (struct sockaddr_in *)&agent->addr;
        sif = agent->ifs + IF_WIFI;
        printf("[%04lld] alive %d mac %08llx chn %d ip %s:%d sid %04x:%04x\n", agent->agent_id, agent->alive,
               sif->mac_addr, sif->channel, inet_ntoa(paddr->sin_addr), ntohs(paddr->sin_port),
               sif->status.netid, sif->status.sid);
    }

    return 0;
}

static int lossrate_handler(master_info_t *master, char **argv)
{
    if (!argv[1]) {
        printf("current loss rate : %d%%100\n", master->loss_rate);
        return 0;
    }

    int rate = atoi(argv[1]);
    master->loss_rate = rate;
    return 0;
}

static int quit_handler(master_info_t *master, char **argv)
{
    rl_free_line_state ();
    rl_cleanup_after_signal ();

    /* kill all the agents we forked */
    int i;
    for (i=0;i<master->devices;i++) {
        agent_t *agent = master->agents[i];

        if (!agent)
            continue;
        if (!agent->pid)
            continue;

        kill(agent->pid, SIGINT);
        usleep(10 * 1000);
    }

    for (i=0;i<master->devices;i++) {
        agent_t *agent = master->agents[i];

        if (!agent)
            continue;
        if (!agent->pid)
            continue;
        kill(agent->pid, SIGKILL);
        waitpid(agent->pid, NULL, 0);
        agent->alive = 0;
    }

    exit(0);

    return 0;
}

static int help_handler(master_info_t *master, char **argv)
{
    int i;

    printf("--------------usage-------------\n");
    for (i = 0; cmd_table[i].token != NULL; i++) {
        const cmd_entry_t *entry = &cmd_table[i];

        if (!entry->usage) continue;

        printf("%s : %s\n", entry->token, entry->usage);
    }

    return 0;
}

static const cmd_entry_t cmd_table[] = {
    {
        .token = "h",
        .handler = help_handler,
    },
    {
        .token = "help",
        .handler = help_handler,
    },
    {
        .token = "q",
        .handler = quit_handler,
        .usage = "quit mesh simulator",
    },
    {
        .token = "quit",
        .handler = quit_handler,
        .usage = "quit mesh simulator",
    },
    {
        .token = "attach",
        .handler = attach_handler,
        .usage = "attach [ip] [port]"
    },
    {
        .token = "channel",
        .handler = channel_handler,
        .usage = "channel [id] [chn]"
    },
    {
        .token = "log",
        .handler = filter_handler,
        .usage = "log [on/off] [id]"
    },
    {
        .token = "goto",
        .handler = goto_handler,
        .usage = "goto [device id | master]"
    },
    {
        .token = "isolate",
        .handler = isolate_handler,
        .usage = "isolate [id] - make device isolated from all other devices"
    },
    {
        .token = "ls",
        .handler = ls_handler,
        .usage = "list devices online"
    },
    {
        .token = "lossrate",
        .handler = lossrate_handler,
        .usage = "lossrate [percentage] : lossrate 90 -> 90\% loss rate"
    },
    {
        .token = "watch",
        .handler = watch_handler,
        .usage = "watch [ip] [port] : watch for new agent in specified network segment"
    },
    {
        .token = "send",
        .handler = send_handler,
        .usage = "send id1 cmd - send cmd to device"
    },
    {
        .token = "sendall",
        .handler = send_handler,
        .usage = "sendall cmd - send cmd to all alive devices"
    },
    {
        .token = "start",
        .handler = start_handler,
        .usage = "start [id1-[id2]] [id3-[id4]] - start new device with device id or id ranges"
    },
    {
        .token = "stop",
        .handler = stop_handler,
        .usage = "stop [id] - stop a device started by start"
    },
    {
        .token = "rssi",
        .handler = rssi_handler,
        .usage = "rssi [id1] [id2] [rssi1] [rssi2] - rssi(id1,id2)=rssi1 rssi(id2,id1)=rssi2"
    },
    {
        .token = "tty",
        .handler = tty_handler,
        .usage = "tty [/path/to/tty] - add device connected to tty"
    },
    {
        .token = NULL,
    },
};

static char ** strsplit(char *src, int max_fields)
{
    char *  token;
    char ** argv = calloc(max_fields, sizeof(char *));
    int     args = 0;

    for (token = strsep(&src, " "); token != NULL; token = strsep(&src, " ")) {
        if (strlen(token) == 0)
            continue;
        if (args >= max_fields) {
            printf("too many args!\n");
            break;
        }
        argv[args ++] = token;
    }
    return argv;
}

typedef struct {
    char key[32];
    char value[32];
} kv_pair_t;
static kv_pair_t kv_envs[32];
static int env_idx;

static void add_to_env(char *str)
{
    char *key = kv_envs[env_idx].key;
    char *value = kv_envs[env_idx].value;
    int i;

    env_idx ++;

    i = 0;
    while (*str != '=') {
        key[i++] = *str++;
    }
    key[i] = 0;

    i = 0;
    str ++;
    while (*str != 0) {
        value[i++] = *str++;
    }
    value[i] = 0;
}

static void parse_envs(char **argv)
{
    int i;
    int argc;

    /* clear old environment */
    env_idx = 0;

    for (argc=0;argv[argc];argc++);

    for (i=0;argv[i];i++) {
        int j;

        if (!strchr(argv[i], '='))
            continue;

        add_to_env(argv[i]);

        /* remove it */
        for(j=i;j<argc;j++) {
            argv[j] = argv[j+1];
        }

        argc --;
        i --;
    }
}

static const char *env_get_value(const char *key)
{
    kv_pair_t *kvp;
    int i;
    for (i=0;i<env_idx;i++) {
        kvp = kv_envs + i;
        if (strcmp(kvp->key, key))
            continue;

        return kvp->value;
    }

    return NULL;
}

static void dispatch_command(const char *line_org)
{
    char *line = strdup(line_org);
    char **argv = strsplit(line, 32);
    master_info_t *master = &master_info;
    int  i;

    if (!argv[0])
        goto out;

    parse_envs(argv);

    if (!cli_status.cur_id)
        goto master;

    if (!strcmp(argv[0], "goto")) {
        goto_handler(master, argv);
        goto out;
    }

    agent_t *agent = master->agents[ID2IDX(cli_status.cur_id)];
    if (!agent)
        goto out;

    send_agent_data(master, agent, TYPE_MISC, CMD_MISC_INPUT, line_org, strlen(line_org)+1);
    goto out;

master:
    for (i = 0; cmd_table[i].token != NULL; i++) {
        const cmd_entry_t *entry = &cmd_table[i];
        if (strcmp(entry->token, argv[0])) continue;

        int ret = entry->handler(master, argv);
        if (ret < 0)
            printf("wrong cmd or parameters!\n");
        goto out;
    }
    printf("wrong cmd!\n");

out:
    free(line);
    free(argv);
    return;
}

static int pipefd[2], pipefd_reply[2];
static void cli_run(void)
{
    while(1) {
        char *line = readline(cli_status.cmd_prefix);
        int ret;

        if (!line) {
            perror("pipe closed:");
            exit(0);
            continue;
        }
        if (strlen(line) > 0)
            add_history(line);
        ret = write(pipefd[1], &line, sizeof line);
        if (ret < 0) {
            perror("read from pipe");
        }

        /* a simple handshake */
        ret = read(pipefd_reply[0], &line, 1);
        if (ret < 0)
            perror("read from pipe");
    }
}

static void master_recv_line(int fd, void *d1, void *d2)
{
    char *line;
    int ret;

    if (sizeof(line) == read(fd, &line, sizeof line)) {
        dispatch_command(line);
        free(line);
    }

    ret = write(pipefd_reply[1], &fd, 1);
    if (ret < 0) {
        perror("read from pipe");
    }
}

static int ddm_init(dda_config_result_t *pcres)
{
    pthread_t d_thread;
    master_info_t *master = &master_info;
    int ret;

    ret = pipe2(pipefd, O_NONBLOCK);
    if (ret < 0) {
        goto out;
    }
    ret = pipe2(pipefd_reply, 0);
    if (ret < 0) {
        goto out;
    }

    ret = create_socket();
    if (ret < 0) {
        perror("create socket");
        goto out;
    }

    init_device_metrics(master, pcres);
    __DDA_DGRAPH_INIT__

    eloop_init(0);

    eloop_register_read_sock(pipefd[0], master_recv_line, 0, master);
    eloop_register_read_sock(master->sockfd, master_recv_data, 0, master);
    if (master->watch_addr.sin_port != 0)
        send_attach_timer(master, &master->watch_addr);

    pthread_create(&d_thread, NULL, (void * (*)(void *))cli_run, NULL);

    dgraph_reset();

    eloop_run();

    eloop_destroy();
out:
    return ret;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
static void *map_file(const char *fn, int *psz)
{
    int fd;
    struct stat buf;
    char *mem;

    fd = open(fn, O_RDONLY);
    fstat(fd, &buf);
    *psz = buf.st_size;
    mem = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    close(fd);

    return mem;
}

static void dda_parse_config_file(dda_config_result_t *cres, const char *fn)
{
    int sz;
    char *file_mem = map_file(fn, &sz);

    dda_parse_configs(cres, file_mem, sz);

    munmap(file_mem, sz);
}

int ddm_run(int argc, char **argv)
{
    dda_config_result_t cres;
    dda_parse_config_file(&cres, argv[argc-1]);

    master_info.self_exec = strdup(argv[0]);
    int i;
    for (i=0;i<argc;i++) {
        if (!strcmp(argv[i], "-s")) {
            master_info.log_off = 1;
            continue;
        }

        if (!strcmp(argv[i], "-w")) {
            inet_aton(argv[i+1], &master_info.watch_addr.sin_addr);
            master_info.watch_addr.sin_port = htons(DFL_PORT_AGENT);
            continue;
        }

        if (strcmp(argv[i], "-l"))
            continue;

        if (i+1 >= argc)
            break;
        master_info.log_file = fopen(argv[i+1], "w+");
    }
    return ddm_init(&cres);
}

static void dgraph_reset(void)
{
    __DDA_DGRAPH_CLEAR_DEVICE__();
}

static void dgraph_sync_agent(agent_t *agent)
{
#ifdef CONFIG_DDA_DGRAPH_ENABLE
    struct sockaddr_in *paddr;
    if_status_t *sif;

    paddr = (struct sockaddr_in *)&agent->addr;
    sif = agent->ifs + IF_WIFI;
   __DDA_DGRAPH_SYNC_DEVICE__(agent->agent_id,
                                agent->alive,
                                sif->mac_addr,
                                sif->channel,
                                inet_ntoa(paddr->sin_addr),
                                ntohs(paddr->sin_port),
                                sif->status.netid, sif->status.sid, agent->agent_id - 1);
#endif
}

static void dgraph_sync_msg(ipc_msg_t *msg)
{
#ifdef CONFIG_DDA_DGRAPH_ENABLE
    master_info_t *master = &master_info;
    agent_t *agent_src = get_agent_by_id(master, msg->src);
    agent_t *agent_dst = get_agent_by_id(master, msg->dst);
    if (!agent_src || !agent_dst)
        return;

    if_status_t *dif_src = agent_src->ifs + msg->cmd_priv.mesh.media_type;
    if_status_t *dif_dst = agent_dst->ifs + msg->cmd_priv.mesh.media_type;
    __DDA_DGRAPH_SYNC_MSG__(dif_src->status.sid,
                            dif_dst->status.sid,
                            dif_src->status.netid,
                            dif_dst->status.netid,
                            msg->type, msg->cmd,
                            (char *)(msg + 1), msg->payload_len);
#endif
}

