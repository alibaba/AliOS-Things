/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "aos_fs.h"
#include "aos/vfs.h"
#include "vfs_types.h"

int aos_fs_init(void)
{
    return 0;
}

int aos_rmdir_r(const char *path)
{
    struct aos_stat s;
    int    ret = -1;
    char  *dir, *p;
    int    path_len;

    aos_dir_t      *pdir  = NULL;
    aos_dirent_t   *entry = NULL;

    if (!path)
        return -EINVAL;

    path_len = strlen(path) + 1;
    dir      = aos_malloc(path_len);
    if (dir == NULL) {
        return -1;
    }
    memcpy(dir, path, path_len);

    p = dir + strlen(dir) - 1;
    while ((*p == '/') && (p > dir)) {
        *p = '\0';
        p--;
    }

    if (aos_stat(dir, &s) || !S_ISDIR(s.st_mode)) {
        aos_printf("%s is neither existed nor a directory\n", dir);
        goto out;
    }

    pdir = aos_opendir(dir);
    if (!pdir) {
        aos_printf("opendir %s failed - %s\n", dir, strerror(errno));
        goto out;
    }

    ret = 0;
    while ((ret == 0) && (entry = aos_readdir(pdir))) {
        char fpath[128];

        snprintf(fpath, 128, "%s/%s", dir, entry->d_name);

        ret = aos_stat(fpath, &s);
        if (ret) {
            aos_printf("stat %s failed\n", fpath);
            break;
        }

        if (!strcmp(entry->d_name, "."))
            continue;
        if (!strcmp(entry->d_name, ".."))
            continue;

        if (S_ISDIR(s.st_mode))
            ret = aos_rmdir_r(fpath);
        else
            ret = aos_unlink(fpath);
    }

    aos_closedir(pdir);
    if (ret == 0) {
        ret = aos_rmdir(dir);
        if (ret)
            aos_printf("rmdir %s failed\n", dir);
    }
out:
    aos_free(dir);
    return ret;
}

