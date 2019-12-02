/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include "yunit/yts.h"

#include "aos/cli.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/kv.h"
#include "aos/yloop.h"

#include "netmgr.h"

static void handle_yts_cmd(char *pwbuf, int blen, int argc, char **argv);

#ifdef YTS_LINUX
#else
#if YTS_CONFIG_TEST_SUITE_DETACHED
#include "k_config.h"
#define ARGUMENT_SIZE 4
typedef struct {
    unsigned int test_case_index;
    int          argc;
    char       **argv;
} arg_para_t;

static arg_para_t arg_para_fifo[ARGUMENT_SIZE];

static unsigned char fifo_in  = 0;
static unsigned char fifo_out = 0;

static unsigned long test_case_index = 0;

#ifdef YTS_CONFIG_TASK_DEPTH
#define YTS_TASK_DEPTH YTS_CONFIG_TASK_DEPTH
#else /* !YTS_CONFIG_TASK_DEPTH */
#define YTS_TASK_DEPTH 8192
#endif /* YTS_CONFIG_TASK_DEPTH */
static aos_sem_t  yts_test_case_sem;
static aos_task_t  yts_routine_task;
static char        yts_routine_initd = 0;
static void yts_cmd_routine(void *para)
{
    while (1) {
        if (0 == aos_sem_wait(&yts_test_case_sem, AOS_WAIT_FOREVER)) {
            if(fifo_in != fifo_out) {
                arg_para_t arg_para = {arg_para_fifo[fifo_out].test_case_index, arg_para_fifo[fifo_out].argc, arg_para_fifo[fifo_out].argv};
                fifo_out = (fifo_out+1)%ARGUMENT_SIZE;
                LOGI("YTS", "start yts test(%d), argc: %d", arg_para.test_case_index, arg_para.argc);

                yts_run(arg_para.argc, arg_para.argv);
                LOGI("YTS", "YTS Running Test Case %d Over", arg_para.test_case_index);
            } else {
                LOGW("YTS", "No Test Case in CMD FIFO");
            }
        } else {
            /* should not entry here, unless fatal error happens, so here just for notice user */
            LOGF("YTS", "Fatal error wait cmd sem, may test case abort");
            aos_msleep(1000);
        }
    }
}

static int post_test_case(const int argc, char **argv)
{
    int rc = -1;
    if( ((fifo_in+1)%ARGUMENT_SIZE) == fifo_out ) {
        LOGW("YTS", "Test Case Cmd FIFO is Full, wait test case finished then try");
    } else{
        arg_para_fifo[fifo_in].test_case_index = test_case_index++;
        arg_para_fifo[fifo_in].argc = argc;
        arg_para_fifo[fifo_in].argv = argv;
        fifo_in = (fifo_in+1)%ARGUMENT_SIZE;
        aos_sem_signal(&yts_test_case_sem);
        rc = 0;
    }
    return rc;
}
#endif /* YTS_CONFIG_TEST_SUITE_DETACHED */
static struct cli_command ncmd = {"yts_run", "running yts testcase", handle_yts_cmd};
#endif

static void handle_yts_cmd(char *pwbuf, int blen, int argc, char **argv)
{
#if YTS_LINUX
    LOG("Starting customer yts test...");
	yts_run(argc, argv);
#else
    LOG("Starting customer yts test...");
#if YTS_CONFIG_TEST_SUITE_DETACHED
    LOG("yts test suite is running detached");
    if(0 == yts_routine_initd){
        LOG("Starting customer yts test routine...");
        if (0 == aos_sem_new(&yts_test_case_sem, 0)) {
            if (0 == aos_task_new_ext(&yts_routine_task, "yts", yts_cmd_routine,
                NULL, YTS_TASK_DEPTH,
                RHINO_CONFIG_USER_PRI_MAX - 2)) {
                yts_routine_initd = 1;
            } else {
                aos_sem_free(&yts_test_case_sem);
                LOGE("YTS", "Init YTS Task fail");
            }
        } else {
            LOGE("YTS", "Init Semphore fail");
        }
    }

    if(1 == yts_routine_initd){
        LOGI("YTS", "Add yts test(argc %d) into test queue", argc);
        post_test_case(argc, argv);
    } else {
        LOGE("YTS", "yts routine start fail");
    }
#else /* !YTS_CONFIG_TEST_SUITE_DETACHED */
    yts_run(argc, argv);
#endif /* YTS_CONFIG_TEST_SUITE_DETACHED */
#endif
}

#if YTS_CONFIG_NET_CONNECT
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi service start, ssid %s, board has connected to router", config.ssid);
}
#endif /* YTS_CONFIG_NET_CONNECT */

int application_start(int argc, char **argv)
{
#ifdef YTS_LINUX
    handle_yts_cmd(NULL, 0, argc, argv);
    aos_kv_deinit();
    exit(0);
#else
#if YTS_CONFIG_NET_CONNECT
    aos_set_log_level(AOS_LL_INFO);
    netmgr_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);
#endif /* YTS_CONFIG_NET_CONNECT */

    aos_cli_register_command(&ncmd);
    LOG("Please enter command via CLI: \r\n");
    aos_loop_run();
    return 0;
#endif

}

