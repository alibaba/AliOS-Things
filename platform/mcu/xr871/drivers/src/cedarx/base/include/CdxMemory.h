/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMemory.h
 * Description : Memory
 * History :
 *
 */

#ifndef CDX_MEMORY_H
#define CDX_MEMORY_H
#include <stdlib.h>
#include <string.h>

#define CdxMalloc(size) malloc(size)
#define CdxFree(ptr) free(ptr)
#define CdxRealloc(ptr, size) realloc(ptr, size)

//#define CdxMemcpy(dest, src, n) memcpy(dest, src, n)
//#define CdxMemset(s, c, n) memset(s, c, n)
#define CdxStrdup(s) strdup(s)
//#define CdxStrlen(s) strlen(s)
#endif
