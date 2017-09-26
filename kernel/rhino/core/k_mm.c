/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if (RHINO_CONFIG_MM_TLF > 0)

#define AOS_MM_TLF_ALLOC_MIN_LENGTH  2*sizeof(void *)

typedef enum {
    ACTION_INSERT,
    ACTION_GET
} SEARCH_ACTION;

#define ISFIXEDBLK(mh,ptr) \
        (mh->fixedmblk && (ptr > (void *)mh->fixedmblk->mbinfo.buffer) \
        && (ptr < (void *)mh->fixedmblk->mbinfo.buffer + mh->fixedmblk->size)) ? 1 : 0

RHINO_INLINE k_mm_list_t *init_mm_region(void *regionaddr, size_t len)
{
    k_mm_list_t        *curblk, *lastblk, *firstblk;
    k_mm_region_info_t *region;

    /*mmblk for region info*/
    firstblk = (k_mm_list_t *) regionaddr;

    VGF(VALGRIND_MAKE_MEM_DEFINED(firstblk, MMLIST_HEAD_SIZE));

    firstblk->size = MM_ALIGN_UP(sizeof(k_mm_region_info_t)) | RHINO_MM_ALLOCED |
                     RHINO_MM_PREVALLOCED;

    curblk = (k_mm_list_t *) ((char *)firstblk->mbinfo.buffer +
                              (firstblk->size & RHINO_MM_BLKSIZE_MASK));

    VGF(VALGRIND_MAKE_MEM_DEFINED(curblk, sizeof(k_mm_list_t)));

    curblk->size = MM_ALIGN_DOWN(len - MMREGION_USED_SIZE) | RHINO_MM_ALLOCED |
                   RHINO_MM_PREVALLOCED;
    curblk->mbinfo.free_ptr.prev = curblk->mbinfo.free_ptr.next = 0;

    lastblk = NEXT_MM_BLK(curblk->mbinfo.buffer,
                          curblk->size & RHINO_MM_BLKSIZE_MASK);

    VGF(VALGRIND_MAKE_MEM_DEFINED(lastblk, MMLIST_HEAD_SIZE));

    lastblk->prev = curblk;
    lastblk->size = 0 | RHINO_MM_ALLOCED | RHINO_MM_PREVFREE;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    lastblk->dye    = RHINO_MM_CORRUPT_DYE;
    lastblk->owner  = 0;
    firstblk->dye   = RHINO_MM_CORRUPT_DYE;
    firstblk->owner = 0;
#endif
    region = (k_mm_region_info_t *) firstblk->mbinfo.buffer;

    VGF(VALGRIND_MAKE_MEM_DEFINED(region, sizeof(k_mm_region_info_t)));

    region->next = 0;
    region->end = lastblk;
    VGF(VALGRIND_MAKE_MEM_NOACCESS(firstblk, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(curblk, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(lastblk, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(region, sizeof(k_mm_region_info_t)));
    return firstblk;
}

#if(K_MM_STATISTIC > 0)

static size_t sizetoindex(size_t size)
{
    size_t cnt      = 0;
    cnt = 31 - krhino_find_first_bit(&size);
    return cnt;
}
static void addsize(k_mm_head *mmhead, size_t size, size_t req_size)
{
    size_t index ;
    if (mmhead->free_size > size) {
        mmhead->free_size -= size;
    } else {
        mmhead->free_size = 0;
    }
    mmhead->used_size += size;
    if (mmhead->used_size > mmhead->maxused_size) {
        mmhead->maxused_size = mmhead->used_size;
    }

    index = sizetoindex(req_size - 1);
    if (index > MAX_MM_BIT) {
        index = MAX_MM_BIT;
    }
    if (index < 1) {
        index = 1;
    }
    mmhead->mm_size_stats[index - 1]++;
}

static void removesize(k_mm_head *mmhead, size_t size)
{
    if (mmhead->used_size > size ) {
        mmhead->used_size -= size;
    } else {
        mmhead->used_size = 0;
    }
    mmhead->free_size += size;

}

#define stats_addsize(mmhead,size, req_size)    addsize(mmhead,size, req_size)
#define stats_removesize(mmhead,size) removesize(mmhead,size)
#else
#define stats_addsize(mmhead,size, req_size)    do{}while(0)
#define stats_removesize(mmhead,size) do{}while(0)
#endif

kstat_t krhino_init_mm_head(k_mm_head **ppmmhead, void *addr, size_t len )
{
    k_mm_list_t *nextblk, *curblk, *firstblk;
    k_mm_head   *pmmhead;
    mblk_pool_t *mmblk_pool;
    kstat_t      stat;
    void        *orig_addr;

    NULL_PARA_CHK(ppmmhead);
    NULL_PARA_CHK(addr);

    /*check paramters, addr need algin with 4 and len should be multiple of 4
      1.  the length at least need DEF_TOTAL_FIXEDBLK_SIZE for fixed size memory block
      2.  and also ast least have 1k for user alloced
    */
    orig_addr = addr;
    addr = (void *) MM_ALIGN_UP((size_t)addr);
    len -= (addr - orig_addr);
    len = MM_ALIGN_DOWN(len);

    if (((unsigned long) addr & MM_ALIGN_MASK) || (len != MM_ALIGN_DOWN(len))) {
        return RHINO_INV_ALIGN;
    }

    if ( !len || len < MIN_FREE_MEMORY_SIZE + DEF_TOTAL_FIXEDBLK_SIZE
         || len > MAX_MM_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    pmmhead = (k_mm_head *)addr;

    /* Zeroing the memory head */
    memset(pmmhead, 0, sizeof(k_mm_head));
#if (RHINO_CONFIG_MM_REGION_MUTEX > 0)
    krhino_mutex_create(&pmmhead->mm_mutex, "mm_mutex");
#endif

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(pmmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif

    if ((VGF(VALGRIND_MEMPOOL_EXISTS(addr)) + 0) == 0) {
#if defined(__VALGRIND_MAJOR__) && defined(__VALGRIND_MINOR__)   \
                        && (__VALGRIND_MAJOR__ > 3                                   \
                            || (__VALGRIND_MAJOR__ == 3 && __VALGRIND_MINOR__ >= 12))
        /*valgrind support VALGRIND_CREATE_MEMPOOL_EXT from 3.12.0*/
        VGF(VALGRIND_CREATE_MEMPOOL_EXT(addr, 0, 0,
                                        VALGRIND_MEMPOOL_METAPOOL | VALGRIND_MEMPOOL_AUTO_FREE));
#else
        VGF(VALGRIND_CREATE_MEMPOOL((uint8_t *)addr, 0, 0));
#endif
        VGF(VALGRIND_MAKE_MEM_NOACCESS(addr, len));

    }

    VGF(VALGRIND_MAKE_MEM_DEFINED(pmmhead, sizeof(k_mm_head)));

    firstblk = init_mm_region(addr + MM_ALIGN_UP(sizeof(k_mm_head)),
                              MM_ALIGN_DOWN(len - sizeof(k_mm_head)));


    pmmhead->regioninfo = (k_mm_region_info_t *) firstblk->mbinfo.buffer;

    VGF(VALGRIND_MAKE_MEM_DEFINED(firstblk, sizeof(k_mm_list_t)));

    nextblk = NEXT_MM_BLK(firstblk->mbinfo.buffer,
                          firstblk->size & RHINO_MM_BLKSIZE_MASK);

    VGF(VALGRIND_MAKE_MEM_DEFINED(nextblk, sizeof(k_mm_list_t)));

    *ppmmhead = pmmhead;

    /*before freed it, we need mark it as alloced*/
    VGF(VALGRIND_MALLOCLIKE_BLOCK(nextblk->mbinfo.buffer, nextblk->size & RHINO_MM_BLKSIZE_MASK, 0, 0));

    /*mark it as free and set it to bitmap*/
    nextblk->dye = RHINO_MM_CORRUPT_DYE;
    k_mm_free(pmmhead, nextblk->mbinfo.buffer);
    /*after free, we need acess mmhead and nextblk again*/

    VGF(VALGRIND_MAKE_MEM_DEFINED(nextblk, sizeof(k_mm_list_t)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(pmmhead, sizeof(k_mm_head)));
#if (K_MM_STATISTIC > 0)
    pmmhead->free_size = nextblk->size & RHINO_MM_BLKSIZE_MASK;
    pmmhead->used_size = len - (nextblk->size & RHINO_MM_BLKSIZE_MASK);
    pmmhead->maxused_size = pmmhead->used_size;
#endif

    VGF(VALGRIND_MAKE_MEM_NOACCESS(firstblk, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(nextblk, MMLIST_HEAD_SIZE));

    mmblk_pool = k_mm_alloc(pmmhead,
                            DEF_TOTAL_FIXEDBLK_SIZE + MM_ALIGN_UP(sizeof(mblk_pool_t)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(pmmhead, sizeof(k_mm_head)));
    if (mmblk_pool) {
        curblk = (k_mm_list_t *) ((char *) mmblk_pool - MMLIST_HEAD_SIZE);
        VGF(VALGRIND_MAKE_MEM_DEFINED(curblk, sizeof(k_mm_list_t)));
        VGF(VALGRIND_FREELIKE_BLOCK(mmblk_pool, 0));
        VGF(VALGRIND_MAKE_MEM_DEFINED(mmblk_pool, curblk->size & RHINO_MM_BLKSIZE_MASK));
        stat = krhino_mblk_pool_init(mmblk_pool, "fixed_mm_blk",
                                     (void *)mmblk_pool + MM_ALIGN_UP(sizeof(mblk_pool_t)),
                                     DEF_FIX_BLK_SIZE, DEF_TOTAL_FIXEDBLK_SIZE);
        if (stat == RHINO_SUCCESS) {
            pmmhead->fixedmblk = curblk;
        } else {
            k_mm_free(pmmhead, mmblk_pool);
            VGF(VALGRIND_MAKE_MEM_DEFINED(pmmhead, sizeof(k_mm_head)));
        }
#if (K_MM_STATISTIC > 0)
        stats_removesize(pmmhead, DEF_TOTAL_FIXEDBLK_SIZE);
#endif
    }

    VGF(VALGRIND_MAKE_MEM_NOACCESS(pmmhead, sizeof(k_mm_head)));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&(pmmhead->mm_mutex));
#endif


    return RHINO_SUCCESS;
}

kstat_t krhino_deinit_mm_head(k_mm_head *mmhead)
{

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(mmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
    memset(mmhead, 0, sizeof(k_mm_head));
    VGF(VALGRIND_DESTROY_MEMPOOL(mmhead));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif
    return RHINO_SUCCESS;

}

kstat_t krhino_add_mm_region(k_mm_head *mmhead, void *addr, size_t len)
{
    k_mm_region_info_t *ptr, *ptr_prev, *ai;
    k_mm_list_t        *ib0, *b0, *lb0, *ib1, *b1, *lb1, *next_b;

    NULL_PARA_CHK(mmhead);
    NULL_PARA_CHK(addr);

    if (((unsigned long) addr & MM_ALIGN_MASK) || (len != MM_ALIGN_DOWN(len))) {
        return RHINO_INV_ALIGN;
    }

    if ( !len || len < sizeof(k_mm_region_info_t) + sizeof(k_mm_list_t) * 2) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(mmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif

    memset(addr, 0, len);

    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));

    ptr = mmhead->regioninfo;
    ptr_prev = 0;


    VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));


    ib0 = init_mm_region(addr, len);

    VGF(VALGRIND_MAKE_MEM_DEFINED(ib0, MMLIST_HEAD_SIZE));

    b0  = NEXT_MM_BLK(ib0->mbinfo.buffer, ib0->size & RHINO_MM_BLKSIZE_MASK);

    VGF(VALGRIND_MAKE_MEM_DEFINED(b0, MMLIST_HEAD_SIZE));

    lb0 = NEXT_MM_BLK(b0->mbinfo.buffer, b0->size & RHINO_MM_BLKSIZE_MASK);

    VGF(VALGRIND_MAKE_MEM_DEFINED(lb0, MMLIST_HEAD_SIZE));

    /* Before inserting the new area, we have to merge this area with the
       already existing ones */

    while (ptr) {
        ib1 = (k_mm_list_t *) ((char *) ptr - MMLIST_HEAD_SIZE);

        VGF(VALGRIND_MAKE_MEM_DEFINED(ib1, MMLIST_HEAD_SIZE));

        b1 = NEXT_MM_BLK(ib1->mbinfo.buffer, ib1->size & RHINO_MM_BLKSIZE_MASK);

        VGF(VALGRIND_MAKE_MEM_DEFINED(b1, MMLIST_HEAD_SIZE));

        lb1 = ptr->end;

        VGF(VALGRIND_MAKE_MEM_DEFINED(lb1, MMLIST_HEAD_SIZE));

        /* Merging the new area with the next physically contigous one */
        if ((unsigned long) ib1 == (unsigned long) lb0 + MMLIST_HEAD_SIZE) {
            if (mmhead->regioninfo == ptr) {
                mmhead->regioninfo = ptr->next;
                ptr = ptr->next;

                VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));

            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;

                VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));
            }

            b0->size = MM_ALIGN_DOWN((b0->size & RHINO_MM_BLKSIZE_MASK) +
                                     (ib1->size & RHINO_MM_BLKSIZE_MASK)
                                     + 2 * MMLIST_HEAD_SIZE) | RHINO_MM_ALLOCED | RHINO_MM_PREVALLOCED;

            b1->prev = b0;
            lb0 = lb1;

            continue;
        }

        /* Merging the new area with the previous physically contigous
           one */
        if ((unsigned long) lb1->mbinfo.buffer == (unsigned long) ib0) {
            if (mmhead->regioninfo == ptr) {
                mmhead->regioninfo = ptr->next;
                ptr = ptr->next;

                VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));
            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;

                VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));
            }

            lb1->size = MM_ALIGN_DOWN((b0->size & RHINO_MM_BLKSIZE_MASK) +
                                      (ib0->size & RHINO_MM_BLKSIZE_MASK)
                                      + 2 * MMLIST_HEAD_SIZE) | RHINO_MM_ALLOCED | (lb1->size &
                                                                                    RHINO_MM_PRESTAT_MASK);
            next_b = NEXT_MM_BLK(lb1->mbinfo.buffer, lb1->size & RHINO_MM_BLKSIZE_MASK);
            next_b->prev = lb1;
            b0 = lb1;
            ib0 = ib1;

            continue;
        }
        ptr_prev = ptr;
        ptr = ptr->next;

        VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, sizeof(k_mm_region_info_t)));
    }

    /* Inserting the area in the list of linked areas */
    ai = (k_mm_region_info_t *) ib0->mbinfo.buffer;
    VGF(VALGRIND_MAKE_MEM_DEFINED(ai, sizeof(k_mm_region_info_t)));
    ai->next = mmhead->regioninfo;
    ai->end = lb0;
    mmhead->regioninfo = ai;
    VGF(VALGRIND_MAKE_MEM_NOACCESS(ai, sizeof(k_mm_region_info_t)));

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    b0->dye = RHINO_MM_CORRUPT_DYE;
#endif
    /*before free, we need tell valgrind it's a malloced memory*/
    VGF(VALGRIND_MALLOCLIKE_BLOCK(b0->mbinfo.buffer, b0->size & RHINO_MM_BLKSIZE_MASK, 0, 0));
    /* change used_size with b0 size*/
