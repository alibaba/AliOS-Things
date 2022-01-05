/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>
#include <string.h>
#include "esp_vfs.h"
#define VFS_PATH_MAX 256
static char g_current_working_directory[VFS_PATH_MAX] = "/";

char *getcwd(char *buf, size_t size)
{
    if (buf == NULL) {
        return NULL;
    }
    strncpy(buf, g_current_working_directory, strlen(g_current_working_directory) + 1);
    return buf;
}

int chdir(const char *path)
{
    if ((path == NULL) || (strlen(path) > VFS_PATH_MAX)) {
        return -1;
    }
    memset(g_current_working_directory, 0, sizeof(g_current_working_directory));
    strncpy(g_current_working_directory, path, strlen(path) + 1);
    return 0;
}

int statfs(const char *path, struct statfs *buf)
{
    int ret;
    ret = esp_vfs_statfs(path, buf);
    return ret;
}
