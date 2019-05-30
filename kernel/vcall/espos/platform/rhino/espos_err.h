/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ESPOS_ERR_MAP_H
#define ESPOS_ERR_MAP_H

#define IRAM_ATTR __attribute__((section(".text")))

int espos_err_map (kstat_t err_code);

#endif

