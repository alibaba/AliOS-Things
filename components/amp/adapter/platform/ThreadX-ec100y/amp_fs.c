/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "ql_fs.h"
#include "wrappers_defs.h"
#include "amp_fs.h"

#define JSE_FS_USER_DIR "U:"

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
    return ql_fopen(path, mode);
}

int32_t HAL_Fclose(void *stream)
{
    return ql_fclose((QFILE *)stream);
}

int32_t HAL_Fseek(void *stream, int32_t offset, int32_t whence, int32_t *curpos)
{
    int _whence;
    *curpos = -1;
    switch (whence) {
    case HAL_SEEK_SET:
        _whence = SEEK_SET;
        break;
    case HAL_SEEK_CUR:
        _whence = SEEK_CUR;
        break;
    case HAL_SEEK_END:
        _whence = SEEK_END;
        break;
    default:
        return -1;
    }
    return ql_fseek((QFILE *)stream, offset, _whence);
}

int32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return ql_fread(buff, (size_t)size, (size_t)count, (QFILE *)stream);
}

int32_t HAL_Fwrite(void *buff, uint32_t size, uint32_t count, void *stream)
{
    return ql_fwrite(buff, (size_t)size, (size_t)count, (QFILE *)stream);
}

int32_t HAL_Fsync(void *stream)
{
    return ql_fsync((QFILE *)stream);
}

int32_t HAL_Fremove(const char *path)
{
	return ql_remove(path);
}

int32_t HAL_stat(const char *path, struct hal_stat *st)
{
    struct lfs_info info;
    int ret = ql_stat(path, &info);
    st->st_mode = info.type;
    st->st_size = info.size;
    return ret;
}

int32_t HAL_Funlink(const char *path)
{
    return ql_remove(path);
}

int32_t HAL_Fmkdir(const char *path)
{
    return ql_mkdir(path, 0x777);
}

int32_t HAL_Frmdir(const char *path)
{
    return ql_remove(path);
}

long HAL_Ftell(void *stream)
{
    return ql_ftell((QFILE *)stream);
}

char HAL_S_ISREG(uint32_t mode)
{
	return 1;
}