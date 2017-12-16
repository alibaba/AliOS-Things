/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _DDM_UTIL_H_
#define _DDM_UTIL_H_

#include <aos/kernel.h>
#include <umesh.h>
#include "config_parser.h"
#include "yts.h"

#ifndef DDA_DEBUG
#define DDA_DEBUG 0
#endif

extern void ur_ut_send_cmd_to_ddm(const char *cmd);

static void cmd_to_master(char *cmd)
{
    ur_ut_send_cmd_to_ddm("goto master");
    ur_ut_send_cmd_to_ddm(cmd);
}

static void cmd_to_agent(char *cmd)
{
    umesh_cli_cmd(cmd, strlen(cmd), NULL, NULL);
}

static void set_rssi_ext(int if_idx, int i, int j, int rssi1, int rssi2)
{
    char rssi_cmd[64];
    if (if_idx >= 0)
        sprintf(rssi_cmd, "if=%s rssi %d %d %d %d",
                if_idx2name(if_idx), i, j, rssi1, rssi2);
    else
        sprintf(rssi_cmd, "rssi %d %d %d %d", i, j, rssi1, rssi2);
    cmd_to_master(rssi_cmd);
}

static void set_rssi(int i, int j, int rssi)
{
    set_rssi_ext(-1, i, j, rssi, rssi);
}

static void set_line_rssi_ext(int if_idx, int start, int end)
{
    int i;
    while (start < end) {
        set_rssi(start, start+1, 1);
        for (i=start+2;i<=end;i++) {
            set_rssi_ext(if_idx, start, i, 0, 0);
        }

        start ++;
    }
}

static void set_line_rssi(int start, int end)
{
    set_line_rssi_ext(IF_WIFI, start, end);
    set_line_rssi_ext(IF_BLE, start, end);
}

static void set_full_rssi_ext(int if_idx, int start, int end)
{
    int i,j;
    for (i=start;i<end;i++) {
        for(j=start;j<=end;j++) {
            if (i == j)
                continue;
            set_rssi_ext(if_idx, i, j, 1, 1);
        }
    }
}

static void set_full_rssi(int start, int end)
{
    set_full_rssi_ext(IF_WIFI, start, end);
    set_full_rssi_ext(IF_BLE, start, end);
}

static void clear_full_rssi_ext(int if_idx, int start, int end)
{
    int i,j;
    for (i=start;i<end;i++) {
        for(j=start;j<=end;j++) {
            if (i == j)
                continue;
            set_rssi_ext(if_idx, i, j, 0, 0);
        }
    }
}

static void clear_full_rssi(int start, int end)
{
    clear_full_rssi_ext(IF_WIFI, start, end);
    clear_full_rssi_ext(IF_BLE, start, end);
}

static void start_node(int id)
{
    char cmd[32];
    sprintf(cmd, "start %d", id);
    cmd_to_master(cmd);
}

static void start_node_ext(int id, int mode, int router, int ifs)
{
    char tmp[16], cmd[64];
    sprintf(cmd, "start %d", id);
    if (mode >= 0) {
        sprintf(tmp, " mode=%d", mode);
        strcat(cmd, tmp);
    }
    if (router >= 0) {
        sprintf(tmp, " router=%d", router);
        strcat(cmd, tmp);
    }
    if (ifs >= 0) {
        sprintf(tmp, " ifs=%d", ifs);
        strcat(cmd, tmp);
    }
    cmd_to_master(cmd);
}

static void stop_node(int id)
{
    char cmd[32];
    sprintf(cmd, "stop %d", id);
    cmd_to_master(cmd);
}

inline static void __dummy__(void)
{
    cmd_to_agent(NULL);
    set_line_rssi(0, 0);
    set_full_rssi(0, 0);
    start_node(0);
    start_node_ext(0, -1, -1, -1);
    stop_node(0);
    clear_full_rssi(0, 0);
}

typedef void (*dda_p2p_cb)(const char *, int, void *);
void dda_p2p_request(int dst_id, const char *cmd, dda_p2p_cb cb, void *cb_data);
void dda_p2p_remove_request(dda_p2p_cb cb, void *cb_data);

struct wait_cb_data {
    aos_sem_t sem;
    char *buf;
};

static void dda_p2p_wait_cb(const char *buf,  int len, void *cb_data)
{
    struct wait_cb_data *pdata = cb_data;
    int sz = strlen(buf) + 1;
    pdata->buf = aos_malloc(sz);
    memcpy(pdata->buf, buf, sz);
    aos_sem_signal(&pdata->sem);
}

inline static char *dda_p2p_req_and_wait(int dst_id, const char *cmd, int max_wait_second)
{
    struct wait_cb_data wait_data = {};

    aos_sem_new(&wait_data.sem, 0);
    dda_p2p_request(dst_id, cmd, dda_p2p_wait_cb, &wait_data);

    aos_sem_wait(&wait_data.sem, max_wait_second * 1000);

    dda_p2p_remove_request(dda_p2p_wait_cb, &wait_data);

    aos_sem_free(&wait_data.sem);

    return wait_data.buf;
}

/**
 * ret: expected return
 * did: agent id
 * cmd: command to agent
 * sec: max waiting seconds
 */
#define check_p2p_str_wait(ret, did, cmd, sec) do { \
    int i; \
    for (i=0;i<sec;i++) { \
        char *str; \
        aos_msleep(1000); \
        str = dda_p2p_req_and_wait(did, cmd, 1); \
        if ((i==(sec-1) || DDA_DEBUG) && str) printf("%d %s -> %s\n", did, cmd, str); \
        if (str && strcmp(ret, str) == 0) { aos_free(str);break;} \
        if (str) { aos_free(str);} \
    } \
    YUNIT_ASSERT(i<sec); \
} while(0)
#endif
