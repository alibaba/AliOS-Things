#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "aos_system.h"
#include "amp_memmgt.h"

typedef struct amp_memmgt_rec_stru_tag {
    void *ptr;
    unsigned int size;
    unsigned int lr;
    unsigned int seq;
    struct amp_memmgt_rec_stru_tag *next;
    struct amp_memmgt_rec_stru_tag *prev;
} amp_memmgt_rec_stru;

static amp_memmgt_rec_stru *g_mem_rec_head = NULL;
static amp_memmgt_rec_stru *g_mem_rec_tail = NULL;
static amp_memmgt_config_t g_mem_config;

static unsigned int g_mem_rec_num = 0;
static unsigned int g_mem_max[MEMMGT_TOTAL_PT_NUM] = {0};
static unsigned int g_mem_total[MEMMGT_TOTAL_PT_NUM] = {0};
static aos_mutex_t g_mem_rec_lock = NULL;

#ifdef MEMMGT_MEM_DBG_STATIC
static void add_node(void *ptr, unsigned int size, unsigned int lr, int ptno)
{
    amp_memmgt_rec_stru * node = NULL;

    node = g_mem_config.malloc_fn(sizeof(amp_memmgt_rec_stru));
    if(NULL == node) {
        return;
    }
    memset(node, 0, sizeof(amp_memmgt_rec_stru));

    aos_mutex_lock(&g_mem_rec_lock, AOS_WAIT_FOREVER);
    g_mem_rec_tail->next = node;
    node->prev           = g_mem_rec_tail;
    g_mem_rec_tail       = node;

    node->ptr  = ptr;
    node->size = size;
    node->seq  = g_mem_rec_num;
    node->lr   = lr;

    *(unsigned int *)ptr = (unsigned int)node;

    g_mem_rec_num ++;
    aos_mutex_unlock(&g_mem_rec_lock);
}

static void delete_node(amp_memmgt_rec_stru *node)
{
    if(node == NULL) {
        return;
    }
    aos_mutex_lock(&g_mem_rec_lock, AOS_WAIT_FOREVER);
    if(node == g_mem_rec_tail) {
        g_mem_rec_tail = node->prev;
    }
    if(NULL != node->prev) {
        node->prev->next = node->next;
    }

    if(NULL != node->next) {
        node->next->prev = node->prev;
    }

    if (g_mem_config.free_fn) {
        g_mem_config.free_fn(node);
    }

    aos_mutex_unlock(&g_mem_rec_lock);
}
#endif


int amp_memmgt_init(amp_memmgt_config_t *config)
{
    if ((config->malloc_fn == NULL) || (config->free_fn == NULL)) {
        return -1;
    }

    memcpy(&g_mem_config, config, sizeof(amp_memmgt_config_t));

#ifdef MEMMGT_MEM_DBG_STATIC
    if(NULL == g_mem_rec_head) {
        g_mem_rec_head = config->malloc_fn(sizeof(amp_memmgt_rec_stru));
        memset(g_mem_rec_head, 0, sizeof(amp_memmgt_rec_stru));
        aos_mutex_new(&g_mem_rec_lock);
        g_mem_rec_tail = g_mem_rec_head;
    }
#endif

    return 0;
}

void amp_memmgt_mem_show_rec()
{
#ifdef MEMMGT_MEM_DBG_STATIC
    aos_mutex_lock(&g_mem_rec_lock, AOS_WAIT_FOREVER);
    amp_memmgt_rec_stru *rec_node = g_mem_rec_head;
    if(rec_node != NULL) {
        aos_printf("seq   addr       size         LR\r\n");

        while(rec_node) {
            if(rec_node->ptr) {
                aos_printf("%4u ", rec_node->seq);
                aos_printf("0x%x ", rec_node->ptr);
                aos_printf("0x%x   ", rec_node->size);
                aos_printf("0x%x \n", rec_node->lr);
            }
            rec_node = rec_node->next;
        }
    }
    aos_mutex_unlock(&g_mem_rec_lock);
#endif

    for (int i = 0; i < g_mem_config.pt_num; i++) {
        aos_printf("\r\nPT[%d]: max mem %u, now mem %u \n", i, g_mem_max[i], g_mem_total[i]);
    }
}

