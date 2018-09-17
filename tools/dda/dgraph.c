/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cJSON.h>
#include <netdb.h>
#include <sys/socket.h>
#ifdef CSP_LINUXHOST
#include <arpa/inet.h>
#endif

#define DG_TYPE_RSSIS 'r'
#define DG_TYPE_AGENT 'a'
#define DG_TYPE_DEVICE 'd'
#define DG_TYPE_MSG 'm'
#define DG_TYPE_DEVICE_CLEAR 'c'

#define DG_SOCKET_IP "127.0.0.1"
#define DG_SOCKET_PORT 26900

#define TAG "dgraph"

static int s_sockfd;
static struct sockaddr_in s_addr;

static int dgraph_send_udp(const char *buf, unsigned int len)
{
    return sendto(s_sockfd, buf, len, 0, (struct sockaddr *)(&s_addr), sizeof(s_addr));
}

int dgraph_sync_agent( uint64_t agent_id,
                                uint64_t mac_addr,
                                int channel,
                                int log_off,
                                int pid,
                                int alive, unsigned int index)
{
    cJSON *agent_obj = cJSON_CreateObject();
    char *in_buf = NULL;
    char *in_json = NULL;
    unsigned int in_j_len = 0;
    int ret;

    /* Laogong warning! Here agent_id is 64bits,
        needs to accurately measure and be asure of if cJSON supports 64 int*/
    cJSON_AddItemToObject(agent_obj, "index", cJSON_CreateNumber(index));
    cJSON_AddItemToObject(agent_obj, "agent_id", cJSON_CreateNumber(agent_id));
    cJSON_AddItemToObject(agent_obj, "mac_addr", cJSON_CreateNumber(mac_addr));
    cJSON_AddItemToObject(agent_obj, "channel", cJSON_CreateNumber(channel));
    cJSON_AddItemToObject(agent_obj, "log_off", cJSON_CreateNumber(log_off));
    cJSON_AddItemToObject(agent_obj, "pid", cJSON_CreateNumber(pid));
    cJSON_AddItemToObject(agent_obj, "alive", cJSON_CreateNumber(alive));

    in_json = cJSON_Print(agent_obj);
    in_j_len = strlen(in_json);

    in_buf = malloc(in_j_len + 1);
    in_buf[0] = DG_TYPE_AGENT;
    memcpy(in_buf + 1, in_json, in_j_len);

    ret = dgraph_send_udp(in_buf, in_j_len + 1);

    free(in_json);
    free(in_buf);
    cJSON_Delete(agent_obj);

    return ret;
}

int dgraph_clear_device()
{
    char in_buf[2] = {0};
    int ret;

    in_buf[0] = DG_TYPE_DEVICE_CLEAR;
    ret = dgraph_send_udp(in_buf, 2);
    return ret;
}

int dgraph_sync_device ( uint64_t agent_id,
                         int alive,
                         uint64_t mac_addr,
                         int channel,
                         const char *ip_addr,
                         unsigned int port,
                         unsigned int netid,
                         unsigned int sid,
                         unsigned int index )
{
    cJSON *agent_obj = cJSON_CreateObject();
    char *in_buf = NULL;
    char *in_json = NULL;
    unsigned int in_j_len = 0;
    int ret;

    /* Laogong warning! Here agent_id is 64bits,
        needs to accurately measure and be asure of if cJSON supports 64 int*/
    cJSON_AddItemToObject(agent_obj, "index", cJSON_CreateNumber(index));
    cJSON_AddItemToObject(agent_obj, "agent_id", cJSON_CreateNumber(agent_id));
    cJSON_AddItemToObject(agent_obj, "alive", cJSON_CreateNumber(alive));
    cJSON_AddItemToObject(agent_obj, "mac_addr", cJSON_CreateNumber(mac_addr));
    cJSON_AddItemToObject(agent_obj, "channel", cJSON_CreateNumber(channel));
    cJSON_AddItemToObject(agent_obj, "ip_addr", cJSON_CreateString(ip_addr));
    cJSON_AddItemToObject(agent_obj, "port", cJSON_CreateNumber(port));
    cJSON_AddItemToObject(agent_obj, "netid", cJSON_CreateNumber(netid));
    cJSON_AddItemToObject(agent_obj, "sid", cJSON_CreateNumber(sid));
    cJSON_AddItemToObject(agent_obj, "index", cJSON_CreateNumber(index));

    in_json = cJSON_Print(agent_obj);
    in_j_len = strlen(in_json);

    in_buf = malloc(in_j_len + 1);
    in_buf[0] = DG_TYPE_DEVICE;
    memcpy(in_buf + 1, in_json, in_j_len);

    ret = dgraph_send_udp(in_buf, in_j_len + 1);

    free(in_json);
    free(in_buf);
    cJSON_Delete(agent_obj);

    return ret;
}


