/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _SYS_STATFS_H
#define _SYS_STATFS_H

#ifdef __cplusplus
extern "C" {
#endif

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

int statfs(const char *path, struct statfs *buf);

#ifdef __cplusplus
}
#endif

#endif /*_SYS_STATFS_H*/
