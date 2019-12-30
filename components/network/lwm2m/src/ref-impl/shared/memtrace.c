/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innvoations GmbH and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/

#ifdef LWM2M_MEMORY_TRACE

#undef malloc
#undef free
#undef strdup

typedef struct MemoryEntry {
    struct MemoryEntry* next;
    const char *file;
    const char *function;
    int         lineno;
    size_t      size;
    int         count;
    uint32_t    data[1];
} memory_entry_t;

static memory_entry_t prv_memory_malloc_list = { .next = NULL, .file = "head", .function="malloc", .lineno = 0, .size = 0, .count = 0};
static memory_entry_t prv_memory_free_list = { .next = NULL, .file = "head", .function="free", .lineno = 0, .size = 0, .count = 0};

static memory_entry_t* prv_memory_find_previous(memory_entry_t* list, void* memory)
{
    while (NULL != list->next)
    {
        if (list->next->data == memory)
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

static void prv_trace_add_free_list(memory_entry_t* remove, const char* file, const char* function, int lineno)
{
    remove->next = prv_memory_free_list.next;
    prv_memory_free_list.next = remove;
    remove->file = file;
    remove->function = function;
    remove->lineno = lineno;

    if (prv_memory_free_list.count < 200)
    {
        ++prv_memory_free_list.count;
    }
    else if (NULL != remove->next)
    {
        while (NULL != remove->next->next)
        {
            remove = remove->next;
        }
        lwm2m_free(remove->next);
        remove->next = NULL;
    }
}

char* lwm2m_trace_strdup(const char* str, const char* file, const char* function, int lineno)
{
    size_t length = strlen(str);
    char* result = lwm2m_trace_malloc(length +1, file, function, lineno);
    memcpy(result, str, length);
    result[length] = 0;
    return result;
}

void* lwm2m_trace_malloc(size_t size, const char* file, const char* function, int lineno)
{
    static int counter = 0;
    memory_entry_t* entry = lwm2m_malloc(size + sizeof(memory_entry_t));
    entry->next = prv_memory_malloc_list.next;
    prv_memory_malloc_list.next = entry;
    ++prv_memory_malloc_list.count;
    prv_memory_malloc_list.size += size;
    prv_memory_malloc_list.lineno = 1;

    entry->file = file;
    entry->function = function;
    entry->lineno = lineno;
    entry->size = size;
    entry->count = ++counter;

    return &(entry->data);
}

void lwm2m_trace_free(void* mem, const char* file, const char* function, int lineno)
{
    if (NULL != mem)
    {
        memory_entry_t* entry = prv_memory_find_previous(&prv_memory_malloc_list, mem);
        if (NULL != entry)
        {
            memory_entry_t* remove = entry->next;
            entry->next = remove->next;
            --prv_memory_malloc_list.count;
            prv_memory_malloc_list.size -= remove->size;
            prv_memory_malloc_list.lineno = 1;
            prv_trace_add_free_list(remove, file, function, lineno);
        }
        else
        {
            lwm2m_log(LOG_INFO, "memory: free error (no malloc) %s, %d, %s\n", file, lineno, function);
            memory_entry_t* entry = prv_memory_find_previous(&prv_memory_free_list, mem);
            if (NULL != entry)
            {
                entry = entry->next;
                lwm2m_log(LOG_ERR, "memory: already frees at %s, %d, %s\n", entry->file, entry->lineno, entry->function);
            }
        }
    }
}

void trace_print(int loops, int level)
{
    static int counter = 0;
    if (0 == loops)
    {
        counter = 0;
    }
    else
    {
        ++counter;
    }
    if (0 == loops || (((counter % loops) == 0) && prv_memory_malloc_list.lineno))
    {
        prv_memory_malloc_list.lineno = 0;
        if (1 == level)
        {
            size_t total = 0;
            int entries = 0;
            memory_entry_t* entry = prv_memory_malloc_list.next;
            while (NULL != entry)
            {
                lwm2m_log(LOG_INFO, "memory: #%d, %lu bytes, %s, %d, %s\n", entry->count, (unsigned long) entry->size, entry->file, entry->lineno, entry->function);
                ++entries;
                total += entry->size;
                entry = entry->next;
            }
            if (entries != prv_memory_malloc_list.count)
            {
                lwm2m_log(LOG_ERR, "memory: error %d entries != %d\n", prv_memory_malloc_list.count, entries);
            }
            if (total != prv_memory_malloc_list.size)
            {
                lwm2m_log(LOG_ERR, "memory: error %lu total bytes != %lu\n", (unsigned long) prv_memory_malloc_list.size, (unsigned long) total);
            }
        }
        lwm2m_log(LOG_INFO, "memory: %d entries, %lu total bytes\n", prv_memory_malloc_list.count, (unsigned long) prv_memory_malloc_list.size);
    }
}

void trace_status(int* blocks, size_t* size)
{
    if (NULL != blocks)
    {
        *blocks = prv_memory_malloc_list.count;
    }

    if (NULL != size)
    {
        *size = prv_memory_malloc_list.size;
    }
}

#endif
