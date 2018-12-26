/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "k_api.h"
#include "posix/dirent.h"
#include "ramfs_types.h"

#define MAX_PATH_BYTES 32

char path_buffer[MAX_PATH_BYTES];

void posix_dirent_creat_case(void);
void posix_dirent_access_case(void);
void posix_dirent_utime_case(void);
void posix_dirent_rename_case(void);
void posix_dirent_remove_case(void);
void posix_dirent_dir_case(void);
void posix_dirent_pathconf_case(void);
void posix_dirent_cwp_case(void);
void posix_dirent_stat_case(void);
void posix_dirent_link_case(void);

void posix_dirent_case(void)
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
}

/* testcase for access */
void posix_dirent_access_case(void)
{
    int ret = -1;
    int fh  = -1;

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_access_case test failed1 !\n");
        return;
    }

    ret = access("/ramfs/file1" ,F_OK);
    if (ret != 0) {
        printf("posix_dirent_access_case test failed2 !\n");
        return;
    }

    close(fh);

    ret = remove("/ramfs/file1");
    if (ret != 0) {
        printf("posix_dirent_access_case test failed3 !\n");
        return;
    }

    ret = access("/ramfs/file1" ,F_OK);
    if (ret == 0) {
        printf("posix_dirent_access_case test failed4 !\n");
        return;
    }

    remove("/ramfs/file1");

    printf("posix_dirent_access_case test succeed !\n");
}


/* testcase for utime */
void posix_dirent_utime_case(void)
{
    struct utimbuf utimbuf_temp;

    utimbuf_temp.actime = 1234;
    utimbuf_temp.modtime = 5678;

    utime("/ramfs/file1", &utimbuf_temp);
}

/* testcase for creat */
void posix_dirent_creat_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    struct stat stat_temp;

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = creat("/ramfs/file1", 0);
    if (fh < 0) {
        printf("posix_dirent_creat_case test failed1 !\n");
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        printf("posix_dirent_creat_case test failed2 !\n");
        return;
    }

    close(fh);

    /*******************************************************************************/

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_creat_case test failed3 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_creat_case test failed4 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_creat_case test failed5 !\n");
            return;
        }
    }

    close(fh);
    remove("/ramfs/file1");

    printf("posix_dirent_creat_case test succeed !\n");
}

/* testcase for opendir/closedir/mkdir/readdir/rmdir/rewinddir */
void posix_dirent_remove_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    struct stat stat_temp;

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_remove_case test failed1 !\n");
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        printf("posix_dirent_remove_case test failed2 !\n");
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_remove_case test failed3 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_remove_case test failed4 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_remove_case test failed5 !\n");
            return;
        }
    }

    close(fh);

    /*******************************************************************************/

    ret = remove("/ramfs/file1");
    if (ret != 0) {
        printf("posix_dirent_remove_case test failed6 !\n");
        return;
    }

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_remove_case test failed7 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret == 10) {
        printf("posix_dirent_remove_case test failed8 !\n");
        return;
    }

    close(fh);
    remove("/ramfs/file1");

    printf("posix_dirent_remove_case test succeed !\n");
}

void posix_dirent_dir_case(void)
{
    DIR           *dir;
    struct dirent * ptr;

    int i   = 0;
    int ret = -1;
    int fh[11];

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
            printf("posix_dirent_dir_case test failed1 !\n");
            return;
        }
    }

    ret = mkdir("/ramfs/folder1/folder1_1/folder1_1_3", 0);
    ret |= mkdir("/ramfs/folder4/folder4_1", 0);
    if (ret != 0) {
        printf("posix_dirent_dir_case test failed2 !\n");
        return;
    }

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        printf("posix_dirent_dir_case test failed3 !\n");
        return;
    }

    printf("ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            printf("d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs/folder1");
    if (dir == NULL) {
        printf("posix_dirent_dir_case test failed4 !\n");
        return;
    }

    printf("ls /ramfs/folder1:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            printf("d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs/folder1/folder1_1");
    if (dir == NULL) {
        printf("posix_dirent_dir_case test failed5 !\n");
        return;
    }

    printf("ls /ramfs/folder1/folder1_1:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            printf("d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    closedir(dir);

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        printf("posix_dirent_dir_case test failed6 !\n");
        return;
    }

    printf("ls /ramfs(3 files):\n");

    ptr = readdir(dir);
    printf("d_name : %s\n", ptr->d_name);

    ptr = readdir(dir);
    printf("d_name : %s\n", ptr->d_name);

    ptr = readdir(dir);
    printf("d_name : %s\n", ptr->d_name);

    printf("rewinddir dir !\n");
    rewinddir(dir);

    printf("ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            printf("d_name : %s\n", ptr->d_name);
        }
    } while(ptr != NULL);

    /*******************************************************************************/

    dir = opendir("/ramfs");
    if (dir == NULL) {
        printf("posix_dirent_dir_case test failed7 !\n");
        return;
    }

    close(fh[1]);
    printf("remove /ramfs/file2\n");
    ret = remove("/ramfs/file2");
    if (ret != 0) {
        printf("posix_dirent_dir_case test failed8 !\n");
        return;
    }

    close(fh[3]);

    printf("remove /ramfs/file4\n");
    ret = remove("/ramfs/file4");
    if (ret != 0) {
        printf("posix_dirent_dir_case test failed9 !\n");
        return;
    }

    printf("rmdir /ramfs/folder4/folder4_1\n");
    ret = rmdir("/ramfs/folder4/folder4_1");
    if (ret != 0) {
        printf("posix_dirent_dir_case test failed10 !\n");
        return;
    }

    printf("rmdir /ramfs/folder4\n");
    ret = rmdir("/ramfs/folder4");
    if (ret != 0) {
        printf("posix_dirent_dir_case test failed11 !\n");
        return;
    }

    printf("ls /ramfs:\n");
    do {
        ptr = readdir(dir);
        if (ptr != NULL) {
            printf("d_name : %s\n", ptr->d_name);
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

    struct stat stat_temp;

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_rename_case test failed1 !\n");
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        printf("posix_dirent_rename_case test failed2 !\n");
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_rename_case test failed3 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_rename_case test failed4 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_rename_case test failed5 !\n");
            return;
        }
    }

    close(fh);

    /*******************************************************************************/

    ret = rename("/ramfs/file1", "/ramfs/file2");
    if (ret != 0) {
        printf("posix_dirent_rename_case test failed6 !\n");
        return;
    }

    fh = open("/ramfs/file2", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_rename_case test failed7 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_rename_case test failed8 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_rename_case test failed9 !\n");
            return;
        }
    }

    close(fh);
    remove("/ramfs/file1");
    remove("/ramfs/file2");

    printf("posix_dirent_rename_case test succeed !\n");
}

