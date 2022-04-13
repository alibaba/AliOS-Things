/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

#include "k_api.h"
#include "rpc.h"
#include "aos/cli.h"

#define BENCHMARK_RPC_SERVICE_NAME_FMT    "com.rpc.service.%02d"
#define BENCHMARK_RPC_FTST_LOOP_CNT       (2)
#define BENCHMARK_RPC_REQ_SIZE            (512)
#define BENCHMARK_RPC_RSP_SIZE            (512)

#ifdef __cplusplus
extern "C" {
#endif

int srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT + 1];

static void rpc_client_coverage_benchmark(void *param)
{
    char name[32] = {0};
    char msg[128];

    struct aos_parcel_t reply;

    int crpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT];

    int ret, i = 0, j;
    size_t len;

    memset(msg, 0, sizeof(msg));

    memset(crpc_handle, 0, sizeof(crpc_handle));

    while (1) {
        for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
            j = (i + 7) % BENCHMARK_RPC_FTST_LOOP_CNT + 1;
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name) - 1, BENCHMARK_RPC_SERVICE_NAME_FMT, j - 1);
            if (crpc_handle[i - 1] == 0) {
                printf("open service:%s\r\n", name);
                ret = aos_rpc_open_sync(name, BENCHMARK_RPC_RSP_SIZE, &crpc_handle[i - 1]);
                if (RHINO_SUCCESS != ret) {
                    printf("open rpc:%s fail(%d)\r\n", name, ret);
                    break;
                }
            }
            printf("call service:%s\r\n", name);

            ret = aos_rpc_call_sync(crpc_handle[i - 1], &reply, 1, strlen(name), name);
            if (RHINO_SUCCESS != ret) {
                printf("call rpc:%s fail(%d)\r\n", name, ret);
                break;
            }
            len = sizeof(msg);
            memset(msg, 0, sizeof(msg));
            aos_parcel_get(AP_BUF, msg, &len, &reply);
            printf("rpc reply, msg:%s\r\n", msg);
            //aos_msleep(500);
        }
        if (ret != RHINO_SUCCESS) break;
    }

    for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
        aos_rpc_close(crpc_handle[i - 1]);
    }
}

void rpc_coverage_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t cthread;

    if (pthread_create(&cthread, NULL, rpc_client_coverage_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        return;
    }
}

static void rpc_client_demo_benchmark(void *param)
{
    char *name = "alios.mk.rpc.demo0";
    char *async_msg = "alios.mk.rpc.demo0.asyn";
    char msg[128];

    struct aos_parcel_t reply;

    int crpc_handle = 0;

    int ret;
    size_t len;

    while (1) {
        if (crpc_handle == 0) {  // init rpc client handle
            printf("open service:%s\r\n", name);
            ret = aos_rpc_open_sync(name, BENCHMARK_RPC_RSP_SIZE, &crpc_handle);
            if (RHINO_SUCCESS != ret) {
                printf("open rpc:%s fail(%d)\r\n", name, ret);
                break;
            }
        }

        printf("call service:%s\r\n", name);
        ret = aos_rpc_call_sync(crpc_handle, &reply, 1, strlen(name), name);
        if (RHINO_SUCCESS != ret) {
            printf("call rpc:%s fail(%d)\r\n", name, ret);
            break;
        }

        /* parse return value */
        len = sizeof(msg);
        memset(msg, 0, sizeof(msg));
        aos_parcel_get(AP_BUF, msg, &len, &reply);
        printf("rpc reply, msg:%s\r\n", msg);
        //aos_msleep(500);
        aos_rpc_call_async(crpc_handle, 1, strlen(async_msg), async_msg);
    }
    aos_rpc_close(crpc_handle);
}

void rpc_demo_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t cthread;

    if (pthread_create(&cthread, NULL, rpc_client_demo_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        return;
    }
}

static void rpc_client_sum_benchmark(void *param)
{
    const char *rpc_service_sum_name = "alios.mk.rpc.service.sum";
    size_t rpc_sum_resp_buf_len = 16;
    int crpc_handle;
    size_t len;
    int sum;
    int ret;

    int a = 1, b = 2;

    struct aos_parcel_t reply;

    ret = aos_rpc_open_sync(rpc_service_sum_name, rpc_sum_resp_buf_len, &crpc_handle);
    printf("rpc open, ret:%d\r\n", ret);

    ret = aos_rpc_call_sync(crpc_handle, &reply, 2, sizeof(a), &a, sizeof(b), &b);
    printf("rpc call, ret:%d\r\n", ret);

    /* parse response */
    len = sizeof(sum);
    aos_parcel_get(AP_BUF, &sum, &len, &reply);

    printf("rpc sum is %d\r\n", sum);
    aos_rpc_close(crpc_handle);
}

void rpc_sum_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t cthread;

    if (pthread_create(&cthread, NULL, rpc_client_sum_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        return;
    }
}

struct cli_command rpc_ucmds[] = {
    {"rpccc", "Kernel space rpc converage testes", rpc_coverage_benchmark_test},
    {"rpccd", "Kernel space rpc demo testes", rpc_demo_benchmark_test},
    {"rpcc_sum", "Kernel space rpc sum testes", rpc_sum_benchmark_test},
};

void rpc_benchmark_init()
{
    aos_cli_register_commands(rpc_ucmds, sizeof(rpc_ucmds)/sizeof(rpc_ucmds[0]));
}

#ifdef __cplusplus
}
#endif
