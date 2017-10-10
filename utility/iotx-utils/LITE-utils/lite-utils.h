/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __LITE_UTILS_H__
#define __LITE_UTILS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>

#include "lite-list.h"
#include "lite-log.h"

#define LITE_TRUE                   (1)
#define LITE_FALSE                  (0)

#ifndef container_of
#define container_of(ptr, type, member)  \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif

#define LITE_MINIMUM(a, b)          (((a) <= (b)) ? (a) : (b))
#define LITE_MAXIMUM(a, b)          (((a) >= (b)) ? (a) : (b))
#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (LITE_TRUE) : (LITE_FALSE))

#define LITE_calloc(num, size)      LITE_malloc_internal(__func__, __LINE__, (num * size))
#define LITE_malloc(size)           LITE_malloc_internal(__func__, __LINE__, size)
#define LITE_realloc(ptr, size)     LITE_realloc_internal(__func__, __LINE__, ptr, size)
#define LITE_free(ptr)              \
    do { \
        if(!ptr) { \
            log_err("%s == NULL! LITE_free(%s) aborted.", #ptr, #ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)

void       *LITE_malloc_internal(const char *f, const int l, int size);
void       *LITE_realloc_internal(const char *f, const int l, void *ptr, int size);
void        LITE_free_internal(void *ptr);
void       *LITE_malloc_routine(int size);
void        LITE_free_routine(void *ptr);

char       *LITE_strdup(const char *src);
char       *LITE_format_string(const char *fmt, ...);
char       *LITE_format_nstring(const int len, const char *fmt, ...);
void        LITE_hexbuf_convert(unsigned char *digest, char *out, int buflen, int uppercase);
void        LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int len);
void        LITE_replace_substr(char orig[], char key[], char swap[]);

void        LITE_dump_malloc_free_stats(int level);
void        LITE_track_malloc_callstack(int state);

char           *LITE_json_value_of(char *key, char *src);
list_head_t    *LITE_json_keys_of(char *src, char *prefix);
void            LITE_json_keys_release(list_head_t *keylist);

typedef struct _json_key_t {
    char           *key;
    list_head_t     list;
} json_key_t;

#define foreach_json_keys_in(src, iter_key, keylist, pos)   \
    for(keylist = (void *)LITE_json_keys_of((char *)src, ""), \
        pos = (void *)list_first_entry((list_head_t *)keylist, json_key_t, list), \
        iter_key = ((json_key_t *)pos)->key; \
            (iter_key = ((json_key_t *)pos)->key); \
                pos = list_next_entry((json_key_t *)pos, list))

int unittest_string_utils(void);
int unittest_json_parser(void);
int unittest_json_token(void);

#endif  /* __LITE_UTILS_H__ */
