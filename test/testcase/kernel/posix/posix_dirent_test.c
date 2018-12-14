/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "posix_dirent.h"
#include "k_api.h"

#define MAX_PATH_BYTES 32

char path_buffer[MAX_PATH_BYTES];

void posix_dirent_cwp_case(void);

void posix_dirent_case(void)
{
    posix_dirent_cwp_case();
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
