/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/* dirent.h - POSIX directory handling definitions */

#ifndef __INC_DIRENT_H__
#define __INC_DIRENT_H__

#include <k_api.h>
#include <vfs.h>
#include <stdio.h>
#include <vfs_inode.h>


#ifdef __cplusplus
extern "C"
{
#endif

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

    /* function declarations */

    typedef aos_dir_t DIR;

    struct dirent
    {
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

    extern FILE *fopen(const char *restrict filename, const char *restrict
    mode); extern FILE *fdopen(int fildes,const char * mode); extern int
    fclose(FILE *stream); extern size_t fread(void *restrict ptr, size_t size,
    size_t nitems, FILE *restrict stream); extern size_t fwrite(const void
    *restrict ptr, size_t size, size_t nitems, FILE *restrict stream); extern
    int fseek(FILE *stream, long offset, int whence); extern long ftell(FILE
    *stream); extern int fgetpos(FILE *restrict stream, fpos_t *restrict pos);
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
    int            statfs(const char *path, struct statfs *buf);
    int            unlink(const char *path);
    int            remove(const char *filename);
    int            rename(const char *oldname, const char *newname);
    int            fsync(int fd);
    int            mkdir(const char *path, mode_t mode);
    int            rmdir(const char *path);
    void           rewinddir(DIR *dirp);
    int            ioctl(int fildes, int request, ... /* arg */);

    extern int ioctl(int fildes, int request, ... /* arg */);

#ifdef __cplusplus
}
#endif

#endif /* __INC_DIRENT_H__ */
