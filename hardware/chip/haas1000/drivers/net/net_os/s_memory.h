#ifndef _S_MEMORY_H__
#define _S_MEMORY_H__

#include "net_defs.h"
#include "net_debug.h"

struct s_mem_element{
    unsigned int size;
    unsigned char data[0];
};
//#define DEBUG_S_MEMORY
extern void s_system_heap_init(void *begin_addr, void *end_addr);
extern void *s_malloc(size_t size,char* func,unsigned int line);
extern void *s_realloc(void *rmem, size_t newsize,char* func,unsigned int line);
extern void *s_calloc(size_t count, size_t size,char* func,unsigned int line);
extern void s_free(void *rmem);
extern void *s_kmalloc(size_t size, gfp_t flags,char* func,unsigned int line);
extern void *s_kzalloc(size_t size, gfp_t flags,char* func,unsigned int line);
extern int s_kfree(void * mem);
extern void *s_krealloc(const void *p, size_t new_size, gfp_t flags,char* func,unsigned int line);
extern void s_print_mem_dump(void * mem);
#if 1
#define  skmalloc(size, flags) (\
   {\
       uint8_t* ret = NULL;\
       ret = s_kmalloc(size, flags,(char*)__func__, __LINE__);\
       ret;\
    }\
)
 /*memset(test_buff,0,100);\
       strcpy(test_buff,func); \
       strcat(test_buff,' :: '); \
       strcat(test_buff,__func__); \*/

#define  skmalloc1(size, flags,func) (\
   {\
       uint8_t* ret = NULL;\
       ret = s_kmalloc(size, flags,func, __LINE__);\
       ret;\
    }\
)
#else
#define  skmalloc(size, flags) (\
       {\
           uint8_t* ret = NULL;\
           ret = s_kmalloc(size, flags);\
           printk(KERN_DEBUG, "skmalloc %s %d mem = 0x%x size:%d\n", __func__, __LINE__, ret, size);\
           ret;\
        }\
    )
#endif
/**
 * skzalloc - allocate memory. The memory is set to zero.
 * @size: how many bytes of memory are required.
 * @flags: the type of memory to allocate (see kmalloc).
 */
 #if 0
 #define skzalloc(size, flags) (\
   {   \
       uint8_t* ret = NULL;\
       ret = s_kzalloc(size, flags);\
       ret;\
   }\
)
 #else
#define skzalloc(size, flags) (\
   {   \
       uint8_t* ret = NULL;\
       ret = s_kzalloc(size, flags,__func__, __LINE__);\
       printk(KERN_DEBUG, "kzalloc %s %d mem = 0x%x\n", __func__, __LINE__, ret);\
       ret;\
   }\
)
#endif
/**
 * skcalloc - allocate memory for an array. The memory is set to zero.
 * @n: number of elements.
 * @size: element size.
 * @flags: the type of memory to allocate (see kmalloc).
 */
#define skcalloc( n,  size, flags) skzalloc(n*size, flags)
#define smem_calloc(n, size) skzalloc(n*size, 1)

static __INLINE size_t s_ksize(const void *objp)
{
    struct s_mem_element* mem_p = NULL;
    mem_p = (struct s_mem_element *)((size_t)objp - offsetof(struct s_mem_element, data));
    return mem_p->size;
}
#if 1
#define skzfree(mem) ({ \
    uint32_t ret;\
    if(mem) \
        ret = s_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})
#else
#define skzfree(mem) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "skzfree %s %d %x \n", __func__, __LINE__, mem);\
    if(mem) \
        ret = s_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})

#endif
#if 1
#define skfree(ptr) ({ \
    uint32_t ret;\
    if(ptr) \
        ret = s_kfree(ptr);\
    else\
        ret = 0;\
        ret;\
})

#define skrealloc(p, new_size, flags) ({\
    uint8_t* ret = NULL;\
    ret = s_krealloc(p, new_size, flags,__func__, __LINE__);\
    ret;\
})
#else
#define skfree(ptr) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "skfree %s %d %x \n", __func__, __LINE__, ptr);\
    if(ptr) \
        ret = s_kfree(ptr);\
    else\
        ret = 0;\
        ret;\
})

#define skrealloc(p, new_size, flags) ({\
    uint8_t* ret = NULL;\
    printk(KERN_DEBUG, "skrealloc %s %d src_mem = 0x%x new_size:%d\n", __func__, __LINE__, p, new_size);\
    ret = s_krealloc(p, new_size, flags);\
    printk(KERN_DEBUG, "skrealloc mem = 0x%x\n", ret);\
    ret;\
})

#endif
extern void *skmemdup(const void *src, size_t len, gfp_t gfp);
#endif
