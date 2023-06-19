/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#define ROOTFS_MNTP       "/jsamp"
#define TEST_FILE        ROOTFS_MNTP"/testfile.txt"
#define TEST_FILE1       ROOTFS_MNTP"/testfile1.txt"
#define TEST_DIR         ROOTFS_MNTP"/testdir"
#define TEST_DIR_FILE    ROOTFS_MNTP"/testdir/testfile.txt"
#define TEST_DIR_FILE1   ROOTFS_MNTP"/testdir/testfile1.txt"

extern const char test_str[];

void test_fs_mount(void);
void test_fs_open(void);
void test_fs_write(void);
void test_fs_read(void);
void test_fs_stat(void);
void test_fs_statfs(void);
void test_fs_link(void);
void test_fs_fcntl(void);
void test_fs_pathconf(void);
void test_fs_fpathconf(void);
void test_fs_utime(void);
void test_fs_close(void);
void test_fs_fd_leak(void);
void test_fs_unlink(void);
void test_fs_mkdir(void);
void test_fs_readdir(void);
void test_fs_chdir(void);
void test_fs_seekdir(void);
void test_fs_rmdir(void);
