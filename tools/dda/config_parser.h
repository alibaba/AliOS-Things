/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

enum {
    IF_WIFI = 0,
    IF_BLE,
    IF_154,
    IF_NUM
};

static inline int if_name2idx(const char *name)
{
    if (!name)
        goto out;
    if (strcmp(name, "wifi") == 0)
        return IF_WIFI;
    if (strcmp(name, "ble") == 0)
        return IF_BLE;
    if (strcmp(name, "154") == 0)
        return IF_154;
out:
    return IF_WIFI;
}

static inline const char *if_idx2name(int idx)
{
    if (idx == IF_WIFI) return "wifi";
    if (idx == IF_BLE) return "ble";
    if (idx == IF_154) return "154";
    return "unknown";
}

struct per_inf {
    char *metrics;
    uint64_t *macs;
};

struct node_config {
    int ifs;
};

typedef struct dda_config_result_s {
    int devices;
    struct node_config *nodes;
    struct per_inf infs[IF_NUM];
} dda_config_result_t;

void dda_parse_configs(dda_config_result_t *cres, const char *cdata, int sz);
#endif