/* testcase for pathconf/pathconf */
void posix_dirent_pathconf_case(void)
{
    int val          = 0;
    int success_flag = 0;
    int fh           = -1;

    val = pathconf("/ramfs/file1" ,_PC_NAME_MAX);
    if (val == 128) {
        printf("_PC_NAME_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        printf("posix_dirent_pathconf_test failed1 !\n");
        return;
    }

    success_flag = 0;
    fh = open("/ramfs/file1", 2);
    if (fh < 0) {
        printf("posix_dirent_pathconf_test failed2 !\n");
        return;
    }

    val = fpathconf(fh ,_PC_LINK_MAX);
    if (val == 1024) {
        printf("_PC_LINK_MAX is %d\n", val);
        success_flag = 1;
    }

    if (success_flag != 1) {
        printf("posix_dirent_pathconf_test failed3 !\n");
        return;
    }

    close(fh);
    remove("/ramfs/file1");

    printf("posix_dirent_pathconf_test succeed !\n");
}

/* testcase for getcwd/chdir */
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

/* testcase for fstat/stat/fstat */
void posix_dirent_stat_case(void)
{
    int ret = -1;
    int fh  = -1;
    int i   = 0;

    char buf_w[10];
    char buf_r[10];

    struct stat stat_temp;

    for(i = 0; i < 10; i++) {
        buf_w[i] = i + 1;
        buf_r[i] = 0;
    }

    /*******************************************************************************/
    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_stat_case test failed1 !\n");
        return;
    }

    ret = write(fh, buf_w, 10);
    if (ret != 10) {
        printf("posix_dirent_stat_case test failed2 !\n");
        return;
    }

    close(fh);

    fh = open("/ramfs/file1", O_RDWR);
    if (fh < 0) {
        printf("posix_dirent_stat_case test failed3 !\n");
        return;
    }

    ret = read(fh, buf_r, 10);
    if (ret != 10) {
        printf("posix_dirent_stat_case test failed4 !\n");
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (buf_r[i] != (i + 1)) {
            printf("posix_dirent_stat_case test failed5 !\n");
            return;
        }
    }

    /*******************************************************************************/
    ret = stat("/ramfs/file1", &stat_temp);
    if (ret != 0) {
        printf("posix_dirent_stat_case test failed6 !\n");
        return;
    }

    if (stat_temp.st_size != 10) {
        printf("posix_dirent_stat_case test failed7 !\n");
        return;
    }

    /*******************************************************************************/
    memset(&stat_temp, 0, sizeof(stat_temp));
    ret = fstat(fh, &stat_temp);
    if (ret != 0) {
        printf("posix_dirent_stat_case test failed8 !\n");
        return;
    }

    if (stat_temp.st_size != 10) {
        printf("posix_dirent_stat_case test failed9 !\n");
        return;
    }

    close(fh);

    /*******************************************************************************/
    struct statfs statfs_temp;
    ret = statfs("/ramfs/file1", &statfs_temp);
    if (ret != 0) {
        printf("posix_dirent_stat_case test failed10 !\n");
        return;
    }

    if (statfs_temp.f_type != RAMFS_MAGIC) {
        printf("posix_dirent_stat_case test failed11 !\n");
        return;
    }

    remove("/ramfs/file1");

    printf("posix_dirent_stat_case test succeed !\n");
}

/* testcase for link/unlink */
void posix_dirent_link_case(void)
{
    int ret = -1;
    int fh  = -1;
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

    remove("/ramfs/file1");

    printf("posix_dirent_link_case test succeed !\n");
}
