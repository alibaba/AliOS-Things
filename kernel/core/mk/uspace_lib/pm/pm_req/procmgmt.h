/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define PROC_NAME_MAX     100
#define PROC_SERVICE_NAME "alios.mk.rpc.service.proc"
#define PROC_REV_BUF      128

#define PROC_SERVICE_LOAD        0
#define PROC_SERVICE_UNLOAD      1

int aos_process_load(char *porcname);
int aos_process_unload(char *porcname);


