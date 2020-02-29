/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 * Filename     : tee_pool.c
 * Author       : Jun Lin
 * Date Created : 02/12/2015
 * Description  : memory phys pool functions
 */

/*
 ******************************
 *          HEADERS
 ******************************
 */
#include "test_bed.h"

/*
 ******************************
 *          MACROS
 ******************************
 */
//#define DEBUG
//#define TEST
//#define CONFIG_POOL_DEBUG

#ifdef CONFIG_POOL_DBG
#define POOL_DBG_E(str, x...)                               \
    do {                                                    \
        printk("%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)

#define POOL_DBG_I(str, x...)                               \
    do {                                                    \
        printk("%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)
#else
#define POOL_DBG_E(str, x...)
#define POOL_DBG_I(str, x...)
#endif /* DEBUG */

#define PMM_ASSERT(_x_) assert(_x_)

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define ALIGN(a, b) ROUNDUP(a, b)
#define IS_ALIGNED(a, b) (!((a) & ((b)-1)))

#define MIN_ALIGN (sizeof(void *))
#define MIN_ALIGN_MASK ((MIN_ALIGN) - (1))

#define POOL_MAGIC 0x506F4F6C /* PoOl */

/*
 ******************************
 *          TYPES
 ******************************
 */
typedef struct _block_t
{
    list_t   node; /* used/free list */
    uint32_t size; /* block size for use */
    uint8_t  mem[0];
} block_t;

typedef struct _pool_t
{
    uint32_t magic;
#ifdef MUTLI_THREAD
    mutex_t lock;
#endif
    list_t   free_list;
    list_t   used_list;
    uint32_t base;
    uint32_t size; /* total size when create */
    uint32_t align;
    uint32_t remaining; /* free size for allocate */
} pool_t;

/* structure placed at the beginning every allocation */
typedef struct _alloc_struct_begin_t
{
    void *original_addr;
} alloc_struct_begin_t;

#ifdef CONFIG_POOL_DEBUG
typedef struct mem_info
{
    void *   base;
    uint32_t total;
    uint32_t free;
    uint32_t freeblk;
    uint32_t usedblk;
} mem_info_t;

mem_info_t pool_info(void *handle);
#endif /* CONFIG_POOL_DEBUG */

/*
 ******************************
 *          VARIABLES
 ******************************
 */

/*
 ******************************
 *          FUNCTIONS
 ******************************
 */
void *pool_create(void *start, uint32_t size, uint32_t align)
{
    pool_t * info;
    block_t *bk;

    PMM_ASSERT(size > 0);

    if (size < sizeof(pool_t) + sizeof(block_t)) {
        return NULL;
    }

    if (align < MIN_ALIGN) {
        align = MIN_ALIGN;
    }

    /* roundup to 4 byte aligned */
    if (!IS_ALIGNED((uint32_t)start, MIN_ALIGN)) {
        size -= (MIN_ALIGN - ((uint32_t)start & MIN_ALIGN_MASK));
        start = (void *)(ROUNDUP((uint32_t)start, MIN_ALIGN));
    }
    size = ROUNDDOWN(size, MIN_ALIGN);

    POOL_DBG_I("create pool, base 0x%08x, size 0x%08x\n", start, size);
    info        = (pool_t *)start;
    info->magic = POOL_MAGIC;
#ifdef MUTLI_THREAD
    mutex_init(&info->lock);
#endif
    list_init(&info->free_list);
    list_init(&info->used_list);
    info->base  = (uint32_t)start;
    info->size  = size;
    info->align = align;

    /* insert free block */
    bk              = (block_t *)(start + sizeof(pool_t));
    bk->size        = size - sizeof(pool_t) - sizeof(block_t);
    info->remaining = bk->size;
    list_add(&info->free_list, &bk->node);

    return (void *)info;
}

void pool_delete(void *handle)
{
    pool_t *info = (pool_t *)handle;

    PMM_ASSERT(info != NULL);
    PMM_ASSERT(POOL_MAGIC == info->magic);

    /* clear pool info */
    memset(info, 0, sizeof(pool_t));
}

#ifdef CONFIG_POOL_DEBUG
/* pool info API, fetch & calc pool info */
mem_info_t pool_info(void *handle)
{
    pool_t *   info = (pool_t *)handle;
    block_t *  bk   = NULL;
    mem_info_t ret;
    list_t *   it = NULL;

    PMM_ASSERT(info != NULL);

    memset(&ret, 0, sizeof(ret));
    ret.base  = info->base;
    ret.total = info->size;
    ret.free  = info->remaining;

    it = info->used_list.next;
    while (it != &info->used_list) {
        bk = list_entry(it, block_t, node);
        it = it->next;
        ret.usedblk++;
    }

    it = info->free_list.next;
    while (it != &info->free_list) {
        bk = list_entry(it, block_t, node);
        it = it->next;
        ret.freeblk++;
    }
    return ret;
}

static void dump_pool_info(void *handle)
{
    mem_info_t info;

    info = pool_info(handle);

    printf("dump pool 0x%08x info:\n"
           "pool base 0x%08x \n"
           "pool total 0x%08x \n"
           "pool free 0x%08x \n"
           "pool freeblk 0x%08x \n"
           "pool usedblk 0x%08x \n",
           handle, info.base, info.total, info.free, info.freeblk,
           info.usedblk);

    return;
}
#endif /* CONFIG_POOL_DEBUG */

/* allocate memory from pool */
void *pool_alloc(void *handle, uint32_t size)
{
    pool_t *              info = (pool_t *)handle;
    block_t *             bk = NULL, *new_bk = NULL;
    void *                ptr        = NULL;
    alloc_struct_begin_t *asb        = NULL;
    uint32_t              addr_align = 0;
    list_t *              entry = NULL, *temp = NULL, *next_node = NULL;

    PMM_ASSERT(info != NULL);
    PMM_ASSERT(POOL_MAGIC == info->magic);

    if (0 == size) {
        return NULL;
    }

    POOL_DBG_I("alloc handle 0x%08x size 0x%08x\n", (uint32_t)handle, size);
    /* the size must be at least 4 bytes align */
    size += sizeof(alloc_struct_begin_t);
    size = ROUNDUP(size, MIN_ALIGN);
    if (info->remaining < size) {
        return NULL;
    }

#ifdef MUTLI_THREAD
    mutex_lock(&info->lock);
#endif
    list_iterate_safe(&info->free_list, entry, temp)
    {
        bk         = list_entry(entry, block_t, node);
        addr_align = ROUNDUP((uint32_t)bk->mem + sizeof(alloc_struct_begin_t),
                             info->align);
        if (bk->size >= size + (addr_align - ((uint32_t)bk->mem +
                                              sizeof(alloc_struct_begin_t)))) {
            /* enough space to alloc memory */
            ptr = (void *)(addr_align);
            /* remove it from the list */
            next_node = list_next(&info->free_list, &bk->node);
            list_del(&bk->node);
            size = size + (addr_align -
                           ((uint32_t)bk->mem + sizeof(alloc_struct_begin_t)));
            if (bk->size > size + 2 * sizeof(block_t)) {
                /* there's enough space in this block to
                 * create a new one after the allocation */
                new_bk       = (block_t *)(bk->mem + size);
                new_bk->size = bk->size - size - sizeof(block_t);

                bk->size = size;
                /* add the new one where block used to be */
                if (next_node) {
                    list_add_before(next_node, &new_bk->node);
                } else {
                    list_add_tail(&info->free_list, &new_bk->node);
                }
                info->remaining -= sizeof(block_t);
            }

            info->remaining -= bk->size;
            asb = (alloc_struct_begin_t *)ptr;
            asb--;
            asb->original_addr = (void *)bk;
            list_add(&info->used_list, &bk->node);
            break;
        }
    }
#ifdef MUTLI_THREAD
    mutex_unlock(&info->lock);
#endif

    return ptr;
}

/* free memory into pool */
void pool_free(void *handle, void *p)
{
    pool_t *              info = (pool_t *)handle;
    block_t *             bk = NULL, *prev_bk = NULL, *next_bk = NULL;
    alloc_struct_begin_t *asb   = NULL;
    list_t *              entry = NULL, *temp = NULL, *prev_node = NULL;

    PMM_ASSERT(NULL != info);
    PMM_ASSERT(POOL_MAGIC == info->magic);
    PMM_ASSERT(NULL != p);

    POOL_DBG_I("free handle 0x%08x addr  0x%08x\n", (uint32_t)handle,
               (uint32_t)p);
#ifdef MUTLI_THREAD
    mutex_unlock(&info->lock);
#endif

    asb = (alloc_struct_begin_t *)p;
    asb--;
    bk = (block_t *)asb->original_addr;
    list_del(&bk->node);
    info->remaining += bk->size;

    list_iterate_safe(&info->free_list, entry, temp)
    {
        next_bk = list_entry(entry, block_t, node);
        if (bk < next_bk) {
            PMM_ASSERT(((uint32_t)bk->mem + bk->size) <= (uint32_t)next_bk);

            list_add_before(&next_bk->node, &bk->node);

            goto try_merge;
        }
    }
    /* walked off the end of the list, add it at the tail */
    list_add_tail(&info->free_list, &bk->node);

    /* try to merge with the previous or next block */
try_merge:
    prev_node = list_prev(&info->free_list, &bk->node);
    if (prev_node) {
        prev_bk = list_entry(prev_node, block_t, node);
        if ((uint32_t)prev_bk->mem + prev_bk->size == (uint32_t)bk) {
            /* just extend the previous block */
            prev_bk->size += bk->size + sizeof(block_t);
            info->remaining += sizeof(block_t);

            /* remove ourself from the list */
            list_del(&bk->node);

            /* set the block pointer to the newly extended block, in case
             * it needs to merge with the next block below */
            bk = prev_bk;
        }
    }

    /* try to merge with the next block */
    if (next_bk) {
        if ((uint32_t)bk->mem + bk->size == (uint32_t)next_bk) {
            /* extend our block */
            bk->size += next_bk->size + sizeof(block_t);
            info->remaining += sizeof(block_t);

            /* remove them from the list */
            list_del(&next_bk->node);
        }
    }

#ifdef MUTLI_THREAD
    mutex_release(&info->lock);
#endif

    return;
}


static void *   _g_global_pool = NULL;
static uint32_t _g_test_heap[NTW_HEAP_SIZE / sizeof(uint32_t)]
  __attribute__((aligned(8)));

void kmem_init(void)
{
    if (NULL == _g_global_pool) {
        _g_global_pool = pool_create((void *)_g_test_heap, NTW_HEAP_SIZE, 4);
    }
    return;
}

void *kmalloc(size_t size, int flags)
{
    return pool_alloc(_g_global_pool, size);
}
void kfree(const void *ptr)
{
    return pool_free(_g_global_pool, (void *)ptr);
}
