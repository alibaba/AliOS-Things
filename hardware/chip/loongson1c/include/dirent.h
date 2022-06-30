/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef _DIRENT_H
#define _DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/stat.h>
#include <aos/vfs.h>

/* file types. */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14

#define DIRSIZ(dp) (offsetof(struct dirent, d_name) + ((strlen((dp)->d_name) + 1 + 3) & ~3))

typedef aos_dir_t DIR;

struct dirent {
    int     d_ino;    /* file number    */
    uint8_t d_type;   /* type of file   */
    char    d_name[]; /* file name      */
};

DIR *          opendir(const char *dirname);
int            closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);
long           telldir(DIR *dirp);
void           seekdir(DIR *dirp, long loc);
int            mkdir(const char *path, mode_t mode);
int            rmdir(const char *path);
void           rewinddir(DIR *dirp);
int            chdir(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H */
