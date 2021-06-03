/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"

#if AOS_COMP_DEBUG
#include "aos/debug.h"
extern uint32_t debug_task_id_now();
extern void debug_cpu_stop(void);
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
extern k_mm_region_t g_mm_region[];
extern int           g_region_num;

#if RHINO_CONFIG_MM_DEBUG
#if (RHINO_CONFIG_MM_TRACE_LVL > 0)

volatile uint32_t g_kmm_bt = 0;
int backtrace_now_get(void *trace[], int size, int offset);

void kmm_bt_disable(void)
{
    g_kmm_bt = KMM_BT_SET_BY_KV;
}

/* check bt status
 * ret 0 : enable
 * ret 1 : disable
 * */
int kmm_bt_check(void)
{
    return (g_kmm_bt == KMM_BT_SET_BY_KV);
}
#endif

void kmm_error(uint32_t mm_status_locked)
{
    dumpsys_mm_info_func(mm_status_locked);
    k_err_proc(RHINO_SYS_FATAL_ERR);
}
#endif

void k_mm_init(void)
{
    uint32_t e = 0;

    /* init memory region */
    (void)krhino_init_mm_head(&g_kmm_head, g_mm_region[0].start, g_mm_region[0].len);
    for (e = 1 ; e < g_region_num ; e++) {
        krhino_add_mm_region(g_kmm_head, g_mm_region[e].start, g_mm_region[e].len);
    }
}

/* init a region, contain 3 mmblk
 * -------------------------------------------------------------------
 * | k_mm_list_t | k_mm_region_info_t | k_mm_list_t | free space |k_mm_list_t|
 * -------------------------------------------------------------------
 *
 * "regionaddr" and "len" is aligned by caller
 */
RHINO_INLINE k_mm_list_t *init_mm_region(void *regionaddr, size_t len)
{
    k_mm_list_t        *midblk, *lastblk, *firstblk;
    k_mm_region_info_t *region;

    /* first mmblk for region info */
    firstblk           = (k_mm_list_t *) regionaddr;
    firstblk->prev     = NULL;
    firstblk->buf_size = MM_ALIGN_UP(sizeof(k_mm_region_info_t))
                         | MM_BUFF_USED | MM_BUFF_PREV_USED;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    firstblk->dye   = MM_DYE_USED;
    firstblk->owner_id = MM_OWNER_ID_SELF;
    firstblk->trace_id = 0;
    firstblk->owner = 0;
#endif

    /*last mmblk for stop merge */
    lastblk = (k_mm_list_t *)((char *)regionaddr + len - MMLIST_HEAD_SIZE);

    /*middle mmblk for heap use */
    midblk = MM_GET_NEXT_BLK(firstblk);

    midblk->buf_size = ((char *)lastblk - (char *)midblk->mbinfo.buffer)
                       | MM_BUFF_USED | MM_BUFF_PREV_USED;

    midblk->mbinfo.free_ptr.prev = midblk->mbinfo.free_ptr.next = 0;

    /*last mmblk for stop merge */
    lastblk->prev = midblk;

    /* set alloced, can't be merged */
    lastblk->buf_size = 0 | MM_BUFF_USED | MM_BUFF_PREV_FREE;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    lastblk->dye   = MM_DYE_USED;
    lastblk->owner_id = MM_OWNER_ID_SELF;
    lastblk->trace_id = 0;
    lastblk->owner = MM_LAST_BLK_MAGIC;
#endif

    region       = (k_mm_region_info_t *)firstblk->mbinfo.buffer;
    region->next = 0;
    region->end  = lastblk;

    return firstblk;
}

/* 2^(N + MM_MIN_BIT) <= size < 2^(1 + N + MM_MIN_BIT) */
static int32_t size_to_level(size_t size)
{
    size_t cnt = 32 - krhino_clz32(size);

    if (cnt < MM_MIN_BIT) {
        return 0;
    }

    if (cnt > MM_MAX_BIT) {
        return -1;
    }

    return cnt - MM_MIN_BIT;
}

