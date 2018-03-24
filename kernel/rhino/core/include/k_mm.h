/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_H
#define K_MM_H

/*use two level bit map to find free memory block*/

#if (RHINO_CONFIG_MM_TLF > 0)
/* alignment: */
#define MM_ALIGN_BIT        3
#define MM_ALIGN_SIZE       (1<<MM_ALIGN_BIT)
#define MM_ALIGN_MASK       (MM_ALIGN_SIZE-1)
#define MM_ALIGN_UP(a)      (((a) + MM_ALIGN_MASK) & ~MM_ALIGN_MASK)
#define MM_ALIGN_DOWN(a)    ((a) & ~MM_ALIGN_MASK)

/* mm_blk: */
#define DEF_FIX_BLK_SIZE    32

/* mm bitmask freelist: */
#define MM_MAX_BIT          24
#define MM_MAX_SIZE         (1<<MM_MAX_BIT)

#define MM_MIN_BIT          6
#define MM_MIN_SIZE         (1<<(MM_MIN_BIT - 1))
#define MM_BIT_LEVEL        (MM_MAX_BIT - MM_MIN_BIT + 2)


#define MIN_FREE_MEMORY_SIZE    1024 /*at least need 1k for user alloced*/

/*bit 0 and bit 1 mask*/
#define RHINO_MM_CURSTAT_MASK   0x1
#define RHINO_MM_PRESTAT_MASK   0x2

/*bit 0*/
#define RHINO_MM_FREE           1
#define RHINO_MM_ALLOCED        0

/*bit 1*/
#define RHINO_MM_PREVFREE       2
#define RHINO_MM_PREVALLOCED    0

#define MMLIST_HEAD_SIZE        (MM_ALIGN_UP(sizeof(k_mm_list_t) -  sizeof(free_ptr_t)))

/* get buffer size */
#define MM_GET_BUF_SIZE(blk)    \
    ((blk)->buf_size & (~MM_ALIGN_MASK))
/* get blk size : head size + buffer size */
#define MM_GET_BLK_SIZE(blk)    \
    (MM_GET_BUF_SIZE(blk) + MMLIST_HEAD_SIZE)
/* get next blk */
#define MM_GET_NEXT_BLK(blk)    \
    ((k_mm_list_t *)((blk)->mbinfo.buffer + MM_GET_BUF_SIZE(blk)))
/* get this blk */
#define MM_GET_THIS_BLK(buf)    \
    ((k_mm_list_t *)((char *)(buf) - MMLIST_HEAD_SIZE))

/*struct of memory list ,every memory block include this information*/
typedef struct free_ptr_struct {
    struct k_mm_list_struct *prev;
    struct k_mm_list_struct *next;
} free_ptr_t;

typedef struct k_mm_list_struct {
#if (RHINO_CONFIG_MM_DEBUG > 0)
    size_t       dye;
    size_t       owner;
#endif
    struct k_mm_list_struct *prev;
    /* bit 0 indicates whether the block is used and */
    /* bit 1 allows to know whether the previous block is free */
    size_t       buf_size;
    union {
        struct free_ptr_struct free_ptr;
        uint8_t  buffer[1];
    } mbinfo;
} k_mm_list_t;

typedef struct k_mm_region_info_struct {
    k_mm_list_t                    *end;
    struct k_mm_region_info_struct *next;
} k_mm_region_info_t;


typedef struct {
#if (RHINO_CONFIG_MM_REGION_MUTEX == 1)
    kmutex_t            mm_mutex;
#endif
    k_mm_region_info_t *regioninfo;
    k_mm_list_t        *fixedmblk;

#if (K_MM_STATISTIC > 0)
    size_t              used_size;
    size_t              maxused_size;
    size_t              free_size;
    size_t              mm_size_stats[MM_BIT_LEVEL];
#endif
    /* msb (MM_BIT_LEVEL-1) <-> lsb 0, one bit match one freelist */
    uint32_t            free_bitmap;
    /* freelist[N]: contain free blks at level N, 
       2^(N + MM_MIN_BIT) <= level N buffer size < 2^(1 + N + MM_MIN_BIT) */
    k_mm_list_t        *freelist[MM_BIT_LEVEL];
} k_mm_head;


kstat_t krhino_init_mm_head(k_mm_head **ppmmhead, void *addr, size_t len );
kstat_t krhino_deinit_mm_head(k_mm_head *mmhead);

kstat_t krhino_add_mm_region(k_mm_head *mmhead, void *addr, size_t len);


void *k_mm_alloc(k_mm_head *mmhead, size_t size);
void  k_mm_free(k_mm_head       *mmhead, void *ptr);
void *k_mm_realloc(k_mm_head *mmhead, void *oldmem, size_t new_size);
#endif

/**
 * This function is wrapper of mm allocation
 * @param[in]       size        size of the mem to malloc
 * @return  the operation status, NULL is error, others is memory address
 */
void *krhino_mm_alloc(size_t size);

/**
 * This function is wrapper of mm free
 * @param[in]       ptr        address point of the mem
 */

void krhino_mm_free(void *ptr);


/**
 * This function is wrapper of mm rallocation
 * @param[in]       oldmem      oldmem address
 * @param[in]       size        size of the mem to malloc
 * @return  the operation status, NULL is error, others is realloced memory address
 */
void *krhino_mm_realloc(void *oldmem, size_t newsize);



#endif /* K_MM_BESTFIT_H */

