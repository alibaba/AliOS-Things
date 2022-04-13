/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "infra_config.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>

#include "aos/vfs.h"
#include "vfs_types.h"
// #include "fs/spiffs.h"
#include "amp_fs.h"
#include "wrappers_defs.h"

#define JSE_FS_USER_DIR "/data/"

int32_t HAL_Get_User_Dir(char *dir)
{
    memcpy(dir, JSE_FS_USER_DIR, strlen(JSE_FS_USER_DIR));
    return 0;
}

int32_t HAL_Finit(void)
{
    // return spiffs_register();
    return 0;
}

void *HAL_Fopen(const char *path, const char *mode)
{
    int aos_mode = 0;
    int fd;

    if (strcmp(mode, "r") == 0) {
        aos_mode = O_RDONLY;
    } else if(strcmp(mode, "w") == 0) {
        aos_mode = O_WRONLY | O_TRUNC;
    } else if(strcmp(mode, "w+") == 0) {
        aos_mode = O_RDWR | O_CREAT | O_TRUNC;
    }

    fd = aos_open(path, aos_mode);
    if (fd < 0) {
        printf("open %s failed %d!\n", path, fd);
        return NULL;
    }

    return (void *)fd;
}

int32_t HAL_Fclose(void *stream)
{
    return aos_close((int)stream);
}

int32_t HAL_Fseek(void *stream, int32_t offset, int32_t whence, int32_t *curpos)
{
    int32_t aos_whence;
    int len;

    switch (whence)
    {
    case HAL_SEEK_SET:
        aos_whence = SEEK_SET;
        break;
    case HAL_SEEK_CUR:
        aos_whence = SEEK_CUR;
        break;
    case HAL_SEEK_END:
        aos_whence = SEEK_END;
        break;
    default:
        *curpos = 0;
        return -1;
    }

    len = (int)aos_lseek((int)stream, offset, aos_whence);
    if (len < 0) {
        *curpos = 0;
        return -1;
    }
    *curpos = len;
    return 0;
}

int32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return aos_read((int)stream, buff, (size_t)count);
}

int32_t HAL_Fwrite(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return aos_write((int)stream, buff, (size_t)count);
}

int32_t HAL_Fsync(void *stream)
{
    return aos_sync((int)stream);
}

int32_t HAL_stat(const char *path, struct hal_stat *st)
{
	vfs_stat_t _stat;
    int ret = aos_stat(path, &_stat);
	st->st_mode = _stat.st_mode;
	st->st_size = _stat.st_size;
	return ret;
}

int32_t HAL_Funlink(const char *path)
{
    return aos_unlink(path);
}

int32_t HAL_Fmkdir(const char *path)
{
    return aos_mkdir(path);
}

int32_t HAL_Frmdir(const char *path)
{
    return aos_rmdir(path);
}

long HAL_Ftell(void *stream)
{
    return aos_lseek((int)stream, 0, SEEK_CUR);
}

char HAL_S_ISREG(uint32_t mode)
{
    return (char)(8 & mode);
}