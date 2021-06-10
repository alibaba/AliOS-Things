#include <string.h>		// memset()
#include <psram_reserve.h>
#include <ameba_soc.h>

static struct Psram_Heap g_Psram_heap;

int g_Psram_heap_inited = 0;
static	_lock	Psram_heap_lock;

#define configTOTAL_PSRAM_HEAP_SIZE		(0x200000)
#define configBYTE_PSRAM_ALIGNMENT			32

PSRAM_HEAP_SECTION 
static unsigned char psRAMHeap[configTOTAL_PSRAM_HEAP_SIZE];

void Psram_heap_init(void)
{
	/* Initialize heap with a single big chunk */
	g_Psram_heap.FreeList = (PsramMemChunk *)(psRAMHeap);
	g_Psram_heap.FreeList->next = NULL;
	g_Psram_heap.FreeList->size = configTOTAL_PSRAM_HEAP_SIZE;

	g_Psram_heap_inited = 1;

	rtw_spinlock_init(&Psram_heap_lock);
}

/**
  * @brief  allocate free memory from PSRAM.
  * @param  size: size of the memory you want.
  * @retval: header address of the memory allocated.
  */
void *Psram_heap_allocmem(int size)
{
	PsramMemChunk *chunk, *prev;
	struct Psram_Heap* h = &g_Psram_heap;
	_irqL 	irqL;

	rtw_enter_critical(&Psram_heap_lock, &irqL);

	if(!g_Psram_heap_inited)	Psram_heap_init();

	/* Round size up to the allocation granularity */
	size = Psram_ROUND_UP2(size, configBYTE_PSRAM_ALIGNMENT);

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(PsramMemChunk);

	/* Walk on the free list looking for any chunk big enough to
	 * fit the requested block size.
	 */
	for (prev = (PsramMemChunk *)&h->FreeList, chunk = h->FreeList;
		chunk;
		prev = chunk, chunk = chunk->next)
	{
		if (chunk->size >= size)
		{
			if (chunk->size == size)
			{
				/* Just remove this chunk from the free list */
				prev->next = chunk->next;

				rtw_exit_critical(&Psram_heap_lock, &irqL);

				return (void *)chunk;
			}
			else
			{
				/* Allocate from the END of an existing chunk */
				chunk->size -= size;
				void *result = (void *)((uint8_t *)chunk + chunk->size);

				rtw_exit_critical(&Psram_heap_lock, &irqL);

				return result;
			}
		}
	}

	rtw_exit_critical(&Psram_heap_lock, &irqL);

	return NULL; /* fail */
}


/**
  * @brief  free memory in PSRAM.
  * @param  mem: header address of the memory to be freed.
  * @param  size: size of the memory.
  * @retval none
  */
void Psram_reserved_heap_freemem(void *mem, int size)
{
	PsramMemChunk *prev;
	struct Psram_Heap* h = &g_Psram_heap;
	_irqL 	irqL;

	rtw_enter_critical(&Psram_heap_lock, &irqL);
	
	if(!g_Psram_heap_inited)	Psram_heap_init();

	/* Round size up to the allocation granularity */
	size = Psram_ROUND_UP2(size, configBYTE_PSRAM_ALIGNMENT);

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(PsramMemChunk);

	/* Special cases: first chunk in the free list or memory completely full */
	if (((uint8_t *)mem) < ((uint8_t *)h->FreeList) || !h->FreeList)
	{
		/* Insert memory block before the current free list head */
		prev = (PsramMemChunk *)mem;
		prev->next = h->FreeList;
		prev->size = size;
		h->FreeList = prev;
	}
	else /* Normal case: not the first chunk in the free list */
	{
		/*
		 * Walk on the free list. Stop at the insertion point (when mem
		 * is between prev and prev->next)
		 */
		prev = h->FreeList;
		while (prev->next < (PsramMemChunk *)mem && prev->next)
			prev = prev->next;

		/* Should it be merged with previous block? */
		if (((uint8_t *)prev) + prev->size == ((uint8_t *)mem))
		{
			/* Yes */
			prev->size += size;
		}
		else /* not merged with previous chunk */
		{
			PsramMemChunk *curr = (PsramMemChunk*)mem;

			/* insert it after the previous node
			 * and move the 'prev' pointer forward
			 * for the following operations
			 */
			curr->next = prev->next;
			curr->size = size;
			prev->next = curr;

			/* Adjust for the following test */
			prev = curr;
		}
	}

	/* Also merge with next chunk? */
	if (((uint8_t *)prev) + prev->size == ((uint8_t *)prev->next))
	{
		prev->size += prev->next->size;
		prev->next = prev->next->next;
	}

	rtw_exit_critical(&Psram_heap_lock, &irqL);
}

/**
  * @brief  get the free memory size in PSRAM.
  * @retval size of free momery in PSRAM.
  */
int Psram_reserve_free_size(void)
{
	int free_mem = 0;
	struct Psram_Heap* h = &g_Psram_heap;
	_irqL 	irqL;
	PsramMemChunk *chunk;

	rtw_enter_critical(&Psram_heap_lock, &irqL);
	
	if(!g_Psram_heap_inited)	Psram_heap_init();
	
	for (chunk = h->FreeList; chunk; chunk = chunk->next)
		free_mem += chunk->size;

	rtw_exit_critical(&Psram_heap_lock, &irqL);
	return free_mem;
}

/**
  * @brief  allocate free memory from PSRAM.
  * @param  size: size of the memory you want.
  * @retval: header address of the memory allocated.
  */
void *Psram_reserve_malloc(int size)
{
	int *mem;

	size += sizeof(int);
	if ((mem = (int*)Psram_heap_allocmem(size))){
		*mem++ = size;
	}

	return (void *)mem;
}

/**
  * @brief allocate several continuous blocks of free memory from PSRAM.
  * @param  num: num of blocks you want.
  * @param  size: size of every block.
  * @retval: header address of the memory allocated.
  */
void *Psram_reserve_calloc(int num, int size)
{
	void *mem;

	if ((mem = Psram_reserve_malloc(num * size)))
		memset(mem, 0, num * size);

	return mem;
}

/**
  * @brief  free memory in PSRAM.
  * @param  mem: header address of the memory to be freed.
  * @retval none
  */
void Psram_reserve_free(void *mem)
{
	int *_mem = (int *)mem;

	if (_mem)
	{
		--_mem;
		Psram_reserved_heap_freemem(_mem, *_mem);
	}
}


