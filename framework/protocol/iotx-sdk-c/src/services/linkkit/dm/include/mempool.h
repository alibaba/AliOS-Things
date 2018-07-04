#ifndef __AMEMPOOL_H__
#define __AMEMPOOL_H__

typedef struct AMemPool AMemPool;

AMemPool *AMemPool_New(int blk_size, int nb_prealloc, int (*pinit)(void *ptr), int (*pfree)(void *ptr));
int AMemPool_Free(AMemPool *p);

void *AMemPool_Get(AMemPool *p);
int AMemPool_Put(AMemPool *p, void *data);

int AMemPool_NumFree(AMemPool *p);
int AMemPool_NumUsed(AMemPool *p);

#endif /* __AMEMPOOL_H__ */
