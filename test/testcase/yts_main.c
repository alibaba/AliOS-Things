/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <aos/kernel.h>

#include "yunit.h"
#include "yts.h"

extern void test_basic(void);
extern void test_fota(void);
extern void test_rhino(void);
extern void test_rhino_port(void);
extern void test_csp(void);
extern void test_vfs(void);
extern void test_crypto(void);
extern void test_time(void);
extern void test_wifi(void);
extern void test_audio(void);
extern void test_mesh(void);
extern void test_netconf(void);
extern void test_uradar(void);
extern void test_yloop(void);
extern void test_cjson(void);
extern void test_hashtable(void);
extern void test_kv(void);
extern void test_hal(void);
extern void test_tls(void);
extern void test_tfs(void);
extern void test_aosapi(void);
extern void test_vfs(void);
extern void test_vcall(void);
extern void test_netmgr(void);
extern void test_digest_algorithm(void);
extern void test_alicrypto(void);
extern void test_fatfs(void);
extern void test_mqtt(void);
extern void test_coap(void);

static void add_test(void);

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

void add_test(void)
{
#ifdef YTS_COAP
    test_coap();
#else
    test_mqtt();

    test_basic();

    test_fota();

    test_yloop();

    test_hal();

    test_cjson();

    test_hashtable();

    test_digest_algorithm();

    test_vcall();

    test_rhino();

    test_rhino_port();

    test_tls();

    test_tfs();

    test_vfs();

    test_fatfs();

#ifdef CONFIG_AOS_MESH
    test_uradar();
#endif

    test_netmgr();

    test_alicrypto();

    test_kv();

    test_aosapi();

#endif
}

