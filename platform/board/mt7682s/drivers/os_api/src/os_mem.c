#include "os_common.h"
#include "typedef.h"
#include "os_mem.h"

void *nos_malloc(u32_t size)
{
	if(0 == size)
	{
		return NULL;
	}
	return krhino_mm_alloc(size);
}

void *nos_realloc(void *ptr,u32_t size)
{
	if(0 == size)
	{
		return NULL;
	}
	return krhino_mm_realloc(ptr, size);
}

void *nos_calloc(size_t n,u32_t size)
{
	if(0 == size)
	{
		return NULL;
	}
	u32_t calloc_size = n*size;
	void *ptr = NULL;
	ptr = krhino_mm_alloc(calloc_size);
	if(ptr)
	{
		memset(ptr, 0, calloc_size);
	}
	return ptr;
}

void nos_free(void* ptr)
{
	if(NULL == ptr)
	{
		return;
	}
	krhino_mm_free(ptr);
}

void os_print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    printf("============free heap size =%d==========\r\n", free);
}