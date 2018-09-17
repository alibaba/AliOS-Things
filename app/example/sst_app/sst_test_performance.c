#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sst.h>
#include "sst_test.h"

// performance:
// case 1: client: store, get (10000)
// case 2: client1: store, get (10000), client2: store, get(10000)

static int test_sec_sst_performance(int test_times);
static int test_sec_sst_store(char *key, uint8_t *secret, uint32_t secret_len);
static int test_sec_sst_get(char *key);

int sst_test_performance(int task_count, int test_count)
{
    printf("task_count %d, test_count %d\n", task_count, test_count);
    if (task_count == 0 || test_count == 0) {
        printf("Invalid OPTIONS\n");
        return -1;
    }

#if !CONFIG_AOS_SUPPORT
    pid_t fpid;
    for (int i = 0; i < task_count - 1; i ++) {
        fpid = fork();
        if (fpid == 0) {
            printf("child task: %d\n", getpid());
            break;
        } else if (fpid > 0) {
            printf("parent task : %d\n", getpid());
        } else {
            printf("fork child task fail, exit...\n");
            return -1;
        }
    }
#endif /* !CONFIG_AOS_SUPPORT */

    if (sst_init()) {
        printf("sst init failed\n");
        return -1;
    }

    if (test_sec_sst_performance(test_count)) {
        printf("sec sst performance failed\n");
        return -1;
    }

    return 0;
}

static int test_sec_sst_performance(int test_times)
{
#if !CONFIG_AOS_SUPPORT
    char *basic_key = "./data/.sst/test_performance_key";
#else
    char *basic_key = "test_performance_key";
#endif
    char *basic_secret = "test_performance_secret";
    char *key = NULL;
    uint8_t secret[256];
    uint32_t secret_len = 256;
    int success_times = 0;
    int fail_times = 0;
    struct timeval tv_start, tv_now;
    uint32_t i = 0;
    int ret = 0;
    long cost_time = 0;

    key = (char *)sst_test_malloc(strlen(basic_key) + 20);
    if (key == NULL) {
        printf("malloc key fail\n");
        return -1;
    }

    sst_test_memset(key, 0, strlen(basic_key) + 20);
    sst_test_memset(secret, 'a', secret_len - 1);
    secret[secret_len - 1] = 0;
    strncpy((char *)key, basic_key, strlen(basic_key));
    strncpy((char *)secret, basic_secret, strlen(basic_secret));
#if !CONFIG_AOS_SUPPORT
    sprintf(key + strlen(basic_key), "%08d", getpid());
    sprintf((char *)(secret + secret_len - 13), "%08d", getpid());
#endif

    gettimeofday(&tv_start, NULL);
    for (i = 0; i < test_times; i ++) {
#if !CONFIG_AOS_SUPPORT
        sprintf(key + strlen(basic_key) + 8, "%04d", i + 1);
        sprintf((char *)(secret + secret_len - 5), "%04d", i + 1);
#endif
        //        printf("store times: %d, key: %s, secret: %s\n", i + 1, key, secret);
        if (test_sec_sst_store(key, secret, secret_len) == 0) {
            success_times += 1;
        } else {
            printf("times: %d, key: %s, secret: %s ", i + 1, key, secret);
            printf("store fail.\n");
            fail_times += 1;
            break;
        }
    }

    if (success_times != test_times) {
#if !CONFIG_AOS_SUPPORT
        printf("EEEEEE pid %d test failed EEEEEE \n", getpid());
#else
        printf("EEEEEE test failed EEEEEE \n");
#endif
        ret = -1;
        goto clean;
    }

    gettimeofday(&tv_now, NULL);
    cost_time = (tv_now.tv_sec - tv_start.tv_sec) * 1000000 + (tv_now.tv_usec - tv_start.tv_usec);
    if (success_times != 0) {
        cost_time = cost_time / success_times;
    }

#if !CONFIG_AOS_SUPPORT
    printf("<<<<<<<<< pid %d test store success av_time %ldus>>>>>>>>>>>>>>\n", getpid(), cost_time);
#else
    printf("<<<<<<<<< test store success av_time %ldus>>>>>>>>>>>>>>\n", cost_time);
#endif

    success_times = 0;
    fail_times = 0;
    gettimeofday(&tv_start, NULL);
    for (i = 0; i < test_times; i ++) {
#if !CONFIG_AOS_SUPPORT
        sprintf(key + strlen(key) - 4, "%04d", i + 1);
#endif
        if (test_sec_sst_get(key) == 0) {
            success_times += 1;
        } else {
            printf("times: %d, key: %s, secret: %s ", i + 1, key, secret);
            printf("get fail.\n");
            fail_times += 1;
            break;
        }
    }

    if (success_times != test_times) {
#if !CONFIG_AOS_SUPPORT
        printf("EEEEEE pid %d test failed EEEEEE \n", getpid());
#else
        printf("EEEEEE test failed EEEEEE \n");
#endif
        ret = -1;
        goto clean;
    }
    gettimeofday(&tv_now, NULL);

    cost_time = (tv_now.tv_sec - tv_start.tv_sec) * 1000000 + (tv_now.tv_usec - tv_start.tv_usec);
    if (success_times != 0) {
        cost_time = cost_time / success_times;
    }
    ret = 0;

#if !CONFIG_AOS_SUPPORT
    printf("<<<<<<<<< pid %d test get success av_time %ldus>>>>>>>>>>>>>>\n", getpid(), cost_time);
#else
    printf("<<<<<<<<< test success get av_time %ldus>>>>>>>>>>>>>>\n", cost_time);
#endif

clean:
    if (key != NULL) {
        sst_test_free(key);
        key = NULL;
    }

    return ret;
}

static int test_sec_sst_store(char *key, uint8_t *secret, uint32_t secret_len)
{
    uint32_t result = 0;
    uint32_t key_type = 0;

    result = sst_add_item(key, secret, secret_len, key_type, 1);
    //    printf("The result is: 0x%08x, secret is %s\n", result, secret);

    return result;
}

static int test_sec_sst_get(char *key)
{
    uint8_t secret[1024] = { 0 };
    uint32_t result = 0;
    uint32_t secret_len = 1024;
    uint32_t key_type = 0;

    result = sst_get_item(key, secret, &secret_len, &key_type);
    if (result) {
        printf("%s failed the result is: %x\n", __FUNCTION__, result);
        return result;
    }
    //    printf("%s success the secret is: %s\n", __FUNCTION__, secret);

    return result;
}

