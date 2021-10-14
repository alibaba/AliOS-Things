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
extern "C" {
#endif /* __cplusplus */

typedef struct {
    int32_t argc;
    int8_t **argv;
    bool is_repl_mode;
} mpy_thread_args;

int32_t mpy_run(int32_t argc, int8_t **argv);
int32_t mpy_init(mpy_thread_args *args);
int32_t mpy_deinit(void);

void haas_main(int32_t argc, int8_t **argv);

#ifdef __cplusplus
}
#endif

#endif /* PY_ENGINE_EXAMPLE_H */