#if (K_MM_STATISTIC > 0)
    mmhead->used_size += (b0->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE;
#endif
    /*mark b0 as free*/
    k_mm_free(mmhead, b0->mbinfo.buffer);

    VGF(VALGRIND_MAKE_MEM_NOACCESS(ib0, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(b0, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(lb0, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(ib1, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(b1, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(lb1, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->regioninfo, sizeof(k_mm_region_info_t)));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif


    return RHINO_SUCCESS;
}

static void *k_mm_smallblk_alloc(k_mm_head *mmhead, size_t size)
{
    kstat_t sta;
    void   *tmp;

    if (!mmhead) {
        return NULL;
    }

    sta = krhino_mblk_alloc((mblk_pool_t *)mmhead->fixedmblk->mbinfo.buffer, &tmp);
    if (sta != RHINO_SUCCESS) {
        return NULL;
    }

    VGF(VALGRIND_MALLOCLIKE_BLOCK(tmp, size, 0, 0));
    VGF(VALGRIND_MAKE_MEM_UNDEFINED(tmp, size));

    stats_addsize(mmhead, DEF_FIX_BLK_SIZE, size);

    return tmp;
}
static void k_mm_smallblk_free(k_mm_head *mmhead, void *ptr)
{
    kstat_t sta;

    if (!mmhead || !ptr) {
        return;
    }

    VGF(VALGRIND_FREELIKE_BLOCK(ptr, 0));
    VGF(VALGRIND_MAKE_MEM_DEFINED(ptr, DEF_FIX_BLK_SIZE));

    sta = krhino_mblk_free((mblk_pool_t *)mmhead->fixedmblk->mbinfo.buffer, ptr);
    if (sta != RHINO_SUCCESS) {
        assert(0);
    }

    stats_removesize(mmhead, DEF_FIX_BLK_SIZE);
}

static kstat_t bitmap_search(size_t size , size_t *flt, size_t *slt,
                             SEARCH_ACTION action)
{
    size_t tmp_size     = 0;
    size_t padding_size = 0;
    size_t firstbit     = 0;

    if (size < MIN_FLT_SIZE) {
        *flt = 0;
        *slt = size >> (MIN_FLT_BIT - MAX_LOG2_SLT);
    } else {
        *flt = 0;
        firstbit = 31 - (size_t)krhino_find_first_bit(&size);
        tmp_size = size;
        if (action == ACTION_GET) {
            padding_size = (1 << (firstbit - MAX_LOG2_SLT)) - 1;
            tmp_size = size + padding_size;
            firstbit = 31 - (size_t)krhino_find_first_bit(&tmp_size);
        }
        *flt = firstbit - MIN_FLT_BIT + 1;
        tmp_size = tmp_size - (1 << firstbit);
        *slt = tmp_size >> (firstbit - MAX_LOG2_SLT);
    }
    if (*flt >= FLT_SIZE || *slt >= SLT_SIZE) {
        return RHINO_MM_FREE_ADDR_ERR;
    }
    return RHINO_SUCCESS;
}

static size_t find_last_bit(int bitmap)
{
    size_t x, lsbit;

    if (bitmap == 0) {
        return 0;
    }

    x = bitmap & -bitmap;
    lsbit = (size_t)krhino_find_first_bit(&x);
    /* AliOS find fist bit return value is left->right as 0-31, but we need left->right as 31 -0 */
    return 31 - lsbit;
}

static  void insert_block (k_mm_head *mmhead, k_mm_list_t *blk, int flt,
                           int slt)
{
    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = mmhead->mm_tbl [flt][slt];

    if (mmhead->mm_tbl[flt][slt]) {

        VGF(VALGRIND_MAKE_MEM_DEFINED(&mmhead->mm_tbl[flt][slt]->mbinfo, sizeof(struct free_ptr_struct)));
        mmhead->mm_tbl[flt][slt]->mbinfo.free_ptr.prev = blk;
        VGF(VALGRIND_MAKE_MEM_NOACCESS(&mmhead->mm_tbl[flt][slt]->mbinfo, sizeof(struct free_ptr_struct)));
    }

    mmhead->mm_tbl[flt][slt] = blk;
    krhino_bitmap_set (&mmhead->sl_bitmap[flt], 31 - slt);
    krhino_bitmap_set (&mmhead->fl_bitmap , 31 - flt);
}

static  void get_block(k_mm_head *mmhead, k_mm_list_t *blk, int flt, int slt)
{

    VGF(VALGRIND_MAKE_MEM_DEFINED(&blk->mbinfo, sizeof(struct free_ptr_struct)));
    if (blk->mbinfo.free_ptr.next) {
        VGF(VALGRIND_MAKE_MEM_DEFINED(blk->mbinfo.free_ptr.next, sizeof(k_mm_list_t)));
        blk->mbinfo.free_ptr.next->mbinfo.free_ptr.prev = blk->mbinfo.free_ptr.prev;
        VGF(VALGRIND_MAKE_MEM_NOACCESS(blk->mbinfo.free_ptr.next, sizeof(k_mm_list_t)));
    }
    if (blk->mbinfo.free_ptr.prev) {
        VGF(VALGRIND_MAKE_MEM_DEFINED(blk->mbinfo.free_ptr.prev, sizeof(k_mm_list_t)));
        blk->mbinfo.free_ptr.prev->mbinfo.free_ptr.next = blk->mbinfo.free_ptr.next;
        VGF(VALGRIND_MAKE_MEM_NOACCESS(blk->mbinfo.free_ptr.prev, sizeof(k_mm_list_t)));
    }
    if (mmhead->mm_tbl[flt][slt] == blk) {
        mmhead->mm_tbl[flt][slt] = blk->mbinfo.free_ptr.next;
        if (!mmhead->mm_tbl [flt][slt]) {
            krhino_bitmap_clear (&mmhead->sl_bitmap[flt], 31 - slt);
            if (!mmhead->sl_bitmap[flt]) {
                krhino_bitmap_clear(&mmhead->fl_bitmap, 31 - flt);
            }
        }
    }
    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = NULL;
}

static k_mm_list_t *findblk_byidx(k_mm_head *mmhead, size_t *flt, size_t *slt)
{
    uint32_t     tmp  = mmhead->sl_bitmap[*flt] & (~0 << *slt);
    k_mm_list_t *find = NULL;

    if (tmp) {
        /*find last bit*/
        *slt = find_last_bit(tmp);
        find = mmhead->mm_tbl[*flt][*slt];
    } else {
        *flt = find_last_bit(mmhead->fl_bitmap & (~0 << (*flt + 1)));
        if (*flt > 0) {         /* likely */
            *slt = find_last_bit(mmhead->sl_bitmap[*flt]);
            find = mmhead->mm_tbl[*flt][*slt];
        }
    }
    return find;
}

void *k_mm_alloc(k_mm_head *mmhead, size_t size)
{
    void        *retptr;
    k_mm_list_t *b, *b2, *next_b;
    size_t       fl, sl;
    size_t       tmp_size;
    size_t       req_size = size;
    mblk_pool_t  *mm_pool;

    if (!mmhead) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(mmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif

    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    mm_pool = (mblk_pool_t *)mmhead->fixedmblk->mbinfo.buffer;

    if (size <= DEF_FIX_BLK_SIZE && mm_pool->blk_avail > 0) {
        retptr =  k_mm_smallblk_alloc(mmhead, size);
        if (retptr) {

            VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));
            VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
            RHINO_CRITICAL_EXIT();
#else
            krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif

            return retptr;
        }
    }
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    size = MM_ALIGN_UP(size);
    size = (size < AOS_MM_TLF_ALLOC_MIN_LENGTH) ? AOS_MM_TLF_ALLOC_MIN_LENGTH : size;

    /* Rounding up the requested size and calculating fl and sl */
    if (bitmap_search(size, &fl, &sl, ACTION_GET) != RHINO_SUCCESS) {
        retptr = NULL;
        goto ALLOCEXIT;
    }

    /* Searching a free block, recall that this function changes the values of fl and sl,
       so they are not longer valid when the function fails */
    b = findblk_byidx(mmhead, &fl, &sl);
    if (!b) {
        retptr = NULL;
        goto ALLOCEXIT;
    }

    VGF(VALGRIND_MAKE_MEM_DEFINED(b, sizeof(k_mm_list_t)));

    mmhead->mm_tbl[fl][sl] = b->mbinfo.free_ptr.next;
    if (mmhead->mm_tbl[fl][sl]) {
        VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead->mm_tbl[fl][sl], sizeof(k_mm_list_t)));
        mmhead->mm_tbl[fl][sl]->mbinfo.free_ptr.prev = NULL;
        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->mm_tbl[fl][sl], sizeof(k_mm_list_t)));
    } else {
        krhino_bitmap_clear(&mmhead->sl_bitmap[fl], 31 - sl) ;
        if (!mmhead->sl_bitmap[fl]) {
            krhino_bitmap_clear (&mmhead->fl_bitmap, 31 - fl);
        }
    }

    b->mbinfo.free_ptr.prev =  NULL;
    b->mbinfo.free_ptr.next =  NULL;

    /*found: */
    next_b = NEXT_MM_BLK(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK);

    VGF(VALGRIND_MAKE_MEM_DEFINED(next_b, sizeof(k_mm_list_t)));
    /* Should the block be split? */
    tmp_size = (b->size & RHINO_MM_BLKSIZE_MASK) - size;
    if (tmp_size >= sizeof(k_mm_list_t)) {
        tmp_size -= MMLIST_HEAD_SIZE;
        b2 = NEXT_MM_BLK(b->mbinfo.buffer, size);

        VGF(VALGRIND_MAKE_MEM_DEFINED(b2, sizeof(k_mm_list_t)));

        b2->size = tmp_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        b2->dye  = RHINO_MM_FREE_DYE;
#endif
        next_b->prev = b2;
        bitmap_search(tmp_size, &fl, &sl, ACTION_INSERT);
        insert_block(mmhead, b2, fl, sl);
        VGF(VALGRIND_MAKE_MEM_NOACCESS(b2->mbinfo.buffer, b2->size & RHINO_MM_PRESTAT_MASK));
        VGF(VALGRIND_MAKE_MEM_NOACCESS(b2, MMLIST_HEAD_SIZE));

        b->size = size | (b->size & RHINO_MM_PRESTAT_MASK);
    } else {
        next_b->size &= (~RHINO_MM_PREVFREE);
        b->size &= (~RHINO_MM_FREE);       /* Now it's used */
    }
    VGF(VALGRIND_MAKE_MEM_NOACCESS(next_b, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MALLOCLIKE_BLOCK(b->mbinfo.buffer, req_size, 0, 0));
    VGF(VALGRIND_MAKE_MEM_UNDEFINED(b->mbinfo.buffer, req_size));


#if (RHINO_CONFIG_MM_DEBUG > 0u)
    b->dye   = RHINO_MM_CORRUPT_DYE;
#endif
    retptr = (void *) b->mbinfo.buffer;
    if (retptr != NULL) {
        stats_addsize(mmhead, ((b->size & RHINO_MM_BLKSIZE_MASK)
                               + MMLIST_HEAD_SIZE), req_size);
    }
    VGF(VALGRIND_MAKE_MEM_NOACCESS(b, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));

ALLOCEXIT:

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif

    return retptr ;

}

void  k_mm_free(k_mm_head *mmhead, void *ptr)
{
    k_mm_list_t *b,      *tmp_b;
    size_t       fl = 0, sl = 0;

    if (!ptr || !mmhead) {
        return;
    }
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(mmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    if (ISFIXEDBLK(mmhead, ptr)) {

        /*it's fixed size memory block*/
        k_mm_smallblk_free(mmhead, ptr);
        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));
        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
        RHINO_CRITICAL_EXIT();
#else
        krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif

        return;
    }
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    b = (k_mm_list_t *) ((char *) ptr - MMLIST_HEAD_SIZE);
    VGF(VALGRIND_MAKE_MEM_DEFINED(b, sizeof(k_mm_list_t)));

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (b->dye == RHINO_MM_FREE_DYE) {
        printf("WARNING!! memory maybe double free!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
    if (b->dye != RHINO_MM_CORRUPT_DYE) {
        printf("WARNING,memory maybe corrupt!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
    b->dye = RHINO_MM_FREE_DYE;
#endif
    b->size |= RHINO_MM_FREE;

    VGF(VALGRIND_FREELIKE_BLOCK(ptr, 0));
    VGF(VALGRIND_MAKE_MEM_DEFINED(b, sizeof(k_mm_list_t)));

    stats_removesize(mmhead,
                     ((b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE));

    tmp_b = NEXT_MM_BLK(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK);
    VGF(VALGRIND_MAKE_MEM_DEFINED(tmp_b, MMLIST_HEAD_SIZE));
    if (tmp_b->size & RHINO_MM_FREE) {
        bitmap_search(tmp_b->size & RHINO_MM_BLKSIZE_MASK, &fl, &sl, ACTION_INSERT);
        get_block( mmhead, tmp_b, fl, sl);
        b->size += (tmp_b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE;
    }
    if (b->size & RHINO_MM_PREVFREE) {
        tmp_b = b->prev;
        VGF(VALGRIND_MAKE_MEM_DEFINED(tmp_b, sizeof(k_mm_list_t)));
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        if (tmp_b->dye != RHINO_MM_FREE_DYE) {
            printf("WARNING,memory overwritten!!\r\n");
            k_err_proc(RHINO_SYS_FATAL_ERR);
        }
#endif
        bitmap_search(tmp_b->size & RHINO_MM_BLKSIZE_MASK, &fl, &sl, ACTION_INSERT);
        get_block(mmhead, tmp_b, fl, sl);
        tmp_b->size += (b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE;
        b = tmp_b;
    }
    bitmap_search(b->size & RHINO_MM_BLKSIZE_MASK, &fl, &sl, ACTION_INSERT);
    insert_block(mmhead, b, fl, sl);

    tmp_b = NEXT_MM_BLK(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK);
    VGF(VALGRIND_MAKE_MEM_DEFINED(tmp_b, MMLIST_HEAD_SIZE));
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (tmp_b->dye != RHINO_MM_FREE_DYE && tmp_b->dye != RHINO_MM_CORRUPT_DYE) {
        printf("WARNING,memory overwritten!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
#endif
    tmp_b->size |= RHINO_MM_PREVFREE;
    tmp_b->prev = b;
    VGF(VALGRIND_MAKE_MEM_NOACCESS(tmp_b, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(b, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&mmhead->mm_mutex);
#endif

}




void *k_mm_realloc(k_mm_head *mmhead, void *oldmem, size_t new_size)
{
    void        *ptr_aux = NULL;
    unsigned int cpsize;
    k_mm_list_t *b, *tmp_b, *next_b;
    size_t       fl, sl;
    size_t       tmp_size;
    size_t       req_size;

    if (!oldmem) {
        if (new_size) {
            return (void *) k_mm_alloc(mmhead, new_size);
        }

        if (!new_size) {
            return NULL;
        }
    } else if (!new_size) {
        k_mm_free(mmhead, oldmem);
        return NULL;
    }

    req_size =  new_size;

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&mmhead->mm_mutex, RHINO_WAIT_FOREVER);
#endif

    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    /*begin of oldmem in mmblk case*/
    if (ISFIXEDBLK(mmhead, oldmem)) {

        /*it's fixed size memory block*/
        if (new_size <= DEF_FIX_BLK_SIZE) {

            VGF(VALGRIND_FREELIKE_BLOCK(oldmem, 0));
            VGF(VALGRIND_MALLOCLIKE_BLOCK(oldmem, new_size, 0, 0));

            ptr_aux = oldmem;
        } else {
            tmp_size = DEF_FIX_BLK_SIZE;
            ptr_aux  = k_mm_alloc(mmhead, new_size);
            VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
            if (ptr_aux) {
                memcpy(ptr_aux, oldmem, DEF_FIX_BLK_SIZE);
                k_mm_smallblk_free(mmhead, oldmem);
            }
        }

        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));
        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
        RHINO_CRITICAL_EXIT();
#else
        krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif
        return ptr_aux;
    }
    /*end of mmblk case*/
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    /*check if there more free block behind oldmem  */
    b        = (k_mm_list_t *) ((char *) oldmem - MMLIST_HEAD_SIZE);

    VGF(VALGRIND_MAKE_MEM_DEFINED(b, sizeof(k_mm_list_t)));

    tmp_size = (b->size & RHINO_MM_BLKSIZE_MASK);

    next_b   = NEXT_MM_BLK(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK);
    VGF(VALGRIND_MAKE_MEM_DEFINED(next_b, sizeof(k_mm_list_t)));
    new_size = (new_size < sizeof(k_mm_list_t)) ? sizeof(k_mm_list_t) : MM_ALIGN_UP(
                   new_size);

    if (new_size <= tmp_size) {

        VGF(VALGRIND_FREELIKE_BLOCK(oldmem, 0));
        stats_removesize(mmhead,
                         ((b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE));
        if (next_b->size & RHINO_MM_FREE) {
            bitmap_search(next_b->size & RHINO_MM_BLKSIZE_MASK, &fl, &sl, ACTION_INSERT);
            get_block(mmhead, next_b, fl, sl);
            tmp_size += (next_b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE;
            next_b = NEXT_MM_BLK(next_b->mbinfo.buffer,
                                 next_b->size & RHINO_MM_BLKSIZE_MASK);
            VGF(VALGRIND_MAKE_MEM_DEFINED(next_b, sizeof(k_mm_list_t)));
            /* We allways reenter this free block because tmp_size will
               be greater then sizeof (bhdr_t) */
        }
        tmp_size -= new_size;
        if (tmp_size >= sizeof(k_mm_list_t)) {
            tmp_size -= MMLIST_HEAD_SIZE;
            tmp_b = NEXT_MM_BLK(b->mbinfo.buffer, new_size);

            VGF(VALGRIND_MAKE_MEM_DEFINED(tmp_b, sizeof(k_mm_list_t)));

            tmp_b->size = tmp_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
            tmp_b->dye   = RHINO_MM_FREE_DYE;
            tmp_b->owner = 0;
#endif
            next_b->prev = tmp_b;
            next_b->size |= RHINO_MM_PREVFREE;
            bitmap_search(tmp_size, &fl, &sl, ACTION_INSERT);
            insert_block(mmhead, tmp_b, fl, sl);
            VGF(VALGRIND_MAKE_MEM_NOACCESS(tmp_b, MMLIST_HEAD_SIZE + (tmp_b->size & RHINO_MM_BLKSIZE_MASK)));
            b->size = new_size | (b->size & RHINO_MM_PRESTAT_MASK);
        }
        stats_addsize(mmhead, ((b->size & RHINO_MM_BLKSIZE_MASK)
                               + MMLIST_HEAD_SIZE), req_size);
        ptr_aux = (void *) b->mbinfo.buffer;
    } else if ((next_b->size & RHINO_MM_FREE)) {
        if (new_size <= (tmp_size + (next_b->size & RHINO_MM_BLKSIZE_MASK))) {

            VGF(VALGRIND_FREELIKE_BLOCK(oldmem, 0));
            stats_removesize(mmhead,
                             ((b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE));
            bitmap_search(next_b->size & RHINO_MM_BLKSIZE_MASK, &fl, &sl, ACTION_INSERT);
            get_block(mmhead, next_b, fl, sl);
            b->size += (next_b->size & RHINO_MM_BLKSIZE_MASK) + MMLIST_HEAD_SIZE;
            next_b = NEXT_MM_BLK(b->mbinfo.buffer, b->size & RHINO_MM_BLKSIZE_MASK);
            VGF(VALGRIND_MAKE_MEM_DEFINED(next_b, sizeof(k_mm_list_t)));
            next_b->prev = b;
            next_b->size &= ~RHINO_MM_PREVFREE;
            tmp_size = (b->size & RHINO_MM_BLKSIZE_MASK) - new_size;
            if (tmp_size >= sizeof(k_mm_list_t)) {
                tmp_size -= MMLIST_HEAD_SIZE;
                tmp_b = NEXT_MM_BLK(b->mbinfo.buffer, new_size);
                VGF(VALGRIND_MAKE_MEM_DEFINED(tmp_b, sizeof(k_mm_list_t)));
                tmp_b->size = tmp_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
                tmp_b->dye   = RHINO_MM_FREE_DYE;
                tmp_b->owner = 0;
#endif
                next_b->prev = tmp_b;
                next_b->size |= RHINO_MM_PREVFREE;
                bitmap_search(tmp_size, &fl, &sl, ACTION_INSERT);
                insert_block(mmhead, tmp_b, fl, sl);

                VGF(VALGRIND_MAKE_MEM_NOACCESS(tmp_b, MMLIST_HEAD_SIZE + (tmp_b->size & RHINO_MM_BLKSIZE_MASK)));

                b->size = new_size | (b->size & RHINO_MM_PRESTAT_MASK);
            }
            stats_addsize(mmhead, ((b->size & RHINO_MM_BLKSIZE_MASK)
                                   + MMLIST_HEAD_SIZE), req_size);
            ptr_aux = (void *) b->mbinfo.buffer;
        }
    }
    VGF(VALGRIND_MAKE_MEM_NOACCESS(next_b, MMLIST_HEAD_SIZE));

    if (ptr_aux) {

#if (RHINO_CONFIG_MM_DEBUG > 0u)
        b->dye   = RHINO_MM_CORRUPT_DYE;
#endif

        VGF(VALGRIND_MALLOCLIKE_BLOCK(b->mbinfo.buffer, req_size, 0, 0));
        VGF(VALGRIND_MAKE_MEM_UNDEFINED(b->mbinfo.buffer, req_size));
        VGF(VALGRIND_MAKE_MEM_NOACCESS(b, MMLIST_HEAD_SIZE));
        VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
        RHINO_CRITICAL_EXIT();
#else
        krhino_mutex_unlock(&mmhead->mm_mutex);
#endif
        return ptr_aux;
    }

    if (!(ptr_aux = k_mm_alloc(mmhead, new_size))) {
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
        RHINO_CRITICAL_EXIT();
#else
        krhino_mutex_unlock(&mmhead->mm_mutex);
#endif
        return NULL;
    }
    cpsize = ((b->size & RHINO_MM_BLKSIZE_MASK) > new_size) ? new_size :
             (b->size & RHINO_MM_BLKSIZE_MASK);

    /*need define all b->size to defined because algin size cannot  access */
    VGF(VALGRIND_MAKE_MEM_DEFINED(oldmem, b->size & RHINO_MM_BLKSIZE_MASK));
    memcpy(ptr_aux, oldmem, cpsize);
    k_mm_free(mmhead, oldmem);

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&mmhead->mm_mutex);
#endif
    return ptr_aux;

}

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
void krhino_owner_attach(k_mm_head *mmhead, void *addr, size_t allocator)
{
    k_mm_list_t *blk;

    if (!mmhead || !addr) {
        return;
    }

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&(mmhead->mm_mutex), RHINO_WAIT_FOREVER);
#endif

    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead, sizeof(k_mm_head)));
    VGF(VALGRIND_MAKE_MEM_DEFINED(mmhead->fixedmblk, MMLIST_HEAD_SIZE));

    if (!ISFIXEDBLK(mmhead, addr)) {
        blk = (k_mm_list_t *) ((char *) addr - MMLIST_HEAD_SIZE);
        VGF(VALGRIND_MAKE_MEM_DEFINED(blk, MMLIST_HEAD_SIZE));
        blk->owner = allocator;
        VGF(VALGRIND_MAKE_MEM_NOACCESS(blk, MMLIST_HEAD_SIZE));
    }

    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead->fixedmblk, MMLIST_HEAD_SIZE));
    VGF(VALGRIND_MAKE_MEM_NOACCESS(mmhead, sizeof(k_mm_head)));

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&(mmhead->mm_mutex));
#endif
}
#endif

void *krhino_mm_alloc(size_t size)
{
    void *tmp;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    unsigned int app_malloc = size & AOS_UNSIGNED_INT_MSB;
    size = size & (~AOS_UNSIGNED_INT_MSB);
#endif

    if (size == 0) {
        printf("WARNING, malloc size = 0\r\n");
        return NULL;
    }

    tmp = k_mm_alloc(g_kmm_head, size);
    if (tmp == NULL) {
#if (RHINO_CONFIG_MM_DEBUG > 0)
        static int dumped;
        printf("WARNING, malloc failed!!!!\r\n");
        if (dumped) {
            return tmp;
        }
        dumped = 1;
        dumpsys_mm_info_func(NULL, 0);
#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
        dump_mmleak();
#endif
        k_err_proc(RHINO_NO_MEM);
#endif
    }

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_mm_alloc_hook(tmp, size);
#endif

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if (app_malloc == 0) {
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
    }
#endif

    return tmp;
}

void krhino_mm_free(void *ptr)
{
    return k_mm_free(g_kmm_head, ptr);
}

void *krhino_mm_realloc(void *oldmem, size_t newsize)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    unsigned int app_malloc = newsize & AOS_UNSIGNED_INT_MSB;
    newsize = newsize & (~AOS_UNSIGNED_INT_MSB);
#endif

    void *tmp = k_mm_realloc(g_kmm_head, oldmem, newsize);

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if (app_malloc == 0) {
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
    }
#endif
    if (tmp == NULL && newsize != 0) {
#if (RHINO_CONFIG_MM_DEBUG > 0)
        static int reallocdumped;
        printf("WARNING, realloc failed!!!!\r\n");
        if (reallocdumped) {
            return tmp;
        }
        reallocdumped = 1;
        dumpsys_mm_info_func(NULL, 0);
#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
        dump_mmleak();
#endif
        k_err_proc(RHINO_SYS_FATAL_ERR);
#endif
    }
    return tmp;
}

#endif

