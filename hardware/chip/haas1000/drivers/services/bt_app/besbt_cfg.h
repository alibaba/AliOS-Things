/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/***
 * besbt_cfg.h
 */

#ifndef BESBT_CFG_H
#define BESBT_CFG_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct besbt_cfg_t{
    bool sniff;
    bool one_bring_two;
    bool source_enable;
    bool ble_mesh_enable;
};
extern struct besbt_cfg_t besbt_cfg;

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
