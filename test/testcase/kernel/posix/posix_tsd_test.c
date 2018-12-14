/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "pthread.h"

static pthread_t thread1;
static pthread_t thread2;
static pthread_t thread3;
static pthread_t thread4;

pthread_key_t key; 
pthread_key_t key1;
pthread_key_t key2;

struct tsd_test_struct {
    int   data_i;
    float data_f;
};

int tsd_fail_flag = 0;

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    struct tsd_test_struct  struct_data;
    struct tsd_test_struct *struct_data_p = NULL;

    while (1) {
        printf("***********************************\n");
        printf("demo_task1 count  %d\n", count);
        count++;

        if (count == 1) {
            struct_data.data_i = 10;
            struct_data.data_f = 3.1415f;
            pthread_setspecific(key, &struct_data);
        }

        if (count == 2) {
            struct_data_p = (struct test_struct *)pthread_getspecific(key);

            if ((struct_data_p->data_i != 10) || (struct_data_p->data_f != 3.1415f)) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }

            struct_data.data_i = 20;
            struct_data.data_f = 9.1415f;
            pthread_setspecific(key, &struct_data);
        }

        if (count == 3) {
            struct_data_p = (struct test_struct *)pthread_getspecific(key);

            if ((struct_data_p->data_i != 20) || (struct_data_p->data_f != 9.1415f)) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 3) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   test_data = 0;
    int  *test_data_p = NULL;

    while (1) {
        printf("***********************************\n");
        printf("demo_task2 count  %d\n", count);
        count++;

        if (count == 1) {
            test_data = 123;
            pthread_setspecific(key, &test_data);
        }

        if (count == 2) {
            test_data_p = (int *)pthread_getspecific(key);

            if (*test_data_p != 123) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }

            test_data = 456;
            pthread_setspecific(key, &test_data);
        }

        if (count == 3) {
            test_data_p = (int *)pthread_getspecific(key);

            if (*test_data_p != 456) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 3) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   test_data = 0;
    int  *test_data_p = NULL;

    while (1) {
        printf("***********************************\n");
        printf("demo_task3 count  %d\n", count);
        count++;

        if (count == 1) {
            test_data = 123;
            pthread_setspecific(key1, &test_data);
        }

        if (count == 2) {
            test_data_p = (int *)pthread_getspecific(key1);

            if (*test_data_p != 123) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }

            test_data = 456;
            pthread_setspecific(key1, &test_data);
        }

        if (count == 3) {
            test_data_p = (int *)pthread_getspecific(key1);

            if (*test_data_p != 456) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
            test_data = 789;
            pthread_setspecific(key1, &test_data);
        }

        if (count == 4) {
            test_data_p = (int *)pthread_getspecific(key1);

            if (*test_data_p != 789) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
            test_data = 234;
            pthread_setspecific(key1, &test_data);
        }

        if (count == 5) {
            test_data_p = (int *)pthread_getspecific(key1);

            if (*test_data_p != 234) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
        }

        // sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 5) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   test_data = 0;
    int  *test_data_p = NULL;

    while (1) {
        printf("***********************************\n");
        printf("demo_task4 count  %d\n", count);
        count++;

        if (count == 1) {
            test_data = 123;
            pthread_setspecific(key2, &test_data);
        }

        if (count == 2) {
            test_data_p = (int *)pthread_getspecific(key2);

            if (*test_data_p != 123) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }

            test_data = 456;
            pthread_setspecific(key2, &test_data);
        }

        if (count == 3) {
            test_data_p = (int *)pthread_getspecific(key2);

            if (*test_data_p != 456) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
            test_data = 789;
            pthread_setspecific(key2, &test_data);
        }

        if (count == 4) {
            test_data_p = (int *)pthread_getspecific(key2);

            if (*test_data_p != 789) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
            test_data = 234;
            pthread_setspecific(key2, &test_data);
        }

        if (count == 5) {
            test_data_p = (int *)pthread_getspecific(key2);

            if (*test_data_p != 234) {
                tsd_fail_flag = 1;
                printf("tsd get data error !\n");
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 5) {
            pthread_exit(status);
        }
    };

    return NULL;
}

void test_tsd_destructor(void)
{
    printf("tsd destructor is running !\n");
}

void posix_tsd_case(void)
{
    printf("*********** posix_tsd_case start ***********\n");

    pthread_key_create(&key, NULL);
    pthread_key_create(&key1, NULL);
    pthread_key_create(&key2, test_tsd_destructor);

    pthread_create(&thread1, NULL, demo_task1, NULL);
    pthread_create(&thread2, NULL, demo_task2, NULL);
    pthread_create(&thread3, NULL, demo_task3, NULL);
    pthread_create(&thread4, NULL, demo_task4, NULL);    

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_key_delete(key);
    printf("key deteled !\n");
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);     

    pthread_key_delete(key1);
    pthread_key_delete(key2);   

    printf("******************************************\n");
    if (tsd_fail_flag == 1) {
        printf("tsd test failed !\n");
    } else {
        printf("tsd test succed !\n");
    }

    printf("*********** posix_tsd_case end ***********\n");
}
