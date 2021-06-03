/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_H
#define LEXBOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"


LXB_API void *
lexbor_malloc(size_t size);

LXB_API void *
lexbor_realloc(void *dst, size_t size);

LXB_API void *
lexbor_calloc(size_t num, size_t size);

LXB_API void *
lexbor_free(void *dst);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_H */

