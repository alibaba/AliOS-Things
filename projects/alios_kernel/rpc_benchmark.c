/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

#include "rpc.h"
#include "aos/cli.h"

#define BENCHMARK_RPC_SERVICE_NAME_FMT    "com.rpc.service.%02d"
#define BENCHMARK_RPC_FTST_LOOP_CNT       (2)
#define BENCHMARK_RPC_REQ_SIZE            (512)
#define BENCHMARK_RPC_RSP_SIZE            (512)

#ifdef __cplusplus
extern "C" {
#endif

ksem_t* rpc_sem[2];
int srpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT + 1];

void rpc_event_arrived(int eid, void *val)
{
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)val;
    printf("%s, eid:%d\r\n", __func__, eid);
    if (eid != RPC_EID_STOP) {
        aos_rpc_close(param->srpc_handle);

        printf("%s\r\n", "rpc_event_arrived");

        if (rpc_sem[0] != NULL) {
            krhino_sem_dyn_del(rpc_sem[0]);
            rpc_sem[0] = NULL;
        }
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

    krhino_sem_give(rpc_sem[0]);
}

static void rpc_client_coverage_benchmark(void *param)
{
    char name[32] = {0};
    char msg[128];

    struct aos_parcel_t reply;

    int crpc_handle[BENCHMARK_RPC_FTST_LOOP_CNT];

    int ret, i = 0, j;
    size_t len;

    memset(msg, 0, sizeof(msg));

    krhino_sem_take(rpc_sem[0], RHINO_WAIT_FOREVER);
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
    pthread_t cthread, sthread;

    krhino_sem_dyn_create(&rpc_sem[0], __func__, 0);

    if (pthread_create(&sthread, NULL, rpc_server_coverage_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }

    if (pthread_create(&cthread, NULL, rpc_client_coverage_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        pthread_kill(sthread, 0);
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

    krhino_sem_give(rpc_sem[1]);
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

static void rpc_client_demo_benchmark(void *param)
{
    char *name = "alios.mk.rpc.demo0";
    char *async_msg = "alios.mk.rpc.demo0.asyn";
    char msg[128];

    struct aos_parcel_t reply;

    int crpc_handle = 0;

    int ret;
    size_t len;

    krhino_sem_take(rpc_sem[1], RHINO_WAIT_FOREVER);

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
    pthread_t cthread, sthread;

    krhino_sem_dyn_create(&rpc_sem[1], __func__, 0);

    if (pthread_create(&sthread, NULL, rpc_server_demo_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }

    if (pthread_create(&cthread, NULL, rpc_client_demo_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        pthread_kill(sthread, 0);
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

    krhino_sem_give(rpc_sem[1]);
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

    krhino_sem_take(rpc_sem[1], RHINO_WAIT_FOREVER);

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
    pthread_t cthread, sthread;

    krhino_sem_dyn_create(&rpc_sem[1], __func__, 0);

    if (pthread_create(&sthread, NULL, rpc_server_sum_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "client");
        return;
    }

    if (pthread_create(&cthread, NULL, rpc_client_sum_benchmark, NULL) < 0) {
        printf("create %s fail\r\n", "server");
        pthread_kill(sthread, 0);
        return;
    }
}

extern void dservice_benchmark_test(char *pbuffer, int outlen, int argc, char **argv);
struct cli_command rpc_ucmds[] = {
    {"rpcc", "Kernel space rpc converage testes", rpc_coverage_benchmark_test},
    {"rpcd", "Kernel space rpc demo testes", rpc_demo_benchmark_test},
    {"rpcdr", "Kernel space rpc resume demo testes", rpc_server_demo_resume_benchmark},
    {"rpcdp", "Kernel space rpc resume demo testes", rpc_server_demo_pend_benchmark},
    {"rpc_sum", "Kernel space rpc sum testes", rpc_sum_benchmark_test},
    {"dservice", "Kernel space dservice testes", dservice_benchmark_test},
};

#ifdef __cplusplus
}
#endif
