/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#if (RHINO_CONFIG_MM_TLF > 0)
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
#define MM_CRITICAL_ENTER(pMutex)   \
    RHINO_CRITICAL_ENTER()
#define MM_CRITICAL_EXIT(pMutex)    \
    RHINO_CRITICAL_EXIT()
#else
#define MM_CRITICAL_ENTER(pMutex)   \
    do {                            \
        RHINO_CRITICAL_ENTER();     \
        if (g_intrpt_nested_level[cpu_cur_get()] > 0u) { \
            k_err_proc(RHINO_NOT_CALLED_BY_INTRPT); \
        }                           \
        RHINO_CRITICAL_EXIT();      \
        krhino_mutex_lock(pMutex, RHINO_WAIT_FOREVER); \
    }while(0);
#define MM_CRITICAL_EXIT(pMutex)    \
    krhino_mutex_unlock(pMutex)
#endif

#define MM_IS_FIXEDBLK(mh,ptr) \
        (mh->fixedmblk && ((void *)ptr > (void *)(mh->fixedmblk->mbinfo.buffer))            \
        && ((void *)ptr < (void *)(mh->fixedmblk->mbinfo.buffer + mh->fixedmblk->buf_size)))
        
extern k_mm_region_t   g_mm_region[];
extern int             g_region_num;
extern void aos_mm_leak_region_init(void);

/* Count Leading Zeros (clz) 
   counts the number of zero bits preceding the most significant one bit. */
RHINO_INLINE int k_mm_clz(uint32_t x)
{
    int n = 0;
    
    if ( x == 0 ) {
        return 32;
    }
    
    if ((x & 0XFFFF0000) == 0) {
        x <<= 16;
        n += 16;
    }
    if ((x & 0XFF000000) == 0) {
        x <<= 8;
        n += 8;
    }
    if ((x & 0XF0000000) == 0) {
        x <<= 4;
        n += 4;
    }
    if ((x & 0XC0000000) == 0) {
        x <<= 2;
        n += 2;
    }
    if ((x & 0X80000000) == 0) {
        n += 1;
    }
    
    return n;
}

/* Count Trailing Zeros (ctz) 
   counts the number of zero bits succeeding the least significant one bit. */
RHINO_INLINE int k_mm_ctz(uint32_t x)
{
    int n = 0;
    
    if ( x == 0 ) {
        return 32;
    }
    
    if ((x & 0X0000FFFF) == 0) {
        x >>= 16;
        n += 16;
    }
    if ((x & 0X000000FF) == 0) {
        x >>= 8;
        n += 8;
    }
    if ((x & 0X0000000F) == 0) {
        x >>= 4;
        n += 4;
    }
    if ((x & 0X00000003) == 0) {
        x >>= 2;
        n += 2;
    }
    if ((x & 0X00000001) == 0) {
        n += 1;
    }
    
    return n;
}


void k_mm_init(void)
{
    uint32_t e = 0;

    /* init memory region */
    krhino_init_mm_head(&g_kmm_head, g_mm_region[0].start, g_mm_region[0].len);
    for (e = 1 ; e < g_region_num ; e++) {
        krhino_add_mm_region(g_kmm_head, g_mm_region[e].start, g_mm_region[e].len);
    }

#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )
    aos_mm_leak_region_init();
#endif
}

/* init a region, contain 3 mmblk
   -------------------------------------------------------------------
   | k_mm_list_t | k_mm_region_info_t | k_mm_list_t | free space |k_mm_list_t|
   -------------------------------------------------------------------

   "regionaddr" and "len" is aligned by caller */
