/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "base.h"


void *
lexbor_malloc(size_t size)
{
    return malloc(size);
}

void *
lexbor_realloc(void *dst, size_t size)
{
    return realloc(dst, size);
}

void *
lexbor_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void *
lexbor_free(void *dst)
{
    free(dst);
    return NULL;
}
