/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ARG_OPTIONS_H
#define ARG_OPTIONS_H

#include <stdbool.h>

typedef struct {
    int    argc;
    char **argv;
#ifdef TFS_EMULATE
    int    id2_index;
#endif
    int    log_level;

    struct {
        bool enable;
        bool tapif;
    } lwip;

    struct {
        bool enable;
    } mesh;

    struct {
        bool enable;
    } dda;

    struct {
        bool enable;
    } ddm;

    struct {
        bool enable;
    } alink;

    struct {
        bool per_pid;
    } flash;

    struct {
        bool enable;
    } cli;
} options_t;

void parse_options(options_t *config);

#endif