RHINO_INLINE k_mm_list_t *init_mm_region(void *regionaddr, size_t len)
{
    k_mm_list_t        *midblk, *lastblk, *firstblk;
    k_mm_region_info_t *region;

    /* "regionaddr" and "len" is aligned by caller */

    /*first mmblk for region info*/
    firstblk = (k_mm_list_t *) regionaddr;
    firstblk->prev  = NULL;
    firstblk->buf_size = MM_ALIGN_UP(sizeof(k_mm_region_info_t))
                       | RHINO_MM_ALLOCED | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    firstblk->dye   = RHINO_MM_CORRUPT_DYE;
    firstblk->owner = 0;
#endif

    /*last mmblk for stop merge */
    lastblk = (k_mm_list_t *)((char *)regionaddr + len - MMLIST_HEAD_SIZE);

    /*middle mmblk for heap use */
    midblk = MM_GET_NEXT_BLK(firstblk);
    midblk->buf_size = ((char *)lastblk - (char *)midblk->mbinfo.buffer)
                     | RHINO_MM_ALLOCED | RHINO_MM_PREVALLOCED;
    midblk->mbinfo.free_ptr.prev = midblk->mbinfo.free_ptr.next = 0;

    /*last mmblk for stop merge */
    lastblk->prev   = midblk;
    /* set alloced, can't be merged */
    lastblk->buf_size = 0 | RHINO_MM_ALLOCED | RHINO_MM_PREVFREE;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    lastblk->dye    = RHINO_MM_CORRUPT_DYE;
    lastblk->owner  = 0;
#endif

    region = (k_mm_region_info_t *)firstblk->mbinfo.buffer;
    region->next = 0;
    region->end = lastblk;

    return firstblk;
}

/* 2^(N + MM_MIN_BIT) <= size < 2^(1 + N + MM_MIN_BIT) */
static int size_to_level(size_t size)
{
    size_t cnt;
    cnt = 32 - k_mm_clz(size);
    if ( cnt < MM_MIN_BIT )
    {
        return 0;
    }
    
    if ( cnt > MM_MAX_BIT)
    {
        return -1;
    }
    return cnt - MM_MIN_BIT;
}

