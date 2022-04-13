/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_REPORT_H
#define UND_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif

int und_report_init();

int und_report_deinit();

int und_update_report(int cap_idx);

int und_update_report_cycle(int fast);

void und_targets_report(void *param);

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_REPORT_H */
