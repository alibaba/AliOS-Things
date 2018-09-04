/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <arg_options.h>
#include <aos/aos.h>

static void shift_argv(options_t *options, int i)
{
    options->argc --;
    while (i < options->argc) {
        options->argv[i] = options->argv[i+1];
        i ++;
    }
}

void parse_options(options_t *options)
{
    char      **argv = options->argv;
    int         i;

    for (i = 0; i < options->argc;) {
        if (!strcmp(argv[i], "--mesh-master")) {
            options->cli.enable = false;
            i ++;
            continue;
        }

        if (!strcmp(argv[i], "--no-cli")) {
            options->cli.enable = false;
            shift_argv(options, i);
            continue;
        }

        if (!strcmp(argv[i], "--per-pid-flash")) {
            options->flash.per_pid = true;
            shift_argv(options, i);
            continue;
        }

        if (!strcmp(argv[i], "--no-per-pid-flash")) {
            options->flash.per_pid = false;
            shift_argv(options, i);
            continue;
        }

        if (!strcmp(argv[i], "--tapif")) {
            options->lwip.tapif = true;
            shift_argv(options, i);
            continue;
        }

        if (!strcmp(argv[i], "--no-tapif")) {
            options->lwip.tapif = false;
            shift_argv(options, i);
            continue;
        }

        if (!strcmp(argv[i], "--log")) {
            shift_argv(options, i);
            if (i >= options->argc) {
                options->log_level = AOS_LL_DEBUG;
                continue;
            }

            const char *ll = argv[i];
            if (strcmp(ll, "fatal") == 0)
                options->log_level = AOS_LL_FATAL;
            else if (strcmp(ll, "error") == 0)
                options->log_level = AOS_LL_ERROR;
            else if (strcmp(ll, "warn") == 0)
                options->log_level = AOS_LL_WARN;
            else if (strcmp(ll, "info") == 0)
                options->log_level = AOS_LL_INFO;
            else if (strcmp(ll, "debug") == 0)
                options->log_level = AOS_LL_DEBUG;
            shift_argv(options, i);
            continue;
        }

        if (strcmp(argv[i], "-i")) {
            i ++;
            continue;
        }

        if (i + 1 >= options->argc) {
            i ++;
            continue;
        }

#ifdef TFS_EMULATE
        options->id2_index = atoi(argv[i + 1]);
#endif
        shift_argv(options, i);
        shift_argv(options, i);
        break;
    }
}

