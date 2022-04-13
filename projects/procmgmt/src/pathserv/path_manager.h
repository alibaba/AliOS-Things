/*
* Copyright (C) 2020-2021 Alibaba Group Holding Limited
*/

#define PATH_MANAGER_RPC_NAME  "vfs.path.manager"

#define REGISTER_RPC_REQ_SIZE       1024

#define NAME_LENGTH 256
#define NAME_MAX_NUM 1000

enum {
    DRIVER_RPC_REGISTER = 0,
    DRIVER_RPC_UNREGISTER = 1,
    GET_MAIN_RPC_NAME = 2,
};

int  path_manager_server_setup(void);