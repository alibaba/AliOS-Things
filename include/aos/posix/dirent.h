/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef DIRENT_H
#define DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "k_api.h"
#include "fs/vfs.h"
#include "posix_config.h"

#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14

#define TMP_MAX      1024
#define PATH_MAX     64
#define NAME_MAX     64
#define FILESIZEBITS (NAME_MAX * 8)
#define LINK_MAX     0

/* temp file will be saved in this dirctory */
#define TEMP_FILE_NAME_MAGIC "/ramfs/du2s5sz3p1jdi97ds"

typedef aos_dir_t DIR;

struct utimbuf {
    time_t actime;  /* time of last access */
    time_t modtime; /* time of last modify */
};

struct statfs {
    long f_type;    /* fs type */
    long f_bsize;   /* optimized transport block size */
    long f_blocks;  /* total blocks */
    long f_bfree;   /* available blocks */
    long f_bavail;  /* number of blocks that non-super users can acquire */
    long f_files;   /* total number of file nodes */
    long f_ffree;   /* available file nodes */
    long f_fsid;    /* fs id */
    long f_namelen; /* max file name length */
};

struct dirent {
    int     d_ino;    /* file number    */
    uint8_t d_type;   /* type of file   */
    char    d_name[]; /* file name      */
};

/* device io functions, open,read,write are defined by newlib */
/************************************************************************
extern int open(const char *path, int oflag, ...);
extern int close(int fildes);
extern ssize_t read(int fildes, void *buf, size_t nbyte);
extern ssize_t write(int fildes, const void *buf, size_t nbyte);

extern FILE *fopen(const char *restrict filename, const char *restrict mode);
extern FILE *fdopen(int fildes,const char * mode);
extern int fclose(FILE *stream);
extern size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
extern size_t fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
extern int fseek(FILE *stream, long offset, int whence);
extern long ftell(FILE *stream);
extern int fgetpos(FILE *restrict stream, fpos_t *restrict pos);
extern int fsetpos(FILE *stream, const fpos_t *pos);
extern char *fgets(char *restrict s, int n, FILE *restrict stream);
extern int fputs(const char *restrict s, FILE *restrict stream);
extern int access(const char *path, int amode);
extern int fflush(FILE *stream);
extern int fprintf(FILE *restrict stream, const char *restrict format, ...);
**************************************************************************/

DIR *          opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int            closedir(DIR *dirp);
long           telldir(DIR *dirp);
void           seekdir(DIR *dirp, long loc);
off_t          lseek(int fildes, off_t offset, int whence);
int            stat(const char *path, struct stat *buf);
int            fstat(int fh, struct stat *buf);
int            link(const char *path1, const char *path2);
int            unlink(const char *path);
int            remove(const char *filename);
int            rename(const char *oldname, const char *newname);
int            fsync(int fd);
int            fdatasync(int fildes);
int            mkdir(const char *path, mode_t mode);
int            rmdir(const char *path);
void           rewinddir(DIR *dirp);
int            ioctl(int fildes, int request, ... /* arg */);
int            chdir(const char *path);
char          *getcwd(char *buf, size_t size);
int            creat(const char *path, mode_t mode);
long           pathconf(const char *path, int name);
long           fpathconf(int fildes, int name);

/* the bytes of s must bigger than 32 */
char          *tmpnam(char *s);
/* ramfs must be added to the prj if you want to use tmpfile */
FILE          *tmpfile(void);

extern int ioctl(int fildes, int request, ... /* arg */);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DIRENT_H */

