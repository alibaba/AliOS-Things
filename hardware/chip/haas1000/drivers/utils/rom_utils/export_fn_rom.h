/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __EXPORT_FN_ROM_H__
#define __EXPORT_FN_ROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdarg.h"

typedef void (*__BOOT_FROM_FLASH_REENT)(void);
typedef unsigned long (*__CRC32)(unsigned long crc, const unsigned char *buf, unsigned int len);
typedef int (*__VERIFY_SIGNATURE)(const unsigned char *key, const unsigned char *sig, const unsigned char *data, unsigned int len);
typedef const uint8_t* (*__SHA256_HASH)(const void* data, uint32_t len, uint8_t* digest);
typedef void (*__SHA256_INIT)(void* ctx);
typedef void (*__SHA256_UPDATE)(void* ctx, const void* data, uint32_t len);
typedef const uint8_t* (*__SHA256_FINAL)(void* ctx);

typedef char * (*__STRSEP)(char **stringp, const char *delim);
typedef char * (*__STRTOK_R)(char *,const char *, char **);
typedef char * (*__STRCPY)(char *,const char *);
typedef char * (*__STRNCPY)(char *,const char *, size_t);
typedef char * (*__STRCAT)(char *, const char *);
typedef char * (*__STRNCAT)(char *, const char *,size_t);
typedef int (*__STRCMP)(const char *,const char *);
typedef int (*__STRNCMP)(const char *,const char *,size_t);
typedef int (*__STRNICMP)(const char *, const char *, size_t);
typedef char * (*__STRCHR)(const char *,int);
typedef char * (*__STRRCHR)(const char *,int);
typedef char * (*__STRSTR)(const char *,const char *);
typedef size_t (*__STRLEN)(const char *);
typedef size_t (*__STRNLEN)(const char *,size_t);
typedef void * (*__MEMSET)(void *,int,size_t);
typedef void * (*__MEMCPY)(void *,const void *,size_t);
typedef void * (*__MEMMOVE)(void *,const void *,size_t);
typedef void * (*__MEMSCAN)(void *,int,size_t);
typedef int (*__MEMCMP)(const void *,const void *,size_t);
typedef void * (*__MEMCHR)(const void *,int,size_t);
typedef size_t  (*__STRCSPN)(const char *s, const char *reject);
typedef size_t  (*__STRSPN)(const char *s1, const char *s2);
typedef int (*__SPRINTF)(char * buf, const char *fmt, ...);
typedef int (*__SNPRINTF)(char * buf, size_t len, const char *fmt, ...);
typedef int (*__SSCANF)(const char * buf, const char * fmt, ...);
typedef int (*__VSPRINTF)(char *buf, const char *fmt, va_list ap);
typedef int (*__VSNPRINTF)(char *buf, size_t size, const char *fmt, va_list ap);
typedef int (*__VSSCANF)(const char *fp, const char *fmt0, va_list ap);
typedef int (*__ATOI)(const char *s);
typedef long (*__STRTOL)(const char *nptr, char **endptr, int base);
typedef unsigned long (*__STRTOUL)(const char *nptr, char **endptr, int base);
typedef void (*__QSORT)(void *baseP, size_t nElem, size_t width, int (*compar)(const void *, const void *));
typedef void * (*__BSEARCH)(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

struct EXPORT_FN_ROM_T {
    __BOOT_FROM_FLASH_REENT boot_from_flash_reent;
    __CRC32 crc32;
    __VERIFY_SIGNATURE verify_signature;
    __SHA256_HASH SHA256_hash;
    __SHA256_INIT SHA256_init;
    __SHA256_UPDATE SHA256_update;
    __SHA256_FINAL SHA256_final;

    __STRSEP strsep;
    __STRTOK_R strtok_r;
    __STRNCAT strncat;
    __STRNICMP strnicmp;
    __STRSTR strstr;
    __MEMSET memset;
    __MEMCPY memcpy;
    __MEMMOVE memmove;
    __ATOI atoi;
    __STRTOL strtol;
    __STRTOUL strtoul;
    __QSORT qsort;
    __BSEARCH bsearch;
    __SPRINTF sprintf;
    __SNPRINTF snprintf;
    __SSCANF sscanf;
    __VSPRINTF vsprintf;
    __VSNPRINTF vsnprintf;
    __VSSCANF vsscanf;

#ifdef CHIP_HAS_AUDIO_CONST_ROM
    const unsigned int *audio_const;
#endif
};

extern struct EXPORT_FN_ROM_T __export_fn_rom;

#ifdef __cplusplus
}
#endif

#endif

