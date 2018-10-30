/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include "rec_pub.h"

/*******************************************************************************/

void *rec_memcpy(void *dest, const void *src, size_t n)
{
    if (NULL == dest || NULL == src)
        return NULL;
    char *tempDest = (char *)dest;
    char *tempSrc = (char *)src;

    while (n-- > 0)
        *tempDest++ = *tempSrc++;
    return dest;
}

void *rec_memset(void *s, int c, size_t n)
{
    if (NULL == s)
        return NULL;
    char * tmpS = (char *)s;
    while(n-- > 0)
        *tmpS++ = c;
        return s;
}

size_t rec_strlen(char * str)
{
    const char *eos = str;

    while( *eos++ ) ;
    return( eos - str - 1 );
}

char *rec_strcat (char * dst, const char * src)
{
    char * cp = dst;

    while( *cp )
            cp++;                   /* find end of dst */

    while( cp && src)
    {
        *cp++ = *src++ ;       /* Copy src to end of dst */
    }

    return( dst );                  /* return dst */
}

int rec_memcmp(const void * buf1, const void * buf2, size_t count)
{
    if (!count)
        return(0);

    while ( --count && *(char *)buf1 == *(char *)buf2 ) {
        buf1 = (char *)buf1 + 1;
        buf2 = (char *)buf2 + 1;
    }

    return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );
}

void *rec_memmove(void *dest, const void *src, size_t count)
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

/*******************************************************************************/

#define PRT_BUFF_LEN    256

int rec_puts(const char *str)
{
    rec_uart_send_string((char *)str);
    return 1;
}

char* uitoa_16(uint32_t value, char *str){
    char reverse[36];
    char *p = reverse;

    *p++ = '\0';
    do{
        *p++ = "0123456789abcdef"[value%16];
        value /= 16;
    }while(value != 0);
    p--;

    while (p >= reverse){
        *str++ = *p--;
    }

    return str;
}

char* uitoa_10(uint32_t value, char *str){
    char reverse[36];
    char *p = reverse;

    *p++ = '\0';
    do{
        *p++ = "0123456789"[value%10];
        value /= 10;
    }while(value != 0);
    p--;

    while (p >= reverse){
        *str++ = *p--;
    }

    return str;
}

