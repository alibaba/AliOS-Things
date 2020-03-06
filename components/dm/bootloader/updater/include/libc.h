/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_LIBC_H
#define OTA_LIBC_H

#define ALIGN(x,a)                 (((x) + (a) - 1) & ~((a) - 1))
#define XZ_HEAP_ALIGNMENT          4
#define XZ_HEAP_BLK_HEAD_SIZE      ALIGN(sizeof(blk_head_t), XZ_HEAP_ALIGNMENT)
#define XZ_HEAP_BLK_MIN            ((XZ_HEAP_BLK_HEAD_SIZE) << 1)
#define XZ_HEAP_MAGIC              (0xF0000000)

#define OTA_LOG_I(fmt, ...) printf("%d: "fmt"\r\n", __LINE__, ##__VA_ARGS__)

#if defined __LINUX_HOST__
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#else
extern void *_rec_heap_start;
extern void *_rec_heap_len;
#define XZ_HEAP_BASE ((int)&_rec_heap_start)
#define XZ_HEAP_SIZE ((int)&_rec_heap_len)

#define memset  ota_memset
#define memcpy  ota_memcpy
#define memcmp  ota_memcmp
#define memmove ota_memmove
#define strlen  ota_strlen
#define strcat  ota_strcat
#define puts    ota_puts
#define printf  ota_printf
#define malloc  ota_heap_malloc
#define free    ota_heap_free
#endif

void *ota_memset(void *s, int c, unsigned int n);
void *ota_memcpy(void *dest, const void *src, unsigned int n);
int ota_memcmp(const void * buf1, const void * buf2, unsigned int count);
void *ota_memmove(void *dest, const void *src, unsigned int count);
unsigned int ota_strlen(char * str);
char *ota_strcat (char * dst, const char * src);
int ota_puts(const char *str);
int ota_printf(const char *format, ...);
void *ota_heap_malloc(unsigned int alloc_size);
void ota_heap_free(void *pfree);

typedef struct {
    unsigned short crc;
} CRC16_CTX;

void crc16_init(CRC16_CTX *inContext);
void crc16_update(CRC16_CTX *inContext, const void *inSrc, long inLen);
void crc16_final(CRC16_CTX *inContext, unsigned short *outResult);
unsigned short crc16_computer(void *buf, long len);
#endif /* _OTA_LIBC_H_ */
