/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/dirent.h"

#define MAX_PATH_BYTES 32

char path_buffer[MAX_PATH_BYTES];

void posix_dirent_pathconf_case(void);
void posix_dirent_cwp_case(void);
void posix_dirent_link_case(void);

void posix_dirent_case(void)
{
    posix_dirent_pathconf_case();
    posix_dirent_cwp_case();
    posix_dirent_link_case();
}

void posix_dirent_pathconf_case(void)
{
    int val          = 0;
    int success_flag = 0;
    int fh           = 0;

    val = pathconf("/ramfs/file" ,_PC_NAME_MAX);
    if (val == 128) {
        printf("_PC_NAME_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        printf("posix_dirent_pathconf_test failed !\n");
        return;
    }

    success_flag = 0;
    fh = open("/ramfs/file1", 2);
    val = fpathconf(fh ,_PC_LINK_MAX);
    if (val == 1024) {
        printf("_PC_LINK_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        printf("posix_dirent_pathconf_test failed !\n");
        return;
    }

    close(fh);

    printf("posix_dirent_pathconf_test succeed !\n");
}

void posix_dirent_cwp_case(void)
{
    char *ret_p        = NULL;
    int   ret          = -1;
    int   success_flag = 0;

    ret_p = getcwd(path_buffer, MAX_PATH_BYTES);
    if (ret_p != NULL) {
        if (strcmp(path_buffer, "/default") == 0) {
            success_flag = 1;
        }
    }

    if (success_flag != 1) {
        printf("getcwd test failed1 !\n");
        return;
    }

    ret = chdir("c/my_test_dir/dir1");
    if (ret != 0) {
        printf("chdir test failed2 !\n");
        return;
    }

    success_flag = 0;
    ret_p = getcwd(path_buffer, MAX_PATH_BYTES);
    if (ret_p != NULL) {
        if (strcmp(path_buffer, "c/my_test_dir/dir1") == 0) {
            success_flag = 1;
        }
    }

    if (success_flag != 1) {
        printf("chdir test failed3 !\n");
        return;
    }

    printf("current working directory test succeed !\n");
}

void posix_dirent_link_case(void)
{
    int ret = 0;
    int fh  = 0;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed1 !\n");
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed2 !\n");
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed3 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed4 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed5 !\n");
            return;
        }
    }

    close(fh);

    /*******************************************************************************/
    ret  = link("/ramfs/file1", "/ramfs/file2");
    ret |= link("/ramfs/file2", "/ramfs/file3");
    ret |= link("/ramfs/file3", "/ramfs/file4");
    ret |= link("/ramfs/file4", "/ramfs/file5");
    ret |= link("/ramfs/file5", "/ramfs/file6");

    if (ret != 0) {
        printf("posix_dirent_link_case test failed6 !\n");
        return;
    }

    /*****************************************************/
    fh = open("/ramfs/file2", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed7 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed8 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed9 !\n");
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file6", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed10 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed11 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed12 !\n");
            return;
        }
    }

    close(fh); 

    /*******************************************************************************/
    ret = unlink("/ramfs/file2");
    if (ret != 0) {
        printf("posix_dirent_link_case test failed13 !\n");
        return;
    }

    /*****************************************************/
    fh = open("/ramfs/file3", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed14 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed15 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed16 !\n");
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file4", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed17 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed18 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed19 !\n");
            return;
        }
    }

    close(fh);

    /*****************************************************/
    struct stat stat_buf;
    fh = stat("/ramfs/file2", &stat_buf);

    if (fh == 0) {
        printf("posix_dirent_link_case test failed20 !\n");
        return;
    }

    /*******************************************************************************/
    ret = unlink("/ramfs/file4");
    if (ret != 0) {
        printf("posix_dirent_link_case test failed21 !\n");
        return;
    }

    /*****************************************************/
   fh = open("/ramfs/file3", 2);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed22 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed23 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed24 !\n");
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file5", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_link_case test failed25 !\n");
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_link_case test failed26 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_link_case test failed27 !\n");
            return;
        }
    }

    close(fh);

    /*******************************************************************************/
    ret = unlink("/ramfs/file1");
    if (ret != 0) {
        printf("posix_dirent_link_case test failed28 !\n");
        return;
    }

    printf("posix_dirent_link_case test succeed !\n");
}
