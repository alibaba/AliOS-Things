/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "posix/enviro.h"
#include "ulog/ulog.h"

static char *envname[5] = {"os", "version", "component", "author", "email"};
static char *envval[5]  = {"AliOS Things", "v2.1", "osal.posix", "Jason", "jason_sophia@aliyun.com"};
static char *env[5]     = {"os=AliOS Things", "version=v2.1", "component=osal.posix", "author=Jason","email=jason_sophia@aliyun.com"};

static int succeed_flag = 1;

void posix_setenv_case(void);
void posix_getenv_case(void);
void posix_unsetenv_case(void);
void posix_putenv_case(void);
void posix_uname_case(void);
void posix_sysconf_case(void);
void posix_confstr_case(void);

void posix_enviro_test_case(void)
{
    posix_setenv_case();
    posix_getenv_case();
    posix_unsetenv_case();
    posix_putenv_case();
    posix_uname_case();
    posix_sysconf_case();
    posix_confstr_case();
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "setenv\n");

            for (i = 0; i < 5; ++i) {
                ret = setenv(envname[i], envval[i], 1);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 2) {
            LOGI(TAG, "getenv \n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);
                if (val == NULL) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 2 !\n");
                } else {
                    if (strcmp(val, envval[i]) != 0) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 3 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "setenv\n");

            for (i = 0; i < 3; ++i) {
                ret = setenv(envname[i], envval[i], 1);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 2) {
            LOGI(TAG, "getenv \n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);

                if ((i >= 0) && (i < 3)) {
                    if (val == NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 2 !\n");
                    } else {
                        if (strcmp(val, envval[i]) != 0) {
                            succeed_flag = 0;
                            LOGI(TAG, "error 3 !\n");
                        }
                    }
                } else {
                    if (val != NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 4 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "setenv\n");

            for (i = 3; i < 5; ++i) {
                ret = setenv(envname[i], envval[i], 1);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 2) {
            LOGI(TAG, "getenv\n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);

                if ((i >= 3) && (i <= 4)) {
                    if (val == NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 2 !\n");
                    } else {
                        if (strcmp(val, envval[i]) != 0) {
                            succeed_flag = 0;
                            LOGI(TAG, "error 3 !\n");
                        }
                    }
                } else {
                    if (val != NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 4 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "setenv\n");

            for (i = 0; i < 5; ++i) {
                ret = setenv(envname[i], envval[i], 1);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        if (count == 2) {
            LOGI(TAG, "unsetenv\n");

            for (i = 3; i < 5; ++i)
            {
                ret = unsetenv(envname[i]);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 2 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 3) {
            LOGI(TAG, "getenv\n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);

                if ((i >= 0) && (i < 3)) {
                    if (val == NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 3 !\n");
                    } else {
                        if (strcmp(val, envval[i]) != 0) {
                            succeed_flag = 0;
                            LOGI(TAG, "error 4 !\n");
                        }
                    }
                } else {
                    if (val != NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 5 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task5(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "setenv\n");

            for (i = 0; i < 5; ++i) {
                ret = setenv(envname[i], envval[i], 1);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        if (count == 2) {
            LOGI(TAG, "unsetenv\n");

            for (i = 0; i < 3; ++i) {
                ret = unsetenv(envname[i]);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 2 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 3) {
            LOGI(TAG, "getenv\n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);

                if ((i >= 3) && (i <= 4)) {
                    if (val == NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 3 !\n");
                    } else {
                        if (strcmp(val, envval[i]) != 0) {
                            succeed_flag = 0;
                            LOGI(TAG, "error 4 !\n");
                        }
                    }
                } else {
                    if (val != NULL) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 5 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task6(void *arg)
{
    int   count  = 0;
    int   i      = 0;
    void *status = NULL;
    char *val    = NULL;
    int   ret    = -1;

    while (1) {
        count++;

        if (count == 1) {
            LOGI(TAG, "putenv\n");

            for (i = 0; i < 5; ++i) {
                ret = putenv(env[i]);
                if (ret != 0) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 1 !\n");
                }
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 2) {
            LOGI(TAG, "getenv\n");

            for (i = 0; i < 5; ++i) {
                val = getenv(envname[i]);
                if (val == NULL) {
                    succeed_flag = 0;
                    LOGI(TAG, "error 2 !\n");
                } else {
                    if (strcmp(val, envval[i]) != 0) {
                        succeed_flag = 0;
                        LOGI(TAG, "error 3 !\n");
                    }
                }
            }

            pthread_exit(status);
        }
    }

    return NULL;
}

/******************* test posix_setenv_case **********************/
void posix_setenv_case(void)
{
    pthread_t thread;
    int       ret    = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_setenv_case start ***********\n");

    succeed_flag = 1;

    ret = pthread_create(&thread, NULL, demo_task1, NULL);

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_setenv_case test OK !\n");
    } else {
        LOGI(TAG, "posix_setenv_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_setenv_case end ***********\n");
}

/******************* test posix_getenv_case **********************/
void posix_getenv_case(void)
{
    pthread_t thread1;
    pthread_t thread2;
    int       ret    = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_getenv_case start ***********\n");

    succeed_flag = 1;

    ret = pthread_create(&thread1, NULL, demo_task2, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task3, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_getenv_case test OK !\n");
    } else {
        LOGI(TAG, "posix_getenv_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_getenv_case end ***********\n");
}

/******************* test posix_unsetenv_case **********************/
void posix_unsetenv_case(void)
{
    pthread_t thread1;
    pthread_t thread2;
    int       ret    = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_unsetenv_case start ***********\n");

    succeed_flag = 1;

    ret = pthread_create(&thread1, NULL, demo_task4, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task5, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_unsetenv_case test OK !\n");
    } else {
        LOGI(TAG, "posix_unsetenv_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_unsetenv_case end ***********\n");
}

/******************* test posix_putenv_case **********************/
void posix_putenv_case(void)
{
    pthread_t thread;
    int       ret    = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_putenv_case start ***********\n");

    succeed_flag = 1;

    ret = pthread_create(&thread, NULL, demo_task6, NULL);

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_putenv_case test OK !\n");
    } else {
        LOGI(TAG, "posix_putenv_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_putenv_case end ***********\n");
}

/******************* test uname **********************/
void posix_uname_case(void)
{
    int ret = -1;

    LOGI(TAG, "*********** posix_uname_case start ***********\n");

    succeed_flag = 1;

    struct utsname utsname_temp;

    ret = uname(&utsname_temp);
    if ((ret != 0)||(strcmp(utsname_temp.sysname, "AliOS Things") != 0)) {
        succeed_flag = 0;
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_uname_case test OK !\n");
    } else {
        LOGI(TAG, "posix_uname_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_uname_case end ***********\n");
}

/******************* test sysconf **********************/
void posix_sysconf_case(void)
{
    long val = 0;

    LOGI(TAG, "*********** posix_sysconf_case start ***********\n");

    succeed_flag = 1;

    val = sysconf(_SC_JOB_CONTROL);
    if (val != _POSIX_JOB_CONTROL) {
        succeed_flag = 0;
    }

    val = sysconf(_SC_FSYNC);
    if (val != _POSIX_FSYNC) {
        succeed_flag = 0;
    }

    val = sysconf(_SC_SEMAPHORES);
    if (val != _POSIX_SEMAPHORES) {
        succeed_flag = 0;
    }

    val = sysconf(_SC_BARRIERS);
    if (val != _POSIX_BARRIERS) {
        succeed_flag = 0;
    }

    val = sysconf(_SC_ADVISORY_INFO);
    if (val != _POSIX_ADVISORY_INFO) {
        succeed_flag = 0;
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_sysconf_case test OK !\n");
    } else {
        LOGI(TAG, "posix_sysconf_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_sysconf_case end ***********\n");
}

/******************* test confstr **********************/
void posix_confstr_case(void)
{
    int  size = -1;
    char buf[32];

    LOGI(TAG, "*********** posix_confstr_case start ***********\n");

    succeed_flag = 1;

    size = confstr(_CS_GNU_LIBC_VERSION ,buf, sizeof(buf));
    if ((size == 0)||(strncmp(buf, _POSIX_GNU_LIBC_VERSION, size) != 0)) {
        succeed_flag = 0;
    }

    size = confstr(_CS_GNU_LIBPTHREAD_VERSION ,buf, sizeof(buf));
    if ((size == 0)||(strncmp(buf, _POSIX_GNU_LIBPTHREAD_VERSION, size) != 0)) {
        succeed_flag = 0;
    }

    if (succeed_flag == 1) {
        LOGI(TAG, "posix_confstr_case test OK !\n");
    } else {
        LOGI(TAG, "posix_confstr_case test failed !\n");
    }

    LOGI(TAG, "*********** posix_confstr_case end ***********\n");
}
