/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>


void *HAL_Fopen(const char *path, const char *mode)
{
    return NULL;
}

size_t HAL_Fread(void * buff,size_t size, size_t count, void *stream)
{
    return 0;
}
size_t HAL_Fwrite(const void * ptr, size_t size, size_t count, void * stream)
{
    return 0;
}

int HAL_Fseek(void *stream,long offset,int framewhere)
{
    return 0;
}

int HAL_Fclose(FILE *stream)
{
    return 0;
}

long HAL_Ftell(void *stream)
{
    return 0;
}
