#ifndef _SYS_STATVFS_H_
#define _SYS_STATVFS_H_

struct statvfs
{
    unsigned long f_bsize;   /* file system block size */
    unsigned long f_frsize;  /* fragment size */
    unsigned long f_blocks;  /* size of fs in f_frsize units */
    unsigned long f_bfree;   /* free blocks in fs */
    unsigned long f_bavail;  /* free blocks avail to non-superuser */
    unsigned long f_files;   /* total file nodes in file system */
    unsigned long f_ffree;   /* free file nodes in fs */
    unsigned long f_favail;  /* avail file nodes in fs */
    unsigned long f_fsid;    /* file system id */
    unsigned long f_flag;    /* mount flags */
    unsigned long f_namemax; /* maximum length of filenames */
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int statvfs(const char *__path, struct statvfs *__buf);
int fstatvfs(int __fd, struct statvfs *__buf);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /*_SYS_STATVFS_H_*/