/* only support %x %p %d %c %s */
void vsprint(char *buf, const char *fmt, va_list args)
{
    char *p = NULL;
    char *s = NULL;
    int i   = 0;
    int len = 0;
    int   waiting_fmt = 0;
    va_list next_arg = args;

    for (p = buf; *fmt;)
    {
        if (waiting_fmt == 0 && *fmt != '%')
        {
            *p++ = *fmt++;
            continue;
        }
        if ( waiting_fmt > 8 )
        {
            puts("Non supported format!\r\n");
            *p = '\0';
            return;
        }
        fmt++;
        switch (*fmt){
            case 'd':
            case 'u':
                uitoa_10(va_arg(next_arg, unsigned int),p);
                p += strlen(p);
                waiting_fmt = 0;
                fmt++;
                break;
            case 'x':
            case 'p':
                uitoa_16(va_arg(next_arg, unsigned int),p);
                p += strlen(p);
                waiting_fmt = 0;
                fmt++;
                break;
            case 'c':
                *p++ = va_arg(next_arg, int);
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
}

int rec_printf(const char *fmt, ...)
{
    char buf[PRT_BUFF_LEN];
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);

    return puts(buf);
}

/*******************************************************************************/

/********* RECOVERY heap config: *********/
extern void         *_rec_heap_start;
extern void         *_rec_heap_end;
extern void         *_rec_heap_len;
/* Heap should align to REC_HEAP_ALIGNMENT */
#define REC_HEAP_BASE              ((intptr_t)&_rec_heap_start)
/* Size should be smaller than ~REC_HEAP_MAGIC */
#define REC_HEAP_SIZE              ((intptr_t)&_rec_heap_len)
/********* RECOVERY heap config end ******/

#define ALIGN(x,a)                 (((x) + (a) - 1) & ~((a) - 1))
#define REC_HEAP_ALIGNMENT         4

/* At the beginning of allocated memory */
#define REC_HEAP_BLK_HEAD_SIZE     ALIGN(sizeof(blk_head_t), REC_HEAP_ALIGNMENT)
/* Block sizes must not get too small. */
#define REC_HEAP_BLK_MIN	        ((REC_HEAP_BLK_HEAD_SIZE) << 1)

/* blk belong to APP, magic set
   blk belong to free list, magic clear */
#define REC_HEAP_MAGIC             (0xF0000000)

/* block head, before each blocks */
typedef struct blklist
{
    /* free memory block list, order by address*/
	struct blklist *next;
	/* when blk in APP: REC_HEAP_MAGIC & size
	   when blk in freelist: only size
	   size include blk_head_t self */
	size_t magic_size;
} blk_head_t;

/* free block list: order by address, from low to high */
/* first free block. */
static blk_head_t freelist_head;
/* point to last free block. */
static blk_head_t *freelist_tail;

/* For statistic. */
static size_t heap_free_size      = REC_HEAP_SIZE;
static size_t heap_free_size_min  = REC_HEAP_SIZE;

/* Init heap, with one free block */
static void heap_init(void)
{
    blk_head_t *free_blk;

    if (  REC_HEAP_BASE%REC_HEAP_ALIGNMENT != 0
       || REC_HEAP_SIZE > (~REC_HEAP_MAGIC)
       || REC_HEAP_SIZE < REC_HEAP_BLK_MIN*2 )
    {
        printf("err:param invalid!\n");
        return;
    }

	freelist_head.next = (void *) REC_HEAP_BASE;
	freelist_head.magic_size = (size_t) 0;

	freelist_tail = (void *)(REC_HEAP_BASE + ALIGN(REC_HEAP_SIZE, REC_HEAP_ALIGNMENT) -
                             REC_HEAP_BLK_HEAD_SIZE);
	freelist_tail->next = NULL;
	freelist_tail->magic_size = 0;

    /* Only one block when init. */
	free_blk = (void *) REC_HEAP_BASE;
	free_blk->next = freelist_tail;
	free_blk->magic_size = (size_t)freelist_tail - (size_t) free_blk;

	heap_free_size     = free_blk->magic_size;
	heap_free_size_min = free_blk->magic_size;
}

static void heap_freeblk_insert(blk_head_t *blk_insert)
{
    blk_head_t *blk_before; /* before the blk_insert */
    blk_head_t *blk_after;  /* after  the blk_insert */

    /* freelist is ordered by address, find blk_before */
    for (blk_before = &freelist_head; blk_before->next < blk_insert; blk_before = blk_before->next)
    {
        if (blk_before == freelist_tail)
        {
            return;
        }
    }
    blk_after = blk_before->next;

    /* now: blk_before < blk_insert < blk_after */

    /* try to merge blk_before and blk_insert */
	if ((char *)blk_before + blk_before->magic_size == (char *)blk_insert)
    {
    	blk_before->magic_size += blk_insert->magic_size;
    	blk_insert = blk_before;
    }
    else
    {
        /* do not merge, just insert new node to freelist */
    	blk_before->next = blk_insert;
    }

    /* try to merge blk_insert and blk_after */
	if (blk_after != freelist_tail &&
	    (char *)blk_insert + blk_insert->magic_size == (char *)blk_after)
    {
    	blk_insert->magic_size += blk_after->magic_size;
    	blk_insert->next = blk_after->next;
    }
	else
    {
        /* do not merge, just insert new node to freelist */
    	blk_insert->next = blk_after;
    }
}

static void *heap_malloc(size_t alloc_size)
{
    blk_head_t *blk_alloc;
    blk_head_t *blk_prev;
    blk_head_t *blk_left;

	//krhino_sched_disable();

    /* first call to malloc, init is needed */
	if (freelist_tail == NULL)
    {
    	heap_init();
    }

	if  ((alloc_size == 0)
      || (alloc_size > heap_free_size))
    {
        /* not enough memory */
        //krhino_sched_enable();
        return NULL;
    }

	alloc_size += REC_HEAP_BLK_HEAD_SIZE;
    alloc_size  = ALIGN(alloc_size, REC_HEAP_ALIGNMENT);

    /* find a free block bigger than alloc_size */
	blk_prev = &freelist_head;
	blk_alloc = freelist_head.next;
	while ((blk_alloc->magic_size < alloc_size) && (blk_alloc->next != NULL))
    {
    	blk_prev = blk_alloc;
    	blk_alloc = blk_alloc->next;
    }
	if (blk_alloc->next == NULL)
    {
        /* this means "blk_alloc == freelist_tail" */
        //krhino_sched_enable();
        return NULL;
    }

    /* delete blk_alloc from freelist */
	blk_prev->next = blk_alloc->next;

    /* check whether blk_alloc can split */
	if (blk_alloc->magic_size - alloc_size > REC_HEAP_BLK_MIN)
    {
        /* split */
        blk_left = (void *)((char *)blk_alloc + alloc_size);
    	blk_left->magic_size = blk_alloc->magic_size - alloc_size;

    	blk_alloc->magic_size = alloc_size;

        /* Insert the new block into the list of free blocks. */
    	heap_freeblk_insert(blk_left);
    }

    /* update statistic */
	heap_free_size -= blk_alloc->magic_size;
	if (heap_free_size < heap_free_size_min)
    {
    	heap_free_size_min = heap_free_size;
    }

    //krhino_sched_enable();

    /* blk belong to APP, magic set */
	blk_alloc->magic_size |= REC_HEAP_MAGIC;
	blk_alloc->next = NULL;

    /* app used addr is after blk_head_t */
	return (void *)((char *)blk_alloc + REC_HEAP_BLK_HEAD_SIZE);
}

static void heap_free(void *pfree)
{
    blk_head_t *free_blk;

	if (pfree == NULL)
    {
        return;
    }

    /* app used addr is after blk_head_t */
	free_blk = (blk_head_t *)((char *)pfree - REC_HEAP_BLK_HEAD_SIZE);

    /* blk damaged check */
    if (free_blk->next != NULL
      ||(free_blk->magic_size & REC_HEAP_MAGIC) != REC_HEAP_MAGIC)
    {
        printf("err:block destroyed!\n");
        return;
    }

    /* blk belong to free list, magic clear */
	free_blk->magic_size &= ~REC_HEAP_MAGIC;
    /* update statistic */
	heap_free_size += free_blk->magic_size;

	//krhino_sched_disable();

	heap_freeblk_insert(free_blk);

    //krhino_sched_enable();
}

void *rec_malloc(size_t alloc_size)
{
    return heap_malloc(alloc_size);
}

void rec_free(void *pfree)
{
    heap_free(pfree);
}

size_t rec_freesize(void)
{
	return heap_free_size;
}

size_t rec_freesize_min(void)
{
	return heap_free_size_min;
}

/*******************************************************************************/

static uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte | 0x100;

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

void rec_CRC16_Init( CRC16_Context *inContext )
{
    inContext->crc = 0;
}


void rec_CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen )
{
    const uint8_t *src = (const uint8_t *) inSrc;
    const uint8_t *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inContext->crc = UpdateCRC16(inContext->crc, *src++);
    }
}

void rec_CRC16_Final( CRC16_Context *inContext, uint16_t *outResult )
{
    inContext->crc = UpdateCRC16(inContext->crc, 0);
    inContext->crc = UpdateCRC16(inContext->crc, 0);
    *outResult = inContext->crc & 0xffffu;
}



