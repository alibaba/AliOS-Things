#include "lite-utils_internal.h"
#include "cut.h"

static char *module_name = "ut_mem_stats";

extern void *_create_mem_table(char *module_name, struct list_head *list_head);
extern void *_find_mem_table(char *module_name, struct list_head *list_head);
extern int _del_mem_table(char *module_name, struct list_head *list_head);
extern int _count_malloc_internal(const char *f, const int l, int size, va_list ap);


CASE(mem_stats, case_memory_internal)
{
#define ALLOC_SIZE 10
    char *pmalloc = NULL;
    char *prealloc = NULL;

    pmalloc = LITE_malloc_internal(__FUNCTION__, __LINE__, -1, MEM_MAGIC, module_name);
    ASSERT_NULL(pmalloc);

    prealloc = LITE_realloc_internal(__FUNCTION__, __LINE__, pmalloc, -1, MEM_MAGIC, module_name);
    ASSERT_NULL(prealloc);

    LITE_free_internal(NULL);

    pmalloc = LITE_malloc_internal(__FUNCTION__, __LINE__, ALLOC_SIZE, MEM_MAGIC, module_name);
    prealloc = LITE_realloc_internal(__FUNCTION__, __LINE__, pmalloc, ALLOC_SIZE + 10, MEM_MAGIC, module_name);
    log_info("LITE_malloc_internal:%p LITE_realloc_internal:%p", pmalloc, prealloc);
    ASSERT_NOT_NULL(pmalloc);
    ASSERT_NOT_NULL(prealloc);
    LITE_free_internal(prealloc);
    log_info("LITE_free_internal:%p", prealloc);

#undef ALLOC_SIZE
}

CASE(mem_stats, case_memory_routine)
{
#define ALLOC_SIZE 10

    char *pmalloc = LITE_malloc_routine(ALLOC_SIZE, MEM_MAGIC, module_name);
    log_info("LITE_malloc_routine:%p", pmalloc);
    ASSERT_NOT_NULL(pmalloc);
    log_info("LITE_free_routine:%p", pmalloc);
    LITE_free_routine(pmalloc);

#undef ALLOC_SIZE
}

CASE(mem_stats, case_dump_malloc_free_stats)
{
    char *p = NULL;
    int loglevel_bakups = LITE_get_loglevel();

    LITE_set_loglevel(LOG_CRIT_LEVEL);
    LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
    LITE_set_loglevel(loglevel_bakups);

    p = LITE_malloc(100);
    ASSERT_NOT_NULL(p);
}

CASE(mem_stats, case_track_malloc_callstack)
{
    LITE_track_malloc_callstack(0);
    LITE_track_malloc_callstack(1);
}

CASE(mem_stats, case_calloc_routine)
{
    char *pcalloc = LITE_calloc_routine(1, 100, MEM_MAGIC, module_name);
    ASSERT_NOT_NULL(pcalloc);
    LITE_free(pcalloc);
}


CASE(mem_stats, case_mem_table)
{
#if WITH_MEM_STATS_PER_MODULE
    LIST_HEAD(case_mem_list);
    void *handle = NULL;
    int ret = -1;

    handle = _create_mem_table("debug_case", NULL);
    ASSERT_NULL(handle);

    handle = _create_mem_table("debug_case", &case_mem_list);
    ASSERT_NOT_NULL(handle);

    handle = _find_mem_table("debug_case", NULL);
    ASSERT_NULL(handle);

    handle = _find_mem_table("debug_case", &case_mem_list);
    ASSERT_NOT_NULL(handle);

    ret = _del_mem_table("debug_case", &case_mem_list);
    ASSERT_EQ(ret, 0);

    ret = _del_mem_table("debug_case", &case_mem_list);
    ASSERT_EQ(ret, -1);

#endif
}

SUITE(mem_stats) = {
    ADD_CASE(mem_stats, case_memory_internal),
    ADD_CASE(mem_stats, case_memory_routine),
    ADD_CASE(mem_stats, case_dump_malloc_free_stats),
    ADD_CASE(mem_stats, case_track_malloc_callstack),
    ADD_CASE(mem_stats, case_calloc_routine),
    ADD_CASE(mem_stats, case_mem_table),
    ADD_CASE_NULL
};

