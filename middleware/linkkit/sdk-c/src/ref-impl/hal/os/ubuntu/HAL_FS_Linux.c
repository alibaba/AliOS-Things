/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>


void *HAL_Fopen(const char *path, const char *mode)
{
    return (void *)fopen(path,mode);
}

size_t HAL_Fread(void * buff,size_t size, size_t count, void *stream)
{
    return fread(buff,size,count,(FILE *)stream);
}
size_t HAL_Fwrite(const void * ptr, size_t size, size_t count, void * stream)
{
    return fwrite (ptr, size, count,(FILE *)stream);
}

int HAL_Fseek(void *stream,long offset,int framewhere)
{
    return fseek((FILE *)stream, offset, framewhere);
}

int HAL_Fclose(FILE *stream)
{
    return fclose((FILE *)stream);
}

long HAL_Ftell(void *stream)
{
    return ftell((FILE *)stream);
}