void *amp_memmgt_malloc(unsigned int size, unsigned int lr, int ptno)
{
    void *ptr = NULL;

    unsigned int alloc_size = size + MEMMGT_OFFSET;
    unsigned int once_size  = alloc_size;

    if(NULL == g_mem_config.malloc_fn) {
        return NULL;
    }

#ifdef MEMMGT_MEM_SIZE_CHECK
#ifdef MEMMGT_MEM_DBG_STATIC
    once_size += sizeof(amp_memmgt_rec_stru);
#endif
    if((g_mem_config.mem_limit[ptno] != 0) && (g_mem_total[ptno] + once_size > g_mem_config.mem_limit[ptno])) {
        aos_printf("[amp_memory] memory leak, pt %d, size %d, lr 0x%x, total size 0x%x, limit 0x%x\n", ptno, size, lr, g_mem_total[ptno], g_mem_config.mem_limit[ptno]);
        return NULL;
    }
#endif
    ptr = g_mem_config.malloc_fn(alloc_size);
    if(NULL == ptr) {
        aos_printf("[amp_memory%s] lr 0x%x memory alloc failed, system will stop !!!\n", __TIME__, lr);
        amp_memmgt_mem_show_rec();
        while(1) {
            aos_msleep(1000);
        }
        return NULL;
    }

    g_mem_total[ptno] += alloc_size;

#ifdef MEMMGT_MEM_DBG_STATIC
    add_node(ptr, alloc_size, lr, ptno);
    g_mem_total[ptno] += sizeof(amp_memmgt_rec_stru);
#else
    *(unsigned int *)ptr = alloc_size;
#endif

    if(g_mem_total[ptno] > g_mem_max[ptno]) {
        g_mem_max[ptno] = g_mem_total[ptno];
    }
#ifdef MEMMGT_MEM_TRACE
    if(((1<<ptno) & g_mem_config.trace_pt) != 0) {
        aos_printf("[amp_memory:%s] lr 0x%x alloc %p, size %d\n", __TIME__, lr, (void *)((unsigned int)ptr + MEMMGT_OFFSET), size);
        aos_printf("                      now total malloc %u, max malloc %u\n", g_mem_total[ptno], g_mem_max[ptno]);
    }
#endif
    return (void *)((unsigned int)ptr + MEMMGT_OFFSET);
}