#if (K_MM_STATISTIC > 0)
static void addsize(k_mm_head *mmhead, size_t size, size_t req_size)
{
    int32_t level;

    if (mmhead->free_size > size) {
        mmhead->free_size -= size;
    } else {
        mmhead->free_size = 0;
    }

    mmhead->used_size += size;
    if (mmhead->used_size > mmhead->maxused_size) {
        mmhead->maxused_size = mmhead->used_size;
    }

    if (req_size > 0) {
        level = size_to_level(req_size);
        if (level != -1) {
            mmhead->alloc_times[level]++;
        }
    }
}

static void removesize(k_mm_head *mmhead, size_t size)
{
    if (mmhead->used_size > size) {
        mmhead->used_size -= size;
    } else {
        mmhead->used_size = 0;
    }
    mmhead->free_size += size;
}

/* used_size++, free_size--, maybe maxused_size++ */
#define stats_addsize(mmhead, size, req_size) addsize(mmhead, size, req_size)
/* used_size--, free_size++ */
#define stats_removesize(mmhead, size)        removesize(mmhead, size)
#else
#define stats_addsize(mmhead, size, req_size) do {} while (0)
#define stats_removesize(mmhead, size)        do {} while (0)
#endif

kstat_t krhino_init_mm_head(k_mm_head **ppmmhead, void *addr, size_t len)
{
    k_mm_list_t *nextblk;
    k_mm_list_t *firstblk;
    k_mm_head   *pmmhead;
    void        *orig_addr;
#if (RHINO_CONFIG_MM_BLK > 0)
    mblk_pool_t *mmblk_pool;
    kstat_t      stat;
#endif

    NULL_PARA_CHK(ppmmhead);
    NULL_PARA_CHK(addr);

    memset(addr, 0, len);

    /* check paramters, addr and len need algin
     *  1.  the length at least need RHINO_CONFIG_MM_TLF_BLK_SIZE  for fixed size memory block
     *  2.  and also ast least have 1k for user alloced
     */
    orig_addr = addr;

    addr = (void *) MM_ALIGN_UP((size_t)addr);
    len -= (size_t)addr - (size_t)orig_addr;
    len  = MM_ALIGN_DOWN(len);

    if (len == 0
        || len < MM_MIN_HEAP_SIZE + RHINO_CONFIG_MM_TLF_BLK_SIZE
        || len > MM_MAX_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    pmmhead = (k_mm_head *)addr;

    /* Zeroing the memory head */
    memset(pmmhead, 0, sizeof(k_mm_head));

#if (RHINO_CONFIG_MM_REGION_MUTEX > 0)
    krhino_mutex_create(&pmmhead->mm_mutex, "mm_mutex");
#else
    krhino_spin_lock_init(&pmmhead->mm_lock);
#endif

    firstblk = init_mm_region((void *)((size_t)addr + MM_ALIGN_UP(sizeof(k_mm_head))),
                              MM_ALIGN_DOWN(len - sizeof(k_mm_head)));


    pmmhead->regioninfo = (k_mm_region_info_t *)firstblk->mbinfo.buffer;

    nextblk = MM_GET_NEXT_BLK(firstblk);

    *ppmmhead = pmmhead;

    /*mark it as free and set it to bitmap*/
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    nextblk->dye   = MM_DYE_USED;
    nextblk->owner_id = MM_OWNER_ID_SELF;
    nextblk->trace_id = 0;
    nextblk->owner = 0;
#endif

    /* release free blk */
    k_mm_free(pmmhead, nextblk->mbinfo.buffer);

#if (K_MM_STATISTIC > 0)
    pmmhead->free_size    = MM_GET_BUF_SIZE(nextblk);
    pmmhead->used_size    = len - MM_GET_BUF_SIZE(nextblk);
    pmmhead->maxused_size = pmmhead->used_size;
#endif

#if (RHINO_CONFIG_MM_BLK > 0)
    pmmhead->fix_pool = NULL;
    mmblk_pool = k_mm_alloc(pmmhead, RHINO_CONFIG_MM_TLF_BLK_SIZE + MM_ALIGN_UP(sizeof(mblk_pool_t)));
    if (mmblk_pool) {
        stat = krhino_mblk_pool_init(mmblk_pool, "fixed_mm_blk",
                                     (void *)((size_t)mmblk_pool + MM_ALIGN_UP(sizeof(mblk_pool_t))),
                                     RHINO_CONFIG_MM_TLF_BLK_SIZE);
        if (stat == RHINO_SUCCESS) {
            pmmhead->fix_pool = mmblk_pool;
        } else {
            k_mm_free(pmmhead, mmblk_pool);
        }
    }
#endif

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
    k_mm_list_t        *firstblk;
    k_mm_list_t        *nextblk;
    cpu_cpsr_t flags_cpsr;
    (void)flags_cpsr;

    NULL_PARA_CHK(mmhead);
    NULL_PARA_CHK(addr);

    orig_addr = addr;

    addr = (void *) MM_ALIGN_UP((size_t)addr);
    len -= (size_t)addr - (size_t)orig_addr;
    len  = MM_ALIGN_DOWN(len);

    if (!len || len < sizeof(k_mm_region_info_t) + MMLIST_HEAD_SIZE * 3 + MM_MIN_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    memset(addr, 0, len);

    MM_CRITICAL_ENTER(mmhead, flags_cpsr);

    firstblk = init_mm_region(addr, len);
    nextblk  = MM_GET_NEXT_BLK(firstblk);

    /* Inserting the area in the list of linked areas */
    region             = (k_mm_region_info_t *)firstblk->mbinfo.buffer;
    region->next       = mmhead->regioninfo;
    mmhead->regioninfo = region;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    nextblk->dye   = MM_DYE_USED;
    nextblk->owner_id = MM_OWNER_ID_SELF;
    nextblk->trace_id = 0;
    nextblk->owner = 0;
#endif

#if (K_MM_STATISTIC > 0)
    /* keep "used_size" not changed.
     * change "used_size" here then k_mm_free will decrease it.
     */
    mmhead->used_size += MM_GET_BLK_SIZE(nextblk);
#endif

    MM_CRITICAL_EXIT(mmhead, flags_cpsr);

    /*mark nextblk as free*/
    k_mm_free(mmhead, nextblk->mbinfo.buffer);

    return RHINO_SUCCESS;
}

/* insert blk to freelist[level], and set freebitmap */
static void k_mm_freelist_insert(k_mm_head *mmhead, k_mm_list_t *blk)
{
    int32_t level;

    level = size_to_level(MM_GET_BUF_SIZE(blk));
    if (level < 0 || level >= MM_BIT_LEVEL) {
        return;
    }

    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = mmhead->freelist[level];

    if (mmhead->freelist[level] != NULL) {
        mmhead->freelist[level]->mbinfo.free_ptr.prev = blk;
    }

    mmhead->freelist[level] = blk;

    /* freelist not null, so set the bit  */
    mmhead->free_bitmap |= (1 << level);
}

static void k_mm_freelist_delete(k_mm_head *mmhead, k_mm_list_t *blk)
{
    int32_t level;

    level = size_to_level(MM_GET_BUF_SIZE(blk));
    if (level < 0 || level >= MM_BIT_LEVEL) {
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
            mmhead->free_bitmap &= (~(1 << level));
        }
    }

    blk->mbinfo.free_ptr.prev = NULL;
    blk->mbinfo.free_ptr.next = NULL;
}

static k_mm_list_t *find_up_level(k_mm_head *mmhead, int32_t level)
{
    uint32_t bitmap;

    bitmap = mmhead->free_bitmap & (0xfffffffful << (level + 1));
    level  = krhino_ctz32(bitmap);

    if (level < MM_BIT_LEVEL) {
        return mmhead->freelist[level];
    }

    return NULL;
}

void *k_mm_alloc(k_mm_head *mmhead, size_t size)
{
    void        *retptr;
    k_mm_list_t *get_b, *new_b, *next_b;
    int32_t      level;
    size_t       left_size;
    size_t       req_size = size;
    cpu_cpsr_t   flags_cpsr;

    if (!mmhead) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    MM_CRITICAL_ENTER(mmhead, flags_cpsr);

#if (RHINO_CONFIG_MM_BLK > 0)
    /* little blk, try to get from mm_pool */
    if (mmhead->fix_pool != NULL && size <= RHINO_CONFIG_MM_BLK_SIZE) {
        retptr = krhino_mblk_alloc_nolock((mblk_pool_t *)mmhead->fix_pool, size);
        if (retptr) {
            MM_CRITICAL_EXIT(mmhead, flags_cpsr);
            return retptr;
        }
    }
#endif

    retptr = NULL;

    size = MM_ALIGN_UP(size);
    size = size < MM_MIN_SIZE ? MM_MIN_SIZE : size;

    if ((level = size_to_level(size)) == -1) {
        goto ALLOCEXIT;
    }

#if (RHINO_CONFIG_MM_QUICK > 0)
    /* try to find in higher level */
    get_b = find_up_level(mmhead, level);
    if (get_b == NULL) {
        /* try to find in same level */
        get_b = mmhead->freelist[level];
        while (get_b != NULL) {
            if (MM_GET_BUF_SIZE(get_b) >= size) {
                break;
            }
            get_b = get_b->mbinfo.free_ptr.next;
        }

        if (get_b == NULL) {
            /* do not find availalbe freeblk */
            goto ALLOCEXIT;
        }
    }
#else
    /* try to find in same level */
    get_b = mmhead->freelist[level];
    while (get_b != NULL) {
        if (MM_GET_BUF_SIZE(get_b) >= size) {
            break;
        }
        get_b = get_b->mbinfo.free_ptr.next;
    }

    if (get_b == NULL) {
        /* try to find in higher level */
        get_b = find_up_level(mmhead, level);
        if (get_b == NULL) {
            /* do not find availalbe freeblk */
            goto ALLOCEXIT;
        }
    }
#endif

    k_mm_freelist_delete(mmhead, get_b);

    next_b = MM_GET_NEXT_BLK(get_b);

    /* Should the block be split? */
    if (MM_GET_BUF_SIZE(get_b) >= size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
        left_size = MM_GET_BUF_SIZE(get_b) - size - MMLIST_HEAD_SIZE;

        get_b->buf_size = size | (get_b->buf_size & MM_PRESTAT_MASK);
        new_b = MM_GET_NEXT_BLK(get_b);

        new_b->prev = get_b;
        new_b->buf_size = left_size | MM_BUFF_FREE | MM_BUFF_PREV_USED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        new_b->dye   = MM_DYE_FREE;
        new_b->owner_id = 0;
        new_b->trace_id = 0;
        new_b->owner = 0;
#endif
        next_b->prev = new_b;
        k_mm_freelist_insert(mmhead, new_b);
    } else {
        next_b->buf_size &= (~MM_BUFF_PREV_FREE);
    }
    get_b->buf_size &= (~MM_BUFF_FREE);       /* Now it's used */

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    get_b->dye   = MM_DYE_USED;
    get_b->owner_id = (uint8_t)debug_task_id_now();
    get_b->trace_id = g_mmlk_cnt;
    get_b->owner = 0;
#endif
    retptr = (void *)get_b->mbinfo.buffer;
    if (retptr != NULL) {
        stats_addsize(mmhead, MM_GET_BLK_SIZE(get_b), req_size);
    }

ALLOCEXIT:

    MM_CRITICAL_EXIT(mmhead, flags_cpsr);

    return retptr ;
}

void  k_mm_free(k_mm_head *mmhead, void *ptr)
{
    k_mm_list_t *free_b, *next_b, *prev_b;
    cpu_cpsr_t flags_cpsr;
    (void)flags_cpsr;

    if (!ptr || !mmhead) {
        return;
    }

    MM_CRITICAL_ENTER(mmhead, flags_cpsr);

#if (RHINO_CONFIG_MM_BLK > 0)
    if (krhino_mblk_check(mmhead->fix_pool, ptr)) {
        (void)krhino_mblk_free_nolock((mblk_pool_t *)mmhead->fix_pool, ptr);
        MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        return;
    }
#endif

    free_b = MM_GET_THIS_BLK(ptr);

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (free_b->dye == MM_DYE_FREE) {
        /* step 1 : do not call mm_critical_exit to stop malloc by other core */
        //MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        /* step 2 : freeze other core */
        debug_cpu_stop();
        /* step 3 :printk(do not use printf, maybe malloc) log */
        printk("WARNING, memory maybe double free!! 0x%x\r\n", (unsigned int)free_b);
        /* setp 4 :dumpsys memory and then go to fatal error */
        kmm_error(KMM_ERROR_LOCKED);
    }
    if (free_b->dye != MM_DYE_USED) {
        //MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        debug_cpu_stop();
        printk("WARNING, memory maybe corrupt!! 0x%x\r\n", (unsigned int)free_b);
        kmm_error(KMM_ERROR_LOCKED);
    }
    free_b->dye   = MM_DYE_FREE;
    free_b->owner_id = 0;
    free_b->trace_id = 0;
    free_b->owner = 0;
#endif
    free_b->buf_size |= MM_BUFF_FREE;

    stats_removesize(mmhead, MM_GET_BLK_SIZE(free_b));

    /* if the blk after this freed one is freed too, merge them */
    next_b = MM_GET_NEXT_BLK(free_b);
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (next_b->dye != MM_DYE_FREE && next_b->dye != MM_DYE_USED) {
        //MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        debug_cpu_stop();
        printk("WARNING, memory overwritten!! 0x%x  0x%x\r\n", (unsigned int)free_b, (unsigned int)next_b);
        kmm_error(KMM_ERROR_LOCKED);
    } else if (MM_LAST_BLK_MAGIC != next_b->owner) {
        k_mm_list_t *nnext_b = MM_GET_NEXT_BLK(next_b);
        if (nnext_b->dye != MM_DYE_FREE && nnext_b->dye != MM_DYE_USED) {
            debug_cpu_stop();
            printk("WARNING, nnext memory overwritten!! 0x%x   0x%x  0x%x\r\n", (unsigned int)free_b, (unsigned int)next_b,
                   (unsigned int)nnext_b);
            kmm_error(KMM_ERROR_LOCKED);
        }
    }
#endif

    if (next_b->buf_size & MM_BUFF_FREE) {
        k_mm_freelist_delete(mmhead, next_b);
        free_b->buf_size += MM_GET_BLK_SIZE(next_b);
    }

    /* if the blk before this freed one is freed too, merge them */
    if (free_b->buf_size & MM_BUFF_PREV_FREE) {
        prev_b = free_b->prev;
        k_mm_freelist_delete(mmhead, prev_b);
        prev_b->buf_size += MM_GET_BLK_SIZE(free_b);
        free_b = prev_b;
    }

    /* after merge, free to list */
    k_mm_freelist_insert(mmhead, free_b);

    next_b = MM_GET_NEXT_BLK(free_b);
    next_b->prev = free_b;
    next_b->buf_size |= MM_BUFF_PREV_FREE;

    MM_CRITICAL_EXIT(mmhead, flags_cpsr);
}

void *k_mm_realloc(k_mm_head *mmhead, void *oldmem, size_t new_size)
{
    void        *ptr_aux = NULL;
    uint32_t     cpsize;
    k_mm_list_t *this_b, *split_b, *next_b;
    size_t       old_size, split_size;
    size_t       req_size = 0;
    cpu_cpsr_t flags_cpsr;
    (void)flags_cpsr;

    (void)req_size;

    if (oldmem == NULL) {
        if (new_size > 0) {
            return (void *)k_mm_alloc(mmhead, new_size);
        } else {
            return NULL;
        }
    } else if (new_size == 0) {
        k_mm_free(mmhead, oldmem);
        return NULL;
    }

    req_size =  new_size;

#if (RHINO_CONFIG_MM_BLK > 0)
    if (krhino_mblk_check(mmhead->fix_pool, oldmem)) {
        ptr_aux = k_mm_alloc(mmhead, new_size);
        if (ptr_aux) {
            int cp_len = krhino_mblk_get_size(mmhead->fix_pool, oldmem);
            cp_len = cp_len > new_size ? new_size : cp_len;
            memcpy(ptr_aux, oldmem, cp_len);
            MM_CRITICAL_ENTER(mmhead, flags_cpsr);
            (void)krhino_mblk_free_nolock((mblk_pool_t *)mmhead->fix_pool, oldmem);
            MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        }
        return ptr_aux;
    }
#endif
    MM_CRITICAL_ENTER(mmhead, flags_cpsr);

    this_b   = MM_GET_THIS_BLK(oldmem);
    old_size = MM_GET_BUF_SIZE(this_b);
    next_b   = MM_GET_NEXT_BLK(this_b);
    new_size = MM_ALIGN_UP(new_size);
    new_size = new_size < MM_MIN_SIZE ? MM_MIN_SIZE : new_size;

    if (new_size <= old_size) {
        /* shrink blk */
        stats_removesize(mmhead, MM_GET_BLK_SIZE(this_b));
        if (next_b->buf_size & MM_BUFF_FREE) {
            /* merge next free */
            k_mm_freelist_delete(mmhead, next_b);
            old_size += MM_GET_BLK_SIZE(next_b);
            next_b    = MM_GET_NEXT_BLK(next_b);
        }
        if (old_size >= new_size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
            /* split blk */
            split_size = old_size - new_size - MMLIST_HEAD_SIZE;

            this_b->buf_size = new_size | (this_b->buf_size & MM_PRESTAT_MASK);

            split_b = MM_GET_NEXT_BLK(this_b);

            split_b->prev     = this_b;
            split_b->buf_size = split_size | MM_BUFF_FREE | MM_BUFF_PREV_USED;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
            split_b->dye   = MM_DYE_FREE;
            split_b->owner_id = 0;
            split_b->trace_id = 0;
            split_b->owner = 0;
#endif

            next_b->prev      = split_b;
            next_b->buf_size |= MM_BUFF_PREV_FREE;

            k_mm_freelist_insert(mmhead, split_b);
        }
        stats_addsize(mmhead, MM_GET_BLK_SIZE(this_b), req_size);
        ptr_aux = (void *)this_b->mbinfo.buffer;
    } else if ((next_b->buf_size & MM_BUFF_FREE)) {
        /* enlarge blk */
        if (new_size <= (old_size + MM_GET_BLK_SIZE(next_b))) {
            stats_removesize(mmhead, MM_GET_BLK_SIZE(this_b));

            /* delete next blk from freelist */
            k_mm_freelist_delete(mmhead, next_b);

            /* enlarge this blk */
            this_b->buf_size += MM_GET_BLK_SIZE(next_b);

            next_b = MM_GET_NEXT_BLK(this_b);
            next_b->prev = this_b;
            next_b->buf_size &= ~MM_BUFF_PREV_FREE;

            if (MM_GET_BUF_SIZE(this_b) >= new_size + MMLIST_HEAD_SIZE + MM_MIN_SIZE) {
                /* split blk */
                split_size = MM_GET_BUF_SIZE(this_b) - new_size - MMLIST_HEAD_SIZE;

                this_b->buf_size = new_size | (this_b->buf_size & MM_PRESTAT_MASK);
                split_b = MM_GET_NEXT_BLK(this_b);

                split_b->prev = this_b;
                split_b->buf_size = split_size | MM_BUFF_FREE | MM_BUFF_PREV_USED;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
                split_b->dye   = MM_DYE_FREE;
                split_b->owner_id = 0;
                split_b->trace_id = 0;
                split_b->owner = 0;
#endif
                next_b->prev = split_b;
                next_b->buf_size |= MM_BUFF_PREV_FREE;
                k_mm_freelist_insert(mmhead, split_b);
            }
            stats_addsize(mmhead, MM_GET_BLK_SIZE(this_b), req_size);
            ptr_aux = (void *)this_b->mbinfo.buffer;
        }
    }

    if (ptr_aux) {

#if (RHINO_CONFIG_MM_DEBUG > 0u)
        this_b->dye      = MM_DYE_USED;
        this_b->owner_id = (uint8_t)debug_task_id_now();
        this_b->trace_id = g_mmlk_cnt;
#endif

        MM_CRITICAL_EXIT(mmhead, flags_cpsr);
        return ptr_aux;
    }

    MM_CRITICAL_EXIT(mmhead, flags_cpsr);

    /* re alloc blk */
    ptr_aux = k_mm_alloc(mmhead, new_size);
    if (!ptr_aux) {
        return NULL;
    }

    cpsize = (MM_GET_BUF_SIZE(this_b) > new_size) ? new_size : MM_GET_BUF_SIZE(this_b);

    memcpy(ptr_aux, oldmem, cpsize);
    k_mm_free(mmhead, oldmem);

    return ptr_aux;
}

#if (RHINO_CONFIG_MM_DEBUG > 0u)
void krhino_owner_attach(void *addr, size_t allocator)
{
    k_mm_list_t *blk;

    char *PC;
    int  *SP;

    __asm__ volatile("mov %0, sp\n" : "=r"(SP));
    __asm__ volatile("mov %0, pc\n" : "=r"(PC));

    if (NULL == addr) {
        return;
    }

#if (RHINO_CONFIG_MM_BLK > 0)
    /* fix blk, do not support debug info */
    if (krhino_mblk_check(g_kmm_head->fix_pool, addr)) {
        return;
    }
#endif

    blk = MM_GET_THIS_BLK(addr);

#if (RHINO_CONFIG_MM_TRACE_LVL > 0)
    if ((g_sys_stat == RHINO_RUNNING) &&
        (kmm_bt_check() == 0)) {
        backtrace_now_get((void **) blk->trace, RHINO_CONFIG_MM_TRACE_LVL, 2);
    } else {
        memset(blk->trace, 0, sizeof(blk->trace));
    }
#endif

    blk->owner = allocator;
}
#endif

void *krhino_mm_alloc(size_t size)
{
    void *tmp;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    uint32_t app_malloc = size & AOS_UNSIGNED_INT_MSB;
    size = size & (~AOS_UNSIGNED_INT_MSB);
#endif

    if (size == 0) {
        printf("WARNING, malloc size = 0\r\n");
        return NULL;
    }

    tmp = k_mm_alloc(g_kmm_head, size);
    if (tmp == NULL) {
#if (RHINO_CONFIG_MM_DEBUG > 0)
        static int32_t dumped;
        int32_t freesize;

        freesize = g_kmm_head->free_size;

        printf("WARNING, malloc failed!!!! need size:%d, but free size:%d\r\n", size, freesize);

        if (dumped) {
            return tmp;
        }

        dumped = 1;

        debug_cpu_stop();
        kmm_error(KMM_ERROR_UNLOCKED);
#endif
    }

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_mm_alloc_hook(tmp, size);
#endif

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (app_malloc == 0) {
        krhino_owner_return_addr(tmp);
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

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    uint32_t app_malloc = newsize & AOS_UNSIGNED_INT_MSB;
    newsize = newsize & (~AOS_UNSIGNED_INT_MSB);
#endif

    tmp = k_mm_realloc(g_kmm_head, oldmem, newsize);

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (app_malloc == 0) {
        krhino_owner_return_addr(tmp);
    }
#endif
    if (tmp == NULL && newsize != 0) {
#if (RHINO_CONFIG_MM_DEBUG > 0)
        static int32_t reallocdumped;
        printf("WARNING, realloc failed!!!! newsize : %d\r\n", newsize);
        if (reallocdumped) {
            return tmp;
        }
        reallocdumped = 1;
        debug_cpu_stop();
        kmm_error(KMM_ERROR_UNLOCKED);
#endif
    }
    return tmp;
}

size_t krhino_mm_max_free_size_get(void)
{
    int32_t      index;
    k_mm_list_t *max, *tmp;
    size_t       max_free_block_size = 0;

    /* In order to avoid getting stuck after the exception, the critical zone protection is removed.
       Currently, this interface can only be invoked in exception handling. */

    //cpu_cpsr_t flags_cpsr;

    //MM_CRITICAL_ENTER(g_kmm_head,flags_cpsr);

    index = krhino_clz32(g_kmm_head->free_bitmap);
    if (index > 31) {
        return 0;
    }

    max = g_kmm_head->freelist[31 - index];

    while (max) {
        if (max_free_block_size < MM_GET_BUF_SIZE(max)) {
            max_free_block_size = MM_GET_BUF_SIZE(max);
        }
        tmp  = max->mbinfo.free_ptr.next;
        max = tmp;
    }

    return max_free_block_size;
}
#endif

