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

void rpc_event_arrived(int eid, void *val)
{
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)val;
    printf("%s, eid:%d\r\n", __func__, eid);
    if (eid != RPC_EID_STOP) {
        aos_rpc_close(param->srpc_handle);

        printf("%s\r\n", "rpc_event_arrived");
    }
}

void rpc_req_arrived(struct aos_parcel_t *pkt)
{
    int ret;
    char msg[128];

    size_t len;
    int reply_rpc_handle;

    len = sizeof(reply_rpc_handle);
    aos_parcel_get(AP_UVARINT, &reply_rpc_handle, &len, pkt);
    len = sizeof(msg);
    memset(msg, 0, sizeof(msg));
    aos_parcel_get(AP_BUF, msg, &len, pkt);
    // service works as printf
    printf("rpc_req_arrived recv from %p, msg is:%s\r\n", reply_rpc_handle, msg);

    if (reply_rpc_handle != 0) {
        ret = aos_rpc_resp(reply_rpc_handle, 1, len, msg);

        if (ret != RHINO_SUCCESS) {
            printf("%s reply %s fail(%d)\r\n", "rpc_req_arrived", msg, ret);
        }
        printf("rpc_req_arrived tx reply to %p\r\n", reply_rpc_handle);
    }
    printf("rpc_req_arrived recv from %p, exit\r\n", reply_rpc_handle);
}

static void rpc_server_coverage_benchmark(void *param)
{
    char name[32] = {0};
    char msg[128];
    size_t msg_sz, len;

    struct aos_parcel_t data;
    int ret, i, j;

    for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
        j = (i + 5) % BENCHMARK_RPC_FTST_LOOP_CNT + 1;
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name) - 1, BENCHMARK_RPC_SERVICE_NAME_FMT, j - 1);
        ret = aos_rpc_regist(name, BENCHMARK_RPC_REQ_SIZE, &srpc_handle[i - 1]);
        printf("regist service:%s, ret:%d\r\n", name, ret);
    }
    for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
        aos_rpc_close(srpc_handle[i - 1]);
    }

    for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
        j = (i + 7) % BENCHMARK_RPC_FTST_LOOP_CNT + 1;
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name) - 1, BENCHMARK_RPC_SERVICE_NAME_FMT, j - 1);
        printf("regist service:%s\r\n", name);
        aos_rpc_regist(name, BENCHMARK_RPC_REQ_SIZE, &srpc_handle[i - 1]);
    }
    for (i = BENCHMARK_RPC_FTST_LOOP_CNT; i > 0; i --) {
        ret = aos_rpc_run(srpc_handle[i - 1], rpc_req_arrived, rpc_event_arrived);
        printf("run rpc %d, ret:%d\r\n", i - 1, ret);
    }
}

void rpc_coverage_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t sthread;

    if (pthread_create(&sthread, NULL, rpc_server_coverage_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }
}

static void rpc_server_demo_benchmark(void *param)
{
    char *name = "alios.mk.rpc.demo0";
    int ret;

    ret = aos_rpc_regist(name, BENCHMARK_RPC_REQ_SIZE,
            &srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT]);
    printf("regist service:%s, ret:%d\r\n", name, ret);
    ret = aos_rpc_run(srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT],
            rpc_req_arrived, rpc_event_arrived);
    printf("run rpc %s, ret:%d\r\n", name, ret);
}

static void rpc_server_demo_resume_benchmark(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;
    ret = aos_rpc_run(srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT],
            rpc_req_arrived, rpc_event_arrived);
    printf("resume run rpc %p, ret:%d\r\n", srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT], ret);
}

static void rpc_server_demo_pend_benchmark(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;
    ret = aos_rpc_stop(srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT]);
    printf("stop run rpc %p, ret:%d\r\n", srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT], ret);
}

void rpc_demo_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t sthread;

    if (pthread_create(&sthread, NULL, rpc_server_demo_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }
}

static int server_service_sum(int a, int b)
{
    return a + b;
}

void service_sum_event_cb(int eid, void *val)
{
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)val;

    if (eid != RPC_EID_STOP) {
        aos_rpc_close(param->srpc_handle);
    }

    printf("%s, eid:%d\r\n", __func__, eid);
}

int rpc_service_sum_deserial(struct aos_parcel_t *pkt, int *reply_handle, int *a, int *b)
{
    size_t len = sizeof(int);
    aos_parcel_get(AP_UVARINT, reply_handle, &len, pkt);

    len = sizeof(*a);
    aos_parcel_get(AP_BUF, a, &len, pkt);
    len = sizeof(*b);
    aos_parcel_get(AP_BUF, b, &len, pkt);

    return RHINO_SUCCESS;
}

static int service_sum_srpc_handle;
void service_sum_req_arrived(struct aos_parcel_t *pkt)
{
    int crpc_handle;
    int a, b, sum;
    int ret = 0;

    /* Step-1: deserail parameters of rpc */
    rpc_service_sum_deserial(pkt, &crpc_handle, &a, &b);

    /* Step-2: Call service */
    sum = server_service_sum(a, b);
    printf("recv req from %p, a:%d, b:%d\r\n", crpc_handle, a, b);

    if (crpc_handle != 0) {
        /* Step-3: send response packet back (Optional) */
        ret = aos_rpc_resp(crpc_handle, 1, sizeof(sum), &sum);
        printf("send response back, ret:%d\r\n", ret);
    }
    aos_rpc_close(service_sum_srpc_handle);
}

static void rpc_server_sum_benchmark(void *param)
{
    const char *rpc_service_sum_name = "alios.mk.rpc.service.sum";
    size_t rpc_sum_req_buf_len = sizeof(int) * 2 * 32;
    int ret;

    ret = aos_rpc_regist(rpc_service_sum_name, rpc_sum_req_buf_len, &service_sum_srpc_handle);
    printf("rpc regist, ret:%d\r\n", ret);
    ret = aos_rpc_run(service_sum_srpc_handle, service_sum_req_arrived, service_sum_event_cb);
    printf("rpc run, ret:%d\r\n", ret);
}

void rpc_sum_benchmark_test(char *pbuffer, int outlen, int argc, char **argv)
{
    pthread_t sthread;

    if (pthread_create(&sthread, NULL, rpc_server_sum_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }
}

struct cli_command rpc_ucmds[] = {
    {"rpcsc", "Kernel space rpc converage testes", rpc_coverage_benchmark_test},
    {"rpcsd", "Kernel space rpc demo testes", rpc_demo_benchmark_test},
    {"rpcsr", "Kernel space rpc resume demo testes", rpc_server_demo_resume_benchmark},
    {"rpcsp", "Kernel space rpc resume demo testes", rpc_server_demo_pend_benchmark},
    {"rpcs_sum", "Kernel space rpc sum testes", rpc_sum_benchmark_test},
};

void rpc_benchmark_init()
{
    aos_cli_register_commands(rpc_ucmds, sizeof(rpc_ucmds)/sizeof(rpc_ucmds[0]));
}

#ifdef __cplusplus
}
#endif
