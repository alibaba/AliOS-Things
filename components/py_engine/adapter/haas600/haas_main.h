/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides example entry.
*/

#ifndef PY_ENGINE_EXAMPLE_H
#define PY_ENGINE_EXAMPLE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct {
    int argc;
    char **argv;
    bool is_bootup;
    bool board_cli_support;
} mpy_thread_args;

int mpy_run(int argc, char **argv);
int mpy_init(mpy_thread_args *args);
int mpy_deinit(void);

void haas_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* PY_ENGINE_EXAMPLE_H */