void *amp_memmgt_realloc(void *ptr, unsigned int size, unsigned int lr, int ptno)
{
    void *ptr_new    = NULL;
    void *origin_ptr = NULL;

    unsigned int alloc_size = size + MEMMGT_OFFSET;
    unsigned int old_size   = size;
    unsigned int once_size  = alloc_size;

    if(NULL == g_mem_config.realloc_fn) {
        return NULL;
    }

#ifdef MEMMGT_MEM_TRACE
    if(((1<<ptno) & g_mem_config.trace_pt) != 0) {
        aos_printf("[amp_memory:%s] lr 0x%x realloc %p, size %d\n", __TIME__, lr, ptr, size);
    }
#endif

#ifdef MEMMGT_MEM_DBG_STATIC
    amp_memmgt_rec_stru *node = *(unsigned int *)((uint32_t)ptr - MEMMGT_OFFSET);
    amp_memmgt_rec_stru tmp_node;
    memcpy(&tmp_node, node, sizeof(amp_memmgt_rec_stru));

    old_size = node->size;
#else
    old_size = *(unsigned int *)((unsigned int)ptr - 4);
#endif
    origin_ptr = (void *)((unsigned int)ptr - 4);

#ifdef MEMMGT_MEM_SIZE_CHECK
#ifdef MEMMGT_MEM_DBG_STATIC
    once_size += sizeof(amp_memmgt_rec_stru);
#endif
    once_size -= old_size;
    if((g_mem_config.mem_limit[ptno] != 0) && (g_mem_total[ptno] + once_size > g_mem_config.mem_limit[ptno])) {
        aos_printf("[amp_memory:%s] memory leak, pt %d, size %d, lr 0x%x, total size 0x%x, limit 0x%x\n", __TIME__,
                ptno, size, lr, g_mem_total[ptno], g_mem_config.mem_limit[ptno]);
        return NULL;
    }
#endif

    ptr_new = g_mem_config.realloc_fn(origin_ptr, alloc_size);
    if(NULL == ptr_new) {
        aos_printf("[amp_memory:%s] lr 0x%x memory realloc failed, system will stop !!!\n", __TIME__, lr);
        amp_memmgt_mem_show_rec();
        while(1) {
            aos_msleep(1000);
        }
        return NULL;
    }

    g_mem_total[ptno] += alloc_size - old_size;
    if(g_mem_total[ptno] > g_mem_max[ptno]) {
        g_mem_max[ptno] = g_mem_total[ptno];
    }

#ifdef MEMMGT_MEM_DBG_STATIC
    if(origin_ptr != ptr_new) {
        delete_node(node);
        add_node(ptr_new, alloc_size, lr, ptno);
    } else {
        node->size = size;
    }
#else
    *(unsigned int *)ptr_new = alloc_size;
#endif

#ifdef MEMMGT_MEM_TRACE
    if(((1<<ptno) & g_mem_config.trace_pt) != 0) {
        aos_printf("[amp_memory:%s] lr 0x%x realloc result %p\n", __TIME__, lr, (void *)((unsigned int)ptr_new + MEMMGT_OFFSET));
        aos_printf("                      now total malloc %u, max malloc %u\n", g_mem_total[ptno], g_mem_max[ptno]);
    }
#endif

    return (void *)((unsigned int)ptr_new + MEMMGT_OFFSET);
}

void amp_memmgt_free(void *ptr, unsigned int lr, int ptno)
{
    void       *origin_ptr = (void *)((uint32_t)ptr - MEMMGT_OFFSET);
    unsigned int free_size = 0;

    if (ptr == NULL) {
        return;
    }

#ifdef MEMMGT_MEM_DBG_STATIC
    amp_memmgt_rec_stru *node = NULL;
    node = (amp_memmgt_rec_stru *)(*(unsigned int *)origin_ptr);
    free_size = node->size;
#else
    free_size  = *(unsigned int *)((unsigned int)origin_ptr);
#endif
    g_mem_total[ptno] -= free_size;

#ifdef MEMMGT_MEM_DBG_STATIC
    delete_node(node);
#endif
    g_mem_config.free_fn(origin_ptr);

#ifdef MEMMGT_MEM_TRACE
    if(((1<<ptno) & g_mem_config.trace_pt) != 0) {
        aos_printf("[amp_memory:%s] lr 0x%x free %p\n", __TIME__, lr, ptr);
        aos_printf("                      now total malloc %u, max malloc %u\n", g_mem_total[ptno], g_mem_max[ptno]);
    }
#endif
}

unsigned int amp_malloc_usable_size(void *ptr)
{
    void *origin_ptr = (void *)((uint32_t)ptr - MEMMGT_OFFSET);
    uint32_t size = 0;

    if (ptr == NULL) {
        return 0;
    }

#ifdef MEMMGT_MEM_DBG_STATIC
    amp_memmgt_rec_stru *node = NULL;
    node = (amp_memmgt_rec_stru *)(*(unsigned int *)origin_ptr);
    if (node != NULL) {
        size = node->size;
    }
#else
    size  = *(unsigned int *)((unsigned int)origin_ptr);
#endif
    return size;
}