#if(K_MM_STATISTIC > 0)
static void addsize(k_mm_head *mmhead, size_t size, size_t req_size)
{
    int level;
    
    if (mmhead->free_size > size) {
        mmhead->free_size -= size;
    } else {
        mmhead->free_size = 0;
    }
    mmhead->used_size += size;
    if (mmhead->used_size > mmhead->maxused_size) {
        mmhead->maxused_size = mmhead->used_size;
    }

    level = size_to_level(req_size);
    if ( level != -1 )
    {
        mmhead->mm_size_stats[level]++;
    }
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
    k_mm_list_t *nextblk;
    k_mm_list_t *firstblk;
    k_mm_head   *pmmhead;
    void        *orig_addr;
#if (RHINO_CONFIG_MM_TLF_BLK_SIZE > 0)
    k_mm_list_t *curblk;
    mblk_pool_t *mmblk_pool;
    kstat_t      stat;
#endif
    CPSR_ALLOC();

    NULL_PARA_CHK(ppmmhead);
    NULL_PARA_CHK(addr);

    /*check paramters, addr and len need algin
      1.  the length at least need RHINO_CONFIG_MM_TLF_BLK_SIZE  for fixed size memory block
      2.  and also ast least have 1k for user alloced
    */
    orig_addr = addr;
    addr = (void *) MM_ALIGN_UP((size_t)addr);
    len -= (size_t)addr - (size_t)orig_addr;
    len = MM_ALIGN_DOWN(len);

    if ( len == 0
         || len < MIN_FREE_MEMORY_SIZE + RHINO_CONFIG_MM_TLF_BLK_SIZE
         || len > MM_MAX_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    pmmhead = (k_mm_head *)addr;

    /* Zeroing the memory head */
    memset(pmmhead, 0, sizeof(k_mm_head));
#if (RHINO_CONFIG_MM_REGION_MUTEX > 0)
    krhino_mutex_create(&pmmhead->mm_mutex, "mm_mutex");
#endif

    MM_CRITICAL_ENTER(&(pmmhead->mm_mutex));

    firstblk = init_mm_region((void *)((size_t)addr + MM_ALIGN_UP(sizeof(k_mm_head))),
                              MM_ALIGN_DOWN(len - sizeof(k_mm_head)));


    pmmhead->regioninfo = (k_mm_region_info_t *)firstblk->mbinfo.buffer;

    nextblk = MM_GET_NEXT_BLK(firstblk);

    *ppmmhead = pmmhead;

    /*mark it as free and set it to bitmap*/
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    nextblk->dye   = RHINO_MM_CORRUPT_DYE;
    nextblk->owner = 0;
#endif

    /* release free blk */
    k_mm_free(pmmhead, nextblk->mbinfo.buffer);
    
    /*after free, we need acess mmhead and nextblk again*/
    
#if (K_MM_STATISTIC > 0)
    pmmhead->free_size = MM_GET_BUF_SIZE(nextblk);
    pmmhead->used_size = len - MM_GET_BUF_SIZE(nextblk);
    pmmhead->maxused_size = pmmhead->used_size;
#endif
    /* default no fixblk */
    pmmhead->fixedmblk = NULL;

#if (RHINO_CONFIG_MM_TLF_BLK_SIZE > 0)
    mmblk_pool = k_mm_alloc(pmmhead,
                            RHINO_CONFIG_MM_TLF_BLK_SIZE + MM_ALIGN_UP(sizeof(mblk_pool_t)));
    if (mmblk_pool) {
        curblk = MM_GET_THIS_BLK(mmblk_pool);
        stat = krhino_mblk_pool_init(mmblk_pool, "fixed_mm_blk",
                                     (void *)((size_t)mmblk_pool + MM_ALIGN_UP(sizeof(mblk_pool_t))),
                                     DEF_FIX_BLK_SIZE, RHINO_CONFIG_MM_TLF_BLK_SIZE);
        if (stat == RHINO_SUCCESS) {
            pmmhead->fixedmblk = curblk;
        } else {
            k_mm_free(pmmhead, mmblk_pool);
        }
#if (K_MM_STATISTIC > 0)
        stats_removesize(pmmhead, RHINO_CONFIG_MM_TLF_BLK_SIZE);
        pmmhead->maxused_size = pmmhead->used_size;
#endif
    }
#endif

    MM_CRITICAL_EXIT(&(pmmhead->mm_mutex));

    return RHINO_SUCCESS;
}

kstat_t krhino_deinit_mm_head(k_mm_head *mmhead)
{
#if (RHINO_CONFIG_MM_REGION_MUTEX > 0)
    krhino_mutex_del(&mmhead->mm_mutex);
#endif

    memset(mmhead, 0, sizeof(k_mm_head));
    return RHINO_SUCCESS;
}

kstat_t krhino_add_mm_region(k_mm_head *mmhead, void *addr, size_t len)
{
    void *orig_addr;
    k_mm_region_info_t *region;
    k_mm_list_t        *firstblk, *nextblk;
    CPSR_ALLOC();

    NULL_PARA_CHK(mmhead);
    NULL_PARA_CHK(addr);

    orig_addr = addr;
    addr = (void *) MM_ALIGN_UP((size_t)addr);
    len -= (size_t)addr - (size_t)orig_addr;
    len = MM_ALIGN_DOWN(len);

    if ( !len || len < sizeof(k_mm_region_info_t) + MMLIST_HEAD_SIZE * 3 + MM_MIN_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    memset(addr, 0, len);

    MM_CRITICAL_ENTER(&(mmhead->mm_mutex));

    firstblk = init_mm_region(addr, len);
    nextblk  = MM_GET_NEXT_BLK(firstblk);

    /* Inserting the area in the list of linked areas */
    region = (k_mm_region_info_t *)firstblk->mbinfo.buffer;
    region->next = mmhead->regioninfo;
    mmhead->regioninfo = region;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    nextblk->dye   = RHINO_MM_CORRUPT_DYE;
    nextblk->owner = 0;
#endif

    /* change used_size with nextblk size*/
#if (K_MM_STATISTIC > 0)
    mmhead->used_size += len;
#endif
    /*mark nextblk as free*/
    k_mm_free(mmhead, nextblk->mbinfo.buffer);

    MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
    
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

    stats_addsize(mmhead, DEF_FIX_BLK_SIZE, size);

    return tmp;
}
static void k_mm_smallblk_free(k_mm_head *mmhead, void *ptr)
{
    kstat_t sta;

    if (!mmhead || !ptr) {
        return;
    }

    sta = krhino_mblk_free((mblk_pool_t *)mmhead->fixedmblk->mbinfo.buffer, ptr);
    if (sta != RHINO_SUCCESS) {
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    stats_removesize(mmhead, DEF_FIX_BLK_SIZE);
}

/* insert blk to freelist[level], and set freebitmap */
static void k_mm_freelist_insert(k_mm_head *mmhead, k_mm_list_t *blk)
{
    int level;

    level = size_to_level(MM_GET_BUF_SIZE(blk));
    if ( level < 0 || level >= MM_BIT_LEVEL )
    {
        return;
    }
    
    /* free list is LIFO */

    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = mmhead->freelist[level];

    if (mmhead->freelist[level] != NULL) {
        mmhead->freelist[level]->mbinfo.free_ptr.prev = blk;
    }

    mmhead->freelist[level] = blk;
    
    /* freelist not null, so set the bit  */
    mmhead->free_bitmap |= (1<<level);
}

/* get blk from freelist[level], and clear freebitmap if needed */
static void k_mm_freelist_delete(k_mm_head *mmhead, k_mm_list_t *blk)
{
    int level;

    level = size_to_level(MM_GET_BUF_SIZE(blk));
    if ( level < 0 || level >= MM_BIT_LEVEL )
    {
        return;
    }

    if (blk->mbinfo.free_ptr.next != NULL) {
        blk->mbinfo.free_ptr.next->mbinfo.free_ptr.prev = blk->mbinfo.free_ptr.prev;
    }
    if (blk->mbinfo.free_ptr.prev != NULL) {
        blk->mbinfo.free_ptr.prev->mbinfo.free_ptr.next = blk->mbinfo.free_ptr.next;
    }
    
    if (mmhead->freelist[level] == blk) {
        /* first blk in this freelist */
        mmhead->freelist[level] = blk->mbinfo.free_ptr.next;
        if (mmhead->freelist[level] == NULL) {
            /* freelist null, so clear the bit  */
            mmhead->free_bitmap &= (~(1<<level));
        }
    }
    
    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = NULL;
}

/* find a freelist at higher level */
static k_mm_list_t *find_up_level(k_mm_head *mmhead, int level)
{
    uint32_t bitmap;
    
    bitmap = mmhead->free_bitmap & (0xfffffffful<<(level+1));
    level  = k_mm_ctz(bitmap);

    if ( level < MM_BIT_LEVEL )
    {
        return mmhead->freelist[level];
    }

    return NULL;
}

void *k_mm_alloc(k_mm_head *mmhead, size_t size)
{
    void        *retptr;
    k_mm_list_t *get_b, *new_b, *next_b;
    int          level;
    size_t       left_size;
    size_t       req_size = size;
    mblk_pool_t *mm_pool;
    CPSR_ALLOC();

    (void)req_size;

    if (!mmhead) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    MM_CRITICAL_ENTER(&(mmhead->mm_mutex));

    /* little blk, try to get from mm_pool */
    if(mmhead->fixedmblk != NULL) {
        mm_pool = (mblk_pool_t *)mmhead->fixedmblk->mbinfo.buffer;
        if (size <= DEF_FIX_BLK_SIZE && mm_pool->blk_avail > 0) {
            retptr =  k_mm_smallblk_alloc(mmhead, size);
            if (retptr) {
                MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
                return retptr;
            }
        }
    }

    retptr = NULL;

    size = MM_ALIGN_UP(size);
    size = size < MM_MIN_SIZE ? MM_MIN_SIZE : size;
    
    if ((level = size_to_level(size)) == -1) {
        goto ALLOCEXIT;
    }

    /* try to find in higher level */
    get_b = find_up_level(mmhead, level);
    if (get_b == NULL) {
        /* try to find in same level */
        get_b = mmhead->freelist[level];
        while ( get_b != NULL )        
        {
            if ( MM_GET_BUF_SIZE(get_b) >= size )
            {
                break;
            }
            get_b = get_b->mbinfo.free_ptr.next;
        }

        if ( get_b == NULL )
        {   
            /* do not find availalbe freeblk */
            goto ALLOCEXIT;
        }
    }
    k_mm_freelist_delete(mmhead, get_b);

    next_b = MM_GET_NEXT_BLK(get_b);

    /* Should the block be split? */
    if (MM_GET_BUF_SIZE(get_b) >= size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
        left_size = MM_GET_BUF_SIZE(get_b) - size - MMLIST_HEAD_SIZE;
        
        get_b->buf_size = size | (get_b->buf_size & RHINO_MM_PRESTAT_MASK);
        new_b = MM_GET_NEXT_BLK(get_b);
        
        new_b->prev = get_b;
        new_b->buf_size = left_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        new_b->dye   = RHINO_MM_FREE_DYE;
        new_b->owner = 0;
#endif
        next_b->prev = new_b;
        k_mm_freelist_insert(mmhead, new_b);
    } else {
        next_b->buf_size &= (~RHINO_MM_PREVFREE);
    }
    get_b->buf_size &= (~RHINO_MM_FREE);       /* Now it's used */

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    get_b->dye   = RHINO_MM_CORRUPT_DYE;
    get_b->owner = 0;
#endif
    retptr = (void *)get_b->mbinfo.buffer;
    if (retptr != NULL) {
        stats_addsize(mmhead, MM_GET_BLK_SIZE(get_b), req_size);
    }

ALLOCEXIT:

    MM_CRITICAL_EXIT(&(mmhead->mm_mutex));

    return retptr ;
}

void  k_mm_free(k_mm_head *mmhead, void *ptr)
{
    k_mm_list_t *free_b, *next_b, *prev_b;
    CPSR_ALLOC();

    if (!ptr || !mmhead) {
        return;
    }

    MM_CRITICAL_ENTER(&(mmhead->mm_mutex));

    /* little blk, free to mm_pool */
    if (MM_IS_FIXEDBLK(mmhead, ptr)) {
        /*it's fixed size memory block*/
        k_mm_smallblk_free(mmhead, ptr);
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        return;
    }
    
    free_b = MM_GET_THIS_BLK(ptr);

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (free_b->dye == RHINO_MM_FREE_DYE) {
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        printf("WARNING!! memory maybe double free!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
    if (free_b->dye != RHINO_MM_CORRUPT_DYE) {
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        printf("WARNING,memory maybe corrupt!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
    free_b->dye   = RHINO_MM_FREE_DYE;
    free_b->owner = 0;
#endif
    free_b->buf_size |= RHINO_MM_FREE;

    stats_removesize(mmhead, MM_GET_BLK_SIZE(free_b));
    
    /* if the blk after this freed one is freed too, merge them */
    next_b = MM_GET_NEXT_BLK(free_b);
    if (next_b->buf_size & RHINO_MM_FREE) {
        k_mm_freelist_delete(mmhead, next_b);
        free_b->buf_size += MM_GET_BLK_SIZE(next_b);
    }

    /* if the blk before this freed one is freed too, merge them */
    if (free_b->buf_size & RHINO_MM_PREVFREE) {
        prev_b = free_b->prev;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        if (prev_b->dye != RHINO_MM_FREE_DYE) {
            MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
            printf("WARNING,memory overwritten!!\r\n");
            k_err_proc(RHINO_SYS_FATAL_ERR);
        }
#endif
        k_mm_freelist_delete(mmhead, prev_b);
        prev_b->buf_size += MM_GET_BLK_SIZE(free_b);
        free_b = prev_b;
    }

    /* after merge, free to list */
    k_mm_freelist_insert(mmhead, free_b);

    next_b = MM_GET_NEXT_BLK(free_b);
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (next_b->dye != RHINO_MM_FREE_DYE && next_b->dye != RHINO_MM_CORRUPT_DYE) {
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        printf("WARNING,memory overwritten!!\r\n");
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }
#endif
    next_b->prev = free_b;
    next_b->buf_size |= RHINO_MM_PREVFREE;

    MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
}

void *k_mm_realloc(k_mm_head *mmhead, void *oldmem, size_t new_size)
{
    void        *ptr_aux = NULL;
    unsigned int cpsize;
    k_mm_list_t *this_b, *split_b, *next_b;
    size_t       old_size, split_size;
    size_t       req_size = 0;
    CPSR_ALLOC();

    (void)req_size;

    if (oldmem == NULL) {
        if (new_size > 0) {
            return (void *) k_mm_alloc(mmhead, new_size);
        }
        else
        {
            return NULL;
        }
    } else if (new_size == 0) {
        k_mm_free(mmhead, oldmem);
        return NULL;
    }

    req_size =  new_size;

    MM_CRITICAL_ENTER(&(mmhead->mm_mutex));

    /*begin of oldmem in mmblk case*/
    if (MM_IS_FIXEDBLK(mmhead, oldmem)) {
        /*it's fixed size memory block*/
        if (new_size <= DEF_FIX_BLK_SIZE) {
            ptr_aux = oldmem;
        } else {
            ptr_aux  = k_mm_alloc(mmhead, new_size);
            if (ptr_aux) {
                memcpy(ptr_aux, oldmem, DEF_FIX_BLK_SIZE);
                k_mm_smallblk_free(mmhead, oldmem);
            }
        }
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        return ptr_aux;
    }
    /*end of mmblk case*/

    /*check if there more free block behind oldmem  */
    this_b   = MM_GET_THIS_BLK(oldmem);
    old_size = MM_GET_BUF_SIZE(this_b);
    next_b   = MM_GET_NEXT_BLK(this_b);
    new_size = MM_ALIGN_UP(new_size);
    new_size = new_size < MM_MIN_SIZE ? MM_MIN_SIZE : new_size;

    if (new_size <= old_size) {
        /* shrink blk */        
        stats_removesize(mmhead, MM_GET_BLK_SIZE(this_b));
        if (next_b->buf_size & RHINO_MM_FREE) {
            /* merge next free */
            k_mm_freelist_delete(mmhead, next_b);
            old_size += MM_GET_BLK_SIZE(next_b);
            next_b = MM_GET_NEXT_BLK(next_b);
        }
        if (old_size >= new_size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
            /* split blk */
            split_size = old_size - new_size - MMLIST_HEAD_SIZE;
            
            this_b->buf_size = new_size | (this_b->buf_size & RHINO_MM_PRESTAT_MASK);
            split_b = MM_GET_NEXT_BLK(this_b);
            
            split_b->prev  = this_b;
            split_b->buf_size = split_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
            split_b->dye   = RHINO_MM_FREE_DYE;
            split_b->owner = 0;
#endif
            next_b->prev = split_b;
            next_b->buf_size |= RHINO_MM_PREVFREE;
            k_mm_freelist_insert(mmhead, split_b);
        }
        stats_addsize(mmhead, MM_GET_BLK_SIZE(this_b), req_size);
        ptr_aux = (void *)this_b->mbinfo.buffer;
    } else if ((next_b->buf_size & RHINO_MM_FREE)) {
        /* enlarge blk */
        if (new_size <= (old_size + MM_GET_BLK_SIZE(next_b))) {
            stats_removesize(mmhead, MM_GET_BLK_SIZE(this_b));

            /* delete next blk from freelist */
            k_mm_freelist_delete(mmhead, next_b);
            
            /* enlarge this blk */
            this_b->buf_size += MM_GET_BLK_SIZE(next_b);

            next_b = MM_GET_NEXT_BLK(this_b);
            next_b->prev = this_b;
            next_b->buf_size &= ~RHINO_MM_PREVFREE;

            if (MM_GET_BUF_SIZE(this_b) >= new_size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
                /* split blk */
                split_size = MM_GET_BUF_SIZE(this_b) - new_size - MMLIST_HEAD_SIZE;
                
                this_b->buf_size = new_size | (this_b->buf_size & RHINO_MM_PRESTAT_MASK);
                split_b = MM_GET_NEXT_BLK(this_b);
                
                split_b->prev = this_b;
                split_b->buf_size = split_size | RHINO_MM_FREE | RHINO_MM_PREVALLOCED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
                split_b->dye   = RHINO_MM_FREE_DYE;
                split_b->owner = 0;
#endif
                next_b->prev = split_b;
                next_b->buf_size |= RHINO_MM_PREVFREE;
                k_mm_freelist_insert(mmhead, split_b);
            }
            stats_addsize(mmhead, MM_GET_BLK_SIZE(this_b), req_size);
            ptr_aux = (void *)this_b->mbinfo.buffer;
        }
    }

    if (ptr_aux) {

#if (RHINO_CONFIG_MM_DEBUG > 0u)
        this_b->dye   = RHINO_MM_CORRUPT_DYE;
#endif

        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        return ptr_aux;
    }

    /* re alloc blk */
    ptr_aux = k_mm_alloc(mmhead, new_size);
    if (!ptr_aux) {
        MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
        return NULL;
    }
    
    cpsize = (MM_GET_BUF_SIZE(this_b) > new_size) ? new_size : MM_GET_BUF_SIZE(this_b);

    memcpy(ptr_aux, oldmem, cpsize);
    k_mm_free(mmhead, oldmem);

    MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
    return ptr_aux;

}

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
void krhino_owner_attach(k_mm_head *mmhead, void *addr, size_t allocator)
{
    k_mm_list_t *blk;
    CPSR_ALLOC();

    if (!mmhead || !addr) {
        return;
    }

    MM_CRITICAL_ENTER(&(mmhead->mm_mutex));

    if (mmhead->fixedmblk == NULL) {
        blk = MM_GET_THIS_BLK(addr);
        blk->owner = allocator;
    }
    else {
        if (!MM_IS_FIXEDBLK(mmhead, addr)) {
            blk = MM_GET_THIS_BLK(addr);
            blk->owner = allocator;
        }
    }

    MM_CRITICAL_EXIT(&(mmhead->mm_mutex));
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
#if defined (__CC_ARM)
        krhino_owner_attach(g_kmm_head, tmp, __return_address());
#elif defined (__GNUC__)
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif /* __CC_ARM */
    }
#endif

    return tmp;
}

void krhino_mm_free(void *ptr)
{
    k_mm_free(g_kmm_head, ptr);
}

void *krhino_mm_realloc(void *oldmem, size_t newsize)
{
    void *tmp;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    unsigned int app_malloc = newsize & AOS_UNSIGNED_INT_MSB;
    newsize = newsize & (~AOS_UNSIGNED_INT_MSB);
#endif

    tmp = k_mm_realloc(g_kmm_head, oldmem, newsize);

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if (app_malloc == 0) {
#if defined (__CC_ARM)
        krhino_owner_attach(g_kmm_head, tmp, __return_address());
#elif defined (__GNUC__)
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif /* __CC_ARM */
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

