/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_MANAGE_H
#define UND_MANAGE_H

#ifdef __cplusplus
extern "C" {
#endif

int und_cap_manage_init();

int und_cap_manage_deinit();

int und_update_cap(int cap_idx, int reason_code, int wb, int sync);

int und_collect_package(char *buf, int buf_len, int insert_split, int with_exception);

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_MANAGE_H */
