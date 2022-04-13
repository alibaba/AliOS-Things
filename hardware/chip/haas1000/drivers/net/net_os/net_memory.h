#ifndef _NET_MEMORY_H__
#define _NET_MEMORY_H__

#include <stdlib.h>
#include "net_defs.h"
#include "net_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mem_element{
    unsigned int size;
    unsigned char data[0];
};

void rt_system_heap_init(void *begin_addr, void *end_addr);
void *rt_malloc(size_t size);
void *rt_realloc(void *rmem, size_t newsize);
void *rt_calloc(size_t count, size_t size);
void rt_free(void *rmem);
void *rt_kmalloc(size_t size, gfp_t flags);
void *rt_kzalloc(size_t size, gfp_t flags);
int rt_kfree(void * mem);
void *rt_krealloc(const void *p, size_t new_size, gfp_t flags);
uint32 rt_get_free_heap_size(void);
void print_mem_dump(void * mem);
#if 1
#define  kmalloc(size, flags) (\
   {\
       void* ret = NULL;\
       ret = rt_kmalloc(size, (gfp_t)flags);\
       ret;\
    }\
)
#else
#define  kmalloc(size, flags) (\
       {\
           uint8_t* ret = NULL;\
           ret = rt_kmalloc(size, flags);\
           printk(KERN_DEBUG, "kmalloc %s %d mem = 0x%x size:%d\n", __func__, __LINE__, ret, size);\
           ret;\
        }\
    )
#endif
/**
 * kzalloc - allocate memory. The memory is set to zero.
 * @size: how many bytes of memory are required.
 * @flags: the type of memory to allocate (see kmalloc).
 */
 #if 1
 #define kzalloc(size, flags) (\
   {   \
       void* ret = NULL;\
       ret = rt_kzalloc(size, (gfp_t)flags);\
       ret;\
   }\
)
 #else
#define kzalloc(size, flags) (\
   {   \
       uint8_t* ret = NULL;\
       ret = rt_kzalloc(size, flags);\
       printk(KERN_DEBUG, "kzalloc %s %d mem = 0x%x\n", __func__, __LINE__, ret);\
       ret;\
   }\
)
#endif
/**
 * kcalloc - allocate memory for an array. The memory is set to zero.
 * @n: number of elements.
 * @size: element size.
 * @flags: the type of memory to allocate (see kmalloc).
 */
#define kcalloc( n,  size, flags) kzalloc(n*size, flags)
#ifndef mem_calloc_skb
#define mem_calloc_skb(n, size) kzalloc(n*size, 1)
#endif

static __INLINE size_t ksize(const void *objp)
{
    struct mem_element* mem_p = NULL;
    mem_p = (struct mem_element *)((size_t)objp - offsetof(struct mem_element, data));
    return mem_p->size;
}
#if 1
#define kzfree(mem) ({ \
    uint32_t ret;\
    if(mem) \
        ret = rt_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})
#else
#define kzfree(mem) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "kzfree %s %d %x \n", __func__, __LINE__, mem);\
    if(mem) \
        ret = rt_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})

#endif
#if 1
#define kfree(ptr) ({ \
    uint32_t ret;\
    if(ptr) \
        ret = rt_kfree((void *)ptr);\
    else\
        ret = 0;\
        ret;\
})

#define krealloc(p, new_size, flags) ({\
    void* ret = NULL;\
    ret = rt_krealloc(p, new_size, (gfp_t)flags);\
    ret;\
})
#else
#define kfree(ptr) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "kfree %s %d %x \n", __func__, __LINE__, ptr);\
    if(ptr) \
        ret = rt_kfree(ptr);\
    else\
        ret = 0;\
        ret;\
})

#define krealloc(p, new_size, flags) ({\
    uint8_t* ret = NULL;\
    printk(KERN_DEBUG, "krealloc %s %d src_mem = 0x%x new_size:%d\n", __func__, __LINE__, p, new_size);\
    ret = rt_krealloc(p, new_size, flags);\
    printk(KERN_DEBUG, "krealloc mem = 0x%x\n", ret);\
    ret;\
})

#endif
extern void *kmemdup(const void *src, size_t len, gfp_t gfp);

#ifdef __cplusplus
}
#endif

#endif

