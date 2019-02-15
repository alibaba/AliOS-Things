/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "posix/timer.h"
#include "posix/semaphore.h"
#include "ulog/ulog.h"

timer_t timer1_id;
sem_t   sem;

int test_failed_flag = 0;

struct timespec timer_spec_get_origin;
struct timespec timer_spec_get_cur;
struct timespec timer_spec_get_pre;

void timer1_thread(void)
{
    int ret = -1;

    static int count = 0;

    LOGI(TAG, "timer1_thread is running !\n");

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get_cur);
    if (ret != 0) {
        test_failed_flag = 1;
        LOGI(TAG, "clock_gettime error !\n");
    }

    if (count == 0) {
        if ((timer_spec_get_cur.tv_sec - timer_spec_get_origin.tv_sec) != 3) {
            test_failed_flag = 1;
            LOGI(TAG, "timer it_value error !\n");
        }
    } else {
        if ((timer_spec_get_cur.tv_sec - timer_spec_get_pre.tv_sec) != 2) {
            test_failed_flag = 1;
            LOGI(TAG, "timer it_interval error !\n");
        }
    }

    if (count == 10) {
        ret = timer_delete(timer1_id);
        if (ret != 0) {
            test_failed_flag = 1;
            LOGI(TAG, "timer_delete error !\n");
        }

        ret = sem_post(&sem);
        if (ret != 0) {
            test_failed_flag = 1;
            LOGI(TAG, "sem_init error !\n");
        }
    }

    memcpy(&timer_spec_get_pre, &timer_spec_get_cur, sizeof(struct timespec));

    count++;
}

/* testcase for timer_create/timer_settime/timer_gettime/timer_delete */
void posix_timer_case1(void)
{
    int ret = -1;

    struct itimerspec it;
    struct sigevent   timer1_sig;
    struct timespec   timer_spec_buf;

    LOGI(TAG, "*********** posix_timer_case1 start ***********\n");

    ret = sem_init(&sem, 0, 0);
    if (ret != 0) {
        test_failed_flag = 1;
        LOGI(TAG, "sem_init error !\n");
    }

    memset(&timer_spec_get_origin, 0, sizeof(struct timespec));
    memset(&timer_spec_get_cur, 0, sizeof(struct timespec));
    memset(&timer_spec_get_pre, 0, sizeof(struct timespec));

    timer1_sig.sigev_notify_function = timer1_thread;

    /* create the timer */
    ret = timer_create(CLOCK_MONOTONIC, &timer1_sig, &timer1_id);
    if (ret != 0) {
        test_failed_flag = 1;
        LOGI(TAG, "timer_settime error !\n");
    }

    /* run for the first time after 3 seconds, then run every 2 seconds */
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_nsec = 1000000000;
    it.it_value.tv_sec = 2;
    it.it_value.tv_nsec = 1000000000;

    /* set the timer */
    ret = timer_settime(timer1_id, 0, &it, NULL);
    if (ret != 0) {
        test_failed_flag = 1;
        LOGI(TAG, "timer_settime error !\n");
    }

    /* get the timer value */
    ret = timer_gettime(timer1_id, &timer_spec_buf);
    if (ret == 0) {
        if ((timer_spec_buf.tv_sec != 2) || (timer_spec_buf.tv_nsec != 0)) {
            test_failed_flag = 1;
            LOGI(TAG, "timer_gettime error !\n");
        }
    } else {
        test_failed_flag = 1;
        LOGI(TAG, "timer_gettime error !\n");
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get_origin);
    if (ret != 0) {
        test_failed_flag = 1;
        LOGI(TAG, "clock_gettime error !\n");
    }

    sem_wait(&sem);
    sem_destroy(&sem);

    if (test_failed_flag == 0) {
        LOGI(TAG, "posix_timer_case1 test OK !\n");
    }

    LOGI(TAG, "*********** posix_timer_case1 end ***********\n\n");
}

/* testcase for clock_gettime/clock_nanosleep/nanosleep/sleep */
void posix_timer_case2(void)
{
    int ret     = -1;
    int seconds = 0;

    struct timespec timer_spec_get1;
    struct timespec timer_spec_get2;
    struct timespec timer_spec_set;

    LOGI(TAG, "*********** posix_timer_case2 start ***********\n");

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get1);
    if (ret != 0) {
        LOGI(TAG, "clock_gettime error !\n");
        return;
    }

    timer_spec_set.tv_sec = 2;
    timer_spec_set.tv_nsec = 0;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &timer_spec_set, NULL);

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get2);
    if (ret != 0) {
        LOGI(TAG, "clock_gettime error !\n");
        return;
    }

    seconds = timer_spec_get2.tv_sec - timer_spec_get1.tv_sec;
    if (seconds != 2) {
        LOGI(TAG, "clock_nanosleep error !\n");
        return;
    }

    sleep(3);

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get2);
    if (ret != 0) {
        LOGI(TAG, "clock_gettime error !\n");
        return;
    }

    seconds = timer_spec_get2.tv_sec - timer_spec_get1.tv_sec;
    if (seconds != 5) {
        LOGI(TAG, "clock_nanosleep error !\n");
        return;
    }

    timer_spec_set.tv_sec = 2;
    timer_spec_set.tv_nsec = 0;
    nanosleep(&timer_spec_set, NULL);

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_get2);
    if (ret != 0) {
        LOGI(TAG, "clock_gettime error !\n");
        return;
    }

    seconds = timer_spec_get2.tv_sec - timer_spec_get1.tv_sec;
    if (seconds != 7) {
        LOGI(TAG, "nanosleep error !\n");
        return;
    }

    LOGI(TAG, "posix_timer_case2 test OK !\n");

    LOGI(TAG, "*********** posix_timer_case2 end ***********\n\n");
}

void posix_timer_test(void)
{
    posix_timer_case1();
    posix_timer_case2();
}
