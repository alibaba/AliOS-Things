/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef _BE_OSAL_FS_H
#define _BE_OSAL_FS_H

#if defined(__cplusplus)
 extern "C" {
#endif

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>


 typedef struct {
	 int dd_vfs_fd;
	 int dd_rsv;
 } be_osal_dir_t;

 typedef struct {
	 int	 d_ino;    /* file number */
	 uint8_t d_type;   /* type of file */
	 char	 d_name[]; /* file name */
 } be_osal_dirent_t;

#define BE_FS_ROOT_DIR "/spiffs"

int be_osal_spiffs_init(void);
int be_osal_spiffs_cleanup(void);
int be_osal_open(const char *path, int flags);
int be_osal_close(int fd);
ssize_t be_osal_read(int fd, void *buf, size_t nbytes);
ssize_t be_osal_write(int fd, const void *buf, size_t nbytes);
int be_osal_sync(int fd);
be_osal_dir_t *be_osal_opendir(const char *path);
be_osal_dirent_t *be_osal_readdir(be_osal_dir_t *dir);
int be_osal_closedir(be_osal_dir_t *dir);
int be_osal_rename(const char *oldpath, const char *newpath);
int be_osal_unlink(const char *path);
int be_osal_stat(const char *path, struct stat *st);
off_t be_osal_lseek(int fd, off_t offset, int whence);
int be_osal_mkdir(const char *path);
int be_osal_kv_get(const char *key, void *buffer, int *buffer_len);
int be_osal_kv_set(const char *key, const void *value, int len, int sync);
int be_osal_kv_delete(const char *key);

#if defined(__cplusplus)
 }
#endif
#endif

