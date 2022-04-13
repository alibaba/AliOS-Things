/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "amp_fs.h"

#define JSE_FS_USER_DIR "."

int32_t HAL_Get_User_Dir(char *dir)
{
    memcpy(dir, JSE_FS_USER_DIR, strlen(JSE_FS_USER_DIR));
    return 0;
}

int32_t HAL_Finit(void)
{
    return 0;
}

void *HAL_Fopen(const char *path, const char *mode)
{
    return (void *)fopen(path, mode);
}

int32_t HAL_Fclose(void *stream)
{
    return fclose((FILE *)stream);
}

int32_t HAL_Fseek(void *stream, int32_t offset, int32_t whence, int32_t *curpos)
{
    *curpos = -1;
    return fseek((FILE *)stream, offset, whence);
}

int32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return fread(buff, (size_t)size, (size_t)count, (FILE *)stream);
}

int32_t HAL_Fwrite(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return fwrite(buff, size, count, stream);
}

int32_t HAL_Fsync(void *stream)
{
    return 0;
}

int32_t HAL_stat(const char *path, struct hal_stat *st)
{
	struct stat _stat;
    int ret = stat(path, &_stat);
	st->st_mode = _stat.st_mode;
	st->st_size = _stat.st_size;
	return ret;
}

int32_t HAL_Funlink(const char *path)
{
    return unlink(path);
}

int32_t HAL_Fmkdir(const char *path)
{
    return mkdir(path, mkdir);
}

int32_t HAL_Frmdir(const char *path)
{
    return rmdir(path);
}


long HAL_Ftell(void *stream)
{
    return ftell((FILE *)stream);
}

char HAL_S_ISREG(uint32_t mode)
{
	return S_ISREG(mode);
}