int dgraph_sync_rssis(const char *rssi_metrics, unsigned int device_num)
{
    char *in_json = NULL;
    int in_j_len = 0;
    char *in_buf = NULL;
    int ret;

    cJSON *rssi_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(rssi_obj, "number", cJSON_CreateNumber(device_num));
    cJSON_AddItemToObject(rssi_obj, "rssi_metrics", cJSON_CreateString(rssi_metrics));
    in_json = cJSON_Print(rssi_obj);
    in_j_len = strlen(in_json);

    in_buf = malloc(in_j_len + 1);
    in_buf[0] = DG_TYPE_RSSIS;
    memcpy(in_buf + 1, in_json, in_j_len);

    ret = dgraph_send_udp(in_buf, in_j_len + 1);

    free(in_json);
    free(in_buf);
    cJSON_Delete(rssi_obj);

    return ret;
}

int dgraph_append_behavior_index(uint64_t index_from,
                                 uint64_t index_to,
                                 uint64_t netid_from,
                                 uint64_t netid_to,
                                 uint16_t type,
                                 uint16_t cmd,
                                 const char *msg, unsigned int len)
{
    char *in_json = NULL;
    int in_j_len = 0;
    char *in_buf = NULL;
    int ret = -1;

    cJSON *msg_obj = cJSON_CreateObject();

    /* laogong warning here, uint64_t convert to Number may loss accuracy */
    cJSON_AddItemToObject(msg_obj, "from", cJSON_CreateNumber(index_from));
    cJSON_AddItemToObject(msg_obj, "to", cJSON_CreateNumber(index_to));
    cJSON_AddItemToObject(msg_obj, "netid_from", cJSON_CreateNumber(netid_from));
    cJSON_AddItemToObject(msg_obj, "netid_to", cJSON_CreateNumber(netid_to));
    cJSON_AddItemToObject(msg_obj, "type", cJSON_CreateNumber(type));
    cJSON_AddItemToObject(msg_obj, "cmd", cJSON_CreateNumber(cmd));

    if(msg != NULL && len > 0)
        cJSON_AddItemToObject(msg_obj, "msg", cJSON_CreateString(msg));

    in_json = cJSON_Print(msg_obj);
    in_j_len = strlen(in_json);

    in_buf = malloc(in_j_len + 1);
    in_buf[0] = DG_TYPE_MSG;
    memcpy(in_buf + 1, in_json, in_j_len);

    ret = dgraph_send_udp(in_buf, in_j_len + 1);

    free(in_json);
    free(in_buf);
    cJSON_Delete(msg_obj);
    return ret;
}

int dgraph_create_udp_socket()
{
    int ret = -1;

    s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_sockfd < 0)
        goto out;

    setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    setsockopt(s_sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int));

    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(DG_SOCKET_PORT);

    if (inet_aton(DG_SOCKET_IP, &(s_addr.sin_addr)) == 0) {
        goto out1;
    }
    return ret;
out1:
    close(s_sockfd);
out:
    return ret;
}
