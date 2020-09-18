/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include <k_api.h>

#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define FILE_NAME_MAX 64
#define TEST_PROCESS_MAX 4

typedef struct {
    char *taskname;
    char partition[16];
    uint8_t idx;
    uint32_t total_limit;
} fs_test_arg_t;

/* large bytes on less frequency */
static void operate_fs(void *p)
{
    int rc, fd;
    unsigned char buf[1024];
    char f[FILE_NAME_MAX], *tname;
    fs_test_arg_t *arg;
    unsigned int total = 0;

    if (!p) return;
    else {arg = (fs_test_arg_t *)p; tname = arg->taskname;}

    printf("%s task is running ....\r\n", tname);
    snprintf(f, sizeof(f), "/%s/fs_stress_%d.test",
             arg->partition, arg->idx);
    aos_msleep(rand() % 10000);

    fd = open(f, O_CREAT | O_RDWR);
    if (fd < 0) {
        close(fd);
        printf("%s Failed to open %s, ret: %d, errno: %d\r\n",
               tname, f, fd, errno);
    } else {
        close(fd);
        while (1) {
            int i = 0;
            aos_msleep(rand() % 3000);
            fd = open(f, O_RDWR | O_CREAT | O_APPEND);
            if (fd < 0) {
                printf("%s Failed to open %s\r\n, continue with "
                       "next cycle.", tname, f);
                continue;
            }
            while (i < sizeof(buf)) {
                buf[i++] = (unsigned char)(rand() & 0xff);
            }
            rc = write(fd, buf, rand() % sizeof(buf));
            if (rc < 0) {
                printf("%s Failed to write to file %s, ret: %d, errno: %d, "
                       "total so far: %d\r\n", tname, f, rc, errno, total);
                close(fd);
                break;
            } else {
                close(fd);
                total += rc;
                printf("%s Write %d bytes into file %d, total: "
                       "%d\r\n", tname, rc, fd, total);
            }
            aos_msleep(rand() % 3000);
            fd = open(f, O_RDONLY);
            if (fd < 0) {
                printf("%s Failed to open %s\r\n, continue "
                       "with next cycle.", tname, f);
                continue;
            } else {
                while ((rc = read(fd, buf, sizeof(buf))) > 0) {
                    printf("%s Read %d bytes from file %d\r\n", tname, rc, fd);
                }
            }
            printf("%s Read exit with code %d, error: %d\r\n", tname, rc, errno);
            close(fd);
            if (total > arg->total_limit) {
                printf("%s File %s is very large now (%u bytes), "
                       "remove it.\r\n", tname, f, total);
                remove(f);
            }
        }
    }

    printf("%s task is to exit ....\r\n", tname);
    aos_task_exit(0);
}

static fs_test_arg_t _arg[TEST_PROCESS_MAX];
static void fs_test()
{
    int i;
    unsigned int seed = (unsigned int)aos_now();
    srand(seed);

    printf("%s entry\r\n", __func__);

    for (i = 0; i < TEST_PROCESS_MAX; i++) {
        memset(&_arg[i], 0, sizeof(fs_test_arg_t));
        _arg[i].taskname = (fs_test_arg_t *)malloc(16);
        memset(_arg[i].taskname, 0, 16);

        _arg[i].idx = i;
        _arg[i].total_limit = rand() % (2 * 1024 * 1024);
        snprintf(_arg[i].taskname, 16, "fstest%d", i);
    #ifdef CONFIG_MULTI_FS
        if (i <= (TEST_PROCESS_MAX >> 1))
    #endif
        {
            strncpy(_arg[i].partition, "data", sizeof(_arg[i].partition));
        }
    #ifdef CONFIG_MULTI_FS
         else {
            strncpy(_arg[i].partition, "system", sizeof(_arg[i].partition));
        }
    #endif

        printf("New test task to create: %s %d %u\r\n", _arg[i].taskname, i, _arg[i].total_limit);
        aos_task_new(_arg[i].taskname, operate_fs, &_arg[i], 4096);
    }

    printf("%s exit\r\n", __func__);
}

static void handle_fs_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    fs_test();
}

static struct cli_command fscmd = {
    .name     = "fstest",
    .help     = "File system testes",
    .function = handle_fs_test_cmd
};

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    printf("fs_test0914 start to run...\r\n");
    aos_cli_register_command(&fscmd);

    while (1) {
        printf("fs_test run cnt %d\r\n", cnt++);
        krhino_task_sleep(10000);
    }
}

