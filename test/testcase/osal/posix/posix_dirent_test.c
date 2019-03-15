/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "k_api.h"
#include "posix/dirent.h"
#include "ramfs_types.h"
#include "ulog/ulog.h"

#define MAX_PATH_BYTES 32

char path_buffer[MAX_PATH_BYTES];

void posix_dirent_creat_case(void);    /* testcase for creat/lseek */
void posix_dirent_access_case(void);   /* testcase for access */
void posix_dirent_utime_case(void);    /* testcase for utime */
void posix_dirent_rename_case(void);   /* testcase for rename */
void posix_dirent_remove_case(void);   /* testcase for remove */
void posix_dirent_dir_case(void);      /* testcase for opendir/closedir/mkdir/readdir/rmdir/rewinddir */
void posix_dirent_pathconf_case(void); /* testcase for pathconf/pathconf */
void posix_dirent_cwp_case(void);      /* testcase for getcwd/chdir */
void posix_dirent_stat_case(void);     /* testcase for fstat/stat/fstat */
void posix_dirent_link_case(void);     /* testcase for link/unlink */
void posix_dirent_fsync_case(void);    /* testcase for fsync */
void posix_dirent_fopen_case(void);    /* testcase for fopen/fread/fwrite/fclose */
void posix_dirent_fseek_case(void);    /* testcase for fseek/fseeko */
void posix_dirent_fsetpos_case(void);  /* testcase for fsetpos/fgetpos */
void posix_dirent_ftell_case(void);    /* testcase for ftell/ftello */
void posix_dirent_rewind_case(void);   /* testcase for ftell/ftello */

void posix_dirent_test_case(void)
{
    posix_dirent_creat_case();
    posix_dirent_access_case();
    posix_dirent_remove_case();
    posix_dirent_rename_case();
    posix_dirent_dir_case();
    posix_dirent_pathconf_case();
    posix_dirent_cwp_case();
    posix_dirent_stat_case();
    posix_dirent_link_case();
    posix_dirent_fsync_case();
    posix_dirent_fopen_case();
    posix_dirent_fseek_case();
    posix_dirent_fsetpos_case();
    posix_dirent_ftell_case();
    posix_dirent_rewind_case();
}

/* testcase for access */
void posix_dirent_access_case(void)
{
    int ret = -1;
    int fh  = -1;

    char *casename = "posix_dirent_access_case";

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = access("/ramfs/file1" ,F_OK);
    if (ret != 0) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    ret = remove("/ramfs/file1");
    if (ret != 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = access("/ramfs/file1" ,F_OK);
    if (ret == 0) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for utime */
void posix_dirent_utime_case(void)
{
    struct utimbuf utimbuf_temp;

    utimbuf_temp.actime = 1234;
    utimbuf_temp.modtime = 5678;

    utime("/ramfs/file1", &utimbuf_temp);
}

/* testcase for creat/lseek */
void posix_dirent_creat_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_creat_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = creat("/ramfs/file1", 0);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    /*******************************************************************************/

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    ret = lseek(fh, 5, SEEK_SET);
    if (ret != 5) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 5);
    if (ret != 5) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (buf_r[i] != (i + 1 + 5)) {
            LOGI(TAG, "%s test failed8 !\n", casename);
            return;
        }
    }

    close(fh);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for remove */
void posix_dirent_remove_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_remove_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    close(fh);

    /*******************************************************************************/

    ret = remove("/ramfs/file1");
    if (ret != 0) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret == 10) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    close(fh);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for opendir/closedir/mkdir/readdir/rmdir/rewinddir */
