/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "config_parser.h"

#define DFL_RSSI 1

struct metric_configs {
    int idx;
};

struct config_status {
#define LINE_MODE        0
#define BLOCK_MODE_START 1
#define BLOCK_MODE       2
    int mode;
#define RSSI_METRICS 1
#define RSSI_TREE    2
#define RSSI_PAIRS   3
#define DEVICE_MACS  4
    int sub_mode;

    struct per_inf *cur_inf;
    union {
        struct metric_configs metric;
    };
    bool finished;
    bool interface_specified;

    dda_config_result_t result;
};

static int get_inf_type(const char *str)
{
    if (!strcmp(str, "wifi"))
        return IF_WIFI;
    else if (!strcmp(str, "ble"))
        return IF_BLE;
    else if (!strcmp(str, "154"))
        return IF_154;

    return -1;
}

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

static void dump_metrics(struct config_status *pstatus)
{
    int i, j;
    printf("-------------metrics(dumping max 20 nodes)----------\n");
    for (i=0;i<20&&i<pstatus->result.devices;i++) {
        char *metrics = pstatus->result.infs[0].metrics + i * pstatus->result.devices;
        for (j=0;j<20&&j<pstatus->result.devices;j++) {
            printf("%2d ", metrics[j]);
        }
        printf("\n");
    }
    printf("----------------------------------------------------\n");
}

#define set_metric(p, i, j, v) do { \
    *(p->cur_inf->metrics + i * p->result.devices + j) = v; \
} while(0)

static void set_one_rssi_metric(struct config_status *pstatus, char **layers, bool bidirection)
{
    if (!layers[0] || !layers[1] || !layers[2])
        return;

    int i = atoi(layers[0]);
    int j = atoi(layers[1]);
    int rssi = layers[2] ? atoi(layers[2]) : DFL_RSSI;
    set_metric(pstatus, i, j, rssi);

    if (!bidirection)
        return;
    set_metric(pstatus, j, i, rssi);
}

static void gen_metrics_from_rssi_tree(struct config_status *pstatus, char **layers)
{
    int idx = 0;
    int prev_node_num = 0;

    idx = atoi(*layers++);
    while (*layers) {
        int i,j;
        int node_num = atoi(*layers++);

        /* nodes in same layer are full-connected */
        for (i=idx;i<idx+node_num;i++) {
            for (j=idx;j<idx+node_num;j++) {
                set_metric(pstatus, i, j, DFL_RSSI);
            }
        }

        /* nodes in neighbor layer are full-connected */
        for (i=idx;i<idx+node_num;i++) {
            for (j=idx-prev_node_num;j<idx;j++) {
                set_metric(pstatus, i, j, DFL_RSSI);
                set_metric(pstatus, j, i, DFL_RSSI);
            }
        }

        idx += node_num;
        prev_node_num = node_num;
    }
}

static void parse_range(const char *str, int *ps, int *pe)
{
    char *endptr;

    *pe = -1;
    *ps = strtoul(str, &endptr, 0);
    if (endptr[0] == 0 || endptr[0] != '-')
        return;

    *pe = strtoul(endptr+1, &endptr, 0);
}

static void parse_interface(struct config_status *pstatus, char **fields)
{
    int idx, if_type = get_inf_type(fields[1]);
    if (if_type < 0)
        return;

    pstatus->interface_specified = true;
    pstatus->cur_inf = &pstatus->result.infs[if_type];

    idx = 2;
    while (fields[idx]) {
        int start, end;
        parse_range(fields[idx], &start, &end);
        end = end < 0 || end >= pstatus->result.devices ? pstatus->result.devices - 1 : end;
        while (start <= end) {
            pstatus->result.nodes[start].ifs |= 1 << if_type;
            start ++;
        }

        idx ++;
    }
}

static void alloc_metrics(struct config_status *pstatus)
{
    struct per_inf *pinf;
    int i, sz;

    sz = pstatus->result.devices * sizeof(struct node_config);
    pstatus->result.nodes = realloc(pstatus->result.nodes, sz);
    bzero(pstatus->result.nodes, sz);

    for (i=0;i<IF_NUM;i++) {
        pinf = &pstatus->result.infs[i];
        sz = pstatus->result.devices * pstatus->result.devices;
        pinf->metrics = realloc(pinf->metrics, sz);

        pinf->macs = realloc(pinf->macs, pstatus->result.devices * sizeof(uint64_t));
        bzero(pinf->metrics, sz);
        bzero(pinf->macs, pstatus->result.devices * sizeof(uint64_t));
    }
}

