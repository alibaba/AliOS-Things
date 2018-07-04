#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h>

#include "mempool.h"
#include "lite-utils.h"
#include "iot_import.h"

#define max(a, b)   ((a) > (b) ? (a) : (b))

typedef struct AMemBlks {
    struct AMemBlks *next;
    uint8_t mem[0];
} AMemBlks;

struct AMemPool {
    int blk_size;

    int num_free;
    int num_used;

    AMemBlks *first_block;

    void *lock;

    int (*init)(void *ptr);
    int (*free)(void *ptr);
};

AMemPool *AMemPool_New(int blk_size, int nb_prealloc, int (*pinit)(void *ptr), int (*pfree)(void *ptr))
{
    if (blk_size <= 0 || nb_prealloc < 0)
        return NULL;

    AMemPool *p = LITE_malloc(sizeof(AMemPool));
    if (!p)
        return NULL;
    memset(p, 0, sizeof(AMemPool));

    p->lock = HAL_MutexCreate();
    if (!p->lock) {
        LITE_free(p);
        return NULL;
    }

    p->blk_size = blk_size;
    p->init = pinit;
    p->free = pfree;

    int i;
    for (i = 0; i < nb_prealloc; i++) {
        AMemBlks *mblk = LITE_malloc(sizeof(AMemBlks) + p->blk_size);
        if (!mblk)
            break;

        if (p->init) {
            if (p->init(mblk->mem) != 0) {
                LITE_free(mblk);
                break;
            }
        }

        mblk->next = p->first_block;
        p->first_block = mblk;
        p->num_free++;
    }

    return p;
}

int AMemPool_Free(AMemPool *p)
{
    if (!p)
        return -1;

    printf("num_free = %d, blk_size = %d, total_size = %d\n", p->num_free, p->blk_size, p->num_free * p->blk_size);
    printf("num_used = %d\n", p->num_used);

    while (p->first_block) {
        AMemBlks *mblk = p->first_block;
        p->first_block = mblk->next;

        if (p->free)
            p->free(mblk->mem);

        p->num_free--;

        LITE_free(mblk);
    }

    assert(p->num_free == 0);
    assert(p->num_used == 0);

    HAL_MutexDestroy(p->lock);
    LITE_free(p);

    return 0;
}

void *AMemPool_Get(AMemPool *p)
{
    HAL_MutexLock(p->lock);
    if (!p->first_block) {
        AMemBlks *mblk = LITE_malloc(sizeof(AMemBlks) + p->blk_size);
        if (!mblk) {
            HAL_MutexUnlock(p->lock);
            return NULL;
        }

        if (p->init) {
            if (p->init(mblk->mem) != 0) {
                LITE_free(mblk);
                HAL_MutexUnlock(p->lock);
                return NULL;
            }
        }

        mblk->next = p->first_block;
        p->first_block = mblk;
        p->num_free++;
    }

    AMemBlks *mblk = p->first_block;
    p->first_block = mblk->next;
    p->num_free--;
    p->num_used++;
    HAL_MutexUnlock(p->lock);

    return (void *)mblk->mem;
}

#ifndef container_of
#define container_of(ptr, type, member)     (type *)((char *)ptr - offsetof(type, member))
#endif

int AMemPool_Put(AMemPool *p, void *data)
{
    if (!p || !data)
        return -1;

    AMemBlks *mblk = container_of(data, AMemBlks, mem);

    HAL_MutexLock(p->lock);
    mblk->next = p->first_block;
    p->first_block = mblk;
    p->num_free++;
    p->num_used--;
    HAL_MutexUnlock(p->lock);

    return 0;
}

int AMemPool_NumFree(AMemPool *p)
{
    HAL_MutexLock(p->lock);
    int ret = p->num_free;
    HAL_MutexUnlock(p->lock);

    return ret;
}

int AMemPool_NumUsed(AMemPool *p)
{
    HAL_MutexLock(p->lock);
    int ret = p->num_used;
    HAL_MutexUnlock(p->lock);

    return ret;
}

#ifdef TEST

#define NELEMS(x)   (sizeof(x) / sizeof((x)[0]))

#include <stdio.h>

int main(void)
{
    AMemPool *mp = AMemPool_New(4, 32, NULL, NULL);
    if (!mp)
        return -1;

    assert(AMemPool_NumFree(mp) == 32);
    assert(AMemPool_NumUsed(mp) == 0);

    void *p1 = AMemPool_Get(mp);
    assert(p1 != NULL);

    void *p2 = AMemPool_Get(mp);
    assert(p2 != NULL);

    void *p3 = AMemPool_Get(mp);
    assert(p3 != NULL);

    void *p4 = AMemPool_Get(mp);
    assert(p4 != NULL);

    assert(AMemPool_NumFree(mp) == 28);
    assert(AMemPool_NumUsed(mp) == 4);

    AMemPool_Put(mp, p1);
    AMemPool_Put(mp, p2);
    AMemPool_Put(mp, p3);
    AMemPool_Put(mp, p4);

    assert(AMemPool_NumFree(mp) == 32);
    assert(AMemPool_NumUsed(mp) == 0);

    uint8_t *ptr[64];

    int i;
    for (i = 0; i < 64; i++) {
        if (i < 32) {
            assert(AMemPool_NumFree(mp) == 32 - i);
        }

        if (i >= 32) {
            assert(AMemPool_NumFree(mp) == 0);
        }

        ptr[i] = AMemPool_Get(mp);
        assert(AMemPool_NumUsed(mp) == i + 1);
    }

    assert(AMemPool_NumFree(mp) == 0);
    assert(AMemPool_NumUsed(mp) == 64);

    for (i = 0; i < 64; i++) {
        AMemPool_Put(mp, ptr[i]);
        assert(AMemPool_NumFree(mp) == i + 1);
        assert(AMemPool_NumUsed(mp) == 64 - i - 1);
    }

    assert(AMemPool_NumFree(mp) == 64);
    assert(AMemPool_NumUsed(mp) == 0);

    AMemPool_Free(mp);

    printf("AMemPool test success!\n");

    return 0;
}
#endif
