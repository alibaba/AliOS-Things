/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include "libc.h"
#include "updater.h"

#if !defined __LINUX_HOST__
#include "2ndboot.h"

typedef struct blklist {
    struct blklist *next;
    unsigned int magic_size;
} blk_head_t;

static blk_head_t freelist_head;
static blk_head_t *freelist_tail = NULL;
static unsigned int heap_free_size      = XZ_HEAP_SIZE;
static unsigned int heap_free_size_min  = XZ_HEAP_SIZE;
#endif

/* memory & string operation */
void *ota_memcpy(void *dest, const void *src, unsigned int n)
{
    if (NULL == dest || NULL == src)
        return NULL;
    char *tempDest = (char *)dest;
    char *tempSrc = (char *)src;

    while (n-- > 0)
        *tempDest++ = *tempSrc++;
    return dest;
}

void *ota_memset(void *s, int c, unsigned int n)
{
    if (NULL == s)
        return NULL;
    char * tmpS = (char *)s;
    while(n-- > 0)
        *tmpS++ = c;
        return s;
}

unsigned int ota_strlen(char * str)
{
    const char *eos = str;
    while(*eos++) ;
    return (eos - str - 1);
}

char *ota_strcat (char * dst, const char * src)
{
    char * cp = dst;
    while( *cp )
        cp++;

    while( cp && src) {
        *cp++ = *src++;
    }
    return dst;
}

int ota_memcmp(const void * buf1, const void * buf2, unsigned int count)
{
    if (!count)
        return(0);

    while ( --count && *(char *)buf1 == *(char *)buf2 ) {
        buf1 = (char *)buf1 + 1;
        buf2 = (char *)buf2 + 1;
    }
    return (*((unsigned char *)buf1) - *((unsigned char *)buf2));
}

void *ota_memmove(void *dest, const void *src, unsigned int count)
{
    char *tmp;
    const char *s;

    if (dest <= src) {
        tmp = dest;
        s = src;
        while (count--)
            *tmp++ = *s++;
    } else {
        tmp = dest;
        tmp += count;
        s = src;
        s += count;
        while (count--)
            *--tmp = *--s;
    }
    return dest;
}

#if !defined __LINUX_HOST__
/* PRINTF implement */
int ota_puts(const char *str)
{
    uart_send_string((char *)str);
    return 1;
}

char* uitoa_16(unsigned int value, char *str){
    char reverse[36] = {0};
    char *p = reverse;

    *p++ = '\0';
    do{
        *p++ = "0123456789abcdef"[value%16];
        value /= 16;
    }while(value != 0);
    p--;

    while (p >= reverse) {
        *str++ = *p--;
    }
    return str;
}

char* itoa_10(int value, char *str){
    char reverse[40] = {0};
    char *p = reverse;
    int tmp = value;

    *p++ = '\0';
    if(value < 0) {
        tmp = -value;
    }
    do{
        *p++ = "0123456789"[tmp%10];
        tmp /= 10;
    }while(tmp != 0);
    if(value < 0) {
        *p++ = '-';
    }
    p--;
    while (p >= reverse){
        *str++ = *p--;
    }
    return str;
}

char* uitoa_10(unsigned int value, char *str){
    char reverse[36] = {0};
    char *p = reverse;

    *p++ = '\0';
    do{
        *p++ = "0123456789"[value%10];
        value /= 10;
    }while(value != 0);
    p--;
    while (p >= reverse) {
        *str++ = *p--;
    }
    return str;
}

int ota_printf(const char *fmt, ...)
{
    char buf[256];
    char *p = NULL;
    char *s = NULL;
    int i   = 0;
    int len = 0;
    int   waiting_fmt = 0;
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);

    for (p = buf; *fmt;) {
        if (waiting_fmt == 0 && *fmt != '%') {
            *p++ = *fmt++;
            continue;
        }
        if (waiting_fmt > 8) {
            puts("Non supported format!\r\n");
            *p = '\0';
            va_end(args);
            return puts(buf);
        }
        fmt++;
        switch (*fmt){
            case 'd':
                itoa_10(va_arg(args, unsigned int),p);
                p += strlen(p);
                waiting_fmt = 0;
                fmt++;
                break;
            case 'u':
                uitoa_10(va_arg(args, unsigned int),p);
                p += strlen(p);
                waiting_fmt = 0;
                fmt++;
                break;
            case 'x':
            case 'p':
                uitoa_16(va_arg(args, unsigned int),p);
                p += strlen(p);
                waiting_fmt = 0;
                fmt++;
                waiting_fmt = 0;
                fmt++;
                break;
            case 'c':
                *p++ = va_arg(args, int);
                waiting_fmt = 0;
                fmt++;
                break;
            case 's':
                *p = '\0';
                s = va_arg(args, char *);
                if (!s) {
                     s = "<NULL>";
                     len = 7;
                } else {
                     len = (strlen(s) > 128 ? 128 : strlen(s));
                }
                for (i = 0; i < len; ++i) *p++ = *s++;
                waiting_fmt = 0;
                fmt++;
                break;
            default:
                waiting_fmt++;
                break;
        }
    }
    *p = '\0';
    va_end(args);
    return puts(buf);
}

