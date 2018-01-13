/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_H
#define K_MM_H

#ifdef HAVE_VALGRIND_H
#include <valgrind.h>
#include <memcheck.h>
#define VGF(X) X
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#define VGF(X) X
#else
#define VGF(X)
#endif


/*use two level bit map to find free memory block*/

#if (RHINO_CONFIG_MM_TLF > 0)
#define MAX_MM_BIT           RHINO_CONFIG_MM_MAXMSIZEBIT

#define MAX_MM_SIZE         (1<<MAX_MM_BIT)
#define FIX_BLK_BIT         5 /*32 bytes*/
#define DEF_FIX_BLK_SIZE    (1<<FIX_BLK_BIT) /*32 bytes*/
#define MIN_FLT_BIT         7 /*<2^7 only need one level mapping */
#define MIN_FLT_SIZE        (1<<MIN_FLT_BIT)
#define MAX_LOG2_SLT        5
#define SLT_SIZE            (1<<MAX_LOG2_SLT)
#define FLT_SIZE            (MAX_MM_BIT - MIN_FLT_BIT + 1)
#define MM_ALIGIN_SIZE      (1<<(MIN_FLT_BIT - MAX_LOG2_SLT))
#define MM_ALIGN_MASK       (sizeof(void*)-1)

#define MM_ALIGN_UP(a)   (((a) + MM_ALIGN_MASK) & ~MM_ALIGN_MASK)
#define MM_ALIGN_DOWN(a) ((a) & ~MM_ALIGN_MASK)


#define RHINO_MM_BLKSIZE_MASK (0xFFFFFFFF - MM_ALIGN_MASK)

#define DEF_TOTAL_FIXEDBLK_SIZE     8192 /*by default, total 2k momory fo fix size block */
#define MIN_FREE_MEMORY_SIZE        1024 /*at least need 1k for user alloced*/



/*bit 0 and bit 1 mask*/
#define RHINO_MM_CURSTAT_MASK 0x1
#define RHINO_MM_PRESTAT_MASK 0x2

/*bit 0*/
#define RHINO_MM_FREE         1
#define RHINO_MM_ALLOCED      0

/*bit 1*/
#define RHINO_MM_PREVFREE     2
#define RHINO_MM_PREVALLOCED  0

#define NEXT_MM_BLK(_addr, _r) ((k_mm_list_t *) ((uint8_t *) (_addr) + (_r)))

#define MMLIST_HEAD_SIZE   (sizeof(k_mm_list_t) -  sizeof(free_ptr_t))
/*
-------------------------------------------------------------------
| k_mm_list_t |k_mm_region_info_t|k_mm_list_t|free space      |k_mm_list_t|
-------------------------------------------------------------------
*/
#define MMREGION_USED_SIZE (MM_ALIGN_UP(sizeof(k_mm_region_info_t)) + 3 * MMLIST_HEAD_SIZE )

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
    size_t       size;
    /* bit 0 indicates whether the block is used and */
    /* bit 1 allows to know whether the previous block is free */
    union {
        struct free_ptr_struct free_ptr;
        uint8_t                buffer[1];
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
    size_t              mm_size_stats[MAX_MM_BIT];
#endif

    uint32_t            fl_bitmap;
    uint32_t            sl_bitmap[FLT_SIZE]; /* the second-level bitmap */
    k_mm_list_t        *mm_tbl[FLT_SIZE][SLT_SIZE];
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

void   krhino_mm_free(void *ptr);


/**
 * This function is wrapper of mm rallocation
 * @param[in]       oldmem      oldmem address
 * @param[in]       size        size of the mem to malloc
 * @return  the operation status, NULL is error, others is realloced memory address
 */
void *krhino_mm_realloc(void *oldmem, size_t newsize);



#endif /* K_MM_BESTFIT_H */

