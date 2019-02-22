/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "aos/kernel.h"
#include <yunit.h>
#include <yts.h>
#include <ramfs.h>

ramfs_file_t test_file[10];
char writeBuf[10] = {1,2,3,4,5,6,7,8,9,10};
char readBuf[10];
char *file_name[10] = {"c/test_file0.txt", "c/test_file1.txt", "c/test_file2.txt", "c/test_file3.txt", "c/test_file4.txt",
                       "c/test_file5.txt", "c/test_file6.txt", "c/test_file7.txt", "c/test_file8.txt", "c/test_file9.txt"};

ramfs_dir_t test_ramfs_dir;
char file_name_dir[32];

static int test_ramfs_case1(void)
{
    int ret = -1;
    uint32_t bytes_write = 0;
    uint32_t bytes_read = 0;
    int i = 0;
    int j = 0;

    ramfs_init();

    /******************** test access open write read close file multi imes ********************/
    for(i = 0; i < 10; i++)
    {
        ret = ramfs_access(file_name[i], F_OK);

        if(ret == 0)
        {
            return 1;
        }

        ret = ramfs_open(&test_file[i], file_name[i], RAMFS_MODE_WR|RAMFS_MODE_RD);

        if(ret != 0)
        {
            return 2;
        }

        ret = ramfs_access(file_name[i], F_OK);

        if(ret != 0)
        {
            return 3;
        }

        ret = ramfs_access(file_name[i], R_OK);

        if(ret != 0)
        {
            return 4;
        }

        ret = ramfs_access(file_name[i], W_OK);

        if(ret != 0)
        {
            return 5;
        }

        for (j = 0; j < 10; j++)
        {
            writeBuf[j] += i;
        }

        ret = ramfs_write(&test_file[i], writeBuf, 10, &bytes_write);

        if(ret != 0)
        {
            return 6;
        }

        ret = ramfs_close(&test_file[i]);

        if(ret != 0)
        {
            return 7;
        }

        ret = ramfs_open(&test_file[i], file_name[i], RAMFS_MODE_WR|RAMFS_MODE_RD);

        if(ret != 0)
        {
            return 8;
        }

        ret = ramfs_read(&test_file[i], readBuf, 10, &bytes_read);

        if(ret != 0)
        {
            return 9;
        }

        for (j = 0; j < 10; j++)
        {
            if(readBuf[j] != writeBuf[j])
            {
                return 10;
            }
        }

        ret = ramfs_close(&test_file[i]);

        if(ret != 0)
        {
            return 11;
        }

        ret = ramfs_remove(file_name[i]);

        if(ret != 0)
        {
            return 12;
        }

        ret = ramfs_access(file_name[i], F_OK);

       if(ret == 0)
        {
            return 13;
        }
    }

/******************** test access open write read close multi files ********************/
    for (j = 0; j < 10; j++)
    {
        writeBuf[j] = j;
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_open(&test_file[i], file_name[i], RAMFS_MODE_WR|RAMFS_MODE_RD);

        if(ret != 0)
        {
            return 14;
        }
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_write(&test_file[i], writeBuf, 10, &bytes_write);

        if(ret != 0)
        {
            return 15;
        }

        for (j = 0; j < 10; j++)
        {
            writeBuf[j] += 1;
        }
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_close(&test_file[i]);

        if(ret != 0)
        {
            return 16;
        }
    }

    ret = ramfs_dir_open(&test_ramfs_dir, "c/");

    if(ret != 0)
    {
        return 17;
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_dir_read(&test_ramfs_dir, file_name_dir);

        if(ret != 0)
        {
            return 18;
        }
        else
        {
            ret = strcmp(file_name_dir, file_name[9-i]);

            if(ret != 0)
            {
                return 19;
            }
        }
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_open(&test_file[i], file_name[i], RAMFS_MODE_WR|RAMFS_MODE_RD);

        if(ret != 0)
        {
            return 20;
        }
    }

    for (j = 0; j < 10; j++)
    {
        writeBuf[j] = j;
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_read(&test_file[i], readBuf, 10, &bytes_read);

        if(ret != 0)
        {
            return 21;
        }

        for (j = 0; j < 10; j++)
        {
            if(readBuf[j] != (writeBuf[j] + i))
            {
                return 22;
            }
        }
    }

    for(i = 0; i < 10; i++)
    {
        ret = ramfs_close(&test_file[i]);

        if(ret != 0)
        {
            return 23;
        }
    }
/******************** test read access ********************/

    ret = ramfs_open(&test_file[0], file_name[0], RAMFS_MODE_RD);

    if(ret != 0)
    {
        return 24;
    }

    ret = ramfs_access(file_name[0], R_OK);

    if(ret != 0)
    {
        return 25;
    }

    ret = ramfs_access(file_name[0], W_OK);

    if(ret == 0)
    {
        return 26;
    }

    ret = ramfs_write(&test_file[0], writeBuf, 10, &bytes_write);

    if(ret == 0)
    {
        return 27;
    }

    ret = ramfs_close(&test_file[0]);

    if(ret != 0)
    {
        return 28;
    }

/******************** test write access ********************/
    ret = ramfs_open(&test_file[0], file_name[0], RAMFS_MODE_WR);

    if(ret != 0)
    {
        return 29;
    }

    ret = ramfs_access(file_name[0], R_OK);

    if(ret == 0)
    {
        return 30;
    }

    ret = ramfs_access(file_name[0], W_OK);

    if(ret != 0)
    {
        return 31;
    }

    ret = ramfs_read(&test_file[0], readBuf, 10, &bytes_read);

    if(ret == 0)
    {
        return 32;
    }

    ret = ramfs_close(&test_file[0]);

    if(ret != 0)
    {
        return 33;
    }

    return 0;
}

static void test_ramfs_case(void)
{
    int ret = -1;

    ret = test_ramfs_case1();
    YUNIT_ASSERT(ret == 0);
    if (ret != 0)
    {
        printf("test ramfs failed, error munber is %d\n",ret);
    }
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{

    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_ramfs_testcases[] = {
    { "ramfs", test_ramfs_case},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "ramfs", init, cleanup, setup, teardown, aos_ramfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_ramfs(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_ramfs);