/* HEAP implement */
static void heap_init(void)
{
    blk_head_t *free_blk = NULL;
    if (  XZ_HEAP_BASE%XZ_HEAP_ALIGNMENT != 0
       || XZ_HEAP_SIZE > (~XZ_HEAP_MAGIC)
       || XZ_HEAP_SIZE < XZ_HEAP_BLK_MIN*2 ) {
        OTA_LOG_I("err:param invalid!\n");
        return;
    }
    freelist_head.next = (void *) XZ_HEAP_BASE;
    freelist_head.magic_size = 0;
    freelist_tail = (void *)(XZ_HEAP_BASE + ALIGN(XZ_HEAP_SIZE, XZ_HEAP_ALIGNMENT) -
		    XZ_HEAP_BLK_HEAD_SIZE);
    freelist_tail->next = NULL;
    freelist_tail->magic_size = 0;
    /* Only one block when init. */
    free_blk = (void *) XZ_HEAP_BASE;
    free_blk->next = freelist_tail;
    free_blk->magic_size = (unsigned int)freelist_tail - (unsigned int) free_blk;

    heap_free_size     = free_blk->magic_size;
    heap_free_size_min = free_blk->magic_size;
    OTA_LOG_I("heap start:0x%0x free size:0x%x \n", XZ_HEAP_BASE, heap_free_size);
}

static void heap_freeblk_insert(blk_head_t *blk_insert)
{
    blk_head_t *blk_before = NULL;
    blk_head_t *blk_after = NULL;

    /* freelist is ordered by address, find blk_before */
    for (blk_before = &freelist_head; blk_before->next < blk_insert; blk_before = blk_before->next) {
        if (blk_before == freelist_tail) {
            return;
        }
    }
    blk_after = blk_before->next;
    if ((char *)blk_before + blk_before->magic_size == (char *)blk_insert) {
    	blk_before->magic_size += blk_insert->magic_size;
    	blk_insert = blk_before;
    } else {
        /* do not merge, just insert new node to freelist */
    	blk_before->next = blk_insert;
    }

    /* try to merge blk_insert and blk_after */
    if (blk_after != freelist_tail &&
        (char *)blk_insert + blk_insert->magic_size == (char *)blk_after) {
    	blk_insert->magic_size += blk_after->magic_size;
    	blk_insert->next = blk_after->next;
    } else {
        /* do not merge, just insert new node to freelist */
    	blk_insert->next = blk_after;
    }
}

void * ota_heap_malloc(unsigned int alloc_size)
{
    blk_head_t *blk_alloc = NULL;
    blk_head_t *blk_prev = NULL;
    blk_head_t *blk_left = NULL;

    if (freelist_tail == NULL) {
    	heap_init();
    }
    if  ((alloc_size == 0) || (alloc_size > heap_free_size)) {
        return NULL;
    }
    alloc_size += XZ_HEAP_BLK_HEAD_SIZE;
    alloc_size  = ALIGN(alloc_size, XZ_HEAP_ALIGNMENT);
    /* find a free block bigger than alloc_size */
    blk_prev = &freelist_head;
    blk_alloc = freelist_head.next;
    while ((blk_alloc->magic_size < alloc_size) && (blk_alloc->next != NULL)) {
    	blk_prev = blk_alloc;
    	blk_alloc = blk_alloc->next;
    }
    if (blk_alloc->next == NULL) {
        return NULL;
    }
    /* delete blk_alloc from freelist */
    blk_prev->next = blk_alloc->next;
    /* check whether blk_alloc can split */
    if (blk_alloc->magic_size - alloc_size > XZ_HEAP_BLK_MIN) {
        /* split */
        blk_left = (void *)((char *)blk_alloc + alloc_size);
    	blk_left->magic_size = blk_alloc->magic_size - alloc_size;
    	blk_alloc->magic_size = alloc_size;
        /* Insert the new block into the list of free blocks. */
    	heap_freeblk_insert(blk_left);
    }
    /* update statistic */
    heap_free_size -= blk_alloc->magic_size;
    if (heap_free_size < heap_free_size_min) {
    	heap_free_size_min = heap_free_size;
    }
    /* blk belong to APP, magic set */
    blk_alloc->magic_size |= XZ_HEAP_MAGIC;
    blk_alloc->next = NULL;
    /* app used addr is after blk_head_t */
    return (void *)((char *)blk_alloc + XZ_HEAP_BLK_HEAD_SIZE);
}

void ota_heap_free(void *pfree)
{
    blk_head_t *free_blk;
    if (pfree == NULL) {
        return;
    }
    free_blk = (blk_head_t *)((char *)pfree - XZ_HEAP_BLK_HEAD_SIZE);
    if (free_blk->next != NULL
      ||(free_blk->magic_size & XZ_HEAP_MAGIC) != XZ_HEAP_MAGIC) {
        OTA_LOG_I("err:block destroyed!\n");
        return;
    }
    /* blk belong to free list, magic clear */
    free_blk->magic_size &= ~XZ_HEAP_MAGIC;
    /* update statistic */
    heap_free_size += free_blk->magic_size;
    heap_freeblk_insert(free_blk);
}
#endif
unsigned short update(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;
    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

void crc16_init(CRC16_CTX *inContext)
{
    inContext->crc = 0;
}

void crc16_update(CRC16_CTX *inContext, const void *inSrc, long inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inContext->crc = update(inContext->crc, *src++);
    }
}

void crc16_final(CRC16_CTX *inContext, unsigned short *outResult)
{
    inContext->crc = update(inContext->crc, 0);
    inContext->crc = update(inContext->crc, 0);
    *outResult = inContext->crc & 0xffffu;
}

unsigned short crc16_computer(void *buf, long len)
{
    unsigned short patch_crc = 0;
    CRC16_CTX crc_ctx;
    crc16_init(&crc_ctx);
    crc16_update(&crc_ctx, buf, len);
    crc16_final(&crc_ctx, &patch_crc);
    return patch_crc;
}
