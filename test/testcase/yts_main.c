/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#if defined (__GNUC__)
#include <sys/wait.h>
#endif

#include <aos/kernel.h>

#include "yunit.h"
#include "yts.h"

extern void add_test(void);

static int yts_argc;
static char **yts_argv;
int yts_get_args(const char ***argv)
{
    *argv = (const char **)yts_argv;
    return yts_argc;
}

void yts_run(int argc, char **argv)
{
    yts_argc = argc;
    yts_argv = argv;

    aos_msleep(2 * 1000);

    yunit_test_init();

    add_test();

    int ret = 0;

    if (argc > 1) {
        int i;
        for (i = 1; i < argc; i++) {
            yunit_test_suite_t *test_suite = yunit_get_test_suite(argv[i]);
            if (test_suite != NULL) {
                ret = yunit_run_test_suite(test_suite);
                printf("suite %s completed with %d\n", argv[i], ret);

                continue;
            }

            const char *suite_case = argv[i];
            char *test = strrchr(suite_case, ':');
            if (test != NULL) {
                *test++ = '\0';

                test_suite = yunit_get_test_suite(suite_case);
                yunit_test_case_t *test_case = yunit_get_test_case(test_suite, test);
                if (test_case != NULL) {
                    ret = yunit_run_test_case(test_suite, test_case);
                    printf("suite %s completed with %d\n", argv[i], ret);
                } else {
                    printf("test suite %s not found\n", argv[i]);
                }
            }
        }
    } else {
        ret = yunit_test_run();
        printf("\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