void posix_dirent_dir_case(void)
{
    DIR           *dir;
    struct dirent * ptr;

    int i   = 0;
    int ret = -1;
    int fh[11];

    char *casename = "posix_dirent_dir_case";

    fh[0] = open("/ramfs/file1", O_RDWR);
    fh[1] = open("/ramfs/file2", O_RDWR);
    fh[2] = open("/ramfs/file3", O_RDWR);
    fh[3] = open("/ramfs/file4", O_RDWR);

    fh[4] = open("/ramfs/folder1/file5", O_RDWR);
    fh[5] = open("/ramfs/folder2/file6", O_RDWR);
    fh[6] = open("/ramfs/folder3/file7", O_RDWR);

    fh[7] = open("/ramfs/folder1/folder1_1/file8", O_RDWR);
    fh[8] = open("/ramfs/folder1/folder1_1/file9", O_RDWR);

    fh[9]  = open("/ramfs/folder1/folder1_1/folder1_1_1/file10", O_RDWR);
    fh[10] = open("/ramfs/folder1/folder1_1/folder1_1_2/file11", O_RDWR);

    for (i = 0; i < 11; i++) {
        if (fh[i] < 0) {
            LOGI(TAG, "%s test failed1 !\n", casename);
            return;
        }
    }

    ret = mkdir("/ramfs/folder1/folder1_1/folder1_1_3", 0);
    ret |= mkdir("/ramfs/folder4/folder4_1", 0);
    if (ret != 0) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    LOGI(TAG, "ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            LOGI(TAG, "d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs/folder1");
    if (dir == NULL) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    LOGI(TAG, "ls /ramfs/folder1:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            LOGI(TAG, "d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs/folder1/folder1_1");
    if (dir == NULL) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    LOGI(TAG, "ls /ramfs/folder1/folder1_1:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            LOGI(TAG, "d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        LOGI(TAG, "%s test failed5 !\n", casename);
        return;
    }

    LOGI(TAG, "ls /ramfs(3 files):\n");

    ptr = readdir(dir);
    LOGI(TAG, "d_name : %s\n", ptr->d_name);

    ptr = readdir(dir);
    LOGI(TAG, "d_name : %s\n", ptr->d_name);

    ptr = readdir(dir);
    LOGI(TAG, "d_name : %s\n", ptr->d_name);

    LOGI(TAG, "rewinddir dir !\n");
    rewinddir(dir);

    LOGI(TAG, "ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            LOGI(TAG, "d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    close(fh[1]);
    LOGI(TAG, "remove /ramfs/file2\n");
    ret = remove("/ramfs/file2");
    if (ret != 0) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    close(fh[3]);

    LOGI(TAG, "remove /ramfs/file4\n");
    ret = remove("/ramfs/file4");
    if (ret != 0) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    LOGI(TAG, "rmdir /ramfs/folder4/folder4_1\n");
    ret = rmdir("/ramfs/folder4/folder4_1");
    if (ret != 0) {
        LOGI(TAG, "%s test failed9 !\n", casename);
        return;
    }

    LOGI(TAG, "rmdir /ramfs/folder4\n");
    ret = rmdir("/ramfs/folder4");
    if (ret != 0) {
        LOGI(TAG, "%s test failed10 !\n", casename);
        return;
    }

    LOGI(TAG, "ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            LOGI(TAG, "d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    for (i = 0; i < 11; i++) {
        close(fh[i]);
    }

    remove("/ramfs/file1");
    remove("/ramfs/file2");
    remove("/ramfs/file3");
    remove("/ramfs/file4");
    remove("/ramfs/folder1/file5");
    remove("/ramfs/folder2/file6");
    remove("/ramfs/folder3/file7");
    remove("/ramfs/folder1/folder1_1/file8");
    remove("/ramfs/folder1/folder1_1/file9");
    remove("/ramfs/folder1/folder1_1/folder1_1_1/file10");
    remove("/ramfs/folder1/folder1_1/folder1_1_2/file11");
}

/* testcase for rename */
void posix_dirent_rename_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_rename_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    close(fh);

    /*******************************************************************************/

    ret = rename("/ramfs/file1", "/ramfs/file2");
    if (ret != 0) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    fh = open("/ramfs/file2", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed9 !\n", casename);
            return;
        }
    }

    close(fh);
    remove("/ramfs/file1");
    remove("/ramfs/file2");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for pathconf/pathconf */
void posix_dirent_pathconf_case(void)
{
    int val          = 0;
    int success_flag = 0;
    int fh           = -1;

    char *casename = "posix_dirent_pathconf_case";

    val = pathconf("/ramfs/file1" ,_PC_NAME_MAX);
    if (val == 128) {
        LOGI(TAG, "_PC_NAME_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    success_flag = 0;
    fh = open("/ramfs/file1", 2);
    if (fh < 0) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    val = fpathconf(fh ,_PC_LINK_MAX);
    if (val == 1024) {
        LOGI(TAG, "_PC_LINK_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    close(fh);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for getcwd/chdir */
void posix_dirent_cwp_case(void)
{
    char *ret_p        = NULL;
    int   ret          = -1;
    int   success_flag = 0;

    char *casename = "posix_dirent_cwp_case";

    ret_p = getcwd(path_buffer, MAX_PATH_BYTES);
    if (ret_p != NULL) {
        if (strcmp(path_buffer, "/default") == 0) {
            success_flag = 1;
        }
    }

    if (success_flag != 1) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = chdir("c/my_test_dir/dir1");
    if (ret != 0) {
        LOGI(TAG, "%s test failed2 !\n", casename);
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
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for fstat/stat/fstat */
void posix_dirent_stat_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    struct stat stat_temp;

    char *casename = "posix_dirent_cwp_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    /*******************************************************************************/
    ret = stat("/ramfs/file1", &stat_temp);
    if (ret != 0) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    if (stat_temp.st_size != 10) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    /*******************************************************************************/
    memset(&stat_temp, 0, sizeof(stat_temp));
    ret = fstat(fh, &stat_temp);
    if (ret != 0) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    if (stat_temp.st_size != 10) {
        LOGI(TAG, "%s test failed9 !\n", casename);
        return;
    }

    close(fh);

    /*******************************************************************************/
    struct statfs statfs_temp;
    ret = statfs("/ramfs/file1", &statfs_temp);
    if (ret != 0) {
        LOGI(TAG, "%s test failed10 !\n", casename);
        return;
    }

    if (statfs_temp.f_type != RAMFS_MAGIC) {
        LOGI(TAG, "%s test failed11 !\n", casename);
        return;
    }

    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for link/unlink */
void posix_dirent_link_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_cwp_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
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
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    /*****************************************************/
    fh = open("/ramfs/file2", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed9 !\n", casename);
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file6", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed10 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed11 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed12 !\n", casename);
            return;
        }
    }

    close(fh); 

    /*******************************************************************************/
    ret = unlink("/ramfs/file2");
    if (ret != 0) {
        LOGI(TAG, "%s test failed13 !\n", casename);
        return;
    }

    /*****************************************************/
    fh = open("/ramfs/file3", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed14 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed15 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed16 !\n", casename);
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file4", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed17 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed18 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed19 !\n", casename);
            return;
        }
    }

    close(fh);

    /*****************************************************/
    struct stat stat_buf;
    fh = stat("/ramfs/file2", &stat_buf);

    if (fh == 0) {
        LOGI(TAG, "%s test failed20 !\n", casename);
        return;
    }

    /*******************************************************************************/
    ret = unlink("/ramfs/file4");
    if (ret != 0) {
        LOGI(TAG, "%s test failed21 !\n", casename);
        return;
    }

    /*****************************************************/
   fh = open("/ramfs/file3", 2);
    if (fh < 0) {
        LOGI(TAG, "%s test failed22 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed23 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed24 !\n", casename);
            return;
        }
    }

    close(fh);

    /*****************************************************/
    fh = open("/ramfs/file5", O_RDWR);
    if (fh < 0) {
        LOGI(TAG, "%s test failed25 !\n", casename);
        return;
    }

    memset(buf_r, 0, sizeof(buf_r));
    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed26 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed27 !\n", casename);
            return;
        }
    }

    close(fh);

    /*******************************************************************************/
    ret = unlink("/ramfs/file1");
    if (ret != 0) {
        LOGI(TAG, "%s test failed28 !\n", casename);
        return;
    }

    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for fsync */
void posix_dirent_fsync_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_fseek_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = creat("/ramfs/file1", 0);
    if (fh < 0) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    ret = fsync(fh);
    if (ret != 0) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    close(fh);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for fopen/fread/fwrite/fclose */
void posix_dirent_fopen_case(void)
{
    int   ret   = -1;
    int   i     = 0;
    FILE *file1 = NULL;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_fseek_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = fwrite(buf_w, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    fclose(file1);

    /*******************************************************************************/

    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = fread(buf_r, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    fclose(file1);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for fseek/fseeko */
void posix_dirent_fseek_case(void)
{
    int   ret   = -1;
    int   i     = 0;
    FILE *file1 = NULL;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_fseek_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = fwrite(buf_w, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    fclose(file1);

    /*******************************************************************************/

    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = fseek(file1, 5, SEEK_CUR);
    if (ret != 0) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    ret = fread(buf_r, 5, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed5 !\n", casename);
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (buf_r[i] != (i + 6)) {
            LOGI(TAG, "%s test failed16 !\n", casename);
            return;
        }
    }

    fclose(file1);

   /*******************************************************************************/

    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    ret = fseeko(file1, 5, SEEK_CUR);
    if (ret != 0) {
        LOGI(TAG, "%s test failed8 !\n", casename);
        return;
    }

    ret = fread(buf_r, 5, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed9 !\n", casename);
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (buf_r[i] != (i + 6)) {
            LOGI(TAG, "%s test failed10 !\n", casename);
            return;
        }
    }

    fclose(file1);

    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for fsetpos/fgetpos */
void posix_dirent_fsetpos_case(void)
{
    int   ret   = -1;
    int   i     = 0;
    FILE *file1 = NULL;
    int   pos   = 5;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_fsetpos_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = fwrite(buf_w, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    fclose(file1);

    /*******************************************************************************/

    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = fsetpos(file1, &pos);
    if (ret != 0) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    ret = fread(buf_r, 5, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed5 !\n", casename);
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (buf_r[i] != (i + 6)) {
            LOGI(TAG, "%s test failed6 !\n", casename);
            return;
        }
    }

    ret = fgetpos(file1, &pos);
    if ((ret != 0)||(pos != 10)) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    fclose(file1);

    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for ftell/ftello */
void posix_dirent_ftell_case(void)
{
    int   ret   = -1;
    int   i     = 0;
    FILE *file1 = NULL;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_ftell_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = fwrite(buf_w, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    fclose(file1);

    /*******************************************************************************/

    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    ret = fread(buf_r, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed4 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed5 !\n", casename);
            return;
        }
    }

    ret = ftell(file1);
    if (ret != 10) {
        LOGI(TAG, "%s test failed6 !\n", casename);
        return;
    }

    ret = ftello(file1);
    if (ret != 10) {
        LOGI(TAG, "%s test failed7 !\n", casename);
        return;
    }

    fclose(file1);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}

/* testcase for rewind */
void posix_dirent_rewind_case(void)
{
    int   ret   = -1;
    int   i     = 0;
    FILE *file1 = NULL;

    char buf_w[10];
    char buf_r[10];

    char *casename = "posix_dirent_rewind_case";

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    file1 = fopen("/ramfs/file1", "wb+");
    if (file1 == NULL) {
        LOGI(TAG, "%s test failed1 !\n", casename);
        return;
    }

    ret = fwrite(buf_w, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed2 !\n", casename);
        return;
    }

    rewind(file1);

    ret = fread(buf_r, 10, 1, file1);
    if (ret != 1) {
        LOGI(TAG, "%s test failed3 !\n", casename);
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            LOGI(TAG, "%s test failed3 !\n", casename);
            return;
        }
    }

    fclose(file1);
    remove("/ramfs/file1");

    LOGI(TAG, "%s test succeed !\n", casename);
}
