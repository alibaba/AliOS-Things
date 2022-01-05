/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

void watchdog_feeddog(void);
void watchdog_stopfeed(void);
void watchdog_feeddog_user(void);

#endif
