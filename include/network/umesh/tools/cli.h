/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_CLI_H
#define UR_CLI_H

#include "umesh_types.h"

enum {
    CMD_LINE_SIZE = 128,
};

typedef struct command_s {
    const char *name;
    void (*function)(int argc, char *argv[]);
} cli_command_t;

extern int g_cli_silent;
ur_error_t mesh_cli_init(void);

#endif  /* UR_CLI_H */
