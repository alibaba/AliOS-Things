/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
 
#define _BE_OSAL_FS_C
#include <aos/aos.h>
#include "be_osal_fs.h"
#include <aos_spiffs.h>

int be_osal_spiffs_init(void)
{
    int ret = -1;
	ret = vfs_spiffs_register();
    return ret;
}

int be_osal_spiffs_cleanup(void)
{
    int ret = -1;
	ret = vfs_spiffs_unregister();
    return ret;
}


int be_osal_open(const char *path, int flags)
{
	return aos_open( path, flags);
}

int be_osal_close(int fd)
{
	return aos_close(fd);
}


ssize_t be_osal_read(int fd, void *buf, size_t nbytes)
{
	return aos_read(fd,buf,nbytes);
}

ssize_t be_osal_write(int fd, const void *buf, size_t nbytes)
{
	return aos_write(fd,buf,nbytes);
}

int be_osal_sync(int fd)
{
	return aos_sync(fd);
}

be_osal_dir_t *be_osal_opendir(const char *path)
{
	return (be_osal_dir_t *)aos_opendir(path);
}


be_osal_dirent_t *be_osal_readdir(be_osal_dir_t *dir)
{
	return (be_osal_dirent_t *)aos_readdir((aos_dir_t *)dir);
}


int be_osal_closedir(be_osal_dir_t *dir)
{
	return aos_closedir((aos_dir_t *)dir);
}


int be_osal_rename(const char *oldpath, const char *newpath)
{
	return  aos_rename(oldpath, newpath);
}

int be_osal_unlink(const char *path)
{
	return  aos_unlink(path);
}


int be_osal_stat(const char *path, struct stat *st)
{
	return aos_stat(path,st);
}

off_t be_osal_lseek(int fd, off_t offset, int whence){

	return aos_lseek(fd,offset,whence);
}

int be_osal_mkdir(const char *path)
{
    return aos_mkdir(path);
}

int be_osal_kv_get(const char *key, void *buffer, int *buffer_len)
{
	return aos_kv_get(key, buffer, buffer_len);
}

int be_osal_kv_set(const char *key, const void *value, int len, int sync)
{
	return aos_kv_set(key, value, len, sync);
}

int be_osal_kv_delete(const char *key)
{
	return aos_kv_del(key);
}


#undef _BE_OSAL_FS_C
