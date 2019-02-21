/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOTX_UTILS_H__
#define __IOTX_UTILS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#if defined(_PLATFORM_IS_LINUX_)
    #include <assert.h>
#endif

#include "iotx_utils_config.h"

#include "iotx_log.h"

#include "utils_hmac.h"
#include "utils_httpc.h"
#include "lite-cjson.h"
#include "lite-list.h"
#include "string_utils.h"
#include "json_parser.h"
#include "utils_md5.h"
#include "utils_sha256.h"

#include "iot_import.h"

#define LITE_TRUE                   (1)
#define LITE_FALSE                  (0)

#ifndef container_of
#define container_of(ptr, type, member)  \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif

#define ARGUMENT_SANITY_CHECK(expr, err) \
    do { \
        if (!(expr)) { \
            log_err("utils", "Invalid argument, expression '%s' is FALSE", #expr); \
            return (err); \
        } \
    } while(0)

#define POINTER_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            log_err("utils", "Invalid argument, %s = %p", #ptr, ptr); \
            return (err); \
        } \
    } while(0)

#define STRING_PTR_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            log_err("utils", "Invalid argument, %s = %p", #ptr, (ptr)); \
            return (err); \
        } \
        if (0 == strlen((ptr))) { \
            log_err("utils", "Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return (err); \
        } \
    } while(0)

#define CONFIG_VARS_DUMP(var, logf) logf("%32s : %d\n", #var, var)

#define LITE_MINIMUM(a, b)          (((a) <= (b)) ? (a) : (b))
#define LITE_MAXIMUM(a, b)          (((a) >= (b)) ? (a) : (b))
#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (LITE_TRUE) : (LITE_FALSE))

#if defined(_PLATFORM_IS_LINUX_)
#define LITE_ASSERT(expr)           assert(expr)
#else
#define LITE_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            UTILS_printf("### %s | %s(%d): ASSERT FAILED ###: %s is FALSE\r\n", \
                         __FILE__, __func__, __LINE__, #expr); \
        } \
    } while(0)
#endif

#define MEM_MAGIC                       (0x1234)
#if WITH_MEM_STATS
#define LITE_calloc(num, size, ...)     LITE_malloc_internal(__func__, __LINE__, (num * size), ##__VA_ARGS__)
#define LITE_malloc(size, ...)          LITE_malloc_internal(__func__, __LINE__, size, ##__VA_ARGS__)
#define LITE_realloc(ptr, size, ...)    LITE_realloc_internal(__func__, __LINE__, ptr, size, ##__VA_ARGS__)
#define LITE_free(ptr)              \
    do { \
        if (!ptr) { \
            log_warning("utils", "%s == NULL! LITE_free(%s) aborted.", #ptr, #ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)
    
#else
#define LITE_calloc(num, size, ...)     LITE_malloc_internal(NULL, 0, (num * size))
#define LITE_malloc(size, ...)          LITE_malloc_internal(NULL, 0, size)
#define LITE_realloc(ptr, size, ...)    LITE_realloc_internal(NULL, 0, ptr, size)
#define LITE_free(ptr)                  LITE_free_internal((void *)ptr)
#endif


void       *LITE_malloc_internal(const char *f, const int l, int size, ...);
void       *LITE_realloc_internal(const char *f, const int l, void *ptr, int size, ...);
void        LITE_free_internal(void *ptr);
void       *LITE_malloc_routine(int size, ...);
void        LITE_free_routine(void *ptr);
void       *LITE_calloc_routine(size_t n, size_t s, ...);

char       *LITE_strdup(const char *src, ...);
char       *LITE_format_string(const char *fmt, ...);
char       *LITE_format_nstring(const int len, const char *fmt, ...);
void        LITE_hexbuf_convert(unsigned char *digest, char *out, int buflen, int uppercase);
void        LITE_hexstr_convert(char *input, int input_len, unsigned char *output, int output_len);
void        LITE_replace_substr(char orig[], char key[], char swap[]);

void        LITE_dump_malloc_free_stats(int level);
void        LITE_track_malloc_callstack(int state);

char           *LITE_json_value_of(char *key, char *src, ...);
list_head_t    *LITE_json_keys_of(char *src, char *prefix, ...);

char           *LITE_json_value_of_ext(char *key, char *src, ...);
char           *LITE_json_value_of_ext2(char *key, char *src, int src_len, int *value_len);

list_head_t    *LITE_json_keys_of_ext(char *src, char *prefix, ...);

int             LITE_json_value_type(char *src, int src_len);
char           *LITE_json_array_get_item(int index, char *src, int src_len, int *val_len);

void            LITE_json_keys_release(list_head_t *keylist);

typedef struct _json_key_t {
    char           *key;
    list_head_t     list;
} json_key_t;

#if WITH_JSON_KEYS_OF
#define foreach_json_keys_in(src, iter_key, keylist, pos)   \
    for(keylist = (void *)LITE_json_keys_of((char *)src, ""), \
        pos = (void *)list_first_entry((list_head_t *)keylist, json_key_t, list), \
        iter_key = ((json_key_t *)pos)->key; \
        (iter_key = ((json_key_t *)pos)->key); \
        pos = list_next_entry((json_key_t *)pos, list, json_key_t))
#endif

#if WITH_MEM_STATS
    void **LITE_get_mem_mutex(void);
#endif

#endif  /* __LITE_UTILS_H__ */