static void parse_one_line(struct config_status *pstatus, char *line)
{
    char **fields = strsplit(line, pstatus->result.devices);
    if (!fields[0])
        goto out;

    if (fields[0][0] == '#')
        goto out;

    if (pstatus->mode == LINE_MODE) {
        if (!strcmp(fields[0], "nodes:")) {
            pstatus->result.devices = atoi(fields[1]);

            alloc_metrics(pstatus);
        }
        else if (!strcmp(fields[0], "rssi-metrics:")) {
            pstatus->mode = BLOCK_MODE_START;
            pstatus->sub_mode = RSSI_METRICS;
        }
        else if (!strcmp(fields[0], "rssi-tree:")) {
            gen_metrics_from_rssi_tree(pstatus, fields + 1);
        }
        else if (!strcmp(fields[0], "rssi-pair:")) {
            set_one_rssi_metric(pstatus, fields + 1, true);
        }
        else if (!strcmp(fields[0], "rssi-pairs:")) {
            pstatus->mode = BLOCK_MODE_START;
            pstatus->sub_mode = RSSI_PAIRS;
        }
        else if (!strcmp(fields[0], "device-macs:")) {
            pstatus->mode = BLOCK_MODE_START;
            pstatus->sub_mode = DEVICE_MACS;
        }
        else if (!strcmp(fields[0], "finish")) {
            pstatus->finished = true;
        }
        else if (!strcmp(fields[0], "interface:")) {
            parse_interface(pstatus, fields);
        }
        else {
            goto err_out;
        }
    }
    else if (pstatus->mode == BLOCK_MODE_START) {
        if (!strcmp(fields[0], "start")) {
            pstatus->mode = BLOCK_MODE;
            bzero(&pstatus->metric, sizeof(pstatus->metric));
        }
        else
            goto err_out;
    }
    else if (pstatus->mode == BLOCK_MODE) {
        if (!strcmp(fields[0], "end")) {
            pstatus->mode = LINE_MODE;
        }
        else if (pstatus->sub_mode == RSSI_METRICS) {
            int idx = pstatus->metric.idx ++;
            char *dev_rssi = pstatus->cur_inf->metrics + idx * pstatus->result.devices;
            int i = 0;
            while (fields[i]) {
                dev_rssi[i] = atoi(fields[i]);
                i ++;
            }
        }
        else if (pstatus->sub_mode == RSSI_PAIRS) {
            set_one_rssi_metric(pstatus, fields, false);
        }
        else if (pstatus->sub_mode == DEVICE_MACS) {
            int idx = atoi(fields[0]);
            uint64_t mac_addr = strtoull(fields[1], NULL, 16);
            pstatus->cur_inf->macs[idx] = mac_addr;
        }
        else
            goto err_out;
    }
    goto out;
err_out:
    printf("unknow keyword - %s %s\n", fields[0], line);
out:
    free(fields);
    return;
}

void dda_parse_configs(dda_config_result_t *cres, const char *cdata, int sz)
{
    struct config_status cstatus = {
        .mode = LINE_MODE,
        .result.devices = 2,
        .cur_inf = cstatus.result.infs,
    };
    char *line = malloc(512);
    char *working_line = line;
    
    while (!cstatus.finished && sz--) {
        char c = *cdata++;
        if (c == '\n' || (working_line - line) >= 510) {
            *working_line = 0;
            parse_one_line(&cstatus, line);
            working_line = line;
            continue;
        }
        *working_line++ = c;
    }

    if (!cstatus.interface_specified) {
        printf("no interface information specified, assume all WIFI!\n");
        int i;
        for (i=0;i<cstatus.result.devices;i++)
            cstatus.result.nodes[i].ifs |= 1 << IF_WIFI;
    }

    memcpy(cres, &cstatus.result, sizeof(*cres));

    dump_metrics(&cstatus);

    free(line);
}

