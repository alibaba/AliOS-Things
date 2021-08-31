/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_MGR_H
#define APP_MGR_H

#include <stdio.h>
#include <string.h>
// #include <unistd.h>
void pyamp_apppack_init();

int pyapp_download_and_upgrade(char *url);

int pyamp_apppack_upgrade(char *url);

#endif /* APP_MGR_H */